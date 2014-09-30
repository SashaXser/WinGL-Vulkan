// includes
#include "CFont.h"
#include "SkyBox.h"
#include "CSphere.h"
#include "StdIncludes.h"
#include "QuickGLWindow.h"

// stl includes
#include <sstream>

// crt includes
#include <assert.h>

// global typedefs
typedef void (* SkySphereRenderOrder)( const double & rElapsedTime,
                                       CSphere::CubeMapEnumType nFace,
                                       float fBoxRotation,
                                       float fOrbitRotation );

// global variables
bool              g_bDisplayText = true;
double            g_dUpdateTime = 0.005;
CFont *           g_pFPS = NULL;
CFont *           g_pHelp = NULL;
CFont *           g_pInfo = NULL;
SkyBox *          g_pSkyBox = NULL;
CSphere *         g_pSphere = NULL;
unsigned int      g_nCrateDispID = 0;
unsigned int      g_nCrateTexID = 0;
QuickGLWindow *   g_pGLWindow = NULL;

// window width and height
const float g_fWndWidth  = 1024.0f;
const float g_fWndHeight = 768.0f;

// global function ptrs
SkySphereRenderOrder g_pSkySphereDrawOrder = NULL;

// global functions for the draw order
void DrawSkyboxThenSphere( const double & rElapsedTime,
                           CSphere::CubeMapEnumType nFace,
                           float fBoxRotation, float fOrbitRotation );
void DrawSphereThenSkybox( const double & rElapsedTime,
                           CSphere::CubeMapEnumType nFace,
                           float fBoxRotation, float fOrbitRotation );

// callback methods
void ShutdownCallback( void * pClass );
void UpdateCallback( const double & rElapsedTime );
void DrawCallback( void * pCalss,
                   const double & rElapsedTime );
void MouseCallback( void * pClass,
                    const QuickMouse::MouseInput * pData,
                    const double & rElapsedTime );
void KeyboardCallback( void * pClass,
                       const QuickKeyboard::KeyboardInput * pData,
                       const double & rElapsedTime );

// crate functions
void InitCrate( );
void RenderCrate( float fBoxRotation,
                  float fBoxOrbitRotation );

// main app instance
int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nCmdShow )
{
   // determine fullscreen or not
   /*int nYesNo = MessageBox(NULL,
                           "Fullscreen Mode?",
                           "Fullscreen",
                           MB_YESNO);*/

   // create a new gl window
   g_pGLWindow = new QuickGLWindow(hInstance,
                                  (int)g_fWndWidth,
                                  (int)g_fWndHeight,
                                   "Cubemap",
                                   /*nYesNo == IDYES*/false);

   // register for callbacks
   g_pGLWindow->SetUpdateCallback(UpdateCallback);
   g_pGLWindow->SetDrawCallback(NULL, DrawCallback);
   g_pGLWindow->SetMouseCallback(NULL, MouseCallback);
   g_pGLWindow->SetKeyboardCallback(NULL, KeyboardCallback);
   g_pGLWindow->SetShutdownCallback(NULL, ShutdownCallback);

   // set the skybox / sphere draw order
   // using the pbuffer or the frame buffer draws
   // directly to the texture objects.  setting the
   // order determines how the face will be rendered.
   g_pSkySphereDrawOrder = DrawSkyboxThenSphere;

   // obtain a reference to the keyboard
   QuickKeyboard & rKeyboard = g_pGLWindow->GetKeyboard();

   // add keys to the keyboard
   rKeyboard.AddKey(DIK_A);         // moves camera left
   rKeyboard.AddKey(DIK_S);         // moves camera backwards
   rKeyboard.AddKey(DIK_D);         // moves camera right
   rKeyboard.AddKey(DIK_W);         // moves camera forward
   rKeyboard.AddKey(DIK_T);         // increases sphere radius
   rKeyboard.AddKey(DIK_G);         // decreases sphere radius
   rKeyboard.AddKey(DIK_Y);         // increase stacks
   rKeyboard.AddKey(DIK_H);         // decrease stacks
   rKeyboard.AddKey(DIK_U);         // increase slices
   rKeyboard.AddKey(DIK_J);         // decrease slices
   rKeyboard.AddKey(DIK_R);         // changes the reflection type
   rKeyboard.AddKey(DIK_F);         // changes the between fill and lines
   rKeyboard.AddKey(DIK_I);         // increases the dynamic cubemap size
   rKeyboard.AddKey(DIK_K);         // decreases the dynamic cubemap size
   rKeyboard.AddKey(DIK_O);         // increases the update time for the maps
   rKeyboard.AddKey(DIK_L);         // decreases the update time for the maps
   rKeyboard.AddKey(DIK_Z);         // hides / displays the text
   rKeyboard.AddKey(DIK_ESCAPE);    // quits applicaiton

   // obtain the camera
   Camera & rCamera = g_pGLWindow->GetCamera();

   // set the orthographic attributes
   rCamera.SetOrthographic(0.0f, g_fWndWidth,
                           0.0f, g_fWndHeight,
                           -1.0f, 1.0f);

   // set the perspective attributes
   rCamera.SetPerspective(Angle(AngleType::DEGREES, 45.0),
                          g_fWndWidth / g_fWndHeight,
                          1.0f,
                          1000.0f);

   // set the cameras position
   rCamera.SetCamera(Vector(0.0f, 0.0f, 5.0f),
                     Vector(0.0f, 0.0f, 0.0f),
                     Vector(0.0f, 1.0f, 0.0f));

   // define a local set of images
   const char * pImageFiles[6] =
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
   g_pSphere = new CSphere(g_pGLWindow->GetWindow(),
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
   g_pSkyBox = new SkyBox;
   // initialize the images
   g_pSkyBox->InitImages(*(pImageFiles),
                         *(pImageFiles + 1),
                         *(pImageFiles + 2),
                         *(pImageFiles + 3),
                         *(pImageFiles + 4),
                         *(pImageFiles + 5));

   // create a new font
   g_pFPS = new CFont(0,
                      Vector(g_fWndWidth * 0.5f, g_fWndHeight, 0.0f),
                      ".\\Images\\Font.tga",
                      20, 25,
                      "0 FPS",
                      1.0f,
                      CFont::ALIGN_CENTER);
   // create a new font
   g_pInfo = new CFont(0,
                       Vector(0.0f, g_fWndHeight, 0.0f),
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
   g_pHelp = new CFont(0,
                       Vector(g_fWndWidth, g_fWndHeight, 0.0f),
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

   // begin main loop
   g_pGLWindow->Run();

   // release the window
   delete g_pGLWindow;
   
   return 0;
}

void ShutdownCallback( void * pClass )
{
   // release the display list
   glDeleteLists(g_nCrateDispID, 1);

   // release resources
   delete g_pFPS;
   delete g_pInfo;
   delete g_pHelp;
   delete g_pSkyBox;
   delete g_pSphere;
}

// called to handle updates
void UpdateCallback( const double & rElapsedTime )
{
   // local(s)
   static double dElapsedTime = 1.0;
   static unsigned int nFrames = 0;

   // increase the number of frames
   nFrames++;
   // decrease the time
   dElapsedTime -= rElapsedTime;

   // determine if the time has elapsed
   if (dElapsedTime <= 0.0)
   {
      // set the string
      std::stringstream ssFPS; ssFPS << nFrames << " FPS";
      // reset the static local variables
      nFrames = 0;
      dElapsedTime = 1.0;
      // update the text shape
      *g_pFPS = ssFPS.str().c_str();
   }

   //// light position
   //static float fLightPos[] = { 0.0f, 0.0f, 10.0f, 1.0f };
   //static float fLightDir[] = { 0.0f, 0.0f, -1.0f };

   //// set the direcitonal light
   //glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0f);
   //glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);
   //glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, fLightDir);

   // update the sphere
   g_pSphere->Update(rElapsedTime, g_pGLWindow->GetCamera().GetEyePosition());
}

// called to handle drawing
void DrawCallback( void * pClass,
                   const double & rElapsedTime )
{
   // values for the crate
   static float fBoxRotation = 0.0f;
   static float fBoxOrbitRotation = 0.0f;

   // update the rotation values
   fBoxRotation += (90.0f * (float)rElapsedTime);
   fBoxOrbitRotation += (40.0f * (float)rElapsedTime);

   // obtain a pointer to the camera
   Camera & rCam = g_pGLWindow->GetCamera();

   if (g_pSphere->GetReflectionType() & CSphere::REFLECT_STATIC)
   {
      // render the skybox then the sphere
      g_pSkyBox->Render(rElapsedTime);
      g_pSphere->Draw(rElapsedTime);
      // render the crate
      RenderCrate(fBoxRotation, fBoxOrbitRotation);
   }
   else
   {
      // determines the amount of time until an update
      static double dUpdateMaps = rElapsedTime;
      // update the time stamp
      dUpdateMaps -= rElapsedTime;

      if (dUpdateMaps <= 0.0)
      {
         // reset the time
         dUpdateMaps = g_dUpdateTime;

         // dynamic rendering of the sphere
         // save the current projection and model view matrix
         float fModelView[16];
         // get the model view and projection matrices
         glGetFloatv(GL_MODELVIEW_MATRIX, fModelView);
         // save a copy of the current camera
         Camera camSave = rCam;
         // save the perspective attributes
         Camera::Perspective camPerspective = rCam.GetPerspectiveAttributes();

         // save the viewport settings
         unsigned int nViewport[4];
         // get the current viewport settings
         g_pGLWindow->GetViewport(nViewport);
         // change the current viewport settings
         g_pGLWindow->SetViewport(0, 0,
                                  g_pSphere->GetDynamicCubemapTexSize(),
                                  g_pSphere->GetDynamicCubemapTexSize());

         // set the new projection
         rCam.SetPerspective(Angle(AngleType::DEGREES, 90.0f),
                             1.0f,
                             g_pSphere->GetRadius(),
                             camPerspective.m_fFar);
         // set the initial position of the camera
         rCam.SetCamera(Vector(0, 0, 0),
                        rCam.GetEyePosition(),
                        Vector(0, -1, 0));

         // create a 90 and 180 degree angles
         Angle a90Rotate(AngleType::DEGREES, 90.0f);
         Angle a180Rotate(AngleType::DEGREES, 180.0f);
         // convert to radians
         a90Rotate.SetUnit(AngleType::RADIANS);
         a180Rotate.SetUnit(AngleType::RADIANS);

         // create an array of faces to render
         CSphere::CubeMapEnumType nFace[] = { CSphere::CM_POS_Z,
                                              CSphere::CM_NEG_X,
                                              CSphere::CM_NEG_Z,
                                              CSphere::CM_POS_X,
                                              CSphere::CM_NEG_Y,
                                              CSphere::CM_POS_Y };

         // begin dynamic operations
         g_pSphere->BeginDynamicOperations();

         // render all the first for faces
         for (int i = 0; i < 4; i++)
         {
            // render to the textures
            g_pSkySphereDrawOrder(rElapsedTime, nFace[i],
                                  fBoxRotation, fBoxOrbitRotation);
            // rotate the camera
            rCam.YawRotation(a90Rotate);
         }

         // pitch the camera 90 degrees
         rCam.PitchRotation(a90Rotate);
         // render to the texture
         g_pSkySphereDrawOrder(rElapsedTime, nFace[4],
                               fBoxRotation, fBoxOrbitRotation);

         // pitch the camera 180 degrees
         rCam.PitchRotation(a180Rotate);
         // render to the texture
         g_pSkySphereDrawOrder(rElapsedTime, nFace[5],
                               fBoxRotation, fBoxOrbitRotation);

         // end dynamic operations
         g_pSphere->EndDynamicOperations();

         // restore the viewport
         g_pGLWindow->SetViewport(nViewport);
         // restore the camera
         rCam = camSave;
         // restore the projection matrix
         rCam.SetPerspective(camPerspective);
         // restore the model view matrix
         glMatrixMode(GL_MODELVIEW);
         glLoadMatrixf(fModelView);
      }

      // render the sky box and sphere
      g_pSkyBox->Render(rElapsedTime);
      g_pSphere->Draw(rElapsedTime);
      // render the crate
      RenderCrate(fBoxRotation, fBoxOrbitRotation);
   }

   // switch the camera projection
   rCam.SwitchProjections();

   // push a matrix
   glPushMatrix();
   // load an identity matrix
   glLoadIdentity();
   // disable the depth test
   glDisable(GL_DEPTH_TEST);

   // draw the text
   g_pFPS->Draw(rElapsedTime);

   if (g_bDisplayText)
   {
      g_pInfo->Draw(rElapsedTime);
      g_pHelp->Draw(rElapsedTime);
   }

   // enable depth testing
   glEnable(GL_DEPTH_TEST);
   // pop a matrix
   glPopMatrix();

   // revert the camera projection
   rCam.SwitchProjections();

   // make sure rendering is good
   assert(glGetError() == GL_NO_ERROR);
}

// handles keyboard input
void KeyboardCallback( void * pClass,
                       const QuickKeyboard::KeyboardInput * pData,
                       const double & rElapsedTime )
{
   // local typedefs
   typedef QuickKeyboard::KeyboardInput::const_iterator ConstIt;

   // local(s)
   bool bCalcInfo = false;

   // time keeper
   static double dTime = 0.0;

   for (ConstIt itBegin = pData->begin(),
                itEnd = pData->end();
        itBegin != itEnd;
        itBegin++)
   {
      // obtain the character
      int nKey = *itBegin;

      if (nKey == DIK_T || nKey == DIK_G ||
          nKey == DIK_Y || nKey == DIK_H ||
          nKey == DIK_U || nKey == DIK_J ||
          nKey == DIK_R || nKey == DIK_F ||
          nKey == DIK_I || nKey == DIK_K ||
          nKey == DIK_O || nKey == DIK_L ||
          nKey == DIK_Z)
      {
         // decrease the time
         dTime -= rElapsedTime;
         
         // make sure time has not expired
         if (dTime <= 0.0) dTime = 0.1;
         else continue;
      }

      // determine the type of key pressed
      switch (nKey)
      {
      case DIK_D:
         // move the camera right
         g_pGLWindow->GetCamera().TranslateX(1.0f * (float)rElapsedTime);

         break;

      case DIK_S:
         // move the camera back
         g_pGLWindow->GetCamera().TranslateZ(1.0f * (float)rElapsedTime);

         break;

      case DIK_W:
         // move the camera forward
         g_pGLWindow->GetCamera().TranslateZ(-1.0f * (float)rElapsedTime);

         break;

      case DIK_A:
         // move the camera left
         g_pGLWindow->GetCamera().TranslateX(-1.0f * (float)rElapsedTime);

         break;

      case DIK_T:
         // increases the sphere radius
         g_pSphere->ConstructSphere(g_pSphere->GetRadius() + 0.05f,
                                    g_pSphere->GetSlices(),
                                    g_pSphere->GetStacks());

         // set the flag
         bCalcInfo = true;

         break;

      case DIK_G:
         // decreases the sphere radius
         if (g_pSphere->GetRadius() > 0.5)
         {
            g_pSphere->ConstructSphere(g_pSphere->GetRadius() - 0.05f,
                                       g_pSphere->GetSlices(),
                                       g_pSphere->GetStacks());

            // set the flag
            bCalcInfo = true;
         }

         break;

      case DIK_Y:
         // increase stacks
         g_pSphere->ConstructSphere(g_pSphere->GetRadius(),
                                    g_pSphere->GetSlices(),
                                    g_pSphere->GetStacks() + 1);

         // set the flag
         bCalcInfo = true;

         break;

      case DIK_H:
         // decrease stacks
         g_pSphere->ConstructSphere(g_pSphere->GetRadius(),
                                    g_pSphere->GetSlices(),
                                    g_pSphere->GetStacks() - 1);

         // set the flag
         bCalcInfo = true;

         break;

      case DIK_U:
         // increase slices
         g_pSphere->ConstructSphere(g_pSphere->GetRadius(),
                                    g_pSphere->GetSlices() + 1,
                                    g_pSphere->GetStacks());

         // set the flag
         bCalcInfo = true;

         break;

      case DIK_J:
         // decrease slices
         g_pSphere->ConstructSphere(g_pSphere->GetRadius(),
                                    g_pSphere->GetSlices() - 1,
                                    g_pSphere->GetStacks());

         // set the flag
         bCalcInfo = true;

         break;

      case DIK_R:
         {
            // changes the reflection type
            switch (g_pSphere->GetReflectionType())
            {
            case CSphere::REFLECT_STATIC:
               g_pSphere->SetReflectionType(CSphere::REFLECT_DYNAMIC_COPY_PIX);
               g_pSkySphereDrawOrder = DrawSkyboxThenSphere;

               break;

            case CSphere::REFLECT_DYNAMIC_COPY_PIX:
               g_pSphere->SetReflectionType(CSphere::REFLECT_DYNAMIC_PBUFFER);
               g_pSkySphereDrawOrder = DrawSphereThenSkybox;

               break;

            case CSphere::REFLECT_DYNAMIC_PBUFFER:
               g_pSphere->SetReflectionType(CSphere::REFLECT_DYNAMIC_RBUFFER);
               g_pSkySphereDrawOrder = DrawSphereThenSkybox;

               break;

            case CSphere::REFLECT_DYNAMIC_RBUFFER:
               g_pSphere->SetReflectionType(CSphere::REFLECT_STATIC);

               break;
            }

            // set the flag
            bCalcInfo = true;
         }

         break;

      case DIK_F:
         {
            // determine the new draw mode
            CSphere::DrawType nDrawType = (CSphere::DrawType)(g_pSphere->GetDrawType() + 1);

            // determine if a wrap is needed
            if (nDrawType == CSphere::DRAW_MAX_TYPE) nDrawType = CSphere::DRAW_FILL;

            // set the new draw mode
            g_pSphere->SetDrawType(nDrawType);

            // set the flag
            bCalcInfo = true;
         }

         break;

      case DIK_I:
         // increase the dyn cubemap size
         g_pSphere->IncreaseDynCubemapTexSize();

         // set the flag
         bCalcInfo = true;

         break;

      case DIK_K:
         // decrease the dyn cubemap size
         g_pSphere->DecreaseDynCubemapTexSize();

         // set the flag
         bCalcInfo = true;

         break;

      case DIK_O:
         // increase cubemap update time
         g_dUpdateTime += 0.00025;
         // set the flag
         bCalcInfo = true;

         break;

      case DIK_L:
         // decreases cubemap update time
         g_dUpdateTime -= 0.00025;
         // set the flag
         bCalcInfo = true;

         if (g_dUpdateTime < 0.0) g_dUpdateTime = 0.0;

         break;

      case DIK_Z:
         // hide the text
         g_bDisplayText = !g_bDisplayText;
         // set the flag
         bCalcInfo = true;

         break;

      case DIK_ESCAPE:
         // shutdown the app
         g_pGLWindow->ShutdownApp();

         break;
      }
   }

   if (bCalcInfo)
   {
      // create a string stream
      std::stringstream ssInfo;

      // add the information
      ssInfo << "Sphere Radius         = " << g_pSphere->GetRadius() << "\n";
      ssInfo << "Sphere Slice          = " << g_pSphere->GetSlices() << "\n";
      ssInfo << "Sphere Stack          = " << g_pSphere->GetStacks() << "\n";

      // determine the draw mode
      ssInfo << "Sphere Draw Mode      = ";
      
      switch (g_pSphere->GetDrawType())
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

      switch (g_pSphere->GetReflectionType())
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
      ssInfo << "Sphere Update Time    = " << g_dUpdateTime << "\n";

      // determine if the image is dynamic
      if (g_pSphere->GetReflectionType() != CSphere::REFLECT_STATIC)
      {
         // get the image size
         ssInfo << "Sphere Dynamic Img Sz = " << g_pSphere->GetDynamicCubemapTexSize();
      }

      // copy to the info text
      *g_pInfo = ssInfo.str().c_str();
   }
}

// handles mouse input
void MouseCallback( void * pClass,
                    const QuickMouse::MouseInput * pData,
                    const double & rElapsedTime )
{
   // local typedefs
   typedef QuickMouse::MouseInput::const_iterator ConstIt;

   // traverse the mouse input
   for (ConstIt itBegin = pData->begin(),
                itEnd = pData->end();
        itBegin != itEnd;
        itBegin++)
   {
      switch (itBegin->nType)
      {
      case QuickMouse::QMBT_XAXIS:
         // rotate camera around y
         {
            // create a local rotation matrix
            Matrix mRotY;
            // create the values
            MATRIX_ROTATE_Y(mRotY, Angle(AngleType::DEGREES, 90.0f * (float)rElapsedTime * (float)itBegin->Data.unAxis));
            // determine the new eye position
            Vector vEye = mRotY * g_pGLWindow->GetCamera().GetEyePosition();
            // set the new eye position
            // the ball will be at the center
            // the up vector is always (0, 1, 0)
            g_pGLWindow->GetCamera().SetCamera(vEye,
                                               Vector(0.0f, 0.0f, 0.0f),
                                               Vector(0.0f, 1.0f, 0.0f));
         }

         break;

      case QuickMouse::QMBT_YAXIS:
         // rotate camera around the cross product from the eye to the up vector
         {
            // local(s)
            Vector vUp(0.0f, 1.0f, 0.0f);
            Matrix mRotCross;
            // obtain the camera's eye position
            Vector vEyePos = g_pGLWindow->GetCamera().GetEyePosition();
            // determine the old angle from the eye and the up vector
            float fOldDot = vEyePos.UnitVector() * vUp;
            // get the cross between the eye and the up vector
            Vector vCross = vEyePos ^ vUp;
            // create the values
            MATRIX_ROTATE_XYZ(mRotCross,
                              vCross, 
                              Angle(AngleType::DEGREES, -90.0f * (float)rElapsedTime * (float)itBegin->Data.unAxis));
            // determine the new eye position
            Vector vEye = mRotCross * vEyePos;
            // make sure the new eye does not conflict with the up vector
            float fDot = vEye.UnitVector() * vUp;

            // stop the eye at 0.99 from the up or down vectors
            if (fabs(fDot) > 0.99f)
            {
               // determine the rotation direction
               float fDir = (float)itBegin->Data.unAxis > 0.0f ? -1.0f : 1.0f;
               // determine the offset angle
               fOldDot = 0.99f - fabs(fOldDot);
               // convert the old dot to radians
               Angle aRotation(AngleType::RADIANS, asin(fOldDot) * fDir);
               // rotate the matrix
               MATRIX_ROTATE_XYZ(mRotCross, vCross, aRotation);
               // create the new eye position
               vEye = mRotCross * vEyePos;
            }

            // set the new eye position
            // the ball will be at the center
            // the up vector is always (0, 1, 0)
            g_pGLWindow->GetCamera().SetCamera(vEye,
                                               Vector(0.0f, 0.0f, 0.0f),
                                               Vector(0.0f, 1.0f, 0.0f));
         }

         break;

      case QuickMouse::QMBT_ZAXIS:
         // rotate camera around z
         g_pGLWindow->GetCamera().RollRotation(Angle(AngleType::DEGREES,
                                                     45.0f * (float)rElapsedTime * (float)itBegin->Data.unAxis));

         break;
      }
   }
}

void DrawSkyboxThenSphere( const double & rElapsedTime,
                           CSphere::CubeMapEnumType nFace,
                           float fBoxRotation, float fOrbitRotation )
{
   // render the skybox
   g_pSkyBox->Render(rElapsedTime);
   // render the box
   RenderCrate(fBoxRotation, fOrbitRotation);
   // render the face
   g_pSphere->RenderFace(nFace);
}

void DrawSphereThenSkybox( const double & rElapsedTime,
                           CSphere::CubeMapEnumType nFace,
                           float fBoxRotation, float fOrbitRotation )
{
   // render the face
   g_pSphere->RenderFace(nFace);
   // render the skybox
   g_pSkyBox->Render(rElapsedTime);
   // render the box
   RenderCrate(fBoxRotation, fOrbitRotation);
}

void InitCrate( )
{
   // load the crate texture
   const ImageLibrary::Image * pImage = ImageLibrary::Instance()->Load(".\\Images\\Crate.bmp");

   if (pImage)
   {
      // bind the texture
      glBindTexture(GL_TEXTURE_2D, pImage->m_unImageID);
      // set the texture parameters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   }

   // create a display list
   g_nCrateDispID = glGenLists(1);

   // begin the display list
   glNewList(g_nCrateDispID, GL_COMPILE);

   // enable 2d textures
   glEnable(GL_TEXTURE_2D);

   // bind the texture
   glBindTexture(GL_TEXTURE_2D, pImage->m_unImageID);

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

void RenderCrate( float fBoxRotation,
                  float fBoxOrbitRotation )
{
   // push a matrix on the stack
   glPushMatrix();

   // translate and rotate the stack
   glRotatef(fBoxOrbitRotation, 0.0f, 1.0f, 0.0f);
   glTranslatef(3.0f, 0.0, 0.0f);
   glRotatef(fBoxRotation, 1.0f, 1.0f, 1.0f);
   glScalef(0.25f, 0.25f, 0.25f);

   // render the box
   glCallList(g_nCrateDispID);

   // pop the matrix from the stack
   glPopMatrix();
}