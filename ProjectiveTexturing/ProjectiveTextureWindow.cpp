// local includes
#include "ProjectiveTextureWindow.h"
#include "Timer.h"
#include "WglAssert.h"
#include "ReadTexture.h"
#include "MatrixHelper.h"
#include "OpenGLExtensions.h"

// gl includes
#include <gl/gl.h>

// stl includes
#include <iostream>

// global defines
#define VALIDATE_OPENGL() WGL_ASSERT(glGetError() == GL_NO_ERROR)

ProjectiveTextureWindow::ProjectiveTextureWindow( ) :
mTexWidth            ( 0 ),
mTexHeight           ( 0 ),
mLogoTex             ( 0 ),
mMouseXCoord         ( 0 ),
mMouseYCoord         ( 0 ),
mpActiveMViewMat     ( &mCameraVariables.mMViewMat ),
mpSetupModeFuncPtr   ( &ProjectiveTextureWindow::SetupRenderSceneImmediateModeObjectSpace ),
mpRenderModeFuncPtr  ( &ProjectiveTextureWindow::RenderSceneImmediateMode )
{
   // output keyboard controls
   std::cout << "C - Switch between camera and light" << std::endl
             << "1 - Texture projection object space immediate mode" << std::endl
             << "2 - Texture projection eye space immediate mode" << std::endl
             << "WSAD - Move camera or light forwards or backwards and strafe left or right" << std::endl
             << "Left Mouse Button - Enable movement of camera or light" << std::endl
             << "Esc - Quit application" << std::ends;
}

ProjectiveTextureWindow::~ProjectiveTextureWindow( )
{
   // release the texture object
   glDeleteTextures(1, &mLogoTex);
}

bool ProjectiveTextureWindow::Create( unsigned int nWidth,
                                      unsigned int nHeight,
                                      const char * pWndTitle,
                                      const void * pInitParams )
{
   // initialize with a 4.0 context
   const OpenGLWindow::OpenGLInit glInit40 =
   {
      4, 0, true, true, false
   };

   // initialize with a 3.2 context
   const OpenGLWindow::OpenGLInit glInit32 =
   {
      3, 2, true, false, false
   };

   // initialize 40 first, then 32 second, else nothing
   const OpenGLWindow::OpenGLInit * glInit[] =
   {
      &glInit40, &glInit32, NULL
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, reinterpret_cast< const void ** >(glInit)))
   {
      // make the context current
      MakeCurrent();

      // attach to the debug context
      AttachToDebugContext();

      // init common gl state
      InitGLState(nWidth, nHeight);

      // load the projective texture
      LoadTexture();

      // setup the light parameters
      mLightVariables.mProjMat.MakePerspective(45.0,
                                               static_cast< double >(mTexWidth) /
                                               static_cast< double >(mTexHeight),
                                               1.0, 5.0);
      mLightVariables.mMViewMat.MakeLookAt(Vec3d( 10.0, 10.0,  10.0),
                                           Vec3d(-10.0,  0.0, -10.0),
                                           Vec3d(  0.0,  1.0,   0.0));

      // setup the camera parameters
      mCameraVariables.mProjMat.MakePerspective(45.0, 
                                                static_cast< double >(nWidth) /
                                                static_cast< double >(nHeight),
                                                0.1, 100.0);
      mCameraVariables.mMViewMat.MakeLookAt(Vec3d( 10.0, 10.0,  10.0),
                                            Vec3d(-10.0,  0.0, -10.0),
                                            Vec3d(  0.0,  1.0,   0.0));

      // load the camera parameters
      glMatrixMode(GL_PROJECTION);
      glLoadMatrixd(mCameraVariables.mProjMat);
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixd(mCameraVariables.mMViewMat);

      // update the lighting model
      UpdateImmediateModeLightModel();

      // setup render mode specific attributes
      (this->*mpSetupModeFuncPtr)();
      
      return true;
   }
   else
   {
      // post the quit message
      PostQuitMessage(-1);
   }

   return false;
}

int ProjectiveTextureWindow::Run( )
{
   // create a local timer object...
   // timer object will be used to create
   // a 60 Hz rendering loop...
   Timer localTimer;

   // setup the amount of timer per frame
   const double MS_PER_FRAME = 1000.0 / 60.0;

   // app quit variables
   int appQuitVal = 0;
   bool bQuit = false;

   while (!bQuit)
   {
      // obtain the current tick count
      const long long begTick = localTimer.GetCurrentTick();

      // process all the app messages and then render the scene
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         RenderScene();

         // if there is time left, then do some waiting
         if (const double deltaMS = localTimer.DeltaMS(begTick) <= MS_PER_FRAME)
         {
            // wait for the remainder of the time
            localTimer.Wait(static_cast< unsigned long >(MS_PER_FRAME - deltaMS));
         }
      }
   }

   return appQuitVal;
}

void ProjectiveTextureWindow::InitGLState( int vpWidth, int vpHeight )
{
   // setup basic attributes
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   glViewport(0, 0, vpWidth, vpHeight);

   glEnable(GL_DEPTH_TEST);

   // set the shading model to be smooth
   glShadeModel(GL_SMOOTH);

   // set hit values
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   glHint(OpenGLExt::GL_GENERATE_MIPMAP_HINT, GL_NICEST);

   // setup color tracking
   glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

   // setup global ambient parameters
   const float lightModelAmbient[] = { 0.6f, 0.6f, 0.6f, 1.0f };
   //const float lightModelAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightModelAmbient);

   // initialize the ambient and diffuse params of light 0
   const float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
   const float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
   glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
}

void ProjectiveTextureWindow::RenderScene( )
{
   // clear the color and depth buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // render the scene
   (this->*mpRenderModeFuncPtr)();

   // swap the front and back buffers
   SwapBuffers(GetHDC());

   // make sure there are no errors
   VALIDATE_OPENGL();
}

void ProjectiveTextureWindow::RenderSceneImmediateMode( )
{
   RenderWallsImmediateMode();
   RenderSpotLightImmediateMode();
}

void ProjectiveTextureWindow::RenderWallsImmediateMode( )
{
   // wall constructs
   const float fWallValues[][4] =
   {
      // red bottom wall...
      { -10.0f,  0.0f, -10.0f, 1.0f },
      { -10.0f,  0.0f,  10.0f, 1.0f },
      {  10.0f,  0.0f,  10.0f, 1.0f },
      {  10.0f,  0.0f, -10.0f, 1.0f },
      // green left wall...
      { -10.0f, 20.0f,  10.0f, 1.0f },
      { -10.0f,  0.0f,  10.0f, 1.0f },
      { -10.0f,  0.0f, -10.0f, 1.0f },
      { -10.0f, 20.0f, -10.0f, 1.0f },
      // blue back wall...
      { -10.0f, 20.0f, -10.0f, 1.0f },
      { -10.0f,  0.0f, -10.0f, 1.0f },
      {  10.0f,  0.0f, -10.0f, 1.0f },
      {  10.0f, 20.0f, -10.0f, 1.0f }
   };

   // texture matrix
   Matrixd projTxtMat;

   if (mpSetupModeFuncPtr ==
       &ProjectiveTextureWindow::SetupRenderSceneImmediateModeObjectSpace)
   {
      // create the model matrix...
      // the model matrix is the identity
      // since it has not moved anywhere...
      const Matrixd modelMat;

      // map textures to [0, 1]
      const Matrixd scaleBiasMat(Vec4d(0.5, 0.0, 0.0, 0.0),
                                 Vec4d(0.0, 0.5, 0.0, 0.0),
                                 Vec4d(0.0, 0.0, 0.5, 0.0),
                                 Vec4d(0.5, 0.5, 0.5, 1.0));

      // create the object linear matrix
      projTxtMat = scaleBiasMat *
                   mLightVariables.mProjMat *
                   mLightVariables.mMViewMat *
                   modelMat;
   }
   else if (mpSetupModeFuncPtr ==
            &ProjectiveTextureWindow::SetupRenderSceneImmediateModeEyeSpace)
   {
      // map textures to [0, 1]
      const Matrixd scaleBiasMat(Vec4d(0.5, 0.0, 0.0, 0.0),
                                 Vec4d(0.0, 0.5, 0.0, 0.0),
                                 Vec4d(0.0, 0.0, 0.5, 0.0),
                                 Vec4d(0.5, 0.5, 0.5, 1.0));

      // create the eye linear matrix
      projTxtMat = scaleBiasMat *
                   mLightVariables.mProjMat *
                   mLightVariables.mMViewMat *
                   mCameraVariables.mMViewMat.Inverse();
   }
   else
   {
      // what the?
      WGL_ASSERT(false);
   }

   glEnable(GL_ALPHA_TEST);
   //glEnable(GL_BLEND);

   float c[] = {1,0,0,0};
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

   // enable lighting
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

   // enable color tracking
   glEnable(GL_COLOR_MATERIAL);

   // enable texture generation modes
   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T);
   glEnable(GL_TEXTURE_GEN_R);
   glEnable(GL_TEXTURE_GEN_Q);

   // load the texture matrix
   glMatrixMode(GL_TEXTURE);
   glPushMatrix();
   glLoadMatrixd(projTxtMat);
   glMatrixMode(GL_MODELVIEW);

   // enable texturing
   glEnable(GL_TEXTURE_2D);
   // bind the logo texture
   glBindTexture(GL_TEXTURE_2D, mLogoTex);

   // render the bottom wall first
   glColor3f(1.0f, 0.0f, 0.0f);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glNormal3f(0.0f, 1.0f, 0.0f); glVertex3fv(fWallValues[0]);
   glTexCoord2f(1,0); glNormal3f(0.0f, 1.0f, 0.0f); glVertex3fv(fWallValues[1]);
   glTexCoord2f(1,1); glNormal3f(0.0f, 1.0f, 0.0f); glVertex3fv(fWallValues[2]);
   glTexCoord2f(0,1); glNormal3f(0.0f, 1.0f, 0.0f); glVertex3fv(fWallValues[3]);
   glEnd();

   // render the left wall second
   glColor3f(0.0f, 1.0f, 0.0f);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glNormal3f(1.0f, 0.0f, 0.0f); glVertex3fv(fWallValues[4]);
   glTexCoord2f(1,0); glNormal3f(1.0f, 0.0f, 0.0f); glVertex3fv(fWallValues[5]);
   glTexCoord2f(1,1); glNormal3f(1.0f, 0.0f, 0.0f); glVertex3fv(fWallValues[6]);
   glTexCoord2f(0,1); glNormal3f(1.0f, 0.0f, 0.0f); glVertex3fv(fWallValues[7]);
   glEnd();

   // render the back wall third
   glColor3f(0.0f, 0.0f, 1.0f);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glNormal3f(0.0f, 0.0f, 1.0f); glVertex3fv(fWallValues[8]);
   glTexCoord2f(1,0); glNormal3f(0.0f, 0.0f, 1.0f); glVertex3fv(fWallValues[9]);
   glTexCoord2f(1,1); glNormal3f(0.0f, 0.0f, 1.0f); glVertex3fv(fWallValues[10]);
   glTexCoord2f(0,1); glNormal3f(0.0f, 0.0f, 1.0f); glVertex3fv(fWallValues[11]);
   glEnd();

   // unbind the texture
   glBindTexture(GL_TEXTURE_2D, 0);

   // disable texturing
   glDisable(GL_TEXTURE_2D);

   // disable ligthing
   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);

   // disable color tracking
   glDisable(GL_COLOR_MATERIAL);

   // disable texture gen modes
   glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T);
   glDisable(GL_TEXTURE_GEN_R);
   glDisable(GL_TEXTURE_GEN_Q);

   // restore the texture matrix
   glMatrixMode(GL_TEXTURE);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
}

void ProjectiveTextureWindow::RenderSpotLightImmediateMode( )
{
   // create a local matrix for the light
   const Matrixd invLightMat =
      (mLightVariables.mProjMat * mLightVariables.mMViewMat).Inverse();

   // obtain the vectors that make up the
   // eight sides of the viewing matrix...
   Vec3d f1 = invLightMat * Vec3d(-1.0,  1.0,  1.0);
   Vec3d f2 = invLightMat * Vec3d(-1.0, -1.0,  1.0);
   Vec3d f3 = invLightMat * Vec3d( 1.0, -1.0,  1.0);
   Vec3d f4 = invLightMat * Vec3d( 1.0,  1.0,  1.0);
   Vec3d n1 = invLightMat * Vec3d(-1.0,  1.0, -1.0);
   Vec3d n2 = invLightMat * Vec3d(-1.0, -1.0, -1.0);
   Vec3d n3 = invLightMat * Vec3d( 1.0, -1.0, -1.0);
   Vec3d n4 = invLightMat * Vec3d( 1.0,  1.0, -1.0);

   // obtain the light cam position
   Vec3d lightPos = mLightVariables.mMViewMat.Inverse() * Vec3d(0.0, 0.0, 0.0);

   // set the color to yellow
   glColor3f(1.0f, 1.0f, 0.0f);

   // render the lines
   glBegin(GL_LINE_STRIP);
   glVertex3dv(f1);
   glVertex3dv(f2);
   glVertex3dv(f3);
   glVertex3dv(f4);
   glVertex3dv(f1);
   glVertex3dv(n1);
   glVertex3dv(n2);
   glVertex3dv(n3);
   glVertex3dv(n4);
   glVertex3dv(n1);
   glEnd();

   glBegin(GL_LINES);
   glVertex3dv(f2);
   glVertex3dv(n2);
   glVertex3dv(f3);
   glVertex3dv(n3);
   glVertex3dv(f4);
   glVertex3dv(n4);
   glEnd();

   glBegin(GL_LINES);
   glVertex3dv(lightPos);
   glVertex3dv(n1);
   glVertex3dv(lightPos);
   glVertex3dv(n2);
   glVertex3dv(lightPos);
   glVertex3dv(n3);
   glVertex3dv(lightPos);
   glVertex3dv(n4);
   glEnd();
}

void ProjectiveTextureWindow::SetupRenderSceneImmediateModeEyeSpace( )
{
   // setup the object plane attributes
   const double pObjPlaneSTRQ[][4] =
   {
      { 1.0, 0.0, 0.0, 0.0 },
      { 0.0, 1.0, 0.0, 0.0 },
      { 0.0, 0.0, 1.0, 0.0 },
      { 0.0, 0.0, 0.0, 1.0 }
   };

   // setup object level texture generation
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
   glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
   glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
   glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

   glTexGendv(GL_S, GL_EYE_PLANE, pObjPlaneSTRQ[0]);
   glTexGendv(GL_T, GL_EYE_PLANE, pObjPlaneSTRQ[1]);
   glTexGendv(GL_R, GL_EYE_PLANE, pObjPlaneSTRQ[2]);
   glTexGendv(GL_Q, GL_EYE_PLANE, pObjPlaneSTRQ[3]);

   // update the light model
   UpdateImmediateModeLightModel();
}

void ProjectiveTextureWindow::SetupRenderSceneImmediateModeObjectSpace( )
{
   // setup the object plane attributes
   const double pObjPlaneSTRQ[][4] =
   {
      { 1.0, 0.0, 0.0, 0.0 },
      { 0.0, 1.0, 0.0, 0.0 },
      { 0.0, 0.0, 1.0, 0.0 },
      { 0.0, 0.0, 0.0, 1.0 }
   };

   // setup object level texture generation
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
   glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
   glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
   glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

   glTexGendv(GL_S, GL_OBJECT_PLANE, pObjPlaneSTRQ[0]);
   glTexGendv(GL_T, GL_OBJECT_PLANE, pObjPlaneSTRQ[1]);
   glTexGendv(GL_R, GL_OBJECT_PLANE, pObjPlaneSTRQ[2]);
   glTexGendv(GL_Q, GL_OBJECT_PLANE, pObjPlaneSTRQ[3]);

   // update the light model
   UpdateImmediateModeLightModel();
}

void ProjectiveTextureWindow::UpdateImmediateModeLightModel( )
{
   // modelview matrix required here
   WGL_ASSERT_INIT(int curMatMode = 0; glGetIntegerv(GL_MATRIX_MODE, &curMatMode);,
                   curMatMode == GL_MODELVIEW &&
                   "Current matrix mode is not the modelview");

   // obtain the lights position and view direction
   const Vec3d lPos = mLightVariables.mMViewMat.Inverse() * Vec3d(0.0, 0.0, 0.0);
   const Vec3d lViewDir = MatrixHelper::GetViewVector(mLightVariables.mMViewMat).UnitVector();

   const float lightPos[] =
   {
      static_cast< float >(lPos.mT[0]),
      static_cast< float >(lPos.mT[1]),
      static_cast< float >(lPos.mT[2]),
      static_cast< float >(lPos.mT[3])
   };

   const float lightViewDir[] =
   {
      static_cast< float >(-lViewDir.mT[0]),
      static_cast< float >(-lViewDir.mT[1]),
      static_cast< float >(-lViewDir.mT[2]),
      //static_cast< float >(lViewDir.mT[3])
      0.0f
   };

   // setup the light parameters
   glLightfv(GL_LIGHT0, GL_POSITION, lightViewDir);
   //glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
   //glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightViewDir);

   //glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 10.0f);
   //glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 120.0f);
}

void ProjectiveTextureWindow::LoadTexture( )
{
   // load the texture data
   unsigned char * pTexture = NULL;

   if (ReadRGB("BMLogo.rgb", mTexWidth, mTexHeight, &pTexture))
   {
      // generate a texture id
      glGenTextures(1, &mLogoTex);

      // bind the texture object
      glBindTexture(GL_TEXTURE_2D, mLogoTex);

      // setup the texture parameters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, OpenGLExt::GL_GENERATE_MIPMAP, GL_TRUE);

      // load the texture data
      glTexImage2D(GL_TEXTURE_2D,
                   0,
                   GL_RGBA8,
                   mTexWidth,
                   mTexHeight,
                   0,
                   GL_RGBA,
                   GL_UNSIGNED_BYTE,
                   pTexture);

      // release the bound texture
      glBindTexture(GL_TEXTURE_2D, 0);

      // release the texture
      delete [] pTexture;
   }
}

LRESULT ProjectiveTextureWindow::MessageHandler( UINT uMsg,
                                                 WPARAM wParam,
                                                 LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_SIZE:
      {
      // obtain the width and height
      const int width = lParam & 0x000000000000FFFF;
      const int height = (lParam >> 16) & 0x000000000000FFFF;
      // set the size of the viewport
      glViewport(0, 0, width, height);
      // setup the projection matrix
      mCameraVariables.mProjMat.MakePerspective(45.0,
                                                static_cast< double >(width) /
                                                static_cast< double >(height),
                                                0.1, 100.0);
      // load the projection matrix
      glMatrixMode(GL_PROJECTION);
      glLoadMatrixd(mCameraVariables.mProjMat);
      glMatrixMode(GL_MODELVIEW);
      }

      break;

   case WM_LBUTTONDOWN:
      // capture all mouse input
      SetCapture(GetHWND());

      break;

   case WM_LBUTTONUP:
      // release all mouse input
      ReleaseCapture();

      break;

   case WM_MOUSEMOVE:
      {
      // obtain the current x and y values
      const int nCurX = lParam & 0x000000000000FFFF;
      const int nCurY = (lParam >> 16) & 0x000000000000FFFF;

      // modifies the camera parameters
      if (wParam & MK_LBUTTON)
      {
         // obtain the mouse deltas
         const int nXDelta = nCurX - mMouseXCoord;
         const int nYDelta = nCurY - mMouseYCoord;
         // decompose the current values from the modelview matrix
         double dYaw = 0.0, dPitch = 0.0;
         MatrixHelper::DecomposeYawPitchRollDeg(*mpActiveMViewMat,
                                                &dYaw, &dPitch, (double *)NULL);
         // add the deltas to the current yaw and pitch
         dYaw += nXDelta;
         dPitch += nYDelta;
         // make sure to cap pitch to plus or minus 90
         dPitch = min(dPitch, 90.0);
         dPitch = max(dPitch, -90.0);
         // create a rotation matrix centered
         // around the up and view direction vectors...
         Matrixd matYaw, matPitch;
         matYaw.MakeRotation(dYaw, 0.0, 1.0, 0.0);
         matPitch.MakeRotation(dPitch, 1.0, 0.0, 0.0);
         // obtain the camera world position...
         // take the inverse of the camera world position since
         // the camera must move things into the eye space...
         const Vec3d camEye = mpActiveMViewMat->InverseFromOrthogonal() *
                              Vec3d(0.0, 0.0, 0.0) * -1;
         // create a translation matrix for the camera
         Matrixd matTrans;
         matTrans.MakeTranslation(camEye.mT[0], camEye.mT[1], camEye.mT[2]);
         // modify the camera matrix
         *mpActiveMViewMat = matPitch * matYaw * matTrans;
         
         // load the new modelview matrix on if it is the
         // camera matrix that is being manipulated...
         if (mpActiveMViewMat == &mCameraVariables.mMViewMat)
         {
            glLoadMatrixd(*mpActiveMViewMat);
         }

         // update the light parameters in immediate mode
         if (mpRenderModeFuncPtr == &ProjectiveTextureWindow::RenderSceneImmediateMode)
         {
            UpdateImmediateModeLightModel();
         }
      }

      // update the mouse coordinates
      mMouseXCoord = nCurX;
      mMouseYCoord = nCurY;
      }

      break;

   case WM_KEYUP:
      switch (wParam)
      {
      case 'C':
         // switch the active camera
         mpActiveMViewMat = mpActiveMViewMat == &mCameraVariables.mMViewMat ?
                            &mLightVariables.mMViewMat : &mCameraVariables.mMViewMat;

         break;

      case VK_ESCAPE:
         // post a close message to start the shutdown process
         PostMessage(GetHWND(), WM_CLOSE, 0, 0);

         break;

      case '1':
         // process the setup functions
         mpSetupModeFuncPtr = &ProjectiveTextureWindow::SetupRenderSceneImmediateModeObjectSpace;
         SetupRenderSceneImmediateModeObjectSpace();

         break;

      case '2':
         // process the setup functions
         mpSetupModeFuncPtr = &ProjectiveTextureWindow::SetupRenderSceneImmediateModeEyeSpace;
         SetupRenderSceneImmediateModeEyeSpace();

         break;
      }

      break;

   case WM_KEYDOWN:
      switch (wParam)
      {
      case 'A':
      case 'D':
      case 'W':
      case 'S':
         // decompose the current yaw and pitch
         const double dYaw = 0.0, dPitch = 0.0;
         MatrixHelper::DecomposeYawPitchRollDeg(*mpActiveMViewMat,
                                                const_cast< double * >(&dYaw),
                                                const_cast< double * >(&dPitch),
                                                (double *)NULL);
         // obtain the view and strafe vectors
         const Vec3d vecView = MatrixHelper::GetViewVector(*mpActiveMViewMat);
         const Vec3d vecStrafe = MatrixHelper::GetStrafeVector(*mpActiveMViewMat);
         // obtain the camera world position...
         Vec3d camEye = mpActiveMViewMat->InverseFromOrthogonal() *
                        Vec3d(0.0, 0.0, 0.0);
         // obtain the repeat count of the key press
         const unsigned short nRptCnt =
            static_cast< unsigned short >(lParam & 0x0000FFFF);
         // create a scale factor for the translation
         const double dTransScale = nRptCnt * 0.15;

         switch (wParam)
         {
         case 'A':
            camEye -= (vecStrafe * dTransScale);
            break;

         case 'D':
            camEye += (vecStrafe * dTransScale);
            break;

         case 'W':
            camEye += (vecView * dTransScale);
            break;

         case 'S':
            camEye -= (vecView * dTransScale);
            break;
         }

         // create yaw, pitch and translation matrices
         Matrixd matYaw, matPitch, matTrans;
         matYaw.MakeRotation(dYaw, 0.0, 1.0, 0.0);
         matPitch.MakeRotation(dPitch, 1.0, 0.0, 0.0);
         matTrans.MakeTranslation(-camEye.mT[0], -camEye.mT[1], -camEye.mT[2]);
         // multiply the matrix out
         *mpActiveMViewMat = matPitch * matYaw * matTrans;
         
         // load the new modelview matrix on if it is the
         // camera matrix that is being manipulated...
         if (mpActiveMViewMat == &mCameraVariables.mMViewMat)
         {
            glLoadMatrixd(*mpActiveMViewMat);
         }

         // update the light parameters in immediate mode
         if (mpRenderModeFuncPtr == &ProjectiveTextureWindow::RenderSceneImmediateMode)
         {
            UpdateImmediateModeLightModel();
         }

         break;
      }

      break;

   default:
      // pass to the default handler
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);

      break;
   }

   return result;
}