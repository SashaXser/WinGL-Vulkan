#ifndef _INSTANCING_WINDOW_H_
#define _INSTANCING_WINDOW_H_

// local includes
#include "OpenGLWindow.h"

// std includes
#include <stdint.h>

// gl incudes
#include <GL/glew.h>

class InstancingWindow : public OpenGLWindow
{
public:
   // constructor...
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   InstancingWindow( );

   // creates the application
   virtual bool Create( unsigned int nWidth, unsigned int nHeight,
                        const char * pWndTitle, const void ** pInitParams = nullptr );

   // basic run implementation
   // will process messages until app quit
   virtual int Run( );

protected:
   // destructor...
   virtual ~InstancingWindow( );

private:
   // prohibit copy construction
   InstancingWindow( const InstancingWindow & );
   // prohibit copy operator
   InstancingWindow & operator = ( const InstancingWindow & );

   // generates all the required items for rendering
   void CreateInstances( );

   // renders the scene
   void RenderScene( );

   // returns texture coordinates for the requested set
   const float * GetTextureCoords( const uint32_t set_id );

   // defines an instance
   struct Instance
   {
      // vao id
      GLuint   mVertArrayID;
      // gl buffer ids
      GLuint   mVertBufferID;
      GLuint   mIdxBufferID;
      GLuint   mTexBufferID;
      GLuint   mWorldBufferID;
      // number of indices in the index buffer
      GLuint   mIdxBufferSize;
      // texture to use for the buildings
      GLuint   mTexID;
      // number of instances to render
      GLuint   mNumInstances;
   };

   // defines number of instances and the area
   static const uint32_t NUM_INSTANCES = 15000;
   static const int32_t  INSTANCE_AREA = 500;
   static const uint32_t NUM_BUILDING_TYPES = 5;

   // shader ids
   GLuint   mProgramID;
   GLuint   mVertShaderID;
   GLuint   mFragShaderID;

   // number of instances
   Instance mInstances[NUM_BUILDING_TYPES];

};

#endif // _INSTANCING_WINDOW_H_
