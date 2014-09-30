#ifndef _C_FRAME_BUFFER_H_
#define _C_FRAME_BUFFER_H_

#include "StdIncludes.h"

class CFrameBuffer
{
public:
   // constructor / destructor
               CFrameBuffer( );
              ~CFrameBuffer( );
   
   // render buffer access functions
   void glBindRenderbufferEXT( unsigned int nTarget, unsigned int nRenderBuffer );
   void glDeleteRenderbuffersEXT( int nSize, const unsigned int * pRenderBuffers );
   void glGenRenderbuffersEXT( int nSize, unsigned int * pRenderBuffers );
   void glRenderbufferStorageEXT( unsigned int nTarget, unsigned int nInternalFormat, int nWidth, int nHeight );
   void glGetRenderbufferParameterivEXT( unsigned int nTarget, unsigned int nName, int * pParams );
   void glBindFramebufferEXT( unsigned int nTarget, unsigned int nFrameBuffer );
   void glDeleteFramebuffersEXT( int nSize, const unsigned int * pFrameBuffers );
   void glGenFramebuffersEXT( int nSize, unsigned int * pFrameBuffers );
   void glFramebufferTexture1DEXT( unsigned int nTarget, unsigned int nAttachment, 
                                   unsigned int nTexTarget, unsigned int nTexture, int nLevel );
   void glFramebufferTexture2DEXT( unsigned int nTarget, unsigned int nAttachment, 
                                   unsigned int nTexTarget, unsigned int nTexture, int nLevel );
   void glFramebufferTexture3DEXT( unsigned int nTarget, unsigned int nAttachment,
                                   unsigned int nTexTarget, unsigned int nTexture,
                                   int nLevel, int nZOffset );
   void glFramebufferRenderbufferEXT( unsigned int nTarget, unsigned int nAttachment,
                                      unsigned int nRenderBufferTarget, unsigned int nRenderBuffer );
   void glGetFramebufferAttachmentParameterivEXT( unsigned int nTarget, unsigned int nAttachment,
                                                  unsigned int nRenderBufferTarget, int * pRenderBuffer );
   void glGenerateMipmapEXT( unsigned int nTarget );

   unsigned int  glCheckFramebufferStatusEXT( unsigned int nTarget );
   unsigned char glIsRenderbufferEXT( unsigned int nRenderBuffer );
   unsigned char glIsFramebufferEXT( unsigned int nFrameBuffer );

private:
   // prohibit explicit copying of this class
                     CFrameBuffer( const CFrameBuffer & rCFrameBuffer );
   CFrameBuffer &    operator = ( const CFrameBuffer & rCFrameBuffer );

   // initializes the framebuffer extensions
   bool              ObtainExtensions( );

   // private static member functions
   static PFNGLISFRAMEBUFFEREXTPROC       glIsFrameBufferExt;
   static PFNGLGENERATEMIPMAPEXTPROC      glGenerateMipMapExt;
   static PFNGLISRENDERBUFFEREXTPROC      glIsRenderBufferExt;
   static PFNGLBINDFRAMEBUFFEREXTPROC     glBindFrameBufferExt;
   static PFNGLGENFRAMEBUFFERSEXTPROC     glGenFrameBuffersExt;
   static PFNGLBINDRENDERBUFFEREXTPROC    glBindRenderBufferExt;
   static PFNGLGENRENDERBUFFERSEXTPROC    glGenRenderBuffersExt;
   static PFNGLDELETEFRAMEBUFFERSEXTPROC  glDeleteFrameBuffersExt;

   static PFNGLDELETERENDERBUFFERSEXTPROC       glDeleteRenderBuffersExt;
   static PFNGLRENDERBUFFERSTORAGEEXTPROC       glRenderBufferStorageExt;
   static PFNGLFRAMEBUFFERTEXTURE1DEXTPROC      glFrameBufferTexture1DExt;
   static PFNGLFRAMEBUFFERTEXTURE2DEXTPROC      glFrameBufferTexture2DExt;
   static PFNGLFRAMEBUFFERTEXTURE3DEXTPROC      glFrameBufferTexture3DExt;
   static PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC    glCheckFrameBufferStatusExt;
   static PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC   glFrameBufferRenderBufferExt;

   static PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC            glGetRenderBufferParameterivExt;
   static PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC   glGetFrameBufferAttachmentParameterivExt;

   // private static member variables
   static bool    m_bInitialized;

};

#endif // _C_FRAME_BUFFER_H_