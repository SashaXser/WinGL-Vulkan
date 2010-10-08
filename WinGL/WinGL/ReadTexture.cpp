// local includes
#include "ReadTexture.h"

// stl includes
#include <fstream>

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
////////////////////////// ReadPNG //////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

bool ReadPNG( const char * pFilename,
              unsigned int & rWidth,
              unsigned int & rHeight,
              unsigned char ** ppTexBuffer )
{
   return false;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
////////////////////////// ReadTGA //////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

bool ReadTGA( const char * pFilename,
              unsigned int & rWidth,
              unsigned int & rHeight,
              unsigned char ** ppTexBuffer )
{
   return false;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
////////////////////////// ReadRGB //////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

// local functions to swap data
template < class T > T SwapData16( const T & rT )
{
   // create a local type
   T t;
   // create a pointer to the reference
   const char * pRefData = (const char *)&rT;
   // create a pointer to the local type
   char * pLocalData = (char *)&t;
   // move the data to the little endian format
   *pLocalData = *(pRefData + 1);
   *(pLocalData + 1) = *pRefData;
   // return the local type
   return t;
}

template < class T > T SwapData32( const T & rT )
{
   // create a local type
   T t;
   // create a pointer to the reference
   const char * pRefData = (const char *)&rT;
   // create a pointer to the local type
   char * pLocalData = (char *)&t;
   // move the data to the little endian format
   *pLocalData       = *(pRefData + 3);
   *(pLocalData + 1) = *(pRefData + 2);
   *(pLocalData + 2) = *(pRefData + 1);
   *(pLocalData + 3) = *pRefData;
   // return the local type
   return t;
}

typedef struct RGBHeader
{
   short          nMagic;
   char           nStorage;
   char           nBpc;
   unsigned short nDimension;
   unsigned short nXSize;
   unsigned short nYSize;
   unsigned short nZSize;
   long           nPixMin;
   long           nPixMax;
   char           nDummy[4];
   char           nImageName[80];
   long           nColorMap;
   char           nDummy2[404];
} RGBHeaderType;

bool ReadUncompressedImage( const RGBHeader & rHeader,
                            std::istream & rInput,
                            unsigned char ** ppTexBuffer )
{ 
   // determine the image size
   const unsigned int nImgSize = rHeader.nXSize * rHeader.nYSize;

   // create a local image format
   unsigned char * pImage = new unsigned char[nImgSize * rHeader.nZSize];
   // read in all the image data
   rInput.read((char *)pImage, nImgSize * rHeader.nZSize);

   // point to the components
   unsigned char * pRed   = rHeader.nZSize >= 1 ? pImage : NULL;
   unsigned char * pGreen = rHeader.nZSize >= 2 ? pImage + nImgSize : NULL;
   unsigned char * pBlue  = rHeader.nZSize >= 3 ? pImage + nImgSize * 2 : NULL;
   unsigned char * pAlpha = rHeader.nZSize >= 4 ? pImage + nImgSize * 3 : NULL;

   // create the image
   *ppTexBuffer = new unsigned char[nImgSize * 4];

   // create a local pointer to the image
   unsigned char * pTempImage = *ppTexBuffer;

   // set the image data from the file
   for (unsigned int i = 0; i < nImgSize; i++)
   {
      // set the image attributes
      *(pTempImage++) = pBlue  ? *pBlue++  : 0xFF;
      *(pTempImage++) = pGreen ? *pGreen++ : 0xFF;
      *(pTempImage++) = pRed   ? *pRed++   : 0xFF;
      *(pTempImage++) = pAlpha ? *pAlpha++ : 0xFF;
   }

   // release the temp image
   delete [] pImage;

   return true;
}

bool ReadCompressedRLEImage( const RGBHeader & rHeader,
                             std::istream & rInput,
                             unsigned char ** ppTexBuffer )
{
   return false;
}

bool ReadRGB( const char * pFilename,
              unsigned int & rWidth,
              unsigned int & rHeight,
              unsigned char ** ppTexBuffer )
{
   // local(s)
   bool bRead = false;

   // create a input file stream
   std::ifstream fInputStream;
   // open the stream
   fInputStream.open(pFilename, std::ios_base::in | std::ios_base::binary);

   // make sure the file is open
   if (fInputStream.is_open())
   {
      // create a local header
      RGBHeader rgbHeader;
      // read the local header
      fInputStream.read((char *)&rgbHeader, sizeof(rgbHeader));
      // need to convert the header to little endian
      rgbHeader.nMagic     = SwapData16(rgbHeader.nMagic);
      rgbHeader.nDimension = SwapData16(rgbHeader.nDimension);
      rgbHeader.nXSize     = SwapData16(rgbHeader.nXSize);
      rgbHeader.nYSize     = SwapData16(rgbHeader.nYSize);
      rgbHeader.nZSize     = SwapData16(rgbHeader.nZSize);
      rgbHeader.nPixMin    = SwapData32(rgbHeader.nPixMin);
      rgbHeader.nPixMax    = SwapData32(rgbHeader.nPixMax);
      rgbHeader.nColorMap  = SwapData32(rgbHeader.nColorMap);
      // dimensions of 3 or greater are valid
      // bpc is only valid if it is a 1
      // color map values of 0x00 are only valid
      if (rgbHeader.nDimension >= 0x03 &&
          rgbHeader.nBpc == 0x01       &&
          rgbHeader.nColorMap == 0x00)
      {
         // determine the type of image format
         if (rgbHeader.nStorage)
         {
            bRead = ReadCompressedRLEImage(rgbHeader, fInputStream, ppTexBuffer);
         }
         else
         {
            bRead = ReadUncompressedImage(rgbHeader, fInputStream, ppTexBuffer);
         }

         if (bRead)
         {
            // save off the texture width and height
            rWidth = rgbHeader.nXSize;
            rHeight = rgbHeader.nYSize;
         }
      }

      // close the input stream
      fInputStream.close();
   }

   return bRead;
}