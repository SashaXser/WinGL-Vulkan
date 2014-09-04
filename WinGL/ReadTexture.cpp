// local includes
#include "ReadTexture.h"
#include "WglAssert.h"

// std includes
#include <string>
#include <cstring>
#include <cstdlib>

// resil includes
#include <il/il.h>

namespace details
{

// called to shutdown resil
void ShutdownResIL( )
{
   ilShutDown();
}

// use the crt initialization stage to init resil
bool InitResIL( )
{
   // install an exit handler for resil
   std::atexit(&ShutdownResIL);

   // init the library
   ilInit();

   // enable the lower left for all files
   ilEnable(IL_ORIGIN_SET);
   ilSetInteger(IL_ORIGIN_MODE, IL_ORIGIN_LOWER_LEFT);

   // make sure there are no errors
   return ilGetError() == IL_NO_ERROR;
}

// indicates if resil was initialized
static const bool resil_inited = InitResIL();

} // namespace details

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
////////////////////////// ReadRGB //////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

bool ReadRGB( const char * const pFilename,
              uint32_t & width,
              uint32_t & height,
              std::shared_ptr< uint8_t > & pTexBuffer )
{
   return ReadTexture(pFilename, width, height, pTexBuffer);
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//////////////////////// ReadTexture ////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

bool ReadTexture( const char * const pFilename,
                  uint32_t & width,
                  uint32_t & height,
                  std::shared_ptr< uint8_t > & pTexBuffer )
{
   return ReadTexture(pFilename, GL_RGBA, width, height, pTexBuffer);
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////// ReadTexture Template ////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

template < typename T > struct il_type;
template < > struct il_type< uint8_t > { static const ILenum type = IL_UNSIGNED_BYTE; };
template < > struct il_type< uint16_t > { static const ILenum type = IL_UNSIGNED_SHORT; };
template < > struct il_type< uint32_t > { static const ILenum type = IL_UNSIGNED_INT; };
template < > struct il_type< float > { static const ILenum type = IL_FLOAT; };

template < typename T >
bool ReadTexture( const char * const pFilename,
                  const GLenum format,
                  uint32_t & width,
                  uint32_t & height,
                  std::shared_ptr< T > & pTexBuffer )
{
   // image library should be good to go
   WGL_ASSERT(details::resil_inited);

   bool read = false;

   // is the format supported???
   const ILenum il_format = [ &format ] ( ) -> ILenum
   {
      ILenum il_format = IL_FORMAT_NOT_SUPPORTED;

      switch (format)
      {
      case GL_RGB:   il_format = IL_RGB;  break;
      case GL_RGBA:  il_format = IL_RGBA; break;
      case GL_BGR:   il_format = IL_BGR;  break;
      case GL_BGRA:  il_format = IL_BGRA; break;
      default: il_format = IL_FORMAT_NOT_SUPPORTED; break;
      }

      return il_format;
   }();

   if (il_format != IL_FORMAT_NOT_SUPPORTED)
   {
      // create an image handle for reading
      const ILuint image_handle = ilGenImage();

      // bind the image for processing
      ilBindImage(image_handle);

      // try to load the image
      if (const ILboolean image_loaded =
          ilLoadImage(std::wstring(pFilename, pFilename + std::strlen(pFilename)).c_str()))
      {
         // get the width and height of the image
         width = static_cast< uint32_t >(ilGetInteger(IL_IMAGE_WIDTH));
         height = static_cast< uint32_t >(ilGetInteger(IL_IMAGE_HEIGHT));

         // determine the number of bytes per pixel
         const uint32_t Bpp = [ &format ] ( ) -> uint32_t
         {
            uint32_t Bpp = 0;

            switch (format)
            {
            case GL_RGB:   Bpp = 3; break;
            case GL_RGBA:  Bpp = 4; break;
            case GL_BGR:   Bpp = 3; break;
            case GL_BGRA:  Bpp = 4; break;
            default:       Bpp = 0; break;
            }

            return Bpp;
         }();

         if (Bpp)
         {
            // create buffer large enough to hold texture
            pTexBuffer.reset(new T[width * height * Bpp]);

            // copy the data into the buffer
            ilCopyPixels(0, 0, 0, width, height, 1, il_format, il_type< T >::type, pTexBuffer.get());

            // image has been read
            read = true;
         }
      }

      // done with the image, so release it
      ilDeleteImage(image_handle);
   }

   return read;
}

template bool ReadTexture< uint8_t >( const char * const, const GLenum, uint32_t &, uint32_t &, std::shared_ptr< uint8_t > & );
template bool ReadTexture< uint16_t >( const char * const, const GLenum, uint32_t &, uint32_t &, std::shared_ptr< uint16_t > & );
template bool ReadTexture< uint32_t >( const char * const, const GLenum, uint32_t &, uint32_t &, std::shared_ptr< uint32_t > & );
template bool ReadTexture< float >( const char * const, const GLenum, uint32_t &, uint32_t &, std::shared_ptr< float > & );
