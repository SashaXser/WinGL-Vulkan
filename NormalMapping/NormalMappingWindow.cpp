// local includes
#include "NormalMappingWindow.h"

// wgl includes
#include "Texture.h"
#include "MathHelper.h"
#include "MatrixHelper.h"
#include "ShaderProgram.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

// gl includes
#include <GL/glew.h>
#include <GL/GL.h>

// std includes
#include <string>
#include <vector>
#include <cstring>

NormalMappingWindow::NormalMappingWindow( ) :
mpShader                ( nullptr ),
mpDiffuseTex            ( nullptr ),
mpHeightTex             ( nullptr ),
mpNormalTex             ( nullptr ),
mpWallVAO               ( nullptr ),
mpWallVerts             ( nullptr ),
mpWallNorms             ( nullptr ),
mpWallTexCoords         ( nullptr ),
mManipulate             ( MANIPULATE_CAMERA ),
mDirectionalLightDir    ( 0.0f, -1.0f, 0.0f )
{
   std::memset(mMousePos, 0x00, sizeof(mMousePos));
}

NormalMappingWindow::~NormalMappingWindow( )
{
   WGL_ASSERT(!mpShader);
   WGL_ASSERT(!mpDiffuseTex);
   WGL_ASSERT(!mpHeightTex);
   WGL_ASSERT(!mpNormalTex);
   WGL_ASSERT(!mpWallVAO);
   WGL_ASSERT(!mpWallVerts);
   WGL_ASSERT(!mpWallNorms);
   WGL_ASSERT(!mpWallTexCoords);
}

bool NormalMappingWindow::Create( unsigned int nWidth,
                                  unsigned int nHeight,
                                  const char * pWndTitle,
                                  const void * pInitParams )
{
   // initialize with a context else nothing
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 0, false, true, false },
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      // make the context current
      MakeCurrent();

      // load the textures
      mpDiffuseTex.reset(new Texture);
      mpHeightTex.reset(new Texture);
      mpNormalTex.reset(new Texture);

      if (!mpDiffuseTex->Load2D("normal_mapping_diffuse.jpg", GL_RGB, GL_COMPRESSED_RGB, true) ||
          !mpHeightTex->Load2D("normal_mapping_height.jpg", GL_RGB, GL_COMPRESSED_RGB, true) ||
          !mpNormalTex->Load2D("normal_mapping_normal.jpg", GL_RGB, GL_COMPRESSED_RGB, true))
      {
         // a texture was not loaded, so just quit
         return false;
      }

      // start off with the flat shader
      LoadShader(FLAT_SHADER);

      // if the shader was not loaded, then just quit
      if (!mpShader) return false;

      // init the vertex data
      InitVertexData();

      // init the lighting data
      InitLightingData();

      // initialize the camera matrix
      // the projection will be initialized in the window sizing
      mCamera.MakeLookAt(40.0f, 40.0f, 40.0f,
                         0.0f, 0.0f, 0.0f,
                         0.0f, 1.0f, 0.0f);
      
      // force the projection matrix to get calculated and updated
      SendMessage(GetHWND(), WM_SIZE, 0, nHeight << 16 | nWidth);

      return true;
   }

   return false;
}

void NormalMappingWindow::OnDestroy( )
{
   // release the shader
   mpShader = nullptr;

   // release the textures
   mpDiffuseTex = nullptr;
   mpHeightTex = nullptr;
   mpNormalTex = nullptr;

   // release the vertex data
   mpWallVAO = nullptr;
   mpWallVerts = nullptr;
   mpWallNorms = nullptr;
   mpWallTexCoords = nullptr;

   // call the base class destroy
   OpenGLWindow::OnDestroy();
}

int NormalMappingWindow::Run( )
{
   // vars for the loop
   int appQuitVal = 0;
   bool bQuit = false;

   // basic message pump and render frame
   while (!bQuit)
   {
      // process all the messages
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         glClear(GL_COLOR_BUFFER_BIT);

         mpShader->Enable();
         mpDiffuseTex->Bind(GL_TEXTURE0);
         mpShader->SetUniformValue("diffuse_texture", 0);

         mpWallVAO->Bind();

         glDrawArrays(GL_TRIANGLES, 0, 12);

         mpWallVAO->Unbind();
         mpShader->Disable();

         mpDiffuseTex->Unbind();

         SwapBuffers(GetHDC());
      }
   }

   return appQuitVal;
}

LRESULT NormalMappingWindow::MessageHandler( UINT uMsg,
                                             WPARAM wParam,
                                             LPARAM lParam )
{
   LRESULT result = 0;

   // determines if the shader's matrix needs updating
   bool update_shader_matrix = false;
   bool update_directional_light = false;

   switch (uMsg)
   {
   case WM_SIZE:
      {
      // get the width and height
      const int32_t width = static_cast< int32_t >(lParam & 0x0000FFFF);
      const int32_t height = static_cast< int32_t >(lParam >> 16);

      // change the size of the viewport
      // this will be ignored until a valid gl context is created
      glViewport(0, 0,
                 static_cast< GLsizei >(width),
                 static_cast< GLsizei >(height));

      // update the projection matrix
      mProjection.MakePerspective(45.0f, static_cast< float >(width) / static_cast< float >(height), 0.01f, 1000.0f);

      // update the shader
      update_shader_matrix = true;
      }

      break;

   case WM_CHAR:
      // switch based on aswd being pressed
      switch (wParam)
      {
      case 'a':
      case 'd':
         if (MANIPULATE_CAMERA == mManipulate)
         {
            // take the inverse of the view matrix to get into world space
            mCamera.MakeInverse();

            // strafe translate based on the current view matrix
            mCamera = (mCamera * Matrixf::Translate(wParam == 'a' ? -0.20f : 0.20f, 0.0f, 0.0f)).Inverse();

            // update the shader
            update_shader_matrix = true;
         }

         break;

      case 'w':
      case 's':
         if (MANIPULATE_CAMERA == mManipulate)
         {
            // take the inverse of the view matrix to get into world space
            mCamera.MakeInverse();

            // view translate based on the current view matrix
            mCamera = (mCamera * Matrixf::Translate(0.0f, 0.0f, wParam == 'w' ? -0.20f : 0.20f)).Inverse();

            // update the shader
            update_shader_matrix = true;
         }

         break;

      case '1': mManipulate = MANIPULATE_CAMERA; break;
      case '2': mManipulate = MANIPULATE_DIRECTIONAL_LIGHT; break;
      case '3': mManipulate = MANIPULATE_POINT_LIGHT; break;
      }

      break;

   case WM_MOUSEMOVE:
      {

      // get current mouse positions
      const int32_t current_mouse_x = static_cast< int32_t >(lParam & 0xFFFF);
      const int32_t current_mouse_y = static_cast< int32_t >(lParam >> 16);

      if (wParam & MK_LBUTTON)
      {
         // get the delta between current and previous positions
         const int32_t delta_x = current_mouse_x - mMousePos[0];
         const int32_t delta_y = current_mouse_y - mMousePos[1];

         if (MANIPULATE_CAMERA == mManipulate)
         {
            // obtain the current yaw and pitch rotations from the view matrix
            float view_yaw = 0.0f, view_pitch = 0.0f;
            MatrixHelper::DecomposeYawPitchRollDeg< float >(mCamera, &view_yaw, &view_pitch, nullptr);

            // update the yaw and pitch values
            // need to make this more granular
            // need to get the elapsed time here
            view_yaw += delta_x * 0.05f;
            view_pitch += delta_y * 0.05f;

            // make sure the value for pitch is within [-90, 90]
            view_pitch = MathHelper::Clamp(view_pitch, -89.9f, 89.9f);

            // obtain the current position of the camera
            const Vec4f camera_pos = mCamera.Inverse() * Vec4f();

            // construct the new view matrix
            mCamera = (Matrixf::Translate(Vec3f(camera_pos)) *
                       Matrixf::Rotate(view_yaw, 0.0f, 1.0f, 0.0f) *
                       Matrixf::Rotate(view_pitch, 1.0f, 0.0f, 0.0f)).Inverse();

            // update the shader
            update_shader_matrix = true;
         }
         else if (MANIPULATE_DIRECTIONAL_LIGHT == mManipulate)
         {
            // create a rotation matrix
            const Matrixf rotation = Matrixf::Rotate(delta_x * 0.05f, 0.0f, 1.0f, 0.0f) *
                                     Matrixf::Rotate(delta_y * 0.05f, 1.0f, 0.0f, 0.0f);

            // rotate the view vector
            mDirectionalLightDir = rotation * Vec4f(mDirectionalLightDir, 0.0f);

            // update the directional light
            update_directional_light = true;
         }
      }

      // save the current state
      mMousePos[0] = current_mouse_x;
      mMousePos[1] = current_mouse_y;

      }

   default:
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);

      break;
   }

   // determines if the shader needs to be updated
   if (mpShader)
   {
      if (update_shader_matrix)
      {
         mpShader->Enable();
         mpShader->SetUniformMatrix< 1, 4, 4 >("model_view_mat4", mCamera);
         mpShader->SetUniformMatrix< 1, 4, 4 >("model_view_tinv_mat4", mCamera.Inverse().Transpose());
         mpShader->SetUniformMatrix< 1, 4, 4 >("model_view_proj_mat4", mProjection * mCamera);
         mpShader->Disable();
      }

      if (update_directional_light)
      {
         mpShader->Enable();
         mpShader->SetUniformValue("directional_light.direction_world_space", mDirectionalLightDir);
         mpShader->Disable();
      }
   }
   
   return result;
}

void NormalMappingWindow::LoadShader( const Shader shader )
{
   // release and create a new shader
   mpShader.reset(new ShaderProgram);

   switch (shader)
   {
   case FLAT_SHADER:
   default: 
      // attach the frag and vert sources
      mpShader->AttachFile(GL_VERTEX_SHADER, std::vector< const std::string > { "normal_mapping_lighting.glsl", "normal_mapping_flat_shader.vert" });
      mpShader->AttachFile(GL_FRAGMENT_SHADER, std::vector< const std::string > { "normal_mapping_lighting.glsl", "normal_mapping_flat_shader.frag" });

      break;
   }

   // link the shader
   if (!mpShader->Link())
   {
      // shader was not linked
      mpShader = nullptr;
   }
}

void NormalMappingWindow::InitVertexData( )
{
   // create a new vertex array object
   mpWallVAO.reset(new VertexArrayObject);

   // generate and bind the vertex array object
   mpWallVAO->GenArray();
   mpWallVAO->Bind();

   // generate the vertex data for the object
   mpWallVerts.reset(new VertexBufferObject);
   mpWallVerts->GenBuffer(GL_ARRAY_BUFFER);
   mpWallVerts->Bind();
   const float vertices[] = { -40.0f, 0.0f, 40.0f, -40.0f, 0.0f, -40.0f, 40.0f, 0.0f,  40.0f,
                               40.0f, 0.0f, 40.0f, -40.0f, 0.0f, -40.0f, 40.0f, 0.0f, -40.0f };
   mpWallVerts->BufferData(sizeof(vertices), vertices, GL_STATIC_DRAW);
   mpWallVerts->VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mpWallVerts->Unbind();

   // bind slot 0 to the verts
   mpWallVAO->EnableVertexAttribArray(0);

   // generate the texture coordinates for the object
   mpWallTexCoords.reset(new VertexBufferObject);
   mpWallTexCoords->GenBuffer(GL_ARRAY_BUFFER);
   mpWallTexCoords->Bind();
   const float tex_coords[] = { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
   mpWallTexCoords->BufferData(sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);
   mpWallTexCoords->VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
   mpWallTexCoords->Unbind();

   // bind slot 1 to the texture coords
   mpWallVAO->EnableVertexAttribArray(1);

   // generate the normal data for the object
   mpWallNorms.reset(new VertexBufferObject);
   mpWallNorms->GenBuffer(GL_ARRAY_BUFFER);
   mpWallNorms->Bind();
   const float normals[] = { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                             0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
   mpWallNorms->BufferData(sizeof(normals), normals, GL_STATIC_DRAW);
   mpWallNorms->VertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mpWallNorms->Unbind();

   // bind slot 2 to the vertex normals
   mpWallVAO->EnableVertexAttribArray(2);

   // no longer need the vao
   mpWallVAO->Unbind();
}

void NormalMappingWindow::InitLightingData( )
{
   // enable the shader
   mpShader->Enable();

   // constants shared by both shader attributes
   const float ambient_intensity = 0.05f;
   const float diffuse_intensity = 1.0f;

   // init the directional light
   mpShader->SetUniformValue("directional_light.base.color", Vec3f(1.0f, 1.0f, 1.0f));
   mpShader->SetUniformValue("directional_light.base.ambient_intensity", ambient_intensity);
   mpShader->SetUniformValue("directional_light.base.diffuse_intensity", diffuse_intensity);
   mpShader->SetUniformValue("directional_light.direction_world_space", mDirectionalLightDir);

   // init the point light
   mpShader->SetUniformValue("point_light.base.color", Vec3f(0.0f, 0.0f, 1.0f));
   mpShader->SetUniformValue("point_light.base.ambient_intensity", ambient_intensity);
   mpShader->SetUniformValue("point_light.base.diffuse_intensity", diffuse_intensity);
   mpShader->SetUniformValue("point_light.position_world_space", Vec3f(0.0f, 1.0f, 0.0f));
   mpShader->SetUniformValue("point_light.attenuation.constant_component", 0.5f);
   mpShader->SetUniformValue("point_light.attenuation.linear_component", 0.1f);
   mpShader->SetUniformValue("point_light.attenuation.exponential_component", 0.0f);

   // disable the shader
   mpShader->Disable();
}
