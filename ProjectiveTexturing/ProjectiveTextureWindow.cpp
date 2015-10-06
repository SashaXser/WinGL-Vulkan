// local includes
#include "ProjectiveTextureWindow.h"
#include "Timer.h"
#include "WglAssert.h"
#include "GeomHelper.h"
#include "ReadTexture.h"
#include "MatrixHelper.h"
#include "OpenGLExtensions.h"

// gl includes
#include "GL/glew.h"
#include <gl/gl.h>

// stl includes
#include <memory>
#include <cstdint>
#include <iterator>
#include <iostream>
#include <algorithm>

// global defines
#define VALIDATE_OPENGL() WGL_ASSERT(glGetError() == GL_NO_ERROR)

ProjectiveTextureWindow::ProjectiveTextureWindow( ) :
mDepthBias           ( 0.0003f ),
mDepthFrameBuffer    ( 0 ),
mRenderShadowMap     ( false ),
mpActiveMViewMat     ( &mCameraVariables.mMViewMat ),
mpSetupModeFuncPtr   ( &ProjectiveTextureWindow::SetupRenderSceneImmediateModeObjectSpace ),
mpRenderModeFuncPtr  ( &ProjectiveTextureWindow::RenderSceneImmediateMode )
{
   // output keyboard controls
   std::cout << "C - Switch between camera and light" << std::endl
             << "1 - Texture projection object space immediate mode" << std::endl
             << "2 - Texture projection eye space immediate mode" << std::endl
             << "3 - Texture projection object space using shader" << std::endl
             << "F - Render the depth buffer for case 3" << std::endl
             << "+ - Increase depth bias" << std::endl
             << "- - Decrease depth bias" << std::endl
             << "WSAD - Move camera or light forwards or backwards and strafe left or right" << std::endl
             << "Left Mouse Button - Enable movement of camera or light" << std::endl
             << "Esc - Quit application" << std::endl << std::endl << std::ends;
}

ProjectiveTextureWindow::~ProjectiveTextureWindow( )
{
   // gl should still be active
   WGL_ASSERT(wglGetCurrentContext() == GetGLContext());

   // release the framebuffer
   glDeleteFramebuffers(1, &mDepthFrameBuffer);
}

bool ProjectiveTextureWindow::Create( unsigned int nWidth,
                                      unsigned int nHeight,
                                      const char * pWndTitle,
                                      const void * pInitParams )
{
   // initialize 40 first, then 32 second, else nothing
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 0, true, true, false },
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
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
                                               static_cast< double >(mLogoTex.GetWidth()) /
                                               static_cast< double >(mLogoTex.GetHeight()),
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

      // create and setup the depth texture
      mDepthTex.GenerateTexture(GL_TEXTURE_2D, GL_DEPTH_COMPONENT32, mLogoTex.GetWidth(), mLogoTex.GetHeight(), GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

      // bind and setup some basic texture paramemters
      mDepthTex.Bind();
      mDepthTex.SetParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      mDepthTex.SetParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      mDepthTex.SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      mDepthTex.SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      mDepthTex.Unbind();

      // to check that the framebuffer attachment took,
      // the draw and read must be set to none, since
      // there is no color attachment...
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);

      // todo: turn framebuffer into an object
      // generate and bind the framebuffer
      glGenFramebuffers(1, &mDepthFrameBuffer);
      glBindFramebuffer(GL_FRAMEBUFFER, mDepthFrameBuffer);
      
      // bind the depth texture to the framebuffer object
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTex, 0);

      // do the check...
      const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

      // the framebuffer should be complete
      WGL_ASSERT(status == GL_FRAMEBUFFER_COMPLETE);

      if (status != GL_FRAMEBUFFER_COMPLETE)
      {
         // release the framebuffer
         glBindFramebuffer(GL_FRAMEBUFFER, 0);
         glDeleteFramebuffers(1, &mDepthFrameBuffer);

         return false;
      }

      // no longer needing the framebuffer at this point
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      // restore the default buffer operations
      glDrawBuffer(GL_BACK);
      glReadBuffer(GL_BACK);
      
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
   glEnable(GL_CULL_FACE);

   // set the shading model to be smooth
   glShadeModel(GL_SMOOTH);

   // set hit values
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

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

   // setup the shader
   mProjTexProg.AttachFile(GL_VERTEX_SHADER, "projective_texture.vert");
   mProjTexProg.AttachFile(GL_FRAGMENT_SHADER, "projective_texture.frag");
   mProjTexProg.Link();
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

   // enable texturing and bind the logo
   glEnable(GL_TEXTURE_2D);
   mLogoTex.Bind(GL_TEXTURE0);

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

   // disable texturing
   mLogoTex.Unbind();
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
   const Vec3d f1 = invLightMat * Vec3d(-1.0,  1.0,  1.0);
   const Vec3d f2 = invLightMat * Vec3d(-1.0, -1.0,  1.0);
   const Vec3d f3 = invLightMat * Vec3d( 1.0, -1.0,  1.0);
   const Vec3d f4 = invLightMat * Vec3d( 1.0,  1.0,  1.0);
   const Vec3d n1 = invLightMat * Vec3d(-1.0,  1.0, -1.0);
   const Vec3d n2 = invLightMat * Vec3d(-1.0, -1.0, -1.0);
   const Vec3d n3 = invLightMat * Vec3d( 1.0, -1.0, -1.0);
   const Vec3d n4 = invLightMat * Vec3d( 1.0,  1.0, -1.0);

   // obtain the light cam position
   const Vec3d lightPos = mLightVariables.mMViewMat.Inverse() * Vec3d(0.0, 0.0, 0.0);

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

void ProjectiveTextureWindow::RenderSceneWithShader( )
{
   // render the wall geometry
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

   const float fWallValuesColor[][4] =
   {
      // red bottom wall...
      { 1.0f, 0.0f, 0.0f, 1.0f },
      { 1.0f, 0.0f, 0.0f, 1.0f },
      { 1.0f, 0.0f, 0.0f, 1.0f },
      { 1.0f, 0.0f, 0.0f, 1.0f },
      // green left wall...
      { 0.0f, 1.0f, 0.0f, 1.0f },
      { 0.0f, 1.0f, 0.0f, 1.0f },
      { 0.0f, 1.0f, 0.0f, 1.0f },
      { 0.0f, 1.0f, 0.0f, 1.0f },
      // blue back wall...
      { 0.0f, 0.0f, 1.0f, 1.0f },
      { 0.0f, 0.0f, 1.0f, 1.0f },
      { 0.0f, 0.0f, 1.0f, 1.0f },
      { 0.0f, 0.0f, 1.0f, 1.0f }
   };

   // construct a box to present in the middle of the scene
   const GeomHelper::Shape box_shape = GeomHelper::ConstructBox(5, 5, 5);

   // construct the sphere to present around the box
   const GeomHelper::Shape sphere_shape = GeomHelper::ConstructSphere(30, 30, 1.0f);

   // obtain the viewport parameters
   const std::vector< GLint > viewport =
   [ ] ( )
   {
      GLint viewport[4] = { };
      glGetIntegerv(GL_VIEWPORT, viewport);

      return std::vector< GLint >(viewport, viewport + 4);
   }();

   // set the view port to match the logo texture
   glViewport(0, 0, mLogoTex.GetWidth(), mLogoTex.GetHeight());

   // do not allow the default color buffer to be rendered into
   glDrawBuffer(GL_NONE);
   glReadBuffer(GL_NONE);

   // bind the framebuffer object for rendering
   glBindFramebuffer(GL_FRAMEBUFFER, mDepthFrameBuffer);
   
   // clear the depth texture
   glClear(GL_DEPTH_BUFFER_BIT);

   // the light projection just does not have the right parameters
   // to convey the depth information... just a slightly different projection
   const Matrixd light_projection =
      Matrixd::Perspective(45.0,
                           static_cast< double >(mLogoTex.GetWidth()) / static_cast< double >(mLogoTex.GetHeight()),
                           0.1,
                           100.0);

   // todo: create a shader to handle the fixed function stuff
   // setup the projection and modelview matrices
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadMatrixd(light_projection);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadMatrixd(mLightVariables.mMViewMat);

   // enable the client state to render the walls
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(4, GL_FLOAT, 0, fWallValues);
   glDrawArrays(GL_QUADS, 0, 4);
   glDrawArrays(GL_QUADS, 4, 4);
   glDrawArrays(GL_QUADS, 8, 4);

   // update the modelview matrix to place the box
   glLoadMatrixd(mLightVariables.mMViewMat * Matrixd::Translate(0.0, 5.0, 0.0));

   // enable the client state and render the box
   glVertexPointer(3, GL_FLOAT, 0, &box_shape.vertices[0]);
   glDrawElements(box_shape.geom_type, static_cast< GLsizei >(box_shape.indices.size()), GL_UNSIGNED_INT, &box_shape.indices[0]);

   // enable the client state for the sphere
   glVertexPointer(3, GL_FLOAT, 0, &sphere_shape.vertices[0]);

   // sphere matrices
   const Matrixd sphere_translations[] =
   {
      Matrixd::Translate(0.0, 8.75, 0.0),
      Matrixd::Translate(0.0, 5.0, 3.75),
      Matrixd::Translate(3.75, 5.0, 0.0),
      Matrixd::Translate(0.0, 1.25, 0.0)
   };

   // render all the spheres
   std::for_each(sphere_translations, sphere_translations + sizeof(sphere_translations) / sizeof(*sphere_translations),
   [ this, &sphere_shape ] ( const Matrixd & translation )
   {
      // update the modelview matrix to place the sphere
      glLoadMatrixd(mLightVariables.mMViewMat * translation);

      // render the sphere
      glDrawElements(sphere_shape.geom_type, static_cast< GLsizei >(sphere_shape.indices.size()), GL_UNSIGNED_INT, &sphere_shape.indices[0]);
   });

   // disable the use of the client side vertex pointer
   glDisableClientState(GL_VERTEX_ARRAY);

   // stop rendering into the depth texture
   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   // restore rendering to the default color buffer
   glDrawBuffer(GL_BACK);
   glReadBuffer(GL_BACK);

   // restore the matrices
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
   
   // restore the viewport
   glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

   // enable the shader to render the scene with the depth texture
   mProjTexProg.Enable();

   // update the depth bias...
   // todo: does not need to be done each loop
   mProjTexProg.SetUniformValue("depth_bias", mDepthBias);

   // bind the texture
   mLogoTex.Bind(GL_TEXTURE0);
   mProjTexProg.SetUniformValue("logo_texture", static_cast< GLint >(mLogoTex.GetBoundSamplerID()));

   // bind the depth texture
   mDepthTex.Bind(GL_TEXTURE1);
   mProjTexProg.SetUniformValue("depth_texture", static_cast< GLint >(mDepthTex.GetBoundSamplerID()));

   // update shader uniforms for the walls
   mProjTexProg.SetUniformMatrix< 1, 4, 4 >("mvp_mat4", Matrixf(mCameraVariables.mProjMat * mCameraVariables.mMViewMat));
   mProjTexProg.SetUniformMatrix< 1, 4, 4 >("light_mvp_mat4", Matrixf(light_projection * mLightVariables.mMViewMat));

   // enable the required pointer information
   // going to use client state compat mode opengl here
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);

   // feed the location of the data
   glVertexPointer(4, GL_FLOAT, 0, fWallValues);
   glColorPointer(4, GL_FLOAT, 0, fWallValuesColor);

   // render the 3 walls
   glDrawArrays(GL_QUADS, 0, 4);
   glDrawArrays(GL_QUADS, 4, 4);
   glDrawArrays(GL_QUADS, 8, 4);

   // update shader uniforms for the box
   mProjTexProg.SetUniformMatrix< 1, 4, 4 >("mvp_mat4", Matrixf(mCameraVariables.mProjMat * mCameraVariables.mMViewMat * Matrixd::Translate(0.0, 5.0, 0.0)));
   mProjTexProg.SetUniformMatrix< 1, 4, 4 >("light_mvp_mat4", Matrixf(light_projection * mLightVariables.mMViewMat * Matrixd::Translate(0.0, 5.0, 0.0)));

   // disable the color array, as the box will not use it
   glDisableClientState(GL_COLOR_ARRAY);

   // enable normals as the box will use it
   glEnableClientState(GL_NORMAL_ARRAY);

   // set the box to the color yellow
   glColor3f(1.0f, 1.0f, 0.0f);

   // render the box
   glVertexPointer(3, GL_FLOAT, 0, &box_shape.vertices[0]);
   glNormalPointer(GL_FLOAT, 0, &box_shape.normals[0]);
   glDrawElements(box_shape.geom_type, static_cast< GLsizei >(box_shape.indices.size()), GL_UNSIGNED_INT, &box_shape.indices[0]);

   // enable the client state for the sphere
   glVertexPointer(3, GL_FLOAT, 0, &sphere_shape.vertices[0]);
   glNormalPointer(GL_FLOAT, 0, &sphere_shape.normals[0]);

   // defines colors for the spheres
   const uint8_t sphere_colors[][3] =
   {
      { 64, 128, 128 }, { 128, 255, 0 }, { 255, 128, 0 }, { 255, 0, 128 }
   };

   // render all the spheres
   std::for_each(sphere_translations, sphere_translations + sizeof(sphere_translations) / sizeof(*sphere_translations),
   [ this, &sphere_shape, &sphere_colors, &sphere_translations, &light_projection ] ( const Matrixd & translation )
   {
      // setup the colors
      const size_t distance = std::distance(sphere_translations, &translation);
      glColor3ubv(sphere_colors[distance]);

      // update shader uniforms for the spheres
      mProjTexProg.SetUniformMatrix< 1, 4, 4 >("mvp_mat4", Matrixf(mCameraVariables.mProjMat * mCameraVariables.mMViewMat * translation));
      mProjTexProg.SetUniformMatrix< 1, 4, 4 >("light_mvp_mat4", Matrixf(light_projection * mLightVariables.mMViewMat * translation));

      // render the sphere
      glDrawElements(sphere_shape.geom_type, static_cast< GLsizei >(sphere_shape.indices.size()), GL_UNSIGNED_INT, &sphere_shape.indices[0]);
   });

   // state is no longer required
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);

   // unbind the textures
   // note: mLogoTex is ubound last, as it sets the active texture back to 0
   // and allows any other fixed functions to correct be used for texturing...
   mDepthTex.Unbind();
   mLogoTex.Unbind();

   // disable the shader
   mProjTexProg.Disable();

   // render the spot light
   RenderSpotLightImmediateMode();

   if (mRenderShadowMap)
   {
      // update the projection and modelview matrices
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      glOrtho(0.0, 1.0, 0.0, 1.0, 1.0, -1.0);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();

      // update the texture environment to replace color values
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      // enable 2d texture support
      glEnable(GL_TEXTURE_2D);

      // bind the depth texture 
      mDepthTex.Bind();

      // match the apsect of the logo
      const Size window_size = GetSize();
      const float logo_aspect = static_cast< float >(mLogoTex.GetWidth()) / static_cast< float >(mLogoTex.GetHeight());
      const float window_height_pixels = 1.0f / logo_aspect * window_size.width * 0.5f;
      const float texture_height = window_height_pixels / window_size.height;

      // render the quad in the lower left corner of the screen
      glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.0f, 0.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, texture_height, 0.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, texture_height, 0.0f);
      glEnd();

      // unbind the texture
      mDepthTex.Unbind();

      // disable 2d texture support
      glDisable(GL_TEXTURE_2D);

      // restore the matrices
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
   }

   WGL_ASSERT(glGetError() == GL_NO_ERROR);
}

void ProjectiveTextureWindow::SetupRenderSceneImmediateModeEyeSpace( )
{
   // push the modelview matrix
   glPushMatrix();
   glLoadIdentity();

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

   // pop the modelview matrix
   glPopMatrix();
}

void ProjectiveTextureWindow::SetupRenderSceneImmediateModeObjectSpace( )
{
   // push the modelview matrix
   glPushMatrix();
   glLoadIdentity();

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

   // pop the modelview matrix
   glPopMatrix();
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
   if (mLogoTex.Load2D("BMLogo.rgb", GL_RGBA, GL_COMPRESSED_RGBA, true))
   {
      mLogoTex.Bind(GL_TEXTURE0);
      mLogoTex.SetParameter(GL_TEXTURE_WRAP_S, GL_CLAMP);
      mLogoTex.SetParameter(GL_TEXTURE_WRAP_T, GL_CLAMP);
      mLogoTex.Unbind();
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
         const int nXDelta = static_cast< int >(GetPreviousMousePosition().x) - nCurX;
         const int nYDelta = static_cast< int >(GetPreviousMousePosition().y) - nCurY;
         // decompose the current values from the modelview matrix
         double dYaw = 0.0, dPitch = 0.0;
         MatrixHelper::DecomposeYawPitchRollDeg(*mpActiveMViewMat,
                                                &dYaw, &dPitch, (double *)NULL);
         // add the deltas to the current yaw and pitch
         dYaw += nXDelta;
         dPitch += nYDelta;
         // make sure to cap pitch to plus or minus 90
         dPitch = std::min(dPitch, 89.9);
         dPitch = std::max(dPitch, -89.9);
         // create a rotation matrix centered
         // around the up and view direction vectors...
         Matrixd matYaw, matPitch;
         matYaw.MakeRotation(dYaw, 0.0, 1.0, 0.0);
         matPitch.MakeRotation(dPitch, 1.0, 0.0, 0.0);
         // obtain the camera world position...
         // take the inverse of the camera world position since
         // the camera must move things into the eye space...
         const Vec3d camEye = mpActiveMViewMat->InverseFromOrthogonal() * Vec3d(0.0, 0.0, 0.0);
         // create a translation matrix for the camera
         Matrixd matTrans;
         matTrans.MakeTranslation(camEye.mT[0], camEye.mT[1], camEye.mT[2]);
         // modify the camera matrix
         *mpActiveMViewMat = (matTrans * matYaw * matPitch).Inverse();
         
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
         // setup the render function
         mpRenderModeFuncPtr = &ProjectiveTextureWindow::RenderSceneImmediateMode;

         break;

      case '2':
         // process the setup functions
         mpSetupModeFuncPtr = &ProjectiveTextureWindow::SetupRenderSceneImmediateModeEyeSpace;
         SetupRenderSceneImmediateModeEyeSpace();
         // setup the render function
         mpRenderModeFuncPtr = &ProjectiveTextureWindow::RenderSceneImmediateMode;

         break;

      case '3':
         // blank the setup function
         mpSetupModeFuncPtr = nullptr;
         // setup the render function
         mpRenderModeFuncPtr = &ProjectiveTextureWindow::RenderSceneWithShader;

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
         {

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
         Vec3d camEye = mpActiveMViewMat->InverseFromOrthogonal() * Vec3d(0.0, 0.0, 0.0);
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
         matTrans.MakeTranslation(camEye.mT[0], camEye.mT[1], camEye.mT[2]);
         // multiply the matrix out
         *mpActiveMViewMat = (matTrans * matYaw * matPitch).Inverse();
         
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

         break;

      case 'F':
         // update the boolean to indicate rendering the depth texture
         mRenderShadowMap = !mRenderShadowMap;

         break;

      case VK_ADD: mDepthBias += 0.00005f; std::cout << "Depth bias : " << mDepthBias << std::endl; break;
      case VK_SUBTRACT: mDepthBias -= 0.00005f; std::cout << "Depth bias : " << mDepthBias << std::endl; break;
      }

      break;

   default:
      // pass to the default handler
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);

      break;
   }

   return result;
}