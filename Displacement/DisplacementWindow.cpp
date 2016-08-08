// local includes
#include "DisplacementWindow.h"

// wgl includes
#include "GeomHelper.h"
#include "ReadTexture.h"

// gl includes
#include <GL/glew.h>
#include <GL/GL.h>

// std incluces
#include <vector>
#include <algorithm>

DisplacementWindow::DisplacementWindow( ) :
mWireframe     ( false ),
mShiftDown     ( false ),
mDispMult      ( 40.0f ),
mLighting      ( 1 ),
mNumTiles      ( 15 ),
mNumOfPatches  ( 0 )
{
}

DisplacementWindow::~DisplacementWindow( )
{
}

bool DisplacementWindow::Create( unsigned int nWidth,
                                 unsigned int nHeight,
                                 const char * pWndTitle,
                                 const void * pInitParams )
{
   bool created = false;

   // initialize with a context else nothing
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 4, false, true, false },
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      // make the context current
      MakeCurrent();

      // enable specific state
      mPipeline.EnableCullFace(true);
      mPipeline.EnableDepthTesting(true);

      // generate the required data
      GenerateTerrain(true);

      // force the projection matrix to get calculated and updated
      SendMessage(GetHWND(), WM_SIZE, 0, nHeight << 16 | nWidth);

      created = true;
   }

   return created;
}

void DisplacementWindow::OnDestroy( )
{
   // call the base class destroy
   OpenGLWindow::OnDestroy();
}
            
int DisplacementWindow::Run( )
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
         //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         const GLfloat BLACK[] = { 0.0f, 0.0f, 0.0f, 1.0f };
         mPipeline.ClearBuffer(GL_COLOR, 0, BLACK);
         const float DEPTH = 1.0f;
         mPipeline.ClearBuffer(GL_DEPTH, 0, &DEPTH);

         if (mWireframe) mPipeline.SetPolygonMode(GL_LINE);

         mTerrainPgm.Enable();

         mTerrainPgm.SetUniformValue< uint32_t >("calculate_lighting", mLighting);

         mTerrainPgm.SetUniformValue("disp_multiplier", mDispMult);
         
         //mCamera.SetPerspective(45.0f, 1.0f, 0.5f, 2000.0);
         //mCamera.LookAt(Vec3f(0,0.0,1050), Vec3f(0,0.0,0.0f));
         mTerrainPgm.SetUniformMatrix< 1, 4, 4 >("mv_matrix", mCamera.GetViewMatrix());
         mTerrainPgm.SetUniformMatrix< 1, 4, 4 >("mvp_matrix", mCamera.GetProjectionMatrix() * mCamera.GetViewMatrix());
         
         mDispMapTex.Bind(GL_TEXTURE0); mTerrainPgm.SetUniformValue("disp_map_texture", static_cast< GLint >(mDispMapTex.GetBoundSamplerID()));
         mNormalMap.Bind(GL_TEXTURE1);  mTerrainPgm.SetUniformValue("normal_map_texture", static_cast< GLint >(mNormalMap.GetBoundSamplerID()));
         mGrassTex.Bind(GL_TEXTURE2); mTerrainPgm.SetUniformValue("grass_texture", static_cast< GLint >(mGrassTex.GetBoundSamplerID()));
         mDirtTex.Bind(GL_TEXTURE3); mTerrainPgm.SetUniformValue("dirt_texture", static_cast< GLint >(mDirtTex.GetBoundSamplerID()));
         mRockTex.Bind(GL_TEXTURE4); mTerrainPgm.SetUniformValue("rock_texture", static_cast< GLint >(mRockTex.GetBoundSamplerID()));
         mSnowTex.Bind(GL_TEXTURE5); mTerrainPgm.SetUniformValue("snow_texture", static_cast< GLint >(mSnowTex.GetBoundSamplerID()));

         mTerrainVAO.Bind();
         //mPipeline.DrawElements(GL_PATCHES, mTerrainIdxVBO.Size< uint32_t >(), GL_UNSIGNED_INT, nullptr);
         mPipeline.DrawArrays(GL_PATCHES, 0, mNumOfPatches);
         mTerrainVAO.Unbind();

         mDispMapTex.Unbind();
         
         mTerrainPgm.Disable();

         if (mWireframe) mPipeline.SetPolygonMode(GL_FILL);

         // swap the front and back buffers
         SwapBuffers(GetHDC());
      }
   }

   return appQuitVal;
}

LRESULT DisplacementWindow::MessageHandler( UINT uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_SIZE:
   {
      // obtain the x, y sizes of the view
      const uintptr_t x = lParam & 0xFFFF;
      const uintptr_t y = lParam >> 16;

      // update the viewport
      mPipeline.SetViewport(0, 0,
                            static_cast< GLsizei >(x),
                            static_cast< GLsizei >(y));

      // define the cameras projection information
      mCamera.SetPerspective(45.0f,
                             static_cast< float >(x) / static_cast< float >(y),
                             0.5f,
                             3000.f);
   }

   break;

   case WM_MOUSEMOVE:
   {
      const int cur_x = static_cast< int >(lParam & 0xFFFF);
      const int cur_y = static_cast< int >(lParam >> 16);

      if (wParam & MK_LBUTTON)
      {
         const int dx = static_cast< int >(cur_x - GetPreviousMousePosition().x);
         const int dy = static_cast< int >(cur_y - GetPreviousMousePosition().y);

         const float yaw = mCamera.GetYaw() + dx * 0.25f;
         const float pitch = mCamera.GetPitch() + dy * 0.25f;

         mCamera.SetYaw(yaw);
         mCamera.SetPitch(pitch);
      }
   }

   break;

   case WM_CHAR:
   {
      const float multiplier = GetAsyncKeyState(VK_SHIFT) & 0x8000 ? 1.0f : 5.0f;

      if (wParam == 'a' || wParam == 'A')
      {
         mCamera.TranslateRight(-multiplier);
      }
      else if (wParam == 'd' || wParam == 'D')
      {
         mCamera.TranslateRight(multiplier);
      }
      else if (wParam == 'w' || wParam == 'W')
      {
         mCamera.TranslateForward(-multiplier);
      }
      else if (wParam == 's' || wParam == 'S')
      {
         mCamera.TranslateForward(multiplier);
      }
      else if (wParam == 'f' || wParam == 'F')
      {
         mWireframe = !mWireframe;
      }
      else if (wParam == 'r' || wParam == 'R')
      {
         if (wParam == 'R')
         {
            mDirtTex = Texture();
            mRockTex = Texture();
            mSnowTex = Texture();
            mGrassTex = Texture();
            mNormalMap = Texture();
            mDispMapTex = Texture();
         }

         GenerateTerrain(true);
      }
      else if (wParam == '+')
      {
         ++mNumTiles; GenerateTerrain(false);
      }
      else if (wParam == '-')
      {
         mNumTiles = std::max< uint32_t >(1, mNumTiles - 1);
         GenerateTerrain(false);
      }
      else if (wParam == 'l' || wParam == 'L')
      {
         mLighting = (mLighting + 1) % 3;
      }
   }
   
   break;

   case WM_KEYDOWN:
   {
      if (wParam == VK_UP)
      {
         mDispMult = mShiftDown ? 40.0f : mDispMult + 0.5f;
      }
      else if (wParam == VK_DOWN)
      {
         mDispMult = mShiftDown ? 0.0f : mDispMult - 0.5f;
      }
      else if (wParam == VK_SHIFT)
      {
         mShiftDown = true;
      }
   }

   break;

   case WM_KEYUP:
   {
      if (wParam == VK_SHIFT)
      {
         mShiftDown = false;
      }
   }

   break;

   default:
      // allow default processing to happen
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);

      break;
   }
   
   return result;
}

void DisplacementWindow::GenerateTerrain( const bool reload_shaders )
{
   // read in the visual textures
   if (!mDirtTex) mDirtTex.Load2D(R"(.\displacement\textures\dirt.jpg)", GL_RGB, GL_COMPRESSED_RGB, true);
   if (!mRockTex) mRockTex.Load2D(R"(.\displacement\textures\rock.jpg)", GL_RGB, GL_COMPRESSED_RGB, true);
   if (!mSnowTex) mSnowTex.Load2D(R"(.\displacement\textures\snow.jpg)", GL_RGB, GL_COMPRESSED_RGB, true);
   if (!mGrassTex) mGrassTex.Load2D(R"(.\displacement\textures\grass.jpg)", GL_RGB, GL_COMPRESSED_RGB, true);
   if (!mNormalMap) mNormalMap.Load2D(R"(.\displacement\textures\normal_map.png)", GL_RGB, GL_COMPRESSED_RGB, true);
   //if (!mNormalMap) mNormalMap.Load2D(R"(.\displacement\textures\bricks2_normal.png)", GL_RGB, GL_COMPRESSED_RGB, true);

   if (!mDispMapTex)
   {
      // read in the displacement map...
      const auto tex_data =
         ReadTexture< uint8_t >(R"(.\displacement\textures\displacement_map.bmp)",
         //ReadTexture< uint8_t >(R"(.\displacement\textures\bricks2_height.jpg)",
                                GL_RGB);

      // load the texture data into the texture
      mDispMapTex.GenerateTextureImmutable(GL_TEXTURE_2D, GL_RGB8,
                                           tex_data.width, tex_data.height,
                                           GL_RGB, GL_UNSIGNED_BYTE,
                                           tex_data.pTexture.get(),
                                           false);
   }

   // reset the number of patches
   mNumOfPatches = 0;

   // generate a plane to match the size of the displacement map
   const auto plane = GeomHelper::ConstructPlane(static_cast< float >(mDispMapTex.GetWidth()),
                                                 static_cast< float >(mDispMapTex.GetHeight()));

   // determine the steps between the vertices and the texture coords
   const uint32_t num_subdivisions = mNumTiles;
   const float vert_step_w = static_cast< float >(mDispMapTex.GetWidth()) / num_subdivisions;
   const float vert_step_h = static_cast< float >(mDispMapTex.GetHeight()) / num_subdivisions;
   const float tcoord_step_s = 1.0f / num_subdivisions;
   const float tcoord_step_t = 1.0f / num_subdivisions;

   std::vector< float > verts_tex_coords;

   for (uint32_t i = 0; i < num_subdivisions; ++i)
   {
      const Vec3f ll_vert(plane.vertices.at(0).X(), 0.0f, plane.vertices.at(0).Z() - i * vert_step_h);
      const Vec2f ll_tcoord(plane.tex_coords.at(0).X(), plane.tex_coords.at(0).Y() + i * tcoord_step_t);

      for (uint32_t j = 0; j < num_subdivisions; ++j)
      {
         const Vec3f verts[] =
         {
            Vec3f(ll_vert.X() + j * vert_step_w, 0.0f, ll_vert.Z()),
            Vec3f(ll_vert.X() + (j + 1) * vert_step_w, 0.0f, ll_vert.Z()),
            Vec3f(ll_vert.X() + j * vert_step_w, 0.0f, ll_vert.Z() - vert_step_h),
            Vec3f(ll_vert.X() + j * vert_step_w, 0.0f, ll_vert.Z() - vert_step_h),
            Vec3f(ll_vert.X() + (j + 1) * vert_step_w, 0.0f, ll_vert.Z()),
            Vec3f(ll_vert.X() + (j + 1) * vert_step_w, 0.0f, ll_vert.Z() - vert_step_h)
         };

         const Vec2f tcoords[] =
         {
            Vec2f(ll_tcoord.X() + j * tcoord_step_s, ll_tcoord.Y()),
            Vec2f(ll_tcoord.X() + (j + 1) * tcoord_step_s, ll_tcoord.Y()),
            Vec2f(ll_tcoord.X() + j * tcoord_step_s, ll_tcoord.Y() + tcoord_step_t),
            Vec2f(ll_tcoord.X() + j * tcoord_step_s, ll_tcoord.Y() + tcoord_step_t),
            Vec2f(ll_tcoord.X() + (j + 1) * tcoord_step_s, ll_tcoord.Y()),
            Vec2f(ll_tcoord.X() + (j + 1) * tcoord_step_s, ll_tcoord.Y() + tcoord_step_t)
         };

         const size_t num_floats = sizeof(verts) / sizeof(float);
         verts_tex_coords.insert< const float *>(verts_tex_coords.cbegin() + (i * num_subdivisions * num_floats) + (j * num_floats),
                                 *verts,
                                 reinterpret_cast< const float * >(verts + sizeof(verts) / sizeof(*verts)));

         verts_tex_coords.insert< const float * >(verts_tex_coords.cend(),
                                                  *tcoords,
                                                  reinterpret_cast< const float * >(tcoords + sizeof(tcoords) / sizeof(*tcoords)));

         mNumOfPatches += num_floats / Vec3f::NUM_COMPONENTS;
      }
   }

   // load the data into the array buffers
   if (!mTerrainVAO) mTerrainVAO.GenArray();
   mTerrainVAO.Bind();

   if (!mTerrainVBO) mTerrainVBO.GenBuffer(GL_ARRAY_BUFFER);
   mTerrainVBO.Bind();
   mTerrainVBO.BufferData(verts_tex_coords.size() * sizeof(float), verts_tex_coords.data(), GL_STATIC_DRAW);
   mTerrainVBO.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
   mTerrainVBO.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, num_subdivisions * num_subdivisions * sizeof(Vec3f) * 6);
   mTerrainVAO.EnableVertexArrayAttrib(0);
   mTerrainVAO.EnableVertexArrayAttrib(1);
   mTerrainVBO.Unbind();

   mTerrainVAO.Unbind();

   if (!mTerrainPgm || reload_shaders)
   {
      // release the current shader
      mTerrainPgm = ShaderProgram();

      // load the terrain program
      mTerrainPgm.AttachFile(GL_VERTEX_SHADER, R"(.\displacement\shaders\terrain_tess.vert)");
      mTerrainPgm.AttachFile(GL_TESS_CONTROL_SHADER, R"(.\displacement\shaders\terrain_tess.tctrl)");
      mTerrainPgm.AttachFile(GL_TESS_EVALUATION_SHADER, R"(.\displacement\shaders\terrain_tess.teval)");
      mTerrainPgm.AttachFile(GL_GEOMETRY_SHADER, R"(.\displacement\shaders\terrain_tess.geom)");
      mTerrainPgm.AttachFile(GL_FRAGMENT_SHADER, R"(.\displacement\shaders\terrain_tess.frag)");
      mTerrainPgm.Link();

      // place the camera at a corner of the terrain
      mCamera.LookAt(Vec3f(plane.vertices[0][0], 250.0f, plane.vertices[0][2]), Vec3f(0.0f, 0.0f, 0.0f));
   }
}
