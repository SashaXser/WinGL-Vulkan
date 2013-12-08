#ifndef _INSTANCING_WINDOW_H_
#define _INSTANCING_WINDOW_H_

// local includes

// wingl includdes
#include "OpenGLWindow.h"

// std includes
#include <vector>
#include <cstdint>

class WindowlessRenderWindow : public OpenGLWindow
{
public:
   // constructor...
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   WindowlessRenderWindow( );

   // creates the application
   virtual bool Create( unsigned int nWidth, unsigned int nHeight,
                        const char * pWndTitle, const void ** pInitParams = nullptr );

   // basic run implementation
   // will process messages until app quit
   virtual int Run( );

protected:
   // destructor...
   virtual ~WindowlessRenderWindow( );

   // called when the window is about to be destroyed
   // the opengl context is no longer valid after this call returns
   virtual void OnDestroy( ) override;

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
   // private forward declarations
   struct ImgData;

   // prohibit copy construction
   WindowlessRenderWindow( const WindowlessRenderWindow & );
   // prohibit copy operator
   WindowlessRenderWindow & operator = ( const WindowlessRenderWindow & );

   // renders the scene
   void RenderScene( );

   // obtains the next item to render
   const ImgData * GetNextImgDataToRender( );

   // returns the item back to the pool
   void ReleaseImgData( const ImgData * const pImgData );

   // gets an image data buffer ready to use by the server
   ImgData * GetNextAvailableImgData( const uint32_t width,
                                      const uint32_t height,
                                      const uint32_t depth );

   // adds the data to the list for rendering
   void AddImgDataForRendering( ImgData * const pImgData );

   // entry point for the server thread
   uint32_t RunServerThread( );
   static uint32_t __stdcall ServerThreadEntry( void * pData );

   // buffers representing textures, pbos, or data obtained from server
   uint32_t          mCurBuffers;
   uint32_t          mTextures[2];
   uint32_t          mPixelUnpackBuffers[2];
   const ImgData *   mCurImgDataToRender[2];

   // handle to the server thread
   uintptr_t   mServerThread;

   // indicates if the server windows should be shown or hidden
   bool        mShowServerWnd;

   // vectors containing the image data
   CRITICAL_SECTION  mImgDataPoolMtx;
   CRITICAL_SECTION  mImgDataToRenderMtx;
   std::vector< ImgData * >   mImgDataPool;
   std::vector< ImgData * >   mImgDataToRender;
   std::vector< ImgData * >   mImgDataReadyForMapping;

};

#endif // _INSTANCING_WINDOW_H_
