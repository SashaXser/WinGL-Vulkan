// local includes
#include "CubeMapWindow.h"
#include "CFont.h"
#include "SkyBox.h"

// wingl includes
#include "Timer.h"
#include "ReadTexture.h"
#include "MatrixHelper.h"

// std include
#include <cassert>



CubeMapWindow::CubeMapWindow( ) :
mpSkySphereDrawOrder    ( &CubeMapWindow::DrawSkyboxThenSphere ),
mpFPS                   ( nullptr ),
mpHelp                  ( nullptr ),
mpInfo                  ( nullptr ),
mpSkyBox                ( nullptr ),
mpSphere                ( nullptr ),
mUpdateTimeDelta        ( 0.005 ),
mUpdateTimeout          ( 0.0 ),
mDisplayText            ( true ),
mCrateTexID             ( 0 ),
mCrateDispID            ( 0 ),
mBoxRotation            ( 0.0f ),
mBoxOrbitRotation       ( 0.0f ),
mMouseX                 ( 0 ),
mMouseY                 ( 0 )
{
}

CubeMapWindow::~CubeMapWindow( )
{
}

void CubeMapWindow::OnDestroy( )
{
   // release the display list
   glDeleteLists(mCrateDispID, 1);

   // release the texture
   glDeleteTextures(1, &mCrateTexID);
   
   // release resources
   delete mpFPS;
   delete mpInfo;
   delete mpHelp;
   delete mpSkyBox;
   delete mpSphere;

   // call the base class to clean things up
   OpenGLWindow::OnDestroy();
}

bool CubeMapWindow::Create( unsigned int nWidth,
                            unsigned int nHeight,
                            const char * pWndTitle,
                            const void * pInitParams )
{
   // initialize the opengl context
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 4, true, true, false }, 
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      // make the context current
      MakeCurrent();

      // attach to the debug context
      AttachToDebugContext();

      // setup the projection matrix
      mProjMat.MakePerspective(45.0f,
                               static_cast< float >(nWidth) / static_cast< float >(nHeight),
                               1.0f, 1000.0f);

      glMatrixMode(GL_PROJECTION);
      glLoadMatrixf(mProjMat);

      // setup the modelview matrix
      mViewMat.MakeLookAt(Vec3f(0.0f, 0.0f, 5.0f),
                          Vec3f(0.0f, 0.0f, 0.0f),
                          Vec3f(0.0f, 1.0f, 0.0f));

      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixf(mViewMat);

      // define a local set of images
      const char * const pImageFiles[] =
      {
         ".\\Images\\arch_posy.jpg", // top
         ".\\Images\\arch_negy.jpg", // bottom
         ".\\Images\\arch_posz.jpg", // front
         ".\\Images\\arch_negz.jpg", // back
         ".\\Images\\arch_negx.jpg", // left
         ".\\Images\\arch_posx.jpg"  // right
      };

      // create a new sphere
      // the images on the sphere will be 
      // flipped in the vertical direction
      mpSphere = new CSphere(GetHWND(),
                             1.0f,
                             50,
                             50,
                             *(pImageFiles + 5),
                             *(pImageFiles + 4),
                             *(pImageFiles),
                             *(pImageFiles + 1),
                             *(pImageFiles + 2),
                             *(pImageFiles + 3));

      // create a new skybox
      mpSkyBox = new SkyBox;
      // initialize the images
      mpSkyBox->InitImages(*(pImageFiles),
                           *(pImageFiles + 1),
                           *(pImageFiles + 2),
                           *(pImageFiles + 3),
                           *(pImageFiles + 4),
                           *(pImageFiles + 5));
      
      // create a new font
      mpFPS = new CFont(0,
                        Vec3f(nWidth * 0.5f, static_cast< float >(nHeight), 0.0f),
                        ".\\Images\\Font.tga",
                        20, 25,
                        "0 FPS",
                        1.0f,
                        CFont::ALIGN_CENTER);
      // create a new font
      mpInfo = new CFont(0,
                         Vec3f(0.0f, static_cast< float >(nHeight), 0.0f),
                         ".\\Images\\Font.tga",
                         20, 25,
                         "Sphere Radius      = 1.0\n"
                         "Sphere Slice       = 50\n"
                         "Sphere Stack       = 50\n"
                         "Sphere Draw Mode   = Fill\n"
                         "Sphere Reflection  = Static\n"
                         "Sphere Update Time = 0.005",
                         0.6f,
                         CFont::ALIGN_LEFT);
      // create a new font
      mpHelp = new CFont(0,
                         Vec3f(static_cast< float >(nWidth), static_cast< float >(nHeight), 0.0f),
                         ".\\Images\\Font.tga",
                         20, 25,
                         "  A - Moves camera left         \n"
                         "  S - Moves camera back         \n"
                         "  D - Moves camera right        \n"
                         "  W - Moves camera forward      \n"
                         "  T - Increases sphere radius   \n"
                         "  G - Decreases sphere radius   \n"
                         "  Y - Increases sphere stacks   \n"
                         "  H - Decreases sphere stacks   \n"
                         "  U - Increases sphere slices   \n"
                         "  J - Decreases sphere slices   \n"
                         "  R - Changes reflection type   \n"
                         "  F - Wireframe / Fill / Cubemap\n"
                         "  I - Increases cubemap size    \n"
                         "  K - Decreases cubemap size    \n"
                         "  O - Increases update time     \n"
                         "  L - Decreases update time     \n"
                         "  Z - Hide text                 \n"
                         "Esc - Shutdown application      ",
                         0.5f,
                         CFont::ALIGN_RIGHT);
      
      // init the crate
      InitCrate();
      
      // enable global gl states
      glPointSize(3.0f);
      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_COLOR_MATERIAL);
      
      // create the global ambient light
      float fAmbientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f };
      // modify the light model
      glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fAmbientLight);
      
      return true;
   }
   else
   {
      // issue an error from the application that it could not be created
      PostDebugMessage(GL_DEBUG_TYPE_ERROR, 1, GL_DEBUG_SEVERITY_HIGH, "Unable To Create 4.4 OpenGL Context");

      // post the quit message
      PostQuitMessage(-1);
   }

   return false;
}

int CubeMapWindow::Run( )
{
   // app quit variables
   int appQuitVal = 0;
   bool bQuit = false;

   // stores the previous run time to calculate the elapsed time
   double previous_time_sec = Timer().GetCurrentTimeSec();

   while (!bQuit)
   {
      // get the current time
      const double current_time_sec = Timer().GetCurrentTimeSec();

      // process all the app messages and then render the scene
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         // clear the frame and depth buffers
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         // update and render the frame
         const double elapsed_time = current_time_sec - previous_time_sec;
         UpdateFrame(elapsed_time);
         DrawFrame(elapsed_time);

         // swap the buffers
         SwapBuffers(GetHDC());
      }

      // save the previous time
      previous_time_sec = current_time_sec;
   }

   return appQuitVal;
}

LRESULT CubeMapWindow::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_SIZE:
      {

      // obtain the width and height
      const float width = static_cast< float >(lParam & 0xFFFF);
      const float height = static_cast< float >(lParam >> 16);
      
      // update the viewport
      glViewport(0, 0,
                 static_cast< GLsizei >(lParam & 0xFFFF),
                 static_cast< GLsizei >(lParam >> 16));

      // setup the projection matrix
      mProjMat.MakePerspective(45.0f,
                               static_cast< float >(lParam & 0xFFFF) / static_cast< float >(lParam >> 16),
                               1.0f, 1000.0f);

      glMatrixMode(GL_PROJECTION);
      glLoadMatrixf(mProjMat);

      // update the text locations
      if (mpFPS) mpFPS->SetPosition(Vec3f(width * 0.5f, height, 0.0f));
      if (mpInfo) mpInfo->SetPosition(Vec3f(0.0f, height, 0.0f));
      if (mpHelp) mpHelp->SetPosition(Vec3f(width, height, 0.0f));

      }

      break;

   case WM_CHAR:
      // handle the key press
      OnKeyPressed(static_cast< char >(wParam));

      break;

   case WM_MOUSEMOVE:
      // handle the mouse move
      OnMouseMove(static_cast< uint16_t >(lParam & 0xFFFF) - mMouseX,
                  static_cast< uint16_t >(lParam >> 16) - mMouseY,
                  static_cast< uint32_t >(wParam));

      // save the mouse coordinates
      mMouseX = static_cast< uint16_t >(lParam & 0xFFFF);
      mMouseY = static_cast< uint16_t >(lParam >> 16);

      break;

   default:
      // default handle the messages
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   }

   return result;
}

void CubeMapWindow::UpdateFrame( const double & elapsed_time )
{  
   // determine if the time has elapsed
   if (mUpdateTimeout - elapsed_time <= 0.0)
   {
      // set the string
      std::stringstream ssFPS; ssFPS << static_cast< uint32_t >(1.0 / elapsed_time) << " FPS";
      // update the text shape
      *mpFPS = ssFPS.str().c_str();
   }
   
   //// light position
   //static float fLightPos[] = { 0.0f, 0.0f, 10.0f, 1.0f };
   //static float fLightDir[] = { 0.0f, 0.0f, -1.0f };
   
   //// set the direcitonal light
   //glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0f);
   //glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);
   //glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, fLightDir);
   
   // update the sphere
   mpSphere->Update(elapsed_time, mViewMat.Inverse() * Vec3f(0.0f, 0.0f, 0.0f));
}

void CubeMapWindow::DrawFrame( const double elapsed_time )
{
   // update the rotation values
   mBoxRotation += static_cast< float >(90.0 * elapsed_time);
   mBoxOrbitRotation += static_cast< float >(40.0 * elapsed_time);

   if (mpSphere->GetReflectionType() & CSphere::REFLECT_STATIC)
   {
      // render the skybox then the sphere
      mpSkyBox->Render(elapsed_time);
      mpSphere->Draw(elapsed_time);
      // render the crate
      RenderCrate(mBoxRotation, mBoxOrbitRotation);
   }
   else
   {
      // update the time stamp
      mUpdateTimeout -= elapsed_time;

      if (mUpdateTimeout <= 0.0)
      {
         // reset the time
         mUpdateTimeout = mUpdateTimeDelta;

         // save a copy of the current camera
         Matrixf proj_mat_save = mProjMat;
         Matrixf view_mat_save = mViewMat;

         // save the viewport settings
         const Size view_size = GetSize();

         // change the current viewport settings
         glViewport(0, 0,
                    mpSphere->GetDynamicCubemapTexSize(),
                    mpSphere->GetDynamicCubemapTexSize());

         // set the new projection
         mProjMat.MakePerspective(90.0f, 1.0f, mpSphere->GetRadius(), 500.0f);
         glMatrixMode(GL_PROJECTION);
         glLoadMatrixf(mProjMat);
         // set the initial position of the camera
         mViewMat.MakeLookAt(Vec3f(0.0f, 0.0f, 0.0f),
                             mViewMat.Inverse() * Vec3f(0.0f, 0.0f, 0.0f),
                             Vec3f(0.0f, -1.0f, 0.0f));
         glMatrixMode(GL_MODELVIEW);
         glLoadMatrixf(mViewMat);

         // create an array of faces to render
         CSphere::CubeMapEnumType nFace[] = { CSphere::CM_POS_Z,
                                              CSphere::CM_NEG_X,
                                              CSphere::CM_NEG_Z,
                                              CSphere::CM_POS_X,
                                              CSphere::CM_NEG_Y,
                                              CSphere::CM_POS_Y };

         // begin dynamic operations
         mpSphere->BeginDynamicOperations();

         // render all the first for faces
         for (int i = 0; i < 4; i++)
         {
            // render to the textures
            (this->*mpSkySphereDrawOrder)(elapsed_time, nFace[i],
                                          mBoxRotation, mBoxOrbitRotation);
            // rotate the camera
            mViewMat *= Matrixf::Rotate(-90.0f, MatrixHelper::GetUpVector(mViewMat).UnitVector());
            glLoadMatrixf(mViewMat);
         }

         // pitch the camera 90 degrees
         mViewMat *= Matrixf::Rotate(-90.0f, MatrixHelper::GetStrafeVector(mViewMat).UnitVector());
         glLoadMatrixf(mViewMat);
         // render to the texture
         (this->*mpSkySphereDrawOrder)(elapsed_time, nFace[4],
                                       mBoxRotation, mBoxOrbitRotation);

         // pitch the camera 180 degrees
         mViewMat *= Matrixf::Rotate(180.0f, MatrixHelper::GetStrafeVector(mViewMat).UnitVector());
         glLoadMatrixf(mViewMat);
         // render to the texture
         (this->*mpSkySphereDrawOrder)(elapsed_time, nFace[5],
                                       mBoxRotation, mBoxOrbitRotation);

         // end dynamic operations
         mpSphere->EndDynamicOperations();

         // restore the viewport
         glViewport(0, 0, view_size.width, view_size.height);
         // restore the camera
         mProjMat = proj_mat_save;
         glMatrixMode(GL_PROJECTION);
         glLoadMatrixf(mProjMat);

         mViewMat = view_mat_save;
         glMatrixMode(GL_MODELVIEW);
         glLoadMatrixf(mViewMat);
      }

      // render the sky box and sphere
      mpSkyBox->Render(elapsed_time);
      mpSphere->Draw(elapsed_time);
      // render the crate
      RenderCrate(mBoxRotation, mBoxOrbitRotation);
   }

   // save the projection matrix
   Matrixf proj_mat_save = mProjMat;

   // switch the projection matrix for window overlay rendering
   mProjMat.MakeOrtho(0.0f, static_cast< float >(GetSize().width),
                      0.0f, static_cast< float >(GetSize().height),
                      -1.0f, 1.0f);
   glMatrixMode(GL_PROJECTION);
   glLoadMatrixf(mProjMat);
   glMatrixMode(GL_MODELVIEW);

   // push a matrix
   glPushMatrix();
   // load an identity matrix
   glLoadIdentity();
   // disable the depth test
   glDisable(GL_DEPTH_TEST);

   // draw the text
   mpFPS->Draw(elapsed_time);

   if (mDisplayText)
   {
      mpInfo->Draw(elapsed_time);
      mpHelp->Draw(elapsed_time);
   }

   // enable depth testing
   glEnable(GL_DEPTH_TEST);
   // pop a matrix
   glPopMatrix();

   // restore the projection matrix
   mProjMat = proj_mat_save;

   // revert the camera projection
   glMatrixMode(GL_PROJECTION);
   glLoadMatrixf(mProjMat);
   glMatrixMode(GL_MODELVIEW);

   // make sure rendering is good
   assert(glGetError() == GL_NO_ERROR);
}

void CubeMapWindow::OnKeyPressed( const char key )
{
   // local(s)
   bool bCalcInfo = false;

   // determine the type of key pressed
   switch (key)
   {
   case 'd':
   case 'a':
      // move the camera right or left
      // take the inverse of the view matrix to get into world space
      mViewMat.MakeInverse();

      // strafe translate based on the current view matrix
      mViewMat = (mViewMat * Matrixf::Translate(key == 'a' ? -0.05f : 0.05f, 0.0f, 0.0f)).Inverse();

      // load the matrix
      glLoadMatrixf(mViewMat);

      break;

   case 'w':
   case 's':
      // take the inverse of the view matrix to get into world space
      mViewMat.MakeInverse();

      // view translate based on the current view matrix
      mViewMat = (mViewMat * Matrixf::Translate(0.0f, 0.0f, key == 'w' ? -0.05f : 0.05f)).Inverse();

      // load the matrix
      glLoadMatrixf(mViewMat);

      break;

   case 't':
      // increases the sphere radius
      mpSphere->ConstructSphere(mpSphere->GetRadius() + 0.05f,
                                mpSphere->GetSlices(),
                                mpSphere->GetStacks());

      // set the flag
      bCalcInfo = true;

      break;

   case 'g':
      // decreases the sphere radius
      if(mpSphere->GetRadius() > 0.5)
      {
         mpSphere->ConstructSphere(mpSphere->GetRadius() - 0.05f,
                                   mpSphere->GetSlices(),
                                   mpSphere->GetStacks());

         // set the flag
         bCalcInfo = true;
      }

      break;

   case 'y':
      // increase stacks
      mpSphere->ConstructSphere(mpSphere->GetRadius(),
                                mpSphere->GetSlices(),
                                mpSphere->GetStacks() + 1);

      // set the flag
      bCalcInfo = true;

      break;

   case 'h':
      // decrease stacks
      mpSphere->ConstructSphere(mpSphere->GetRadius(),
                                mpSphere->GetSlices(),
                                mpSphere->GetStacks() - 1);

      // set the flag
      bCalcInfo = true;

      break;

   case 'u':
      // increase slices
      mpSphere->ConstructSphere(mpSphere->GetRadius(),
                                mpSphere->GetSlices() + 1,
                                mpSphere->GetStacks());

      // set the flag
      bCalcInfo = true;

      break;

   case 'j':
      // decrease slices
      mpSphere->ConstructSphere(mpSphere->GetRadius(),
                                mpSphere->GetSlices() - 1,
                                mpSphere->GetStacks());

      // set the flag
      bCalcInfo = true;

      break;

   case 'r':
   {
      // changes the reflection type
      switch(mpSphere->GetReflectionType())
      {
      case CSphere::REFLECT_STATIC:
         mpSphere->SetReflectionType(CSphere::REFLECT_DYNAMIC_COPY_PIX);
         mpSkySphereDrawOrder = &CubeMapWindow::DrawSkyboxThenSphere;

         break;

      case CSphere::REFLECT_DYNAMIC_COPY_PIX:
         mpSphere->SetReflectionType(CSphere::REFLECT_DYNAMIC_PBUFFER);
         mpSkySphereDrawOrder = &CubeMapWindow::DrawSphereThenSkybox;

         break;

      case CSphere::REFLECT_DYNAMIC_PBUFFER:
         mpSphere->SetReflectionType(CSphere::REFLECT_DYNAMIC_RBUFFER);
         mpSkySphereDrawOrder = &CubeMapWindow::DrawSphereThenSkybox;

         break;

      case CSphere::REFLECT_DYNAMIC_RBUFFER:
         mpSphere->SetReflectionType(CSphere::REFLECT_STATIC);

         break;
      }

      // set the flag
      bCalcInfo = true;
   }

      break;

   case 'f':
   {
      // determine the new draw mode
      CSphere::DrawType nDrawType = (CSphere::DrawType)(mpSphere->GetDrawType() + 1);

      // determine if a wrap is needed
      if(nDrawType == CSphere::DRAW_MAX_TYPE) nDrawType = CSphere::DRAW_FILL;

      // set the new draw mode
      mpSphere->SetDrawType(nDrawType);

      // set the flag
      bCalcInfo = true;
   }

      break;

   case 'i':
      // increase the dyn cubemap size
      mpSphere->IncreaseDynCubemapTexSize();

      // set the flag
      bCalcInfo = true;

      break;

   case 'k':
      // decrease the dyn cubemap size
      mpSphere->DecreaseDynCubemapTexSize();

      // set the flag
      bCalcInfo = true;

      break;

   case 'o':
      // increase cubemap update time
      mUpdateTimeDelta += 0.00025;
      // set the flag
      bCalcInfo = true;

      break;

   case 'l':
      // decreases cubemap update time
      mUpdateTimeDelta -= 0.00025;
      // set the flag
      bCalcInfo = true;

      if(mUpdateTimeDelta < 0.0) mUpdateTimeDelta = 0.0;

      break;

   case 'z':
      // hide the text
      mDisplayText = !mDisplayText;
      // set the flag
      bCalcInfo = true;

      break;
   }
   
   if (bCalcInfo)
   {
      // create a string stream
      std::stringstream ssInfo;
   
      // add the information
      ssInfo << "Sphere Radius         = " << mpSphere->GetRadius() << "\n";
      ssInfo << "Sphere Slice          = " << mpSphere->GetSlices() << "\n";
      ssInfo << "Sphere Stack          = " << mpSphere->GetStacks() << "\n";
   
      // determine the draw mode
      ssInfo << "Sphere Draw Mode      = ";
      
      switch (mpSphere->GetDrawType())
      {
      case CSphere::DRAW_FILL:
         ssInfo << "Fill\n";
   
         break;
   
      case CSphere::DRAW_WIRE:
         ssInfo << "Wire\n";
   
         break;
   
      case CSphere::DRAW_TEX_QUAD:
         ssInfo << "Quads\n";
   
         break;
      }
      
      // determine the reflection type
      ssInfo << "Sphere Reflection     = ";
   
      switch (mpSphere->GetReflectionType())
      {
      case CSphere::REFLECT_STATIC:
         ssInfo << "Static\n";
   
         break;
   
      case CSphere::REFLECT_DYNAMIC_COPY_PIX:
         ssInfo << "Dynamic Cpy Pix\n";
   
         break;
   
      case CSphere::REFLECT_DYNAMIC_PBUFFER:
         ssInfo << "Dynamic PBuffer\n";
   
         break;
   
      case CSphere::REFLECT_DYNAMIC_RBUFFER:
         ssInfo << "Dynamic Frame Buffer\n";
   
         break;
      }
   
      // set the update time
      ssInfo << "Sphere Update Time    = " << mUpdateTimeDelta << "\n";
   
      // determine if the image is dynamic
      if (mpSphere->GetReflectionType() != CSphere::REFLECT_STATIC)
      {
         // get the image size
         ssInfo << "Sphere Dynamic Img Sz = " << mpSphere->GetDynamicCubemapTexSize();
      }
   
      // copy to the info text
      *mpInfo = ssInfo.str().c_str();
   }
}

void CubeMapWindow::OnMouseMove( const int32_t delta_x,
                                 const int32_t delta_y,
                                 const uint32_t flags )
{
   if (flags & MK_LBUTTON)
   {
      if (delta_x)
      {
         // rotate the camera around the y world axis
         const Vec3f eye = Matrixf::Rotate(delta_x * 0.1f, 0.0f, 1.0f, 0.0f) * mViewMat.Inverse() * Vec3f();
         // set the new eye position
         // the ball will be at the center
         // the up vector is always (0, 1, 0)
         mViewMat.MakeLookAt(eye, Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f));
      }

      if (delta_y)
      {
         // determine the current pitch
         const Vec3f ypr = MatrixHelper::DecomposeYawPitchRollDeg(mViewMat);
         // determine the delta pitch
         float delta_pitch = delta_y * 0.1f;

         // make sure to stay within (-90.0, 90.0)
         if (ypr[1] + delta_pitch > 89.9f) delta_pitch = 89.9f - ypr[1];
         if (ypr[1] + delta_pitch < -89.9f) delta_pitch = -89.9f - ypr[1];

         // obtain the view matrix's strafe vector
         const Vec3f view_strafe = MatrixHelper::GetStrafeVector(mViewMat);
         // rotate teh camera around the strafe vector world axis
         const Vec3f eye = Matrixf::Rotate(delta_pitch, view_strafe.UnitVector()) * mViewMat.Inverse() * Vec3f();
         // set the new eye position
         // the ball will be at the center
         // the up vector is always (0, 1, 0)
         mViewMat.MakeLookAt(eye, Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f));
      }

      // send new matrix off to the card
      glLoadMatrixf(mViewMat);
   }
}

void CubeMapWindow::DrawSkyboxThenSphere( const double & rElapsedTime,
                                          CSphere::CubeMapEnumType nFace,
                                          float fBoxRotation, float fOrbitRotation )
{
   // render the skybox
   mpSkyBox->Render(rElapsedTime);
   // render the box
   RenderCrate(fBoxRotation, fOrbitRotation);
   // render the face
   mpSphere->RenderFace(nFace);
}

void CubeMapWindow::DrawSphereThenSkybox( const double & rElapsedTime,
                                          CSphere::CubeMapEnumType nFace,
                                          float fBoxRotation, float fOrbitRotation )
{
   // render the face
   mpSphere->RenderFace(nFace);
   // render the skybox
   mpSkyBox->Render(rElapsedTime);
   // render the box
   RenderCrate(fBoxRotation, fOrbitRotation);
}

void CubeMapWindow::InitCrate( )
{
   // load the crate texture
   const auto texture = ReadTexture< uint8_t >(".\\Images\\Crate.bmp", GL_BGRA);

   if (texture.pTexture)
   {
      // bind the texture
      glGenTextures(1, &mCrateTexID);
      glBindTexture(GL_TEXTURE_2D, mCrateTexID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, texture.width, texture.height, 0,
                   texture.format, texture.type, texture.pTexture.get());
      // enable auto generation of the mip maps
      glGenerateMipmap(GL_TEXTURE_2D);
      // set the texture parameters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      // no longer need the texture
      glBindTexture(GL_TEXTURE_2D, 0);
   }

   // create a display list
   mCrateDispID = glGenLists(1);

   // begin the display list
   glNewList(mCrateDispID, GL_COMPILE);

   // enable 2d textures
   glEnable(GL_TEXTURE_2D);

   // bind the texture
   glBindTexture(GL_TEXTURE_2D, mCrateTexID);

   // obtain the current texture mode
   int nTextureMode = 0;
   glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &nTextureMode);

   // set the texture mode to replace
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   // push a matrix
   glPushMatrix();

   glTranslatef(-1.0f, 0.0f, 0.0f);

   glBegin(GL_QUADS);
   glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -1.0f, -1.0f);
   glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, -1.0f,  1.0f);
   glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f,  1.0f,  1.0f);
   glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f,  1.0f, -1.0f);
   glEnd();

   // pop the matrix
   glPopMatrix();
   // push a matrix
   glPushMatrix();

   glTranslatef(1.0f, 0.0f, 0.0f);
   glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

   glBegin(GL_QUADS);
   glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -1.0f, -1.0f);
   glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, -1.0f,  1.0f);
   glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f,  1.0f,  1.0f);
   glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f,  1.0f, -1.0f);
   glEnd();

   // pop the matrix
   glPopMatrix();
   // push a matrix
   glPushMatrix();

   glTranslatef(0.0f, 0.0f, 1.0f);
   glRotatef(90, 0.0f, 1.0f, 0.0f);

   glBegin(GL_QUADS);
   glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -1.0f, -1.0f);
   glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, -1.0f,  1.0f);
   glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f,  1.0f,  1.0f);
   glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f,  1.0f, -1.0f);
   glEnd();

   // pop a matrix
   glPopMatrix();
   // push a matrix
   glPushMatrix();

   glTranslatef(0.0f, 0.0f, -1.0f);
   glRotatef(-90, 0.0f, 1.0f, 0.0f);

   glBegin(GL_QUADS);
   glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -1.0f, -1.0f);
   glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, -1.0f,  1.0f);
   glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f,  1.0f,  1.0f);
   glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f,  1.0f, -1.0f);
   glEnd();

   // pop a matrix
   glPopMatrix();
   // push a matrix
   glPushMatrix();

   glTranslatef(0.0f, 1.0f, 0.0f);
   glRotatef(-90, 0.0f, 0.0f, 1.0f);

   glBegin(GL_QUADS);
   glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -1.0f, -1.0f);
   glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, -1.0f,  1.0f);
   glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f,  1.0f,  1.0f);
   glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f,  1.0f, -1.0f);
   glEnd();

   // pop a matrix
   glPopMatrix();
   // push a matrix
   glPushMatrix();

   glTranslatef(0.0f, -1.0f, 0.0f);
   glRotatef(90, 0.0f, 0.0f, 1.0f);

   glBegin(GL_QUADS);
   glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -1.0f, -1.0f);
   glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, -1.0f,  1.0f);
   glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f,  1.0f,  1.0f);
   glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f,  1.0f, -1.0f);
   glEnd();

   // pop a matrix
   glPopMatrix();

   // replace the texture mode
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, nTextureMode);

   // disable textures
   glDisable(GL_TEXTURE_2D);

   // end the display list
   glEndList();
}

void CubeMapWindow::RenderCrate( const float box_rotation, const float box_orbit_rotation )
{
   // push a matrix on the stack
   glPushMatrix();

   // translate and rotate the stack
   glRotatef(box_orbit_rotation, 0.0f, 1.0f, 0.0f);
   glTranslatef(3.0f, 0.0, 0.0f);
   glRotatef(box_rotation, 1.0f, 1.0f, 1.0f);
   glScalef(0.25f, 0.25f, 0.25f);

   // render the box
   glCallList(mCrateDispID);

   // pop the matrix from the stack
   glPopMatrix();
}
