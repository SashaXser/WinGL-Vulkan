// local includes
#include "ShadowMapWindow.h"
#include "Matrix.h"
#include "Vector3.h"
#include "WglAssert.h"
#include "MatrixHelper.h"
#include "OpenGLExtensions.h"

// assimp includes
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/vector3.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

// std includes
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <iostream>

ShadowMapWindow::ShadowMapWindow( )
{
   memset(&mEnterpriseE, 0x00, sizeof(mEnterpriseE));
}

ShadowMapWindow::~ShadowMapWindow( )
{
}

bool ShadowMapWindow::Create( unsigned int nWidth,
                              unsigned int nHeight,
                              const char * pWndTitle,
                              const void * pInitParams )
{
   // initialize 40 first, then 32 second, else nothing
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 0, true, true, false }, 
      { 3, 3, true, false, false },
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      // make the context current
      MakeCurrent();

      // attach to the debug context
      AttachToDebugContext();

      // create the specific data
      GenerateSceneData();

      // enable specific state
      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);
      
      return true;
   }
   else
   {
      // post the quit message
      PostQuitMessage(-1);
   }

   return false;
}

int ShadowMapWindow::Run( )
{
   // app quit variables
   int appQuitVal = 0;
   bool bQuit = false;

   while (!bQuit)
   {
      // process all the app messages and then render the scene
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         // render the scene
         RenderScene();
      }
   }

   return appQuitVal;
}

// temp
float yaw = 0;
float pitch = 0;
float distance = 1.2;
int prev_x, prev_y;

Matrixf mv = Matrixf::LookAt(0, -distance, 0, -0, 0, 0, 0, 0.0f, 1.0f);
const Matrixf proj = Matrixf::Perspective(45.0f, 4.f/3.f, 0.01f, 3000.f);

LRESULT ShadowMapWindow::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_SIZE:
      // update the viewport
      glViewport(0, 0,
                 static_cast< GLsizei >(lParam & 0xFFFF),
                 static_cast< GLsizei >(lParam >> 16));

      break;

   case WM_MOUSEMOVE:

      // temp
      {
      int cur_x = lParam & 0xFFFF;
      int cur_y = lParam >> 16;

      if (wParam & MK_LBUTTON)
      {
         int dx = cur_x - prev_x;
         int dy = cur_y - prev_y;

         yaw += dx;
         pitch += dy;

         mv.MakeInverse();

         const Vec3f eye(mv.mT + 12);
         mv = (Matrixf::Translate(eye) * Matrixf::Rotate(90.0f, 1.0f, 0.0f, 0.0f) *
               Matrixf::Rotate(yaw, 0.0f, -1.0f, 0.0f) * Matrixf::Rotate(pitch, -1.0f, 0.0f, 0.0f)).Inverse();
      }

      prev_x = cur_x;
      prev_y = cur_y;
      }

      break;

   case WM_MOUSEWHEEL:
      {
      const float delta = static_cast< int16_t >((wParam & 0xFFFF0000) >> 16) / static_cast< float >(WHEEL_DELTA) * 0.01;

      const Vec3f view_dir = MatrixHelper::GetViewVector(mv);
      mv.MakeInverse();

      const Vec3f eye(Vec3f(mv.mT + 12) + view_dir.UnitVector() * delta);
      mv = (Matrixf::Translate(eye) * Matrixf::Rotate(90.0f, 1.0f, 0.0f, 0.0f) *
            Matrixf::Rotate(yaw, 0.0f, -1.0f, 0.0f) * Matrixf::Rotate(pitch, -1.0f, 0.0f, 0.0f)).Inverse();

      }

      break;

   default:
      // default handle the messages
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
   }

   return result;
}



void ShadowMapWindow::RenderScene( )
{
   // clear the buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   

   // temp
   glMatrixMode(GL_PROJECTION);
   glLoadMatrixf(proj);
   glMatrixMode(GL_MODELVIEW);
   glLoadMatrixf(mv);
   glBindVertexArray(mEnterpriseE.mVAO);
   glDrawArrays(GL_TRIANGLES, 0, mEnterpriseE.mVertSize / 3);
   //glColor3f(1,1,1);
   //glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, nullptr);
   //glBindVertexArray(mCube.mVAO);
   //glColor3f(1,0,0);
   //glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, nullptr);
   //glBindVertexArray(mPyramid.mVAO);
   //glPushMatrix();
   //glTranslated(20, 0, 0);
   //glColor3f(0,1,0);
   //glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, nullptr);
   //glPopMatrix();
   //glBindVertexArray(mSphere.mVAO);
   //glPushMatrix();
   //glTranslated(-20, 0, 0);
   //glColor3f(0,0,1);
   //glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, nullptr);
   //glPopMatrix();
   glBindVertexArray(0);


   // swap the front and back
   SwapBuffers(GetHDC());
}

void ShadowMapWindow::GenerateSceneData( )
{
   GenerateEnterpriseE();
}

void ShadowMapWindow::GenerateEnterpriseE( )
{
   // a set of all the mesh data
   std::vector< float > colors;
   std::vector< float > normals;
   std::vector< float > vertices;

   const auto GenColors = [ ] ( const size_t size, std::vector< float > & colors )
   {
      for (size_t i = 0; i < size; i += 3)
      {
         const float r = static_cast< float >(std::rand()) / static_cast< float >(RAND_MAX);
         const float g = static_cast< float >(std::rand()) / static_cast< float >(RAND_MAX);
         const float b = static_cast< float >(std::rand()) / static_cast< float >(RAND_MAX);

         for (size_t j = 0; j < 3; ++j)
         {
            colors.push_back(r);
            colors.push_back(g);
            colors.push_back(b);
         }
      }
   };

   const auto ReadModel = [ & ] ( const char * const pFilename )
   {
      Assimp::Importer model_import;
      const aiScene * const pSceneHull =
         model_import.ReadFile(pFilename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

      WGL_ASSERT(pSceneHull);

      if (pSceneHull)
      {
         for (size_t cur_mesh = 0; cur_mesh < pSceneHull->mNumMeshes; ++cur_mesh)
         {
            // obatin all the required data for this mesh...
            const aiMesh * const pCurMesh = pSceneHull->mMeshes[cur_mesh];
            const aiVector3D *const pVertices = pCurMesh->mVertices;
            const aiVector3D * const pNormals = pCurMesh->mNormals;
            const size_t num_verts = pCurMesh->mNumVertices;

            GenColors(num_verts, colors);
            normals.insert(normals.end(), &pNormals->x, &pNormals->x + num_verts * 3);
            vertices.insert(vertices.end(), &pVertices->x, &pVertices->x + num_verts * 3);
         }
      }
      else
      {
         std::cout << "Error : Unable to read " << pFilename << std::endl;
      }
   };

   // read all the models in
   ReadModel(".\\enterprise-e\\1701e_hull_7.3ds");
   ReadModel(".\\enterprise-e\\1701e_saucer_down_5.3ds");
   ReadModel(".\\enterprise-e\\1701e_saucer_top_6.3ds");

   // create the vao
   glGenVertexArrays(1, &mEnterpriseE.mVAO);
   glBindVertexArray(mEnterpriseE.mVAO);

   // there should always be a vao here
   WGL_ASSERT(mEnterpriseE.mVAO);

   if (!mEnterpriseE.mVAO)
   {
      std::cout << "Error : Unable to generate VAO" << std::endl;
   }

   // create the vbo
   glGenBuffers(1, &mEnterpriseE.mVertBufID);
   glBindBuffer(GL_ARRAY_BUFFER, mEnterpriseE.mVertBufID);
   glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(decltype(vertices.front())), &vertices.front(), GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
   glEnableVertexAttribArray(0);

   glGenBuffers(1, &mEnterpriseE.mClrBufID);
   glBindBuffer(GL_ARRAY_BUFFER, mEnterpriseE.mClrBufID);
   glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(decltype(colors.front())), &colors.front(), GL_STATIC_DRAW);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, nullptr);
   glColorPointer(3, GL_FLOAT, 0, nullptr);
   glEnableClientState(GL_COLOR_ARRAY);
   glEnableVertexAttribArray(1);

   // save off the number of vertices
   mEnterpriseE.mVertSize = vertices.size();

   // disable the vao
   glBindVertexArray(0);
}

//void ShadowMapWindow::GenerateFloor( )
//{
//   // create the vao
//   glGenVertexArrays(1, &mFloor.mVAO);
//   glBindVertexArray(mFloor.mVAO);
//
//   // create some very basic vertex data
//   const float verts[] =
//   {
//      -100.0f, 0.0f,  100.0f,  100.0f, 0.0f,  100.0f,
//       100.0f, 0.0f, -100.0f, -100.0f, 0.0f, -100.0f
//   };
//
//   // create and fill the vertex buffer
//   glGenBuffers(1, &mFloor.mVertBufID);
//   glBindBuffer(GL_ARRAY_BUFFER, mFloor.mVertBufID);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
//   glEnableVertexAttribArray(0);
//
//   // create the index vertex data
//   const GLuint indices[] = { 0, 1, 2, 3 };
//
//   // crate and fill the index buffer
//   glGenBuffers(1, &mFloor.mIdxBufID);
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFloor.mIdxBufID);
//   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//   // disable the vao
//   glBindVertexArray(0);
//}
//
//void ShadowMapWindow::GenerateCube( )
//{
//   // create the vao
//   glGenVertexArrays(1, &mCube.mVAO);
//   glBindVertexArray(mCube.mVAO);
//
//   // create some very basic vertex data
//   const float verts[] =
//   {
//      -10.0f, -10.0f,  10.0f,  10.0f, -10.0f,  10.0f,  10.0f,  10.0f,  10.0f, -10.0f,  10.0f,  10.0f,
//       10.0f, -10.0f,  10.0f,  10.0f, -10.0f, -10.0f,  10.0f,  10.0f, -10.0f,  10.0f,  10.0f,  10.0f,
//       10.0f, -10.0f, -10.0f, -10.0f, -10.0f, -10.0f, -10.0f,  10.0f, -10.0f,  10.0f,  10.0f, -10.0f,
//      -10.0f, -10.0f, -10.0f, -10.0f, -10.0f,  10.0f, -10.0f,  10.0f,  10.0f, -10.0f,  10.0f, -10.0f,
//      -10.0f, -10.0f, -10.0f,  10.0f, -10.0f, -10.0f,  10.0f, -10.0f,  10.0f, -10.0f, -10.0f,  10.0f,
//      -10.0f,  10.0f,  10.0f,  10.0f,  10.0f,  10.0f,  10.0f,  10.0f, -10.0f, -10.0f,  10.0f, -10.0f
//   };
//
//   // create and fill the vertex buffer
//   glGenBuffers(1, &mCube.mVertBufID);
//   glBindBuffer(GL_ARRAY_BUFFER, mCube.mVertBufID);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
//   glEnableVertexAttribArray(0);
//
//   // create the index vertex data
//   const GLuint indices[] =
//   {
//       0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
//      10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
//      20, 21, 22, 23
//   };
//
//   // crate and fill the index buffer
//   glGenBuffers(1, &mCube.mIdxBufID);
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mCube.mIdxBufID);
//   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//   // disable the vao
//   glBindVertexArray(0);
//}
//
//void ShadowMapWindow::GenerateSphere( )
//{
//   // create the vao
//   glGenVertexArrays(1, &mSphere.mVAO);
//   glBindVertexArray(mSphere.mVAO);
//
//   // create some very basic vertex data
//   const float verts[] =
//   {
//      -10.0f, -10.0f,  10.0f,  10.0f, -10.0f,  10.0f,  10.0f,  10.0f,  10.0f, -10.0f,  10.0f,  10.0f,
//       10.0f, -10.0f,  10.0f,  10.0f, -10.0f, -10.0f,  10.0f,  10.0f, -10.0f,  10.0f,  10.0f,  10.0f,
//       10.0f, -10.0f, -10.0f, -10.0f, -10.0f, -10.0f, -10.0f,  10.0f, -10.0f,  10.0f,  10.0f, -10.0f,
//      -10.0f, -10.0f, -10.0f, -10.0f, -10.0f,  10.0f, -10.0f,  10.0f,  10.0f, -10.0f,  10.0f, -10.0f,
//      -10.0f, -10.0f, -10.0f,  10.0f, -10.0f, -10.0f,  10.0f, -10.0f,  10.0f, -10.0f, -10.0f,  10.0f,
//      -10.0f,  10.0f,  10.0f,  10.0f,  10.0f,  10.0f,  10.0f,  10.0f, -10.0f, -10.0f,  10.0f, -10.0f
//   };
//
//   // create and fill the vertex buffer
//   glGenBuffers(1, &mSphere.mVertBufID);
//   glBindBuffer(GL_ARRAY_BUFFER, mSphere.mVertBufID);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
//   glEnableVertexAttribArray(0);
//
//   // create the index vertex data
//   const GLuint indices[] =
//   {
//       0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
//      10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
//      20, 21, 22, 23
//   };
//
//   // crate and fill the index buffer
//   glGenBuffers(1, &mSphere.mIdxBufID);
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mSphere.mIdxBufID);
//   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//   // disable the vao
//   glBindVertexArray(0);
//}
//
//void ShadowMapWindow::GeneratePyramid( )
//{
//   // create the vao
//   glGenVertexArrays(1, &mPyramid.mVAO);
//   glBindVertexArray(mPyramid.mVAO);
//
//   // create some very basic vertex data
//   const float verts[] =
//   {
//      -10.0f, -10.0f, -10.0f,  10.0f, -10.0f, -10.0f,  10.0f, -10.0f,  10.0f,
//       10.0f, -10.0f,  10.0f, -10.0f, -10.0f,  10.0f, -10.0f, -10.0f, -10.0f,
//        0.0f,  10.0f,   0.0f,  10.0f, -10.0f,  10.0f,  10.0f, -10.0f, -10.0f,
//        0.0f,  10.0f,   0.0f,  10.0f, -10.0f, -10.0f, -10.0f, -10.0f, -10.0f,
//        0.0f,  10.0f,   0.0f, -10.0f, -10.0f, -10.0f, -10.0f, -10.0f,  10.0f,
//        0.0f,  10.0f,   0.0f, -10.0f, -10.0f,  10.0f,  10.0f, -10.0f,  10.0f
//   };
//
//   // create and fill the vertex buffer
//   glGenBuffers(1, &mPyramid.mVertBufID);
//   glBindBuffer(GL_ARRAY_BUFFER, mPyramid.mVertBufID);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
//   glEnableVertexAttribArray(0);
//
//   // create the index vertex data
//   const GLuint indices[] =
//   {
//       0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17
//   };
//
//   // crate and fill the index buffer
//   glGenBuffers(1, &mPyramid.mIdxBufID);
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPyramid.mIdxBufID);
//   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//   // disable the vao
//   glBindVertexArray(0);
//}

