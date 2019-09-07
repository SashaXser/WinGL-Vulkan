// local includes
#include "NormalMappingWindow.h"

// wgl includes
#include <Timer.h>
#include <Texture.h>
#include <MathHelper.h>
#include <Quaternion.h>
#include <MatrixHelper.h>
#include <ShaderProgram.h>
#include <VertexArrayObject.h>
#include <VertexBufferObject.h>

// std includes
#include <cmath>
#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <iostream>

// constants shared by both shader attributes
static const float AMBIENT_INTENSITY = 0.05f;
static const float DIFFUSE_INTENSITY = 1.0f;

NormalMappingWindow::NormalMappingWindow( ) :
mpShader                      ( nullptr ),
mpDiffuseTex                  ( nullptr ),
mpHeightTex                   ( nullptr ),
mpNormalTex                   ( nullptr ),
mpWallVAO                     ( nullptr ),
mpWallVerts                   ( nullptr ),
mpWallNorms                   ( nullptr ),
mpWallTangents                ( nullptr ),
mpWallBitangents              ( nullptr ),
mpWallTexCoords               ( nullptr ),
mpWallIndices                 ( nullptr ),
mManipulate                   ( MANIPULATE_CAMERA ),
mDrawElements                 ( nullptr ),
mDirectionalLightDir          ( 0.0f, -1.0f, 0.0f ),
mPointLightAmbientIntensity   ( AMBIENT_INTENSITY ),
mPointLightDiffuseIntensity   ( DIFFUSE_INTENSITY ),
mParallaxBias                 ( 0.03f ),
mParallaxScale                ( -0.0225f ),
mPolygonMode                  ( GL_FILL ),
mInvertNormalY                ( false ),
mCurrentSimTimeSec            ( 0.0 ),
mCurrentSimTimeTick           ( 0 ),
mSimTimeMultiplier            ( 1.0 )
{
}

NormalMappingWindow::~NormalMappingWindow( )
{
}

bool NormalMappingWindow::Create( unsigned int nWidth,
                                  unsigned int nHeight,
                                  const char * pWndTitle,
                                  const void * pInitParams )
{
   // initialize with a context else nothing
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 1, false, true, false },
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

      if (!LoadTexture())
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

      // initialize the camera matrix
      // the projection will be initialized in the window sizing
      mCamera.MakeLookAt(0.0f, 40.0f, 40.0f,
                         0.0f, 0.0f, 0.0f,
                         0.0f, 1.0f, 0.0f);

      // enable the depth buffer and cull faces
      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);
      
      // force the projection matrix to get calculated and updated
      SendMessage(GetHWND(), WM_SIZE, 0, nHeight << 16 | nWidth);

      // indicate what actions can be taken
      std::cout << std::endl
                << "1 - Manipulates camera" << std::endl
                << "2 - Manipulates directional lighting" << std::endl
                << std::endl
                << "a - Moves camera to the left" << std::endl
                << "d - Moves camera to the right" << std::endl
                << "w - Moves camera to the up" << std::endl
                << "s - Moves camera to the down" << std::endl
                << std::endl
                << "Shift + t - Changes texture" << std::endl
                << std::endl
                << "f - Switch to flat shading" << std::endl
                << "n - Switch to normal shading" << std::endl
                << "p - Switch to parallax shading" << std::endl
                << "t - Switch to tessellation shading" << std::endl
                << std::endl
                << "Shift + s - Turn point light on / off" << std::endl
                << "Shift + w - Turn wire frame on / off" << std::endl
                << std::endl
                << "L-Button Down - Activate camera / directional light rotation" << std::endl
                << "Mouse X / Y Delta - Manipulate camera / directional light rotation" << std::endl
                << std::endl
                << "> - Increase Sim Time Multiplier" << std::endl
                << "< - Decrease Sim Time Multiplier" << std::endl << std::ends;

      return true;
   }

   return false;
}

void NormalMappingWindow::OnDestroy( )
{
   // release the shader
   mpShader = nullptr;
   mpShaderDirLight = nullptr;

   // release the textures
   mpDiffuseTex = nullptr;
   mpHeightTex = nullptr;
   mpNormalTex = nullptr;

   // release the vertex data
   mpWallVAO = nullptr;
   mpWallVerts = nullptr;
   mpWallNorms = nullptr;
   mpWallIndices = nullptr;
   mpWallTangents = nullptr;
   mpWallTexCoords = nullptr;
   mpWallBitangents = nullptr;

   mpDirLightVAO = nullptr;
   mpDirLightVerts = nullptr;
   mpDirLightColors = nullptr;

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
         // clear the back buffer and the depth buffer
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         if (mpShader && mpDiffuseTex && mpNormalTex && mpHeightTex && mpWallVAO)
         {
            // enable the shader for the object
            mpShader->Enable();

            // setup the textures to use
            mpDiffuseTex->Bind(GL_TEXTURE0); mpShader->SetUniformValue("diffuse_texture", static_cast< GLint >(mpDiffuseTex->GetBoundSamplerID()));
            mpNormalTex->Bind(GL_TEXTURE1); mpShader->SetUniformValue("normal_texture", static_cast< GLint >(mpNormalTex->GetBoundSamplerID()));
            mpHeightTex->Bind(GL_TEXTURE2); mpShader->SetUniformValue("parallax_texture", static_cast< GLint >(mpHeightTex->GetBoundSamplerID()));

            // bind the object to render
            mpWallVAO->Bind();

            // draw the two triangles that represent the floor
            mDrawElements(static_cast< GLsizei >(mShape.indices.size()),
                          GL_UNSIGNED_INT,
                          nullptr);

            // unbind the object being rendered
            mpWallVAO->Unbind();

            // disable the shader
            mpShader->Disable();

            // disable all the textures
            mpDiffuseTex->Unbind();
            mpNormalTex->Unbind();
            mpHeightTex->Unbind();

            // if directional light is being modified, then display the arrow
            if (MANIPULATE_DIRECTIONAL_LIGHT == mManipulate && mpShaderDirLight && mpDirLightVAO)
            {
               // enable the shader for the directional light shape
               mpShaderDirLight->Enable();

               // bind the arrow shape
               mpDirLightVAO->Bind();

               // render the arrow head
               glDrawArrays(GL_TRIANGLES, 0, 18);
               // render the line coming out the back
               glDrawArrays(GL_LINES, 19, 2);

               // unbind the arrow shape
               mpDirLightVAO->Unbind();

               // disable the shader for the light
               mpShaderDirLight->Disable();
            }

            // update the current simulation time
            mCurrentSimTimeSec += Timer().DeltaSec(mCurrentSimTimeTick) * mSimTimeMultiplier;
            mCurrentSimTimeTick = Timer().GetCurrentTick();

            // determine the x and y location of the light
            const float x = static_cast< float >(std::cos(mCurrentSimTimeSec) * 20.0f);
            const float y = static_cast< float >(std::sin(mCurrentSimTimeSec) * 20.0f);

            // update the lights position
            mpShader->Enable();
            mpShader->SetUniformValue("point_light.position_world_space", Vec3f(x, 1.0f, y));
            mpShader->Disable();
         }

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
      // process key presses
      switch (wParam)
      {
      case 'a':
      case 'd':
         if (MANIPULATE_CAMERA == mManipulate)
         {
            // take the inverse of the view matrix to get into world space
            mCamera.MakeInverse();

            // strafe translate based on the current view matrix
            mCamera = (mCamera * Matrixf::Translate(wParam == 'a' ? -0.5f : 0.5f, 0.0f, 0.0f)).Inverse();

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
            mCamera = (mCamera * Matrixf::Translate(0.0f, 0.0f, wParam == 'w' ? -0.5f : 0.5f)).Inverse();

            // update the shader
            update_shader_matrix = true;
         }

         break;

      case 'S':
         {

         // if the intensity is zero, then turn it on; otherwise turn it off
         if (math::Equals(mPointLightAmbientIntensity, 0.0f))
         {
            mPointLightAmbientIntensity = AMBIENT_INTENSITY;
            mPointLightDiffuseIntensity = DIFFUSE_INTENSITY;
         }
         else
         {
            mPointLightAmbientIntensity = 0.0f;
            mPointLightDiffuseIntensity = 0.0f;
         }

         mpShader->Enable();

         mpShader->SetUniformValue("point_light.base.ambient_intensity", mPointLightAmbientIntensity);
         mpShader->SetUniformValue("point_light.base.diffuse_intensity", mPointLightDiffuseIntensity);

         mpShader->Disable();

         }

         break;

      case 'W':
         {

         // update the mode
         mPolygonMode = mPolygonMode == GL_FILL ? GL_LINE : GL_FILL;

         // set the mode to either fill or wire
         glPolygonMode(GL_FRONT_AND_BACK, mPolygonMode);

         }

         break;

      case '1': mManipulate = MANIPULATE_CAMERA; break;
      case '2': mManipulate = MANIPULATE_DIRECTIONAL_LIGHT; update_directional_light = true; break;
      case '3': mManipulate = MANIPULATE_POINT_LIGHT; break;

      case 'f': LoadShader(FLAT_SHADER); update_shader_matrix = true; update_directional_light = true; break;
      case 'n': LoadShader(NORMAL_SHADER); update_shader_matrix = true; update_directional_light = true; break;
      case 'p': LoadShader(PARALLAX_SHADER); update_shader_matrix = true; update_directional_light = true; break;
      case 't': LoadShader(TESSELLATION_SHADER); update_shader_matrix = true; update_directional_light = true; break;

      case 'T': LoadTexture(); break;

      case '>': mSimTimeMultiplier = math::Clamp(mSimTimeMultiplier - 0.1, 0.0, 2.0); break;
      case '<': mSimTimeMultiplier = math::Clamp(mSimTimeMultiplier + 0.1, 0.0, 2.0); break;
      }

      break;

   case WM_MOUSEMOVE:
      {

      // get current mouse positions
      const intptr_t current_mouse_x = static_cast< int32_t >(lParam & 0xFFFF);
      const intptr_t current_mouse_y = static_cast< int32_t >(lParam >> 16);

      if (wParam & MK_LBUTTON)
      {
         // get the delta between current and previous positions
         const intptr_t delta_x = current_mouse_x - GetPreviousMousePosition().x;
         const intptr_t delta_y = current_mouse_y - GetPreviousMousePosition().y;

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
            view_pitch = math::Clamp(view_pitch, -89.9f, 89.9f);

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
            // determine the pitch axis
            const Vec3f pitch_axis =
            [ this ] ( ) -> Vec3f
            {
               Vec3f pitch_axis = (Vec3f(0.001f, 0.999f, 0.0f).UnitVector() ^ mDirectionalLightDir).UnitVector();

               // make sure to make the axis point in the same direction;
               // otherwise, the cross will flip back and forth causing it to lock at a pole
               if (pitch_axis.X() > 0.0)
               {
                  pitch_axis *= -1.0f;
               }

               return pitch_axis.UnitVector();
            }();

            // construct the rotation matrix from the shift in x and y
            const Matrixf rotation = Matrixf::Rotate(delta_x * 0.1f, 0.0f, 1.0f, 0.0f) *
                                     Matrixf::Rotate(delta_y * 0.1f, pitch_axis);

            // rotate the view vector
            mDirectionalLightDir = (rotation * Vec4f(mDirectionalLightDir, 0.0f)).UnitVector();

            // update the directional light
            update_directional_light = true;
         }
      }

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
         mpShader->SetUniformMatrix< 1, 4, 4 >("model_view_tinv_mat4", (mCamera));
         mpShader->SetUniformMatrix< 1, 4, 4 >("model_view_proj_mat4", mProjection * mCamera);

         mpShader->SetUniformValue("camera_position", Vec3f(mCamera.Inverse()[12]));
         mpShader->Disable();
      }

      if (update_directional_light)
      {
         mpShader->Enable();
         mpShader->SetUniformValue("directional_light.direction_world_space", mDirectionalLightDir);
         mpShader->Disable();

         // update the directional light shader
         UpdateDirLightShader();
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
   case NORMAL_SHADER:
      // attach the frag and vert sources
      mpShader->AttachFile(GL_VERTEX_SHADER, std::vector< std::string >
                                             { "./normal_mapping/shaders/normal_mapping_lighting.glsl", "./normal_mapping/shaders/normal_mapping_normal_shader.vert" });
      mpShader->AttachFile(GL_FRAGMENT_SHADER, std::vector< std::string >
                                               { "./normal_mapping/shaders/normal_mapping_lighting.glsl", "./normal_mapping/shaders/normal_mapping_normal_shader.frag" });

      // indicate that we need to render triangles
      mDrawElements = std::bind(&glDrawElements, GL_TRIANGLES, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

      break;

   case PARALLAX_SHADER:
      // attach the frag and vert sources
      mpShader->AttachFile(GL_VERTEX_SHADER, std::vector< std::string >
                                             { "./normal_mapping/shaders/normal_mapping_lighting.glsl", "./normal_mapping/shaders/normal_mapping_parallax_shader.vert" });
      mpShader->AttachFile(GL_FRAGMENT_SHADER, std::vector< std::string >
                                               { "./normal_mapping/shaders/normal_mapping_lighting.glsl", "./normal_mapping/shaders/normal_mapping_parallax_shader.frag" });

      // indicate that we need to render triangles
      mDrawElements = std::bind(&glDrawElements, GL_TRIANGLES, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

      break;

   case TESSELLATION_SHADER:
      // attach the frag, vert, and tess sources
      mpShader->AttachFile(GL_VERTEX_SHADER, std::vector< std::string >
                                             { "./normal_mapping/shaders/normal_mapping_lighting.glsl", "./normal_mapping/shaders/normal_mapping_tess_shader.vert" });
      mpShader->AttachFile(GL_FRAGMENT_SHADER, std::vector< std::string >
                                               { "./normal_mapping/shaders/normal_mapping_lighting.glsl", "./normal_mapping/shaders/normal_mapping_tess_shader.frag" });
      mpShader->AttachFile(GL_TESS_CONTROL_SHADER, "./normal_mapping/shaders/normal_mapping_tess_shader.tctrl");
      mpShader->AttachFile(GL_TESS_EVALUATION_SHADER, "./normal_mapping/shaders/normal_mapping_tess_shader.teval");

      // indicate that we need to render the control patches to generate the terrain on the fly
      mDrawElements = std::bind(&glDrawElements, GL_PATCHES, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

      break;

   case FLAT_SHADER:
   default: 
      // attach the frag and vert sources
      mpShader->AttachFile(GL_VERTEX_SHADER, std::vector< std::string >
                                             { "./normal_mapping/shaders/normal_mapping_lighting.glsl", "./normal_mapping/shaders/normal_mapping_flat_shader.vert" });
      mpShader->AttachFile(GL_FRAGMENT_SHADER, std::vector< std::string >
                                               { "./normal_mapping/shaders/normal_mapping_lighting.glsl", "./normal_mapping/shaders/normal_mapping_flat_shader.frag" });

      // indicate that we need to render triangles
      mDrawElements = std::bind(&glDrawElements, GL_TRIANGLES, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

      break;
   }

   // link the shader
   if (!mpShader->Link())
   {
      // shader was not linked
      mpShader = nullptr;
   }
   else
   {
      // init the lighting
      InitLightingData();
   }

   if (!mpShaderDirLight)
   {
      // create a new shader
      mpShaderDirLight.reset(new ShaderProgram);

      // attach the frag and vert sources
      mpShaderDirLight->AttachFile(GL_VERTEX_SHADER, "./normal_mapping/shaders/normal_mapping_dir_light.vert");
      mpShaderDirLight->AttachFile(GL_FRAGMENT_SHADER, "./normal_mapping/shaders/normal_mapping_dir_light.frag");

      // link the shader
      if (!mpShaderDirLight->Link())
      {
         // shader was not linked
         mpShaderDirLight = nullptr;
      }
      else
      {
         // init the direction source
         UpdateDirLightShader();
      }
   }
}

void NormalMappingWindow::InitVertexData( )
{
   // create a new vertex array object
   mpWallVAO.reset(new VertexArrayObject);

   // generate and bind the vertex array object
   mpWallVAO->GenArray();
   mpWallVAO->Bind();

   // generate the plane
   mShape = GeomHelper::ConstructPlane(80.0f, 80.0f);

   // generate the vertex data for the object
   mpWallVerts.reset(new VertexBufferObject);
   mpWallVerts->GenBuffer(GL_ARRAY_BUFFER);
   mpWallVerts->Bind();
   mpWallVerts->BufferData(mShape.vertices.size() * sizeof(Vec3f), &mShape.vertices[0], GL_STATIC_DRAW);
   mpWallVerts->VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mpWallVerts->Unbind();

   // bind slot 0 to the verts
   mpWallVAO->EnableVertexAttribArray(0);

   // generate the texture coordinates for the object
   mpWallTexCoords.reset(new VertexBufferObject);
   mpWallTexCoords->GenBuffer(GL_ARRAY_BUFFER);
   mpWallTexCoords->Bind();
   mpWallTexCoords->BufferData(mShape.tex_coords.size() * sizeof(Vec2f), &mShape.tex_coords[0], GL_STATIC_DRAW);
   mpWallTexCoords->VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
   mpWallTexCoords->Unbind();

   // bind slot 1 to the texture coords
   mpWallVAO->EnableVertexAttribArray(1);

   // generate the normal data for the object
   mpWallNorms.reset(new VertexBufferObject);
   mpWallNorms->GenBuffer(GL_ARRAY_BUFFER);
   mpWallNorms->Bind();
   mpWallNorms->BufferData(mShape.normals.size() * sizeof(Vec3f), &mShape.normals[0], GL_STATIC_DRAW);
   mpWallNorms->VertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mpWallNorms->Unbind();

   // bind slot 2 to the vertex normals
   mpWallVAO->EnableVertexAttribArray(2);

   // generate the tangent data for the object
   mpWallTangents.reset(new VertexBufferObject);
   mpWallTangents->GenBuffer(GL_ARRAY_BUFFER);
   mpWallTangents->Bind();
   mpWallTangents->BufferData(mShape.tangents.size() * sizeof(Vec3f), &mShape.tangents[0], GL_STATIC_DRAW);
   mpWallTangents->VertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mpWallTangents->Unbind();

   // bind slot 3 to the vertex tangents
   mpWallVAO->EnableVertexAttribArray(3);

   // generate the bitangent data for the object
   mpWallBitangents.reset(new VertexBufferObject);
   mpWallBitangents->GenBuffer(GL_ARRAY_BUFFER);
   mpWallBitangents->Bind();
   mpWallBitangents->BufferData(mShape.bitangents.size() * sizeof(Vec3f), &mShape.bitangents[0], GL_STATIC_DRAW);
   mpWallBitangents->VertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mpWallBitangents->Unbind();

   // bind slot 4 to the vertex bitangents
   mpWallVAO->EnableVertexAttribArray(4);

   // generate the indices for the object
   // do not unbind the buffer until after the vao is unbound
   mpWallIndices.reset(new VertexBufferObject);
   mpWallIndices->GenBuffer(GL_ELEMENT_ARRAY_BUFFER);
   mpWallIndices->Bind();
   mpWallIndices->BufferData(mShape.indices.size() * sizeof(GLuint), &mShape.indices[0], GL_STATIC_DRAW);

   // no longer need the vao
   mpWallVAO->Unbind();

   // unbind the index buffer
   mpWallIndices->Unbind();

   // visualize the directional light by creating an arrow for it

   // create a new vertex array object for the light
   mpDirLightVAO.reset(new VertexArrayObject);

   // generate and bind the vertex array object
   mpDirLightVAO->GenArray();
   mpDirLightVAO->Bind();

   // generate the vertex data for the object
   mpDirLightVerts.reset(new VertexBufferObject);
   mpDirLightVerts->GenBuffer(GL_ARRAY_BUFFER);
   mpDirLightVerts->Bind();
   const float dir_light_vertices[] = { -0.5f, 1.0f, -0.5f,  0.0f, 0.0f,  0.0f, -0.5f, 1.0f,  0.5f,
                                         0.0f, 0.0f,  0.0f,  0.5f, 1.0f,  0.5f, -0.5f, 1.0f,  0.5f,
                                         0.0f, 0.0f,  0.0f,  0.5f, 1.0f, -0.5f,  0.5f, 1.0f,  0.5f,
                                         0.0f, 0.0f,  0.0f, -0.5f, 1.0f, -0.5f,  0.5f, 1.0f, -0.5f,
                                        -0.5f, 1.0f, -0.5f, -0.5f, 1.0f,  0.5f,  0.5f, 1.0f, -0.5f,
                                         0.5f, 1.0f, -0.5f, -0.5f, 1.0f,  0.5f,  0.5f, 1.0f,  0.5f,
                                         0.0f, 1.0f,  0.0f,  0.0f, 2.5f,  0.0f };
   mpDirLightVerts->BufferData(sizeof(dir_light_vertices), dir_light_vertices, GL_STATIC_DRAW);
   mpDirLightVerts->VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mpDirLightVerts->Unbind();

   // bind slot 0 to the vertices
   mpDirLightVAO->EnableVertexAttribArray(0);

   // generate the vertex data for the object
   mpDirLightColors.reset(new VertexBufferObject);
   mpDirLightColors->GenBuffer(GL_ARRAY_BUFFER);
   mpDirLightColors->Bind();
   const float dir_light_colors[] = { 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                                      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                                      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                                      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                                      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                                      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                                      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f };
   mpDirLightColors->BufferData(sizeof(dir_light_colors), dir_light_colors, GL_STATIC_DRAW);
   mpDirLightColors->VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mpDirLightColors->Unbind();

   // bind slot 1 to the vertices color
   mpDirLightVAO->EnableVertexAttribArray(1);

   // no longer need the vao
   mpDirLightVAO->Unbind();
}

void NormalMappingWindow::InitLightingData( )
{
   if (mpShader)
   {
      // enable the shader
      mpShader->Enable();

      // init the directional light
      mpShader->SetUniformValue("directional_light.base.color", Vec3f(1.0f, 1.0f, 1.0f));
      mpShader->SetUniformValue("directional_light.base.ambient_intensity", AMBIENT_INTENSITY);
      mpShader->SetUniformValue("directional_light.base.diffuse_intensity", DIFFUSE_INTENSITY);
      mpShader->SetUniformValue("directional_light.direction_world_space", mDirectionalLightDir);

      // init the point light
      mpShader->SetUniformValue("point_light.base.color", Vec3f(1.0f, 1.0f, 0.0f));
      mpShader->SetUniformValue("point_light.base.ambient_intensity", mPointLightAmbientIntensity);
      mpShader->SetUniformValue("point_light.base.diffuse_intensity", mPointLightDiffuseIntensity);
      mpShader->SetUniformValue("point_light.position_world_space", Vec3f(0.0f, 1.0f, 0.0f));
      mpShader->SetUniformValue("point_light.attenuation.constant_component", 0.5f);
      mpShader->SetUniformValue("point_light.attenuation.linear_component", 0.1f);
      mpShader->SetUniformValue("point_light.attenuation.exponential_component", 0.15f);
   
      // sometimes the texture y coordinate may be inverted based on the texture
      // if the uniform is not found, it will not be set or cause any issues
      mpShader->SetUniformValue("invert_normal_texture_y_component", mInvertNormalY ? GL_TRUE : GL_FALSE);

      // update the parallax values
      // if the uniform is not found, it will not be set or cause any issues
      mpShader->SetUniformValue("parallax_bias", mParallaxBias);
      mpShader->SetUniformValue("parallax_scale", mParallaxScale);

      // disable the shader
      mpShader->Disable();
   }
}

void NormalMappingWindow::UpdateDirLightShader( )
{
   // indicates the initial light direction
   const Vec3f initial_light_dir(0.0f, -1.0f, 0.0f);

   // create the matrix that rotates the light vector correctly
   const Matrixf mvp =
      mProjection * mCamera *
      Matrixf::Translate(0.0f, 10.0f, 0.0f) *
      Quatf::Rotation(initial_light_dir, mDirectionalLightDir).ToMatrix() *
      Matrixf::Scale(3.0f);

   // update the shader
   mpShaderDirLight->Enable();
   mpShaderDirLight->SetUniformMatrix< 1, 4, 4 >("model_view_proj_mat4", mvp);
   mpShaderDirLight->Disable();
}

bool NormalMappingWindow::LoadTexture( )
{
   bool textures_loaded = true;

   // all the supported textures
   std::vector< std::vector< const char * > > textures =
   {
      // diffuse tex    offset tex     normal tex     scale    offset      invert_y
      { "./normal_mapping/textures/bricks_diffuse.jpg",        "./normal_mapping/textures/bricks_height.png",        "./normal_mapping/textures/bricks_normal.jpg",        "0.03",  "-0.5",  "false" },
      { "./normal_mapping/textures/bricks2_diffuse.jpg",       "./normal_mapping/textures/bricks2_height.jpg",       "./normal_mapping/textures/bricks2_normal.png",       "0.03",  "-0.5",  "false" },
      { "./normal_mapping/textures/genetica_diffuse.jpg",      "./normal_mapping/textures/genetica_height.jpg",      "./normal_mapping/textures/genetica_normal.jpg",      "0.03",  "-0.5",  "false" },
      { "./normal_mapping/textures/lion_diffuse.png",          "./normal_mapping/textures/lion_height.png",          "./normal_mapping/textures/lion_normal.png",          "0.03",  "-0.5",  "false" },
      { "./normal_mapping/textures/cobblestone_diffuse.png",   "./normal_mapping/textures/cobblestone_height.png",   "./normal_mapping/textures/cobblestone_normal.png",   "0.03",  "-0.5",  "false" }
   };

   // indicates what to load
   static size_t next_texture_index = 0;

   if (!mpDiffuseTex->Load2D(textures[next_texture_index][0], GL_RGB, GL_RGB8, true) ||
       !mpHeightTex->Load2D(textures[next_texture_index][1], GL_RGB, GL_R8, true) ||
       !mpNormalTex->Load2D(textures[next_texture_index][2], GL_RGB, GL_RGB8, true))
   {
      // release the textures
      mpDiffuseTex = nullptr;
      mpHeightTex = nullptr;
      mpNormalTex = nullptr;

      // unable to load
      textures_loaded = false;

      // unable to load textures
      std::stringstream err;
      err << "Uanble to load the following textures:" << std::endl
          << textures[next_texture_index][0] << std::endl
          << textures[next_texture_index][1] << std::endl
          << textures[next_texture_index][2] << std::endl;

      PostDebugMessage(GL_DEBUG_TYPE_ERROR, 'LTEX', GL_DEBUG_SEVERITY_HIGH, err.str().c_str());
   }

   // if the textures were loaded, then update the shader with the scale and bias
   if (mpDiffuseTex && mpHeightTex && mpNormalTex)
   {
      // calculate what the scale and bias should be
      mParallaxScale = std::stof(textures[next_texture_index][3]);
      const float base_bias = mParallaxScale * 0.5f;
      mParallaxBias = -base_bias + base_bias * std::stof(textures[next_texture_index][4]);

      // determine if y should be inverted
      std::stringstream(textures[next_texture_index][5]) >> std::boolalpha >> mInvertNormalY;

      // send all the parameters again
      InitLightingData();
   }

   // update the next texture index
   next_texture_index = ++next_texture_index % textures.size();

   return textures_loaded;
}
