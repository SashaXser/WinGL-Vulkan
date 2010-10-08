// local includes
#include "ProjectiveTextureWindow.h"
#include "Timer.h"
#include "ReadTexture.h"
#include "MatrixHelper.h"
#include "OpenGLExtensions.h"

// gl includes
#include <gl/gl.h>

ProjectiveTextureWindow::ProjectiveTextureWindow( ) :
mTexWidth            ( 0 ),
mTexHeight           ( 0 ),
mLogoTex             ( 0 ),
mMouseXCoord         ( 0 ),
mMouseYCoord         ( 0 ),
mpActiveMViewMat     ( &mCameraVariables.mMViewMat ),
mpRenderModeFuncPtr  ( &ProjectiveTextureWindow::RenderSceneImmediateMode )
{
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
   // initialize with a 3.2 context
   OpenGLWindow::OpenGLInit glInit =
   {
      3, 2, true
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, &glInit))
   {
      // make the context current
      MakeCurrent();

      // setup basic attributes
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glViewport(0, 0, nWidth, nHeight);

      glEnable(GL_DEPTH_TEST);

      // load the projective texture
      LoadTexture();

      // setup the light parameters
      mLightVariables.mProjMat.MakePerspective(45.0,
                                               static_cast< double >(mTexWidth) /
                                               static_cast< double >(mTexHeight),
                                               1.0, 5.0);
      mLightVariables.mMViewMat.MakeLookAt(Vectord(5.0, 2.5, 5.0),
                                           Vectord(-2.5, 0.0, -2.5),
                                           Vectord(0.0, 1.0, 0.0));

      // setup the camera parameters
      mCameraVariables.mProjMat.MakePerspective(45.0, 
                                                static_cast< double >(nWidth) /
                                                static_cast< double >(nHeight),
                                                0.1, 100.0);
      mCameraVariables.mMViewMat.MakeLookAt(Vectord(5.0, 2.5, 5.0),
                                            Vectord(-2.5, 0.0, -2.5),
                                            Vectord(0.0, 1.0, 0.0));

      // load the camera parameters
      glMatrixMode(GL_PROJECTION);
      glLoadMatrixd(mCameraVariables.mProjMat);
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixd(mCameraVariables.mMViewMat);
      
      return true;
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

void ProjectiveTextureWindow::RenderScene( )
{
   // clear the color and depth buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // render the scene
   (this->*mpRenderModeFuncPtr)();

   // swap the front and back buffers
   SwapBuffers(GetHDC());
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
      { -2.5f,  0.0f, -2.5f, 1.0f },
      { -2.5f,  0.0f,  2.5f, 1.0f },
      {  2.5f,  0.0f,  2.5f, 1.0f },
      {  2.5f,  0.0f, -2.5f, 1.0f },
      // green left wall...
      { -2.5f,  5.0f,  2.5f, 1.0f },
      { -2.5f,  0.0f,  2.5f, 1.0f },
      { -2.5f,  0.0f, -2.5f, 1.0f },
      { -2.5f,  5.0f, -2.5f, 1.0f },
      // blue back wall...
      { -2.5f,  5.0f, -2.5f, 1.0f },
      { -2.5f,  0.0f, -2.5f, 1.0f },
      {  2.5f,  0.0f, -2.5f, 1.0f },
      {  2.5f,  5.0f, -2.5f, 1.0f }
   };

   // create the model matrix...
   // the model matrix is the identity
   // since it has not moved anywhere...
   const Matrixd modelMat;

   // map textures to [0, 1]
   const Matrixd scaleBiasMat(Vectord(0.5, 0.0, 0.0, 0.0),
                              Vectord(0.0, 0.5, 0.0, 0.0),
                              Vectord(0.0, 0.0, 0.5, 0.0),
                              Vectord(0.5, 0.5, 0.5, 1.0));

   // create the object linear matrix
   const Matrixd objLinearMat = scaleBiasMat *
                                mLightVariables.mProjMat *
                                mLightVariables.mMViewMat *
                                modelMat;

   const Vectord t1 = objLinearMat * Vectorf(fWallValues[0]);
   const Vectord t2 = objLinearMat * Vectorf(fWallValues[1]);
   const Vectord t3 = objLinearMat * Vectorf(fWallValues[2]);
   const Vectord t4 = objLinearMat * Vectorf(fWallValues[3]);
   const Vectord t5 = objLinearMat * Vectorf(fWallValues[4]);
   const Vectord t6 = objLinearMat * Vectorf(fWallValues[5]);
   const Vectord t7 = objLinearMat * Vectorf(fWallValues[6]);
   const Vectord t8 = objLinearMat * Vectorf(fWallValues[7]);
   const Vectord t9 = objLinearMat * Vectorf(fWallValues[8]);
   const Vectord t10 = objLinearMat * Vectorf(fWallValues[9]);
   const Vectord t11 = objLinearMat * Vectorf(fWallValues[10]);
   const Vectord t12 = objLinearMat * Vectorf(fWallValues[11]);

   // enable texturing
   glEnable(GL_TEXTURE_2D);
   //glEnable(GL_ALPHA_TEST);
   //glEnable(GL_BLEND);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

   glMatrixMode(GL_TEXTURE);
   glPushMatrix();
   glLoadMatrixd(objLinearMat);
   glMatrixMode(GL_MODELVIEW);

   //glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
   //glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
//   glTexGendv(GL_S, GL_OBJECT_PLANE, objLinearMat.mT);
//   glTexGendv(GL_T, GL_OBJECT_PLANE, objLinearMat.mT + 4);
//   glTexGendv(GL_R, GL_OBJECT_PLANE, objLinearMat.mT + 8);
//   glTexGendv(GL_Q, GL_OBJECT_PLANE, objLinearMat.mT + 12);
   const double s_plane[] = { 1, 0, 0, 0 };
   const double t_plane[] = { 0, 1, 0, 0 };
   const double r_plane[] = { 0, 0, 1, 0 };
   const double q_plane[] = { 0, 0, 0, 1 };
   glTexGendv(GL_S, GL_EYE_PLANE, s_plane);
   glTexGendv(GL_T, GL_EYE_PLANE, t_plane);
   glTexGendv(GL_R, GL_EYE_PLANE, r_plane);
   glTexGendv(GL_Q, GL_EYE_PLANE, q_plane);
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
   glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
   glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
   glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T);

   // bind the logo texture
   glBindTexture(GL_TEXTURE_2D, mLogoTex);

   // render the bottom wall first
   glColor3f(1.0f, 0.0f, 0.0f);
   glBegin(GL_QUADS);
   glVertex3fv(fWallValues[0]);
   glVertex3fv(fWallValues[1]);
   glVertex3fv(fWallValues[2]);
   glVertex3fv(fWallValues[3]);
   glEnd();

   // render the left wall second
   glColor3f(0.0f, 1.0f, 0.0f);
   glBegin(GL_QUADS);
   glVertex3fv(fWallValues[4]);
   glVertex3fv(fWallValues[5]);
   glVertex3fv(fWallValues[6]);
   glVertex3fv(fWallValues[7]);
   glEnd();

   // render the back wall third
   glColor3f(0.0f, 0.0f, 1.0f);
   glBegin(GL_QUADS);
   glVertex3fv(fWallValues[8]);
   glVertex3fv(fWallValues[9]);
   glVertex3fv(fWallValues[10]);
   glVertex3fv(fWallValues[11]);
   glEnd();

   // unbind the texture
   glBindTexture(GL_TEXTURE_2D, 0);

   // disable texturing
   glDisable(GL_TEXTURE_2D);

   glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T);

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
   Vectord f1 = invLightMat * Vectord(-1.0,  1.0,  1.0);
   Vectord f2 = invLightMat * Vectord(-1.0, -1.0,  1.0);
   Vectord f3 = invLightMat * Vectord( 1.0, -1.0,  1.0);
   Vectord f4 = invLightMat * Vectord( 1.0,  1.0,  1.0);
   Vectord n1 = invLightMat * Vectord(-1.0,  1.0, -1.0);
   Vectord n2 = invLightMat * Vectord(-1.0, -1.0, -1.0);
   Vectord n3 = invLightMat * Vectord( 1.0, -1.0, -1.0);
   Vectord n4 = invLightMat * Vectord( 1.0,  1.0, -1.0);

   // normalize the points back to world space
   f1 *= 1.0 / f1.mT[3];
   f2 *= 1.0 / f2.mT[3];
   f3 *= 1.0 / f3.mT[3];
   f4 *= 1.0 / f4.mT[3];
   n1 *= 1.0 / n1.mT[3];
   n2 *= 1.0 / n2.mT[3];
   n3 *= 1.0 / n3.mT[3];
   n4 *= 1.0 / n4.mT[3];

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
      const int width = lParam & 0xFFFF;
      const int height = lParam >> 16;
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
      const int nCurX = lParam & 0x0000FFFF;
      const int nCurY = lParam >> 16;

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
         const Vectord camEye = mpActiveMViewMat->InverseFromOrthogonal() *
                                Vectord(0.0, 0.0, 0.0) * -1;
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
         const Vectord vecView = MatrixHelper::GetViewVector(*mpActiveMViewMat);
         const Vectord vecStrafe = MatrixHelper::GetStrafeVector(*mpActiveMViewMat);
         // obtain the camera world position...
         Vectord camEye = mpActiveMViewMat->InverseFromOrthogonal() *
                          Vectord(0.0, 0.0, 0.0);
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