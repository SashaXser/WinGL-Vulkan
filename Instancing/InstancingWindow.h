#ifndef _INSTANCING_WINDOW_H_
#define _INSTANCING_WINDOW_H_

// local includes
#include "OpenGLWindow.h"

class InstancingWindow : public OpenGLWindow
{
public:
   // constructor...
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   InstancingWindow( );

protected:
   // destructor...
   virtual ~InstancingWindow( );

private:
   // prohibit copy construction
   InstancingWindow( const InstancingWindow & );
   // prohibit copy operator
   InstancingWindow & operator = ( const InstancingWindow & );


};

#endif // _INSTANCING_WINDOW_H_
