#ifndef _TEXTURE_H_
#define _TEXTURE_H_

// wgl includes
#include "WglAssert.h"

// gl includes
#include <GL/glew.h>
#include <GL/GL.h>

// defines invalid constants used by the texture class
extern const GLenum INVALID_TEXTURE_TARGET;
extern const GLenum INVALID_INTERNAL_TEXTURE_FORMAT;
extern const GLenum INVALID_TEXTURE_UNIT;

class Texture
{
public:
   // static function that gets the active texture for the given texture unit
   static GLuint GetCurrentTexture( const GLenum target, const GLenum texture_unit = GL_TEXTURE0 );

   // static function that sets the active texture unit to the default
   static void SetActiveTextureUnitToDefault( );

   // constructor / destructor
    Texture( );
    Texture( const GLuint id, const bool own_texture = false );
   ~Texture( );

   // move operations
   Texture( Texture && texture );
   Texture & operator = ( Texture && texture );

   // obtains the texture id
   GLuint GetID( ) const { return mTexID; }
   operator GLuint ( ) const { return mTexID; }

   // obtains the texture type
   GLenum GetType( ) const { WGL_ASSERT(mTexID); return mTexTarget; }

   // obtains the textures internal type
   GLenum GetInternalType( ) const { WGL_ASSERT(mTexID); return mTexIFormat; }

   // obtains the width and height
   GLuint GetWidth( ) const { WGL_ASSERT(mTexID); return mTexWidth; }
   GLuint GetHeight( ) const { WGL_ASSERT(mTexID); return mTexHeight; }

   // obtains the sampler id associated to the assigned texture unit id
   // this value is used by a sampler to indicate in the shader which texture to use
   GLuint GetBoundSamplerID( ) const { WGL_ASSERT(mTexID); return mBoundTexUnit - GL_TEXTURE0; }

   // obtains the currently bound texture unit opengl id for this texture
   GLuint GetBoundTexUnitID( ) const { WGL_ASSERT(mTexID); return mBoundTexUnit; }

   // determines if the texture is bound
   bool IsBound( ) const { return mBoundTexUnit != INVALID_TEXTURE_UNIT; }

   // obtains the mipmap status of the texture
   bool IsMipMapped( ) const { WGL_ASSERT(mTexID); return mTexIsMipMapped; }

   // obtains the immutable status of the texture
   bool IsImmutable( ) const { WGL_ASSERT(mTexID); return mTexIsImmutable; }

   // generates a texture of a specific type
   bool GenerateTexture( const GLenum target, const GLenum internal_format,
                         const GLuint width, const GLuint height,
                         const GLenum format, const GLenum type,
                         const void * const pData,
                         const bool generate_mipmap = false );
   bool GenerateTextureImmutable( const GLenum target, const GLenum internal_format,
                                  const GLuint width, const GLuint height,
                                  const GLenum format, const GLenum type,
                                  const void * const pData,
                                  const bool generate_mipmap = false );

   // loads a texture
   bool Load2D( const char * const pFilename,
                const GLenum intermediate_format,
                const GLenum internal_format,
                const bool generate_mipmap = false );

   // sets the texture parameters
   template < typename T >
   void SetParameter( const GLenum param_name, const T param_value );

   // bind / unbind operations
   void Bind( const GLenum texture_unit = GL_TEXTURE0 );
   void Unbind( const GLuint texture_id = 0 );

private:
   // prohibit copy operations (can come later)
   Texture( const Texture & );
   Texture & operator = ( const Texture & );

   // releases a texture
   void DeleteTexture( );

   // copies the contents of one texture into the other
   void CopyTexture( const Texture & texture );

   // texture object id
   GLuint      mTexID;

   // defines the type of texture
   GLenum      mTexTarget;

   // defines the width and height of texture
   GLuint      mTexWidth;
   GLuint      mTexHeight;

   // defines the internal format of the texture
   GLenum      mTexIFormat;

   // currently bound texture unit
   GLuint      mBoundTexUnit;

   // determines if mipmapping was requested
   bool        mTexIsMipMapped;

   // determines if immutable was requested
   bool        mTexIsImmutable;

   // determines if the texture is owned by this impl
   bool        mTexIsOwned;

};

#endif // _TEXTURE_H_
