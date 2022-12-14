//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 2014 by Bj?rn Ganster
//
// Filename: src-IL/src/il_gif.c
//
// Description: Reads from a Graphics Interchange Format (.gif) file.
//
//  The LZW decompression code is based on code released to the public domain
//    by Javier Arevalo and can be found at
//    http://www.programmersheaven.com/zone10/cat452
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifndef IL_NO_GIF

#include "il_gif.h"
#include <stdio.h>

//-----------------------------------------------------------------------------
// ILimage offers all the members needed to reflect a GIF

// Set minimum image size for several images
ILboolean setMinSizes(ILuint w, ILuint h, struct ILimage* img)
{
   struct ILimage* currImg = img;
   int resizeNeeded = IL_FALSE;

   if (img == NULL)
      return IL_FALSE;

   while (currImg != NULL) {
      if (currImg->Width > w)
         w = currImg->Width;
      else if (w > currImg->Width)
         resizeNeeded = IL_TRUE;
      if (currImg->Height > h)
         h = currImg->Height;
      else if (h > currImg->Height)
         resizeNeeded = IL_TRUE;
      currImg = currImg->Next;
   }

   if (resizeNeeded) {
		ILboolean success = IL_TRUE;
		struct ILimage* currImg = img;
		while (currImg != NULL && success) {
			success &= ilResizeImage(currImg, w, h, 1, 1, 1);
			currImg = currImg->Next;
		}
		return success;
   } else
		return IL_TRUE;
}

//-----------------------------------------------------------------------------


// Internal function to get the header and check it.
ILboolean iIsValidGif(SIO* io)
{
	char Header[6];
	ILint64 read = io->read(io->handle, Header, 1, 6);
	io->seek(io->handle, -read, IL_SEEK_CUR);

	if (read != 6)
		return IL_FALSE;
	if (!strnicmp(Header, "GIF87A", 6))
		return IL_TRUE;
	if (!strnicmp(Header, "GIF89A", 6))
		return IL_TRUE;

	return IL_FALSE;
}


// If a frame has a local palette, the global palette is not used by this frame
ILboolean iGetPalette(ILubyte Info, ILpal *Pal, ILimage *PrevImage)
{
	ILuint read = 0;

	// The ld(palettes bpp - 1) is stored in the lower 3 bits of Info
	Pal->PalSize = (1 << ((Info & 0x7) + 1)) * 3; // never larger than 768
	Pal->PalType = IL_PAL_RGB24;
	Pal->Palette = (ILubyte*)ialloc(256 * 3);

	if (Pal->Palette == NULL) {
		Pal->PalSize = 0;
		return IL_FALSE;
	}

	// Read the new palette
	read = iCurImage->io.read(iCurImage->io.handle, Pal->Palette, 1, Pal->PalSize);
	if (read != Pal->PalSize) {
		memset(Pal->Palette + read, 0, Pal->PalSize-read);
		return IL_FALSE;
	} else
		return IL_TRUE;
}

#define MAX_CODES 4096
class GifLoadState {
public:
   ILint	curr_size, clear, ending, newcodes, top_slot, slot, navail_bytes, nbits_left;
   ILubyte	b1;
	static const ILuint byte_buff_size = 257;

   // Default constructor
   GifLoadState() 
   {
		mStackIndex = 0;
		mBufIndex = 0;
		mValidState = true;
   }

   // Destructor
   ~GifLoadState()
   {
   }

	inline void setByteBuff(ILuint index, ILubyte value)
	{
		if (index < byte_buff_size)
			mByteBuff[index] = value;
		else
			mValidState = false;
	}

	inline ILubyte getByteBuff(ILuint index)
	{
		if (index < byte_buff_size) {
			return mByteBuff[index];
		} else {
			mValidState = false;
			return 0;
		}
	}

	inline void push(ILubyte value)
	{
		if (mStackIndex <= MAX_CODES) {
			mStack[mStackIndex] = value;
			++mStackIndex;
		} else
			mValidState = false;
	}

	inline ILboolean stackIndexLegal() 
	{
		if (mStackIndex >= MAX_CODES)
			return IL_FALSE;
		else
			return IL_TRUE;
	}

	inline ILubyte getStack()
	{
		if (mStackIndex <= MAX_CODES) {
			return mStack[mStackIndex];
		} else {
			mValidState = false;
			return 0;
		}
	}

	inline void pop() 
	{
		if (mStackIndex > 0)
			mStackIndex--;
		else
			mValidState = false;
	}

	inline ILuint getStackIndex() const
	{
		return mStackIndex;
	}

	inline ILubyte getSuffix(ILint code)
	{
		if (code < MAX_CODES) {
			return mSuffix[code];
		} else {
			mValidState = false;
			return 0;
		}
	}

	inline void setSuffix(ILubyte code)
	{
		if (slot < MAX_CODES)
			mSuffix[slot] = code;
		else
			mValidState = false;
	}

	inline void setPrefix(ILshort code)
	{
		if (slot < MAX_CODES)
				mPrefix[slot++] = code;
		else
			mValidState = false;
	}

	inline ILshort getPrefix(ILint code)
	{
		if (code >= 0 && code <= MAX_CODES) {
			return mPrefix[code];
		} else {
			mValidState = false;
			return 0;
		}
	}

	inline void resetBufIndex()
	{ mBufIndex = 0; }

	inline ILubyte nextInBuffer()
	{
		if (mBufIndex < byte_buff_size) {
			ILubyte val = mByteBuff[mBufIndex];
			++mBufIndex;
			return val;
		} else 
			return 0;
	}

	inline ILboolean isValid() const
	{ return mValidState; }

private:
   ILuint mBufIndex;
   ILubyte	mByteBuff[byte_buff_size];
   ILuint mStackIndex;
   ILubyte	mStack[MAX_CODES + 1];
   ILubyte	mSuffix[MAX_CODES + 1];
   ILshort	mPrefix[MAX_CODES + 1];
	ILboolean mValidState;
};

ILuint code_mask[13] =
{
   0L,
   0x0001L, 0x0003L,
   0x0007L, 0x000FL,
   0x001FL, 0x003FL,
   0x007FL, 0x00FFL,
   0x01FFL, 0x03FFL,
   0x07FFL, 0x0FFFL
};



ILint get_next_code(GifLoadState* state) {
	ILint	i, t;
	ILuint	ret;

	//20050102: Tests for IL_EOF were added because this function
	//crashed sometimes if iCurImage->io.getc(iCurImage->io.handle) returned IL_EOF
	//(for example "table-add-column-before-active.gif" included in the
	//mozilla source package)

	if (state->nbits_left == 0) {
		if (state->navail_bytes <= 0) {
			state->resetBufIndex();
			state->navail_bytes = iCurImage->io.getc(iCurImage->io.handle);

			if(state->navail_bytes == IL_EOF) {
				return state->ending;
			}

			if (state->navail_bytes) {
				for (i = 0; i < state->navail_bytes; i++) {
					if((t = iCurImage->io.getc(iCurImage->io.handle)) == IL_EOF) {
						return state->ending;
					}
					state->setByteBuff(i, t);
				}
			}
		}
		state->b1 = state->nextInBuffer();
		state->nbits_left = 8;
		state->navail_bytes--;
	}

	ret = state->b1 >> (8 - state->nbits_left);
	while (state->curr_size >state-> nbits_left) {
		if (state->navail_bytes <= 0) {
			state->resetBufIndex();
			state->navail_bytes = iCurImage->io.getc(iCurImage->io.handle);

			if(state->navail_bytes == IL_EOF) {
				return state->ending;
			}

			if (state->navail_bytes) {
				for (i = 0; i < state->navail_bytes; i++) {
					if((t = iCurImage->io.getc(iCurImage->io.handle)) == IL_EOF) {
						return state->ending;
					}
					state->setByteBuff(i, t);
				}
			}
		}
		state->b1 = state->nextInBuffer();
		ret |= state->b1 << state->nbits_left;
		state->nbits_left += 8;
		state->navail_bytes--;
	}
	state->nbits_left -= state->curr_size;

	return (ret & code_mask[state->curr_size]);
}


ILboolean GifGetData(ILimage *Image, ILubyte *Data, ILuint ImageSize, 
   ILuint OffX, ILuint OffY, ILuint Width, ILuint Height, ILuint Stride, 
   GFXCONTROL *Gfx)
{
	GifLoadState state;
	ILint	code, fc, oc;
	ILubyte	DisposalMethod = 0;
	ILint	c, size;
	ILuint	x = OffX, Read = 0, y = OffY;
	ILuint dataOffset = y * Stride + x;

	state.navail_bytes = 0;
	state.nbits_left = 0;

	if (!Gfx->Used)
		DisposalMethod = (Gfx->Packed & 0x1C) >> 2;
	if((size = iCurImage->io.getc(iCurImage->io.handle)) == IL_EOF)
		return IL_FALSE;

	if (size < 2 || 9 < size) {
		return IL_FALSE;
	}

	state.curr_size = size + 1;
	state.top_slot = 1 << state.curr_size;
	state.clear = 1 << size;
	state.ending = state.clear + 1;
	state.slot = state.newcodes = state.ending + 1;
	state.navail_bytes = state.nbits_left = 0;
	oc = fc = 0;

	while ((c = get_next_code(&state)) != state.ending 
	&&     Read < Height
	&& state.isValid()) 
	{
		if (c == state.clear)
		{
			state.curr_size = size + 1;
			state.slot = state.newcodes;
			state.top_slot = 1 << state.curr_size;
			while ((c = get_next_code(&state)) == state.clear);
			if (c == state.ending)
				break;
			if (c >= state.slot)
				c = 0;
			oc = fc = c;

			if (DisposalMethod == 1 && !Gfx->Used && Gfx->Transparent == c && (Gfx->Packed & 0x1) != 0)
				x++;
			else if (x < Width) {
				Data[dataOffset+x] = c;
            ++x;
         }

			if (x >= Width)
			{
				//DataPtr += Stride;
				x = 0;
				Read += 1;
				++y;
            dataOffset = y * Stride +  OffX;
				if (y >= Height) {
				   return IL_FALSE;
				}
			}
		}
		else
		{
			code = c;
         //BG-2007-01-10: several fixes for incomplete GIFs
			if (code >= state.slot)
			{
				code = oc;
				if (!state.stackIndexLegal()) {
					return IL_FALSE;
				}
				state.push(fc);
			}

			if (code >= MAX_CODES)
				return IL_FALSE; 
			while (code >= state.newcodes)
			{
				if (!state.stackIndexLegal()) {
					return IL_FALSE;
				}
				state.push(state.getSuffix(code));
				code = state.getPrefix(code);
			}
            
			if (!state.stackIndexLegal()) {
				return IL_FALSE;
			}

			state.push((ILbyte)code);
			if (state.slot < state.top_slot)
			{
				fc = code;
				state.setSuffix(fc);
				state.setPrefix(oc);
				oc = c;
			}
			if (state.slot >= state.top_slot && state.curr_size < 12)
			{
				state.top_slot <<= 1;
				state.curr_size++;
			}
			while (state.getStackIndex() > 0)
			{
				state.pop();
				if (DisposalMethod == 1 && !Gfx->Used && Gfx->Transparent == state.getStack()
            &&  (Gfx->Packed & 0x1) != 0)
            {
					x++;
            } else if (x < Width) {
					Data[dataOffset+x] = state.getStack();
               x++;
            }

				if (x >= Width) // end of line
				{
					x = OffX % Width;
					Read += 1;
               y = (y+1) % Height;
					// Needs to start from Data, not Image->Data.
					dataOffset = y * Stride +  OffX;
				}
			}
		}
	}

	return state.isValid();
}

ILboolean GetImages(ILimage* Image, ILpal *GlobalPal, GIFHEAD *GifHead)
{
	IMAGEDESC	ImageDesc, OldImageDesc;
	GFXCONTROL	Gfx;
	ILboolean	BaseImage = IL_TRUE;
	ILimage		*TempImage = NULL, *PrevImage = NULL;
	ILuint		NumImages = 0, i;
	ILint		input;

	OldImageDesc.ImageInfo = 0; // to initialize the data with an harmless value 
	Gfx.Used = IL_TRUE;

	while (!iCurImage->io.eof(iCurImage->io.handle)) {
		ILubyte DisposalMethod = 1;
		
		i = iCurImage->io.tell(iCurImage->io.handle);
		if (!SkipExtensions(&Gfx))
			goto error_clean;
		i = iCurImage->io.tell(iCurImage->io.handle);

		if (!Gfx.Used)
			DisposalMethod = (Gfx.Packed & 0x1C) >> 2;

		//read image descriptor
      iCurImage->io.read(iCurImage->io.handle, &ImageDesc, 1, sizeof(ImageDesc));
      #ifdef __BIG_ENDIAN__
      iSwapUShort(ImageDesc.OffX);
      iSwapUShort(ImageDesc.OffY);
      iSwapUShort(ImageDesc.Width);
      iSwapUShort(ImageDesc.Height);
      #endif
      
		if (ImageDesc.Separator != 0x2C) //end of image
			break;

      if (!setMinSizes(ImageDesc.OffX + ImageDesc.Width, ImageDesc.OffY + ImageDesc.Height, Image))
			goto error_clean;

		if (iCurImage->io.eof(iCurImage->io.handle)) {
			ilGetError();  // Gets rid of the IL_FILE_READ_ERROR that inevitably results.
			break;
		}


		if (!BaseImage) {
			NumImages++;
			Image->Next = ilNewImage(Image->Width, Image->Height, 1, 1, 1);
			if (Image->Next == NULL)
				goto error_clean;
			//20040612: DisposalMethod controls how the new images data is to be combined
			//with the old image. 0 means that it doesn't matter how they are combined,
			//1 means keep the old image, 2 means set to background color, 3 is
			//load the image that was in place before the current (this is not implemented
			//here! (TODO?))
			if (DisposalMethod == 2 || DisposalMethod == 3)
				//Note that this is actually wrong, too: If the image has a local
				//color table, we should really search for the best fit of the
				//background color table and use that index (?). Furthermore,
				//we should only memset the part of the image that is not read
				//later (if we are sure that no parts of the read image are transparent).
				if (!Gfx.Used && Gfx.Packed & 0x1)
					memset(Image->Next->Data, Gfx.Transparent, Image->SizeOfData);
				else
					memset(Image->Next->Data, GifHead->Background, Image->SizeOfData);
			else if (DisposalMethod == 1 || DisposalMethod == 0)
				memcpy(Image->Next->Data, Image->Data, Image->SizeOfData);
			//Interlacing has to be removed after the image was copied (line above)
			if (OldImageDesc.ImageInfo & (1 << 6)) {  // Image is interlaced.
				if (!RemoveInterlace(Image))
					goto error_clean;
			}

			PrevImage = Image;
			Image = Image->Next;
			Image->Format = IL_COLOUR_INDEX;
			Image->Origin = IL_ORIGIN_UPPER_LEFT;
		} else {
			BaseImage = IL_FALSE;
			if (!Gfx.Used && Gfx.Packed & 0x1)
				memset(Image->Data, Gfx.Transparent, Image->SizeOfData);
			else
			    memset(Image->Data, GifHead->Background, Image->SizeOfData);
		}

		Image->OffX = ImageDesc.OffX;
		Image->OffY = ImageDesc.OffY;

		// Check to see if the image has its own palette
		if (ImageDesc.ImageInfo & (1 << 7)) {
			if (!iGetPalette(ImageDesc.ImageInfo, &Image->Pal, PrevImage)) {
				goto error_clean;
			}
		} else {
			if (!iCopyPalette(&Image->Pal, GlobalPal)) {
				goto error_clean;
			}
		}

		if (!GifGetData(Image, Image->Data, Image->SizeOfData, ImageDesc.OffX, ImageDesc.OffY,
				ImageDesc.Width, ImageDesc.Height, Image->Bps, &Gfx)) {
			ilSetError(IL_ILLEGAL_FILE_VALUE);
			goto error_clean;
		}

		// See if there was a valid graphics control extension.
		if (!Gfx.Used) {
			Gfx.Used = IL_TRUE;
			Image->Duration = Gfx.Delay * 10;  // We want it in milliseconds.

			// See if a transparent colour is defined.
			if (Gfx.Packed & 1) {
				if (!ConvertTransparent(Image, Gfx.Transparent)) {
					goto error_clean;
				}
	    	}
		}
		i = iCurImage->io.tell(iCurImage->io.handle);
		// Terminates each block.
		if((input = iCurImage->io.getc(iCurImage->io.handle)) == IL_EOF)
			goto error_clean;

		if (input != 0x00)
		    iCurImage->io.seek(iCurImage->io.handle, -1, IL_SEEK_CUR);
		//	break;

		OldImageDesc = ImageDesc;
	}

	//Deinterlace last image
	if (OldImageDesc.ImageInfo & (1 << 6)) {  // Image is interlaced.
		if (!RemoveInterlace(Image))
			goto error_clean;
	}

	if (BaseImage)  // Was not able to load any images in...
		return IL_FALSE;

	return IL_TRUE;

error_clean:
	Image = Image->Next;
    /*	while (Image) {
		TempImage = Image;
		Image = Image->Next;
		ilCloseImage(TempImage);
	}*/
	return IL_FALSE;
}

// Internal function used to load the Gif.
ILboolean iLoadGifInternal(ILimage* image)
{
	GIFHEAD	Header;
	ILpal	GlobalPal;

	if (image == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	GlobalPal.Palette = NULL;
	GlobalPal.PalSize = 0;

	// Read header
	iCurImage->io.read(iCurImage->io.handle, &Header, 1, sizeof(Header));
	#ifdef __BIG_ENDIAN__
	iSwapUShort(Header.Width);
	iSwapUShort(Header.Height);
	#endif

	if (strnicmp(Header.Sig, "GIF87A", 6) != 0 
	&&  strnicmp(Header.Sig, "GIF89A", 6) != 0) 
	{
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	if (!ilTexImage(Header.Width, Header.Height, 1, 1, IL_COLOUR_INDEX, IL_UNSIGNED_BYTE, NULL))
   //if (!ilTexImage(Header.Width, Header.Height, 1, 1, IL_RGB, IL_UNSIGNED_BYTE, NULL))
		return IL_FALSE;
	image->Origin = IL_ORIGIN_UPPER_LEFT;

	// Check for a global colour map.
	if (Header.ColourInfo & (1 << 7)) {
		if (!iGetPalette(Header.ColourInfo, &GlobalPal, NULL)) {
			return IL_FALSE;
		}
	}

	if (!GetImages(image, &GlobalPal, &Header))
		return IL_FALSE;

	if (GlobalPal.Palette && GlobalPal.PalSize)
		ifree(GlobalPal.Palette);
	GlobalPal.Palette = NULL;
	GlobalPal.PalSize = 0;

	return ilFixImage();
}


ILboolean SkipExtensions(GFXCONTROL *Gfx)
{
	ILint	Code;
	ILint	Label;
	ILint	Size;

	do {
		if((Code = iCurImage->io.getc(iCurImage->io.handle)) == IL_EOF)
			return IL_FALSE;

		if (Code != 0x21) {
			iCurImage->io.seek(iCurImage->io.handle, -1, IL_SEEK_CUR);
			return IL_TRUE;
		}

		if((Label = iCurImage->io.getc(iCurImage->io.handle)) == IL_EOF)
			return IL_FALSE;

		switch (Label)
		{
			case 0xF9:
				Gfx->Size = iCurImage->io.getc(iCurImage->io.handle);
				Gfx->Packed = iCurImage->io.getc(iCurImage->io.handle);
				Gfx->Delay = GetLittleUShort(&iCurImage->io);
				Gfx->Transparent = iCurImage->io.getc(iCurImage->io.handle);
				Gfx->Terminator = iCurImage->io.getc(iCurImage->io.handle);
				if (iCurImage->io.eof(iCurImage->io.handle))
					return IL_FALSE;
				Gfx->Used = IL_FALSE;
				break;
			/*case 0xFE:
				break;

			case 0x01:
				break;*/
			default:
				do {
					if((Size = iCurImage->io.getc(iCurImage->io.handle)) == IL_EOF)
						return IL_FALSE;
					iCurImage->io.seek(iCurImage->io.handle, Size, IL_SEEK_CUR);
				} while (!iCurImage->io.eof(iCurImage->io.handle) && Size != 0);
		}

		// @TODO:  Handle this better.
		if (iCurImage->io.eof(iCurImage->io.handle)) {
			ilSetError(IL_FILE_READ_ERROR);
			return IL_FALSE;
		}
	} while (1);

	return IL_TRUE;
}


/*From the GIF spec:

  The rows of an Interlaced images are arranged in the following order:

      Group 1 : Every 8th. row, starting with row 0.              (Pass 1)
      Group 2 : Every 8th. row, starting with row 4.              (Pass 2)
      Group 3 : Every 4th. row, starting with row 2.              (Pass 3)
      Group 4 : Every 2nd. row, starting with row 1.              (Pass 4)
*/

ILboolean RemoveInterlace(ILimage *image)
{
	ILubyte *NewData;
	ILuint	i, j = 0;

	NewData = (ILubyte*)ialloc(image->SizeOfData);
	if (NewData == NULL)
		return IL_FALSE;

	//changed 20041230: images with offsety != 0 were not
	//deinterlaced correctly before...
	for (i = 0; i < image->OffY; i++, j++) {
		memcpy(&NewData[i * image->Bps], &image->Data[j * image->Bps], image->Bps);
	}

	for (i = 0 + image->OffY; i < image->Height; i += 8, j++) {
		memcpy(&NewData[i * image->Bps], &image->Data[j * image->Bps], image->Bps);
	}

	for (i = 4 + image->OffY; i < image->Height; i += 8, j++) {
		memcpy(&NewData[i * image->Bps], &image->Data[j * image->Bps], image->Bps);
	}

	for (i = 2 + image->OffY; i < image->Height; i += 4, j++) {
		memcpy(&NewData[i * image->Bps], &image->Data[j * image->Bps], image->Bps);
	}

	for (i = 1 + image->OffY; i < image->Height; i += 2, j++) {
		memcpy(&NewData[i * image->Bps], &image->Data[j * image->Bps], image->Bps);
	}

	ifree(image->Data);
	image->Data = NewData;

	return IL_TRUE;
}


// Uses the transparent colour index to make an alpha channel.
ILboolean ConvertTransparent(ILimage *Image, ILubyte TransColour)
{
	ILubyte	*Palette;
	ILuint	i, j;
	ILuint newSize = Image->Pal.PalSize / 3 * 4;

	if (Image->Pal.Palette == NULL
	||  Image->Pal.PalSize == 0
	||  Image->Pal.PalType != IL_PAL_RGB24)
	{
		return IL_FALSE;
	}

	Palette = (ILubyte*)ialloc(newSize);
	if (Palette == NULL)
		return IL_FALSE;

	// Copy all colors as opaque
	for (i = 0, j = 0; i < Image->Pal.PalSize; i += 3, j += 4) {
		Palette[j  ] = Image->Pal.Palette[i  ];
		Palette[j+1] = Image->Pal.Palette[i+1];
		Palette[j+2] = Image->Pal.Palette[i+2];
		Palette[j+3] = 0xFF;
	}

	// Store transparent color if TransColour is a legal index
	ILuint TransColourIndex = 4*TransColour+3;
	if (TransColourIndex < newSize)
		Palette[TransColourIndex] = 0x00;

	ifree(Image->Pal.Palette);
	Image->Pal.Palette = Palette;
	Image->Pal.PalSize = newSize;
	Image->Pal.PalType = IL_PAL_RGBA32;

	return IL_TRUE;
}

#endif //IL_NO_GIF
