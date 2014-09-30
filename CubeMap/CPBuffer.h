#ifndef _C_P_BUFFER_H_
#define _C_P_BUFFER_H_

// standard includes
#include "StdIncludes.h"

class CPBuffer
{
public:
   // constructor / destructor
               CPBuffer( );
              ~CPBuffer( );

   // p buffer access functions
   HPBUFFERARB    wglCreatePbufferARB( HDC hDC, int nPixelFormat, int nWidth, int nHeight, const int * pAttrList );
   HDC            wglGetPbufferDCARB( HPBUFFERARB hPBuffer );
   int            wglReleasePbufferDCARB( HPBUFFERARB hPBuffer, HDC hDC );
   BOOL           wglDestroyPbufferARB( HPBUFFERARB hPBuffer );
   BOOL           wglQueryPbufferARB( HPBUFFERARB hPBuffer, int nAttr , int * pValue );
   BOOL           wglMakeContextCurrentARB( HDC hDrawDC, HDC hReadDC, HGLRC hGLRC );
   HDC            wglGetCurrentReadDCARB( void );
   const char *   wglGetExtensionsStringARB( HDC hDC );
   BOOL           wglChoosePixelFormatARB( HDC hDC, const int * piAttribIList, const FLOAT * pfAttribFList,
                                           UINT nMaxFormats, int * piFormats, UINT * pnNumFormats );
   BOOL           wglBindTexImageARB( HPBUFFERARB hPbuffer, int iBuffer );
   BOOL           wglReleaseTexImageARB( HPBUFFERARB hPbuffer, int iBuffer );
   BOOL           wglSetPbufferAttribARB( HPBUFFERARB hPbuffer, const int * piAttribList );

private:
   // prohibit copying of the class
               CPBuffer( const CPBuffer & rBuffer );
   CPBuffer &  operator = ( const CPBuffer & rBuffer );

   // initializes the pbuffer extensions
   bool     InitializeExtensions( );

   // private static member functions
   static PFNWGLCREATEPBUFFERARBPROC         _wglCreatePBufferARB;
   static PFNWGLGETPBUFFERDCARBPROC          _wglGetPBufferDCARB;
   static PFNWGLRELEASEPBUFFERDCARBPROC      _wglReleasePBufferDCARB;
   static PFNWGLDESTROYPBUFFERARBPROC        _wglDestroyPBufferARB;
   static PFNWGLQUERYPBUFFERARBPROC          _wglQueryPBufferARB;
   static PFNWGLMAKECONTEXTCURRENTARBPROC    _wglMakeContextCurrentARB;
   static PFNWGLGETCURRENTREADDCARBPROC      _wglGetCurrentReadDCARB;
   static PFNWGLGETEXTENSIONSSTRINGARBPROC   _wglGetExtensionsStringARB;
   static PFNWGLCHOOSEPIXELFORMATARBPROC     _wglChoosePixelFormatARB;
   static PFNWGLBINDTEXIMAGEARBPROC          _wglBindTexImageARB;
   static PFNWGLRELEASETEXIMAGEARBPROC       _wglReleaseTexImageARB;
   static PFNWGLSETPBUFFERATTRIBARBPROC      _wglSetPbufferAttribARB;

   // private static member variables
   static bool    m_bInitialized;

};

#endif // _C_P_BUFFER_H_