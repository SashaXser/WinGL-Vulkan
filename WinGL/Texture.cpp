// local includes
#include "Texture.h"

// wgl includes
#include "ReadTexture.h"

// gl includes
#include <GL/wglew.h>

// std includes
#include <cmath>
#include <cstdint>
#include <utility>
#include <algorithm>

// defines an invalid texture type
const GLenum INVALID_TEXTURE_TARGET = ~0ul;
const GLenum INVALID_INTERNAL_TEXTURE_FORMAT = ~0ul;
const GLenum INVALID_TEXTURE_UNIT = ~0ul;

GLenum GetTextureTarget( const GLuint id )
{
   GLenum target = INVALID_TEXTURE_TARGET;

   const GLenum targets[] =
   {
      GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D,
      GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D_ARRAY,
      GL_TEXTURE_RECTANGLE,
      GL_TEXTURE_CUBE_MAP,
      // GL_TEXTURE_CUBE_MAP_ARRAY, // need to determine this one
      GL_TEXTURE_BUFFER,
      GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_2D_MULTISAMPLE_ARRAY
   };

   // clear out any errors
   WGL_ASSERT(glGetError() == GL_NONE);

   for (const auto t : targets)
   {
      // bind the texture to the target
      glBindTexture(t, id);

      // if there is an error, then do not use 
      if (glGetError() == GL_NONE)
      {
         target = t; break;
      }
   }

   WGL_ASSERT(target != INVALID_TEXTURE_TARGET);

   return target;
}

GLuint Texture::GetCurrentTexture( const GLenum target, const GLenum texture_unit )
{
   // must happen within a valid gl context
   WGL_ASSERT(wglGetCurrentContext());

   // get the current texture unit that is active
   const GLuint active_texture_unit =
   [ ] ( ) -> GLuint
   {
      GLuint active_texture_unit = 0;
      glGetIntegerv(GL_ACTIVE_TEXTURE, reinterpret_cast< GLint * >(&active_texture_unit));

      return active_texture_unit;
   }();

   // obtain the name to look for based on the target
   const GLenum texture_param_name =
   [ target ] ( ) -> GLenum
   {
      GLenum texture_param_name = 0;

      switch (target)
      {
      case GL_TEXTURE_1D: texture_param_name = GL_TEXTURE_BINDING_1D; break;
      case GL_TEXTURE_2D: texture_param_name = GL_TEXTURE_BINDING_2D; break;
      case GL_TEXTURE_3D: texture_param_name = GL_TEXTURE_BINDING_3D; break;
      case GL_TEXTURE_1D_ARRAY: texture_param_name = GL_TEXTURE_BINDING_1D_ARRAY; break;
      case GL_TEXTURE_2D_ARRAY: texture_param_name = GL_TEXTURE_BINDING_2D_ARRAY; break;
      case GL_TEXTURE_RECTANGLE: texture_param_name = GL_TEXTURE_BINDING_RECTANGLE; break;
      case GL_TEXTURE_CUBE_MAP: texture_param_name = GL_TEXTURE_BINDING_CUBE_MAP; break;
      // case GL_TEXTURE_CUBE_MAP_ARRAY: texture_param_name = <need to determine what this one is>; break;
      case GL_TEXTURE_BUFFER: texture_param_name = GL_TEXTURE_BINDING_BUFFER; break;
      case GL_TEXTURE_2D_MULTISAMPLE: texture_param_name = GL_TEXTURE_BINDING_2D_MULTISAMPLE; break;
      case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: texture_param_name = GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY; break;

      // there is an error if they provide the wrong target type
      default: WGL_ASSERT(false); break;
      };

      return texture_param_name;
   }();

   // set the active texture to look at
   glActiveTexture(texture_unit);

   // get the texture value
   const GLuint texture =
   [ texture_param_name ] ( ) -> GLuint
   {
      GLuint texture = 0;
      glGetIntegerv(texture_param_name, reinterpret_cast< GLint * >(&texture));

      return texture;
   }();

   // restore the active texture unit
   glActiveTexture(active_texture_unit);

   return texture;
}

void Texture::SetActiveTextureUnitToDefault( )
{
   glActiveTexture(GL_TEXTURE0);
}

Texture::Texture( ) :
Texture(0, true)
{
}

Texture::Texture( const GLuint id, const bool own_texture ) :
mTexID            ( id ),
mTexTarget        ( INVALID_TEXTURE_TARGET ),
mTexWidth         ( 0 ),
mTexHeight        ( 0 ),
mTexIFormat       ( INVALID_INTERNAL_TEXTURE_FORMAT ),
mBoundTexUnit     ( INVALID_TEXTURE_UNIT ),
mTexIsMipMapped   ( false ),
mTexIsImmutable   ( false ),
mTexIsOwned       ( own_texture )
{
   if (id)
   {
      // must happen within a valid gl context
      WGL_ASSERT(wglGetCurrentContext());

      if ((mTexTarget = GetTextureTarget(id)) == INVALID_TEXTURE_TARGET)
      {
         mTexID = 0;
         mTexIsOwned = false;
      }
      else
      {
         const GLuint bound_tex = Texture::GetCurrentTexture(mTexTarget, GL_TEXTURE0);

         glActiveTexture(GL_TEXTURE0);
         glBindTexture(mTexTarget, id);

         glGetTexLevelParameteriv(mTexTarget, 0, GL_TEXTURE_WIDTH, reinterpret_cast< GLint * >(&mTexWidth));
         glGetTexLevelParameteriv(mTexTarget, 0, GL_TEXTURE_HEIGHT, reinterpret_cast< GLint * >(&mTexHeight));
         glGetTexLevelParameteriv(mTexTarget, 0, GL_TEXTURE_INTERNAL_FORMAT, reinterpret_cast< GLint * >(&mTexIFormat));
         // no bound texture unit, but should possibly consider for the future

         GLint min_filter;
         glGetTexParameteriv(mTexTarget, GL_TEXTURE_MIN_FILTER, &min_filter);
         mTexIsMipMapped = min_filter != GL_NEAREST && min_filter != GL_LINEAR;

         GLint immutable;
         glGetTexParameteriv(mTexTarget, GL_TEXTURE_IMMUTABLE_FORMAT, &immutable);
         mTexIsImmutable = immutable == GL_TRUE;

         glBindTexture(mTexTarget, bound_tex);
      }
   }
}

Texture::~Texture( )
{
   if (mTexID && mTexIsOwned)
   {
      // must happen within a valid gl context
      WGL_ASSERT(wglGetCurrentContext());

      // there is a valid texture here so delete it
      glDeleteTextures(1, &mTexID);
   }
}

Texture::Texture( Texture && texture )
{
   std::swap(mTexID, texture.mTexID);
   std::swap(mTexTarget, texture.mTexTarget);
   std::swap(mTexWidth, texture.mTexWidth);
   std::swap(mTexHeight, texture.mTexHeight);
   std::swap(mTexIFormat, texture.mTexIFormat);
   std::swap(mBoundTexUnit, texture.mBoundTexUnit);
   std::swap(mTexIsMipMapped, texture.mTexIsMipMapped);
   std::swap(mTexIsImmutable, texture.mTexIsImmutable);
   std::swap(mTexIsOwned, texture.mTexIsOwned);
}

Texture & Texture::operator = ( Texture && texture )
{
   if (&texture != this)
   {
      std::swap(mTexID, texture.mTexID);
      std::swap(mTexTarget, texture.mTexTarget);
      std::swap(mTexWidth, texture.mTexWidth);
      std::swap(mTexHeight, texture.mTexHeight);
      std::swap(mTexIFormat, texture.mTexIFormat);
      std::swap(mBoundTexUnit, texture.mBoundTexUnit);
      std::swap(mTexIsMipMapped, texture.mTexIsMipMapped);
      std::swap(mTexIsImmutable, texture.mTexIsImmutable);
      std::swap(mTexIsOwned, texture.mTexIsOwned);
   }

   return *this;
}

bool Texture::GenerateTexture( const GLenum target, const GLenum internal_format,
                               const GLuint width, const GLuint height,
                               const GLenum format, const GLenum type,
                               const void * const pData,
                               const bool generate_mipmap )
{
   // must happen within a valid gl context
   WGL_ASSERT(wglGetCurrentContext());

   bool texture_created = false;

   // release the current texture if there is one
   DeleteTexture();

   // generate a new texture
   glGenTextures(1, &mTexID);

   if (mTexID)
   {
      // setup some attributes
      mTexTarget = target;
      mTexWidth = width;
      mTexHeight = height;
      mTexIFormat = internal_format;

      // get the current active texture unit 0
      const GLuint current_active_tex_unit_0 = Texture::GetCurrentTexture(target);

      // bind this texture to texture unit 0
      Bind(GL_TEXTURE0);

      // setup the base texture
      switch (target)
      {
      case GL_TEXTURE_2D: glTexImage2D(target, 0, internal_format, width, height, 0, format, type, pData); break;

      // figure out how to handle other cases later
      default: WGL_ASSERT(false); break;
      };

      // the call to a glTexImage2D makes the texture object mutable
      mTexIsImmutable = false;

      // if generate mipmap is turned on, then create it
      if(generate_mipmap)
      {
         // allow the implementation to generate the mipmaps
         glGenerateMipmap(target);

         // setup basic attributes
         SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

         mTexIsMipMapped = true;
      }
      else
      {
         // setup basic attributes
         SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

         mTexIsMipMapped = false;
      }

      // unbind the texture and restore the previous texture unit
      Unbind(current_active_tex_unit_0);

      // texture was loaded, assuming no errors
      texture_created = true;
   }

   return texture_created;
}

bool Texture::GenerateTextureImmutable( const GLenum target, const GLenum internal_format,
                                        const GLuint width, const GLuint height,
                                        const GLenum format, const GLenum type,
                                        const void * const pData,
                                        const bool generate_mipmap )
{
   // must happen within a valid gl context
   WGL_ASSERT(wglGetCurrentContext());

   bool texture_created = false;

   // release the current texture if there is one
   DeleteTexture();

   // generate a new texture
   glGenTextures(1, &mTexID);

   if (mTexID)
   {
      // setup some attributes
      mTexTarget = target;
      mTexWidth = width;
      mTexHeight = height;
      mTexIFormat = internal_format;

      // get the current active texture unit 0
      const GLuint current_active_tex_unit_0 = Texture::GetCurrentTexture(target);

      // bind this texture to texture unit 0
      Bind(GL_TEXTURE0);

      // setup the base texture
      switch (target)
      {
      case GL_TEXTURE_2D:
      {
         const GLsizei num_mip_levels = generate_mipmap ? static_cast< GLsizei >(std::floor(std::log2(std::max(width, height))) + 1.0) : 1;
         glTexStorage2D(target, num_mip_levels, internal_format, width, height);
         
         if (pData) glTexSubImage2D(target, 0, 0, 0, width, height, format, type, pData);
         
         break;
      }

      // figure out how to handle other cases later
      default: WGL_ASSERT(false); break;
      };

      // the call to a glTexStorage makes the texture object immutable
      mTexIsImmutable = true;

      // if generate mipmap is turned on, then create it
      if(generate_mipmap)
      {
         // allow the implementation to generate the mipmaps
         glGenerateMipmap(target);

         // setup basic attributes
         SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

         mTexIsMipMapped = true;
      }
      else
      {
         // setup basic attributes
         SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

         mTexIsMipMapped = false;
      }

      // unbind the texture and restore the previous texture unit
      Unbind(current_active_tex_unit_0);

      // texture was loaded, assuming no errors
      texture_created = true;
   }

   return texture_created;
}

bool Texture::Load2D( const char * const pFilename,
                      const GLenum intermediate_format,
                      const GLenum internal_format,
                      const bool generate_mipmap )
{
   // must happen within a valid gl context
   WGL_ASSERT(wglGetCurrentContext());

   bool texture_loaded = false;

   // try to read the texture
   const TextureData< uint8_t > texture_data =
      ReadTexture< uint8_t >(pFilename, intermediate_format);

   if (texture_data.pTexture)
   {
      // generate an immutable texture
      GenerateTextureImmutable(GL_TEXTURE_2D, internal_format,
                               texture_data.width, texture_data.height,
                               texture_data.format, texture_data.type,
                               texture_data.pTexture.get(),
                               generate_mipmap);

      // texture was loaded, assuming no errors
      texture_loaded = true;
   }

   return texture_loaded;
}

// helper structures to choose the correct function
// this version of the compiler still does not support inline templates to a function
#define DEFINE_TEX_PARAM_FUNC_SELECTOR( type, func ) \
   template < > struct texture_parameter_function_selector< type > \
   { static const decltype( &func ) glTexParameter; }; \
   const decltype( &func ) texture_parameter_function_selector< type >::glTexParameter = &func;

template < typename T > struct texture_parameter_function_selector;

DEFINE_TEX_PARAM_FUNC_SELECTOR(GLint, glTexParameteri);
DEFINE_TEX_PARAM_FUNC_SELECTOR(GLint *, glTexParameteriv);
DEFINE_TEX_PARAM_FUNC_SELECTOR(const GLint *, glTexParameteriv);
DEFINE_TEX_PARAM_FUNC_SELECTOR(GLfloat, glTexParameterf);
DEFINE_TEX_PARAM_FUNC_SELECTOR(GLfloat *, glTexParameterfv);
DEFINE_TEX_PARAM_FUNC_SELECTOR(const GLfloat *, glTexParameterfv);

template < typename T >
void Texture::SetParameter( const GLenum param_name, const T param_value )
{
   // validate some assumptions
   WGL_ASSERT(mTexID);
   WGL_ASSERT(IsBound());
   WGL_ASSERT(wglGetCurrentContext());
   WGL_ASSERT(Texture::GetCurrentTexture(mTexTarget, mBoundTexUnit) == mTexID);

   // this is the active texture if passed all the asserts
   // so, select the correct function to call on this texture
   texture_parameter_function_selector< T >::glTexParameter(mTexTarget, param_name, param_value);
}

// setup the valid functions for the texture parameters
template void Texture::SetParameter< >( const GLenum, const GLint );
template void Texture::SetParameter< >( const GLenum, GLint * const );
template void Texture::SetParameter< >( const GLenum, const GLint * const );
template void Texture::SetParameter< >( const GLenum, const GLfloat );
template void Texture::SetParameter< >( const GLenum, GLfloat * const );
template void Texture::SetParameter< >( const GLenum, const GLfloat * const );

void Texture::Bind( const GLenum texture_unit )
{
   // must happen within a valid gl context
   WGL_ASSERT(wglGetCurrentContext());

   // bind the texture to the specified texture unit
   glActiveTexture(texture_unit);
   glBindTexture(mTexTarget, mTexID);

   // save the texture unit for later
   mBoundTexUnit = texture_unit;
}

void Texture::Unbind( const GLuint texture_id )
{
   // must happen within a valid gl context
   WGL_ASSERT(wglGetCurrentContext());

   // bind the texture to the specified texture unit
   glActiveTexture(mBoundTexUnit);
   glBindTexture(mTexTarget, texture_id);

   // no longer have a bound texture unit
   mBoundTexUnit = INVALID_TEXTURE_UNIT;
}

void Texture::DeleteTexture( )
{
   // must happen within a valid gl context
   WGL_ASSERT(wglGetCurrentContext());

   // rvalue transfer this texture into the temp
   *this = Texture();
}
