#ifndef _FRAME_BUFFER_OBJECT_H_
#define _FRAME_BUFFER_OBJECT_H_

// local includes
#include "Texture.h"

// gl includes
#include <GL/glew.h>
#include <GL/GL.h>

// std includes
#include <cstdint>
#include <unordered_map>

class FrameBufferObject
{
public:
   // static function that defines the currently active framebuffer object
   static GLuint GetCurrentFrameBuffer( const GLenum binding );
   static GLuint GetCurrentRenderBuffer( );

   // constructor / destructor
    FrameBufferObject( );
   ~FrameBufferObject( );

   // allow for move operations
   FrameBufferObject( FrameBufferObject && fbo );
   FrameBufferObject & operator = ( FrameBufferObject && fbo );

   // returns the fbo id
   GLuint Handle( ) const { return mFBO; }
   operator GLuint ( ) const { return mFBO; }

   // returns the rbo id
   GLuint RBOHandle( ) const { return mRBO; }

   // generate / delete the framebuffer
   void GenBuffer( const uint32_t width = 256, const uint32_t height = 256 );
   void DeleteBuffer( );

   // bind / unbind the framebuffer
   void Bind( const GLenum target );
   void Unbind( );

   // todo: attachments (textures)
   // todo: attachments depth (textures / rbo)
   // todo: get color attachments
   // todo: get depth attachments (render buffer object should be own)

   // attaches a component to the framebuffer
   void Attach( const GLenum attachment, Texture && texture );
   void Attach( const GLenum attachment,
                const GLenum target,
                const GLenum internal_format,
                const bool generate_mipmaps = false );

   // gets an assigned attachment
   Texture * GetAttachment( const GLenum attachment );
   const Texture * GetAttachment( const GLenum attachment ) const;

   // width and height of framebuffer attachments
   void Resize( const uint32_t width, const uint32_t height );
   uint32_t Width( ) const { return mWidth; }
   uint32_t Height( ) const { return mHeight; }

   // indicates if currently bound
   bool IsBound( const GLenum target ) const;
   bool IsReadingBound( ) const { return mTarget == GL_FRAMEBUFFER || mTarget == GL_READ_FRAMEBUFFER; }
   bool IsWritingBound( ) const { return mTarget == GL_FRAMEBUFFER || mTarget == GL_DRAW_FRAMEBUFFER; }

   // indicates if the fbo is complete
   bool IsComplete( ) const;
   GLenum CompleteStatus( ) const;

private:
   // prohibit certain actions
   FrameBufferObject( const FrameBufferObject & );
   FrameBufferObject & operator = ( const FrameBufferObject & );

   // identifies the framebuffer object
   GLuint   mFBO;

   // identifies a renderbuffer object
   GLuint   mRBO;

   // identifies the size of the fbo
   uint32_t mWidth;
   uint32_t mHeight;

   // indicates bound for reading or writing
   GLenum   mTarget;

   // defines all the colored components
   typedef std::unordered_map< GLenum, Texture > AttachmentContainer;
   AttachmentContainer  mAttachments;

};

#endif // _FRAME_BUFFER_OBJECT_H_
