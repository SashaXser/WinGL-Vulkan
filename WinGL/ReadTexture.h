#ifndef _READ_TEXTURE_H_
#define _READ_TEXTURE_H_

// platform includes
#include "Window.h"

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// std includes
#include <memory>
#include <cstdint>

// reads a rgb / rgba file
// assumes RGBA and UNSIGNED BYTE
bool ReadRGB( const char * const pFilename,
              uint32_t & width,
              uint32_t & height,
              std::shared_ptr< uint8_t > & pTexBuffer );

// reads a texture file
// assumes RGBA and UNSIGNED BYTE
bool ReadTexture( const char * const pFilename,
                  uint32_t & width,
                  uint32_t & height,
                  std::shared_ptr< uint8_t > & pTexBuffer );

// reads a texture file
template < typename T >
bool ReadTexture( const char * const pFilename,
                  const GLenum format,
                  uint32_t & width,
                  uint32_t & height,
                  std::shared_ptr< T > & pTexBuffer );

template < typename T >
struct TextureData
{
   uint32_t width;
   uint32_t height;
   GLenum format;
   GLenum type;
   std::shared_ptr< T > pTexture;
};

template < typename T > struct TextureType;
template < > struct TextureType< uint8_t > { static const GLenum type = GL_UNSIGNED_BYTE; };
template < > struct TextureType< uint16_t > { static const GLenum type = GL_UNSIGNED_SHORT; };

// reads a texture file
template < typename T >
inline TextureData< T > ReadTexture( const char * const pFilename,
                                     const GLenum format )
{
   TextureData< T > texture = { 0, 0, format, TextureType< T >::type, nullptr };

   ReadTexture(pFilename, format, texture.width, texture.height, texture.pTexture);

   return texture;
}

#endif // _READ_TEXTURE_H_
