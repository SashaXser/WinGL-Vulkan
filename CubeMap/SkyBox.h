#ifndef _SKY_BOX_H_
#define _SKY_BOX_H_

// wingl includes
#include "ReadTexture.h"

// std includes
#include <cstdint>

class SkyBox
{
public:
   // constructor / deconstructor
            SkyBox( );
           ~SkyBox( );

   // initializes the skybox images
   void     InitImages( const char * pTop,
                        const char * pBottom,
                        const char * pFront,
                        const char * pBack,
                        const char * pLeft,
                        const char * pRight );

   // render the sky box
   void     Render( const double & rElapsedTime );

   // updates the sky box
   void     Update( const double & rElapsedTime );

private:
   // private enumerations
   enum
   {
      IMAGE_TOP_ENUM    = 0,
      IMAGE_BOTTOM_ENUM = 1,
      IMAGE_FRONT_ENUM  = 2,
      IMAGE_BACK_ENUM   = 3,
      IMAGE_LEFT_ENUM   = 4,
      IMAGE_RIGHT_ENUM  = 5,
      MAX_IMAGE_SIZE    = 6
   };

   // private member variables
   GLuint               m_TexIDs[MAX_IMAGE_SIZE];
   Texture< uint8_t >   m_aImages[MAX_IMAGE_SIZE];

};

#endif // _SKY_BOX_H_