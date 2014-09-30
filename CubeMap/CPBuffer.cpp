// project includes
#include "CPBuffer.h"

CPBuffer::CPBuffer( )
{
   if (!m_bInitialized)
   {
      InitializeExtensions();
      m_bInitialized = true;
   }
}

CPBuffer::~CPBuffer( )
{
}

bool CPBuffer::InitializeExtensions( )
{
   // initialize the extensions
   _wglCreatePBufferARB =    (PFNWGLCREATEPBUFFERARBPROC)wglGetProcAddress("wglCreatePbufferARB");
   _wglGetPBufferDCARB =     (PFNWGLGETPBUFFERDCARBPROC)wglGetProcAddress("wglGetPbufferDCARB");
   _wglReleasePBufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)wglGetProcAddress("wglReleasePbufferDCARB");
   _wglDestroyPBufferARB =   (PFNWGLDESTROYPBUFFERARBPROC)wglGetProcAddress("wglDestroyPbufferARB");
   _wglQueryPBufferARB =     (PFNWGLQUERYPBUFFERARBPROC)wglGetProcAddress("wglQueryPbufferARB");
   _wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC)wglGetProcAddress("wglMakeContextCurrentARB");
   _wglGetCurrentReadDCARB = (PFNWGLGETCURRENTREADDCARBPROC)wglGetProcAddress("wglGetCurrentReadDCARB");
   _wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
   _wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
   _wglBindTexImageARB = (PFNWGLBINDTEXIMAGEARBPROC)wglGetProcAddress("wglBindTexImageARB");
   _wglReleaseTexImageARB = (PFNWGLRELEASETEXIMAGEARBPROC)wglGetProcAddress("wglReleaseTexImageARB");
   _wglSetPbufferAttribARB = (PFNWGLSETPBUFFERATTRIBARBPROC)wglGetProcAddress("wglSetPbufferAttribARB");

   return _wglCreatePBufferARB     && _wglGetPBufferDCARB        &&
          _wglReleasePBufferDCARB  && _wglDestroyPBufferARB      &&
          _wglQueryPBufferARB      && _wglMakeContextCurrentARB  &&
          _wglGetCurrentReadDCARB  && _wglGetExtensionsStringARB &&
          _wglChoosePixelFormatARB && _wglBindTexImageARB        &&
          _wglReleaseTexImageARB;
}

HPBUFFERARB CPBuffer::wglCreatePbufferARB( HDC hDC, 
                                           int nPixelFormat,
                                           int nWidth,
                                           int nHeight,
                                           const int * pAttrList )
{
   return _wglCreatePBufferARB(hDC,
                               nPixelFormat,
                               nWidth,
                               nHeight,
                               pAttrList);
}

HDC CPBuffer::wglGetPbufferDCARB( HPBUFFERARB hPBuffer )
{
   return _wglGetPBufferDCARB(hPBuffer);
}

int CPBuffer::wglReleasePbufferDCARB( HPBUFFERARB hPBuffer,
                                      HDC hDC )
{
   return _wglReleasePBufferDCARB(hPBuffer, hDC);
}

BOOL CPBuffer::wglDestroyPbufferARB( HPBUFFERARB hPBuffer )
{
   return _wglDestroyPBufferARB(hPBuffer);
}

BOOL CPBuffer::wglQueryPbufferARB( HPBUFFERARB hPBuffer,
                                   int nAttr ,
                                   int * pValue )
{
   return _wglQueryPBufferARB(hPBuffer, nAttr, pValue);
}

BOOL CPBuffer::wglMakeContextCurrentARB( HDC hDrawDC,
                                         HDC hReadDC,
                                         HGLRC hGLRC )
{
   return _wglMakeContextCurrentARB(hDrawDC, hReadDC, hGLRC);
}

HDC CPBuffer::wglGetCurrentReadDCARB( void )
{
   return _wglGetCurrentReadDCARB();
}

const char * CPBuffer::wglGetExtensionsStringARB( HDC hDC )
{
   return _wglGetExtensionsStringARB(hDC);
}

BOOL CPBuffer::wglChoosePixelFormatARB( HDC hDC,
                                        const int * piAttribIList,
                                        const FLOAT * pfAttribFList,
                                        UINT nMaxFormats,
                                        int * piFormats,
                                        UINT * pnNumFormats )
{
   return _wglChoosePixelFormatARB(hDC,
                                   piAttribIList,
                                   pfAttribFList,
                                   nMaxFormats,
                                   piFormats,
                                   pnNumFormats);
}

BOOL CPBuffer::wglBindTexImageARB( HPBUFFERARB hPbuffer, int iBuffer )
{
   return _wglBindTexImageARB(hPbuffer, iBuffer);
}

BOOL CPBuffer::wglReleaseTexImageARB( HPBUFFERARB hPbuffer, int iBuffer )
{
   return _wglReleaseTexImageARB(hPbuffer, iBuffer);
}

BOOL CPBuffer::wglSetPbufferAttribARB( HPBUFFERARB hPbuffer, const int * piAttribList )
{
   return _wglSetPbufferAttribARB(hPbuffer, piAttribList);
}

// initialize the static gl functions
PFNWGLCREATEPBUFFERARBPROC       CPBuffer::_wglCreatePBufferARB = 0;
PFNWGLGETPBUFFERDCARBPROC        CPBuffer::_wglGetPBufferDCARB = 0;
PFNWGLRELEASEPBUFFERDCARBPROC    CPBuffer::_wglReleasePBufferDCARB = 0;
PFNWGLDESTROYPBUFFERARBPROC      CPBuffer::_wglDestroyPBufferARB = 0;
PFNWGLQUERYPBUFFERARBPROC        CPBuffer::_wglQueryPBufferARB = 0;
PFNWGLMAKECONTEXTCURRENTARBPROC  CPBuffer::_wglMakeContextCurrentARB = 0;
PFNWGLGETCURRENTREADDCARBPROC    CPBuffer::_wglGetCurrentReadDCARB = 0;
PFNWGLGETEXTENSIONSSTRINGARBPROC CPBuffer::_wglGetExtensionsStringARB = 0;
PFNWGLCHOOSEPIXELFORMATARBPROC   CPBuffer::_wglChoosePixelFormatARB = 0;
PFNWGLBINDTEXIMAGEARBPROC        CPBuffer::_wglBindTexImageARB = 0;
PFNWGLRELEASETEXIMAGEARBPROC     CPBuffer::_wglReleaseTexImageARB = 0;
PFNWGLSETPBUFFERATTRIBARBPROC    CPBuffer::_wglSetPbufferAttribARB = 0;

// initialize the static member variables
bool CPBuffer::m_bInitialized = false;