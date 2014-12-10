#ifndef _NORMAL_MAPPING_WINDOW_H_
#define _NORMAL_MAPPING_WINDOW_H_

// local includes
#include "OpenGLWindow.h"

// wgl includes
#include "Matrix.h"
#include "Vector3.h"

// std includes
#include <memory>
#include <cstdint>

// forward declarations
class Texture;
class ShaderProgram;
class VertexArrayObject;
class VertexBufferObject;

class NormalMappingWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   NormalMappingWindow( );

   // creates the application
   virtual bool Create( unsigned int nWidth,
                        unsigned int nHeight,
                        const char * pWndTitle,
                        const void * pInitParams = NULL );

   // basic run implementation
   // will process messages until app quit
   virtual int Run( );
   
protected:
   // destructor
   virtual ~NormalMappingWindow( );

   // called when the window is about to be destroyed
   // the opengl context is no longer valid after this call returns
   virtual void OnDestroy( ) override;

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam ) override;

private:
   // defines the types of shaders
   enum Shader
   {
      FLAT_SHADER,
      NORMAL_SHADER,
      PARALLAX_SHADER
   };

   // defines the type of manipulator
   enum Manipulate
   {
      MANIPULATE_CAMERA,
      MANIPULATE_DIRECTIONAL_LIGHT,
      MANIPULATE_POINT_LIGHT
   };

   // loads the specified shader
   void LoadShader( const Shader shader );

   // initializes the vertex data
   void InitVertexData( );

   // initializes the lighting data
   void InitLightingData( );

   // updates the directional light shader
   void UpdateDirLightShader( );

   // helper function to load textures
   bool LoadTexture( );

   // defines the type of shader being used (flat / normal / parallax)
   std::shared_ptr< ShaderProgram >    mpShader;
   
   // defines a very basic shader for the visualization of the directional light
   std::shared_ptr< ShaderProgram >    mpShaderDirLight;

   // textures used by the program
   std::shared_ptr< Texture >    mpDiffuseTex;
   std::shared_ptr< Texture >    mpHeightTex;
   std::shared_ptr< Texture >    mpNormalTex;

   // defines the geometry to be displayed
   std::shared_ptr< VertexArrayObject >   mpWallVAO;
   std::shared_ptr< VertexBufferObject >  mpWallVerts;
   std::shared_ptr< VertexBufferObject >  mpWallNorms;
   std::shared_ptr< VertexBufferObject >  mpWallTangents;
   std::shared_ptr< VertexBufferObject >  mpWallBitangents;
   std::shared_ptr< VertexBufferObject >  mpWallTexCoords;

   std::shared_ptr< VertexArrayObject >   mpDirLightVAO;
   std::shared_ptr< VertexBufferObject >  mpDirLightVerts;
   std::shared_ptr< VertexBufferObject >  mpDirLightColors;

   // defines the projection and camera matrices
   Matrixf     mProjection;
   Matrixf     mCamera;

   // defines the type of manipulator
   Manipulate  mManipulate;

   // defines lighting parameters
   Vec3f       mDirectionalLightDir;
   float       mPointLightAmbientIntensity;
   float       mPointLightDiffuseIntensity;

   // defines the parallax parameters
   float       mParallaxBias;
   float       mParallaxScale;

   // defines the mouse position in native screen coordinates
   int32_t     mMousePos[2];

};

#endif // _NORMAL_MAPPING_WINDOW_H_
