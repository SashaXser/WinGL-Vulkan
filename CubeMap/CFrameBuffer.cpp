// includes
#include "CFrameBuffer.h"

CFrameBuffer::CFrameBuffer( )
{
   if (!m_bInitialized)
   {
      // obtain the extensions
      m_bInitialized = ObtainExtensions();
   }
}

CFrameBuffer::~CFrameBuffer( )
{
}

bool CFrameBuffer::ObtainExtensions( )
{
   glIsFrameBufferExt                       = (PFNGLISFRAMEBUFFEREXTPROC)wglGetProcAddress("glIsFramebufferEXT");
   glGenerateMipMapExt                      = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmapEXT");
   glIsRenderBufferExt                      = (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbufferEXT");
   glBindFrameBufferExt                     = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
   glGenFrameBuffersExt                     = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
   glBindRenderBufferExt                    = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
   glGenRenderBuffersExt                    = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
   glDeleteFrameBuffersExt                  = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
   glDeleteRenderBuffersExt                 = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
   glRenderBufferStorageExt                 = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
   glFrameBufferTexture1DExt                = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
   glFrameBufferTexture2DExt                = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
   glFrameBufferTexture3DExt                = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
   glCheckFrameBufferStatusExt              = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
   glFrameBufferRenderBufferExt             = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
   glGetRenderBufferParameterivExt          = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
   glGetFrameBufferAttachmentParameterivExt = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");

   return glIsFrameBufferExt           && glGenerateMipMapExt &&                   
          glIsRenderBufferExt          && glBindFrameBufferExt &&                  
          glGenFrameBuffersExt         && glBindRenderBufferExt &&                 
          glGenRenderBuffersExt        && glDeleteFrameBuffersExt &&                
          glDeleteRenderBuffersExt     && glRenderBufferStorageExt &&            
          glFrameBufferTexture1DExt    && glFrameBufferTexture2DExt &&        
          glFrameBufferTexture3DExt    && glCheckFrameBufferStatusExt &&    
          glFrameBufferRenderBufferExt &&  glGetRenderBufferParameterivExt &&
          glGetFrameBufferAttachmentParameterivExt;
}

void CFrameBuffer::glBindRenderbufferEXT( unsigned int nTarget,
                                          unsigned int nRenderBuffer )
{
   if (m_bInitialized) glBindRenderBufferExt(nTarget, nRenderBuffer);
}

void CFrameBuffer::glDeleteRenderbuffersEXT( int nSize, 
                                             const unsigned int * pRenderBuffers )
{
   if (m_bInitialized) glDeleteRenderBuffersExt(nSize, pRenderBuffers);
}

void CFrameBuffer::glGenRenderbuffersEXT( int nSize,
                                          unsigned int * pRenderBuffers )
{
   if (m_bInitialized) glGenRenderBuffersExt(nSize, pRenderBuffers);
}

void CFrameBuffer::glRenderbufferStorageEXT( unsigned int nTarget,
                                             unsigned int nInternalFormat,
                                             int nWidth,
                                             int nHeight )
{
   if (m_bInitialized) glRenderBufferStorageExt(nTarget, nInternalFormat, nWidth, nHeight);
}

void CFrameBuffer::glGetRenderbufferParameterivEXT( unsigned int nTarget, 
                                                    unsigned int nName, 
                                                    int * pParams )
{
   if (m_bInitialized) glGetRenderBufferParameterivExt(nTarget, nName, pParams);
}

void CFrameBuffer::glBindFramebufferEXT( unsigned int nTarget,
                                         unsigned int nFrameBuffer )
{
   if (m_bInitialized) glBindFrameBufferExt(nTarget, nFrameBuffer);
}

void CFrameBuffer::glDeleteFramebuffersEXT( int nSize, 
                                            const unsigned int * pFrameBuffers )
{
   if (m_bInitialized) glDeleteFrameBuffersExt(nSize, pFrameBuffers);
}

void CFrameBuffer::glGenFramebuffersEXT( int nSize, 
                                         unsigned int * pFrameBuffers )
{
   if (m_bInitialized) glGenFrameBuffersExt(nSize, pFrameBuffers);
}

void CFrameBuffer::glFramebufferTexture1DEXT( unsigned int nTarget, 
                                              unsigned int nAttachment, 
                                              unsigned int nTexTarget, 
                                              unsigned int nTexture, 
                                              int nLevel )
{
   if (m_bInitialized) glFrameBufferTexture1DExt(nTarget, 
                                                 nAttachment, 
                                                 nTexTarget, 
                                                 nTexture, 
                                                 nLevel);
}

void CFrameBuffer::glFramebufferTexture2DEXT( unsigned int nTarget, 
                                              unsigned int nAttachment, 
                                              unsigned int nTexTarget, 
                                              unsigned int nTexture, 
                                              int nLevel )
{
   if (m_bInitialized) glFrameBufferTexture2DExt(nTarget, 
                                                 nAttachment, 
                                                 nTexTarget, 
                                                 nTexture, 
                                                 nLevel);
}

void CFrameBuffer::glFramebufferTexture3DEXT( unsigned int nTarget, 
                                              unsigned int nAttachment,
                                              unsigned int nTexTarget, 
                                              unsigned int nTexture,
                                              int nLevel, 
                                              int nZOffset )
{
   if (m_bInitialized) glFrameBufferTexture3DExt(nTarget, 
                                                 nAttachment, 
                                                 nTexTarget, 
                                                 nTexture, 
                                                 nLevel, 
                                                 nZOffset);
}

void CFrameBuffer::glFramebufferRenderbufferEXT( unsigned int nTarget, 
                                                 unsigned int nAttachment,
                                                 unsigned int nRenderBufferTarget, 
                                                 unsigned int nRenderBuffer )
{
   if (m_bInitialized) glFrameBufferRenderBufferExt(nTarget, 
                                                    nAttachment, 
                                                    nRenderBufferTarget, 
                                                    nRenderBuffer);
}

void CFrameBuffer::glGetFramebufferAttachmentParameterivEXT( unsigned int nTarget, 
                                                             unsigned int nAttachment,
                                                             unsigned int nRenderBufferTarget, 
                                                             int * pRenderBuffer )
{
   if (m_bInitialized) glGetFrameBufferAttachmentParameterivExt(nTarget, 
                                                                nAttachment, 
                                                                nRenderBufferTarget, 
                                                                pRenderBuffer);
}

void CFrameBuffer::glGenerateMipmapEXT( unsigned int nTarget )
{
   glGenerateMipMapExt(nTarget);
}

unsigned int  CFrameBuffer::glCheckFramebufferStatusEXT( unsigned int nTarget )
{
   unsigned int nResult = 0;

   if (m_bInitialized) nResult = glCheckFrameBufferStatusExt(nTarget);

   return nResult;
}

unsigned char CFrameBuffer::glIsRenderbufferEXT( unsigned int nRenderBuffer )
{
   GLboolean nResult = 0;

   if (m_bInitialized) nResult = glIsRenderBufferExt(nRenderBuffer);

   return nResult;
}

unsigned char CFrameBuffer::glIsFramebufferEXT( unsigned int nFrameBuffer )
{
   GLboolean nResult = 0;

   if (m_bInitialized) nResult = glIsFrameBufferExt(nFrameBuffer);

   return nResult;
}

// initialize static member functions
PFNGLISFRAMEBUFFEREXTPROC       CFrameBuffer::glIsFrameBufferExt      = 0;
PFNGLGENERATEMIPMAPEXTPROC      CFrameBuffer::glGenerateMipMapExt     = 0;
PFNGLISRENDERBUFFEREXTPROC      CFrameBuffer::glIsRenderBufferExt     = 0;
PFNGLBINDFRAMEBUFFEREXTPROC     CFrameBuffer::glBindFrameBufferExt    = 0;
PFNGLGENFRAMEBUFFERSEXTPROC     CFrameBuffer::glGenFrameBuffersExt    = 0;
PFNGLBINDRENDERBUFFEREXTPROC    CFrameBuffer::glBindRenderBufferExt   = 0;
PFNGLGENRENDERBUFFERSEXTPROC    CFrameBuffer::glGenRenderBuffersExt   = 0;
PFNGLDELETEFRAMEBUFFERSEXTPROC  CFrameBuffer::glDeleteFrameBuffersExt = 0;

PFNGLDELETERENDERBUFFERSEXTPROC       CFrameBuffer::glDeleteRenderBuffersExt     = 0;
PFNGLRENDERBUFFERSTORAGEEXTPROC       CFrameBuffer::glRenderBufferStorageExt     = 0;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC      CFrameBuffer::glFrameBufferTexture1DExt    = 0;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC      CFrameBuffer::glFrameBufferTexture2DExt    = 0;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC      CFrameBuffer::glFrameBufferTexture3DExt    = 0;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC    CFrameBuffer::glCheckFrameBufferStatusExt  = 0;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC   CFrameBuffer::glFrameBufferRenderBufferExt = 0;

PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC            CFrameBuffer::glGetRenderBufferParameterivExt = 0;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC   CFrameBuffer::glGetFrameBufferAttachmentParameterivExt = 0;

// initialize static member variables
bool CFrameBuffer::m_bInitialized = false;