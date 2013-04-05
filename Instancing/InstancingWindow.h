#ifndef _INSTANCING_WINDOW_H_
#define _INSTANCING_WINDOW_H_

// local includes
#include "Matrix.h"
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

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam );

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

   // defines a building instance
   struct BuildingInstance
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

   // defines a tree instance
   struct TreeInstance
   {
      // gl buffer ids
      GLuint   mVertBufferID;
      // defines the width and height of the tree
      float    mSize[2];
      // texture to use for the trees
      GLuint   mTexID;
      // defines the texture coordinates of the tree
      float    mTexCoords[8];
      // number of instances to render
      GLuint   mNumInstances;
   };

   // defines number of instances and the area
   static const int32_t  INSTANCE_AREA = 500;
   static const uint32_t NUM_BUILDING_TYPES = 10;
   static const uint32_t NUM_TREE_TYPES = 3;

   // shader ids
   GLuint   mBuildingsProgID;
   GLuint   mBuildingsVertID;
   GLuint   mBuildingsFragID;

   GLuint   mTreesProgID;
   GLuint   mTreesVertID;
   GLuint   mTreesFragID;
   GLuint   mTreesGeomID;

   // number of instances
   uint32_t mNumBuildingInstances;
   uint32_t mNumTreeInstances;

   // instances
   BuildingInstance  mBuildingInstances[NUM_BUILDING_TYPES];
   TreeInstance      mTreeInstances[NUM_TREE_TYPES];

   // camera / view matrix
   Matrixf  mCamera;
   Matrixf  mPerspective;

   // the previous mouse x, y positions
   int16_t  mPrevMouseX;
   int16_t  mPrevMouseY;

};

#endif // _INSTANCING_WINDOW_H_
