// local includes
#include "FrameBufferObject.h"
#include "WglAssert.h"

// std includes
#include <memory>
#include <utility>
#include <algorithm>

// platform includes
#include <windows.h>

GLuint FrameBufferObject::GetCurrentFrameBuffer( const GLenum binding )
{
   GLint frame_buffer = 0;

   switch (binding)
   {
   case GL_DRAW_FRAMEBUFFER: glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &frame_buffer); break;
   case GL_READ_FRAMEBUFFER: glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &frame_buffer); break;
   case GL_FRAMEBUFFER:
      // get the draw buffer binding here...
      glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &frame_buffer);
      // the read buffer binding should be the same as teh draw buffer binding...
      WGL_ASSERT(
         FrameBufferObject::GetCurrentFrameBuffer(GL_READ_FRAMEBUFFER) ==
         static_cast< GLuint >(frame_buffer));
      
      break;
   default: WGL_ASSERT(false); break;
   }

   return static_cast< GLuint >(frame_buffer);
}

GLuint FrameBufferObject::GetCurrentRenderBuffer( )
{
   GLint render_buffer = 0;

   glGetIntegerv(GL_RENDERBUFFER_BINDING, &render_buffer);

   return static_cast< GLuint >(render_buffer);
}

GLuint FrameBufferObject::GetMaxNumberOfColorAttachments( )
{
   static GLuint maximum_color_attachments =
   [ ] ( ) -> GLuint
   {
      GLint maximum = 0;
      glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maximum);

      return static_cast< GLuint >(maximum);
   }();

   return maximum_color_attachments;
}

FrameBufferObject::FrameBufferObject( ) :
mFBO        ( 0 ),
mRBO        ( 0 ),
mWidth      ( 0 ),
mHeight     ( 0 ),
mTarget     ( 0 )
{
}

FrameBufferObject::~FrameBufferObject( )
{
   // there should still be a valid context
   WGL_ASSERT(wglGetCurrentContext());

   // the framebuffer should not be currently set for reading / writing
   WGL_ASSERT(mFBO ? !IsReadingBound() && !IsWritingBound() : true);
   WGL_ASSERT(mFBO ? mFBO != FrameBufferObject::GetCurrentFrameBuffer(GL_READ_FRAMEBUFFER) : true);
   WGL_ASSERT(mFBO ? mFBO != FrameBufferObject::GetCurrentFrameBuffer(GL_DRAW_FRAMEBUFFER) : true);
   WGL_ASSERT(mFBO ? mFBO != FrameBufferObject::GetCurrentFrameBuffer(GL_FRAMEBUFFER) : true);
   WGL_ASSERT(mRBO ? mRBO != FrameBufferObject::GetCurrentRenderBuffer() : true);

   // release any resources associated to the buffer
   if (mFBO) glDeleteFramebuffers(1, &mFBO);
   if (mRBO) glDeleteRenderbuffers(1, &mRBO);
}

FrameBufferObject::FrameBufferObject( FrameBufferObject && fbo )
{
   // swap all the attributes between the two
   std::swap(mFBO, fbo.mFBO);
   std::swap(mRBO, fbo.mRBO);
   std::swap(mWidth, fbo.mWidth);
   std::swap(mHeight, fbo.mHeight);
   std::swap(mTarget, fbo.mTarget);
   std::swap(mAttachments, fbo.mAttachments);

   // todo: fill in the rest of the stuff here...
}

FrameBufferObject & FrameBufferObject::operator = ( FrameBufferObject && fbo )
{
   // swap all the attributes between the two
   std::swap(mFBO, fbo.mFBO);
   std::swap(mRBO, fbo.mRBO);
   std::swap(mWidth, fbo.mWidth);
   std::swap(mHeight, fbo.mHeight);
   std::swap(mTarget, fbo.mTarget);
   std::swap(mAttachments, fbo.mAttachments);

   // todo: fill in the rest of the stuff here...

   return *this;
}

void FrameBufferObject::GenBuffer( const uint32_t width, const uint32_t height )
{
   // must delete before creating again
   WGL_ASSERT(!mFBO);

   // save the initial width and height
   mWidth = width;
   mHeight = height;

   glGenFramebuffers(1, &mFBO);
}

void FrameBufferObject::DeleteBuffer( )
{
   // release the current set of data for this object
   *this = FrameBufferObject();
}

void FrameBufferObject::Bind( const GLenum target )
{
   // must be created and not currently bound
   WGL_ASSERT(mFBO && !IsBound(target));

   // bind for full reading and writing
   glBindFramebuffer(target, mFBO);

   // update the type of target
   mTarget = target;
}

void FrameBufferObject::Unbind( )
{
   // must be created and currently bound
   WGL_ASSERT(mFBO && (IsBound(mTarget)));

   // reset the framebuffer back to the default
   glBindFramebuffer(mTarget, 0);

   // no longer a framebuffer target
   mTarget = 0;
}

void FrameBufferObject::Attach( const GLenum attachment, Texture && texture )
{
   // must be created and bound...
   WGL_ASSERT(mFBO && IsBound(mTarget));

   // make sure the attachement is the same size as the width and height
   if (mWidth != texture.GetWidth() || mHeight != texture.GetHeight())
   {
      // come back to this issues...
      WGL_ASSERT(false);
   }

   // attach to the framebuffer the texture
   glFramebufferTexture(mTarget, attachment, texture, 0);

   // replace the existing texture with the new one
   // move it into place as the ownership is changing
   mAttachments[attachment] = std::move(texture);
}

void FrameBufferObject::Attach( const GLenum attachment,
                                const GLenum target,
                                const GLenum internal_format,
                                const bool generate_mipmaps )
{
   // create a temporary texture
   Texture texture;
   texture.GenerateTextureImmutable(target, internal_format,
                                    mWidth, mHeight,
                                    0, 0, nullptr,
                                    generate_mipmaps);

   // attach the texture
   Attach(attachment, std::move(texture));
}

Texture * FrameBufferObject::GetAttachment( const GLenum attachment )
{
   const auto it = mAttachments.find(attachment);

   return it != mAttachments.cend() ? &(it->second) : nullptr;
}

const Texture * FrameBufferObject::GetAttachment( const GLenum attachment ) const
{
   return const_cast< FrameBufferObject * >(this)->GetAttachment(attachment);
}

void FrameBufferObject::Resize( const uint32_t width, const uint32_t height )
{
   // come back and do this
   WGL_ASSERT(!mRBO);

   // save the new width and height
   mWidth = width;
   mHeight = height;

   // resize all the attachments
   std::for_each(mAttachments.cbegin(), mAttachments.cend(),
   [ this ] ( const AttachmentContainer::value_type & attachment )
   {
      Attach(attachment.first,
             attachment.second.GetType(),
             attachment.second.GetInternalType(),
             attachment.second.IsMipMapped());
   });
}

bool FrameBufferObject::IsBound( const GLenum target ) const
{
   bool bound = false;

   switch (target)
   {
   case GL_READ_FRAMEBUFFER: bound = IsReadingBound(); break;
   case GL_DRAW_FRAMEBUFFER: bound = IsWritingBound(); break;
   case GL_FRAMEBUFFER: bound = IsReadingBound() && IsWritingBound(); break;
   default: WGL_ASSERT(false); break;
   }

   return bound;
}

void FrameBufferObject::Blit( const size_t sx0, const size_t sy0, const size_t sx1, const size_t sy1,
                              const size_t dx0, const size_t dy0, const size_t dx1, const size_t dy1,
                              const GLenum buffer, const GLbitfield mask, const GLenum filter) const
{
   // this target must be setup for reading
   WGL_ASSERT(mFBO && IsBound(GL_READ_FRAMEBUFFER));
   WGL_ASSERT(GL_COLOR_ATTACHMENT0 <= buffer && buffer < GL_COLOR_ATTACHMENT0 + FrameBufferObject::GetMaxNumberOfColorAttachments());

   // perform the blit
   glReadBuffer(buffer);
   glBlitFramebuffer(
      static_cast< GLint >(sx0),
      static_cast< GLint >(sy0),
      static_cast< GLint >(sx1),
      static_cast< GLint >(sy1),
      static_cast< GLint >(dx0),
      static_cast< GLint >(dy0),
      static_cast< GLint >(dx1),
      static_cast< GLint >(dy1),
      mask, filter);
}

void FrameBufferObject::Read( const size_t x, const size_t y,
                              const size_t width, const size_t height,
                              const GLenum buffer,
                              const GLenum format, const GLenum type,
                              void * const pData ) const
{
   // this target must be setup for reading
   WGL_ASSERT(mFBO && IsBound(GL_READ_FRAMEBUFFER));
   WGL_ASSERT(GL_COLOR_ATTACHMENT0 <= buffer && buffer < GL_COLOR_ATTACHMENT0 + FrameBufferObject::GetMaxNumberOfColorAttachments());

   // perform the read
   glReadBuffer(buffer);
   glReadPixels(
      static_cast< GLint >(x),
      static_cast< GLint >(y),
      static_cast< GLsizei >(width),
      static_cast< GLsizei >(height),
      format, type, pData);
}

bool FrameBufferObject::IsComplete( ) const
{
   // must be created and bound...
   WGL_ASSERT(mFBO && IsBound(mTarget));

   const GLenum status = CompleteStatus();

   // todo: come back here and determine if a message to the console is needed...
   switch (status)
   {
   case GL_FRAMEBUFFER_UNDEFINED: break;
   case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: break;
   case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: break;
   case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: break;
   case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: break;
   case GL_FRAMEBUFFER_UNSUPPORTED: break;
   case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: break;
   case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: break;
   default: /* GL_FRAMEBUFFER_COMPLETE */ break;
   }

   WGL_ASSERT(status == GL_FRAMEBUFFER_COMPLETE);

   return status == GL_FRAMEBUFFER_COMPLETE;
}

GLenum FrameBufferObject::CompleteStatus( ) const
{
   // must be created and bound...
   WGL_ASSERT(mFBO && IsBound(mTarget));

   return glCheckFramebufferStatus(mTarget);
}
