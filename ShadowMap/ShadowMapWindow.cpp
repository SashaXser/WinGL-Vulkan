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
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <algorithm>

ShadowMapWindow::ShadowMapWindow( )
{
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

      // setup the shaders
      if (!mEnterpriseE.mProgram.AttachFile(GL_VERTEX_SHADER, "enterprise.vert") ||
          !mEnterpriseE.mProgram.AttachFile(GL_FRAGMENT_SHADER, "enterprise.frag") ||
          !mEnterpriseE.mProgram.Link())
      {
         return false;
      }

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
int prev_x, prev_y;
bool light_per_pixel = true;

Matrixf mv = Matrixf::LookAt(0, 40.0f, 100.0f, 0, 40.0f, 0.0f, 0, 1.0f, 0.0f);
Matrixf mvn = mv.Inverse().Transpose();
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
         mv = (Matrixf::Translate(eye) /** Matrixf::Rotate(90.0f, 1.0f, 0.0f, 0.0f)*/ *
               Matrixf::Rotate(yaw, 0.0f, -1.0f, 0.0f) * Matrixf::Rotate(pitch, -1.0f, 0.0f, 0.0f)).Inverse();
         mvn = mv.Inverse().Transpose();
      }

      prev_x = cur_x;
      prev_y = cur_y;
      }

      break;

   case WM_MOUSEWHEEL:
      {
      const float multiplier = wParam & MK_CONTROL ? 50.0f : wParam & MK_SHIFT ? 100.0f : 1.0f;
      const float delta = static_cast< int16_t >((wParam & 0xFFFF0000) >> 16) / static_cast< float >(WHEEL_DELTA) * 0.005f * multiplier;

      const Vec3f view_dir = MatrixHelper::GetViewVector(mv);
      mv.MakeInverse();

      const Vec3f eye(Vec3f(mv.mT + 12) + view_dir.UnitVector() * delta);
      mv = (Matrixf::Translate(eye) /** Matrixf::Rotate(90.0f, 1.0f, 0.0f, 0.0f)*/ *
            Matrixf::Rotate(yaw, 0.0f, -1.0f, 0.0f) * Matrixf::Rotate(pitch, -1.0f, 0.0f, 0.0f)).Inverse();
      mvn = mv.Inverse().Transpose();

      }

      break;

   case WM_CHAR:
      if (wParam == 'l' || wParam == 'L')
         light_per_pixel = !light_per_pixel;

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
   static float light_dir = 0.0f;
   mEnterpriseE.mProgram.Enable();
   mEnterpriseE.mProgram.SetUniformValue("light_dir", std::sin(light_dir), 0.0f, std::cos(light_dir));
   //mEnterpriseE.mProgram.SetUniformValue("light_dir", 0.0f, 0.0f, -1.0f);
   mEnterpriseE.mProgram.SetUniformMatrix< 1, 4, 4 >("projection", proj);
   mEnterpriseE.mProgram.SetUniformMatrix< 1, 4, 4 >("model_view", mv);
   mEnterpriseE.mProgram.SetUniformMatrix< 1, 4, 4 >("model_view_normal", mvn);
   mEnterpriseE.mProgram.SetUniformValue("light_per_pixel", light_per_pixel ? 1 : 0);

   Vec4f en = mvn * Vec4f(0.0f, 0.0f, 1.0f, 0.0f);

   light_dir += 0.0005f;

   mEnterpriseE.mVAO.Bind();

   //glDrawArrays(GL_TRIANGLES, 0, static_cast< GLsizei >(mEnterpriseE.mVertBuf.Size< float >() / 3));
   glDrawElements(GL_TRIANGLES, static_cast< GLsizei >(mEnterpriseE.mIdxBuf.Size< uint32_t >()), GL_UNSIGNED_INT, nullptr);

   mEnterpriseE.mVAO.Unbind();


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
   std::vector< uint32_t > indices;

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

   const auto ConstructChildNodeMatrix = [ ] ( const char * const pNodeName, const aiScene * const pScene ) -> Matrixf
   {
      const aiNode * const * ppChildrenBeg = pScene->mRootNode->mChildren;
      const aiNode * const * ppChildrenEnd = pScene->mRootNode->mChildren + pScene->mRootNode->mNumChildren;

      const uint32_t node_name_id = std::stoul(pNodeName);

      const auto ppNode =
         std::find_if(ppChildrenBeg, ppChildrenEnd,
         [ & ] ( const aiNode * const pNode ) -> bool
         {
            const auto pID =
               std::find_if(pNode->mMeshes, pNode->mMeshes + pNode->mNumMeshes,
               [ & ] ( const uint32_t id ) { return node_name_id == id; });

            return pID != pNode->mMeshes + pNode->mNumMeshes;
         });

      Matrixf model_matrix;

      if (ppNode)
      {
         for (const aiNode * pNode = *ppNode; pNode; pNode = pNode->mParent)
         {
            model_matrix = Matrixf(&pNode->mTransformation.a1).Transpose() * model_matrix;
         }
      }

      return model_matrix;
   };

   const auto ReadModel = [ & ] ( const char * const pFilename )
   {
      Assimp::Importer model_import;
      const aiScene * const pScene = model_import.ReadFile(pFilename, 0);

      WGL_ASSERT(pScene);

      if (pScene)
      {
         auto beg = pScene->mMaterials;
         auto end = pScene->mMaterials + pScene->mNumMaterials;

         while (beg != end)
         {
            if ((*beg)->GetTextureCount(aiTextureType_DIFFUSE))
            {
               aiString path;
               (*beg)->GetTexture(aiTextureType_DIFFUSE, 0, &path);
               ++beg;
               continue;
            }
            ++beg;
         }

         for (size_t cur_mesh = 0; cur_mesh < pScene->mNumMeshes; ++cur_mesh)
         {
            // obatin all the required data for this mesh...
            const aiMesh * const pCurMesh = pScene->mMeshes[cur_mesh];
            const aiVector3D * const pVertices = pCurMesh->mVertices;
            const aiVector3D * const pNormals = pCurMesh->mNormals;
            const size_t num_verts = pCurMesh->mNumVertices;

            // temp - find better way
            Matrixf mesh_matrix = ConstructChildNodeMatrix(pCurMesh->mName.C_Str(), pScene);

            // there should always be triangles in this model...
            WGL_ASSERT(pCurMesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE);

            GenColors(num_verts, colors);
            normals.insert(normals.end(), &pNormals->x, &pNormals->x + num_verts * 3);
            //vertices.insert(vertices.end(), &pVertices->x, &pVertices->x + num_verts * 3);
            // temp - find better way
            for (int i = 0; num_verts > i; ++i)
            {
               Vec3f verts = mesh_matrix * Vec3f(&((pVertices + i)->x));
               vertices.insert(vertices.end(), verts.mT, verts.mT + 3);
            }

            // establish what the base index for this model is
            const uint32_t base_index = static_cast< uint32_t >(indices.size());

            // read in all the faces for the mesh
            std::for_each(pCurMesh->mFaces, pCurMesh->mFaces + pCurMesh->mNumFaces,
            [ & ] ( const aiFace & cur_face )
            {
               // should always be three indices that make up this triangle
               WGL_ASSERT(cur_face.mNumIndices == 3);

               indices.push_back(base_index + cur_face.mIndices[0]);
               indices.push_back(base_index + cur_face.mIndices[1]);
               indices.push_back(base_index + cur_face.mIndices[2]);
            });
         }
      }
      else
      {
         std::cout << "Error : Unable to read " << pFilename << std::endl;
      }
   };

   // read all the models in
   ReadModel(".\\enterprise\\Enterp TOS - Arena.3DS");

   //vertices.push_back(-5.0f); vertices.push_back(5.0f); vertices.push_back(0.0f);
   //vertices.push_back(-5.0f); vertices.push_back(-5.0f); vertices.push_back(0.0f);
   //vertices.push_back(5.0f); vertices.push_back(5.0f); vertices.push_back(0.0f);
   //vertices.push_back(5.0f); vertices.push_back(5.0f); vertices.push_back(0.0f);
   //vertices.push_back(-5.0f); vertices.push_back(-5.0f); vertices.push_back(0.0f);
   //vertices.push_back(5.0f); vertices.push_back(-5.0f); vertices.push_back(0.0f);

   //colors.push_back(1.0f); colors.push_back(0.0f); colors.push_back(0.0f);
   //colors.push_back(1.0f); colors.push_back(0.0f); colors.push_back(0.0f);
   //colors.push_back(1.0f); colors.push_back(0.0f); colors.push_back(0.0f);
   //colors.push_back(0.0f); colors.push_back(1.0f); colors.push_back(0.0f);
   //colors.push_back(0.0f); colors.push_back(1.0f); colors.push_back(0.0f);
   //colors.push_back(0.0f); colors.push_back(1.0f); colors.push_back(0.0f);

   //Vec3f right(10.0f, 0.0f, 1.0f); right.Normalize();
   //Vec3f left(-10.0f, 0.0f, 1.0f); left.Normalize();
   //normals.insert(normals.end(), left.mT, left.mT + 3);
   //normals.insert(normals.end(), left.mT, left.mT + 3);
   //normals.insert(normals.end(), right.mT, right.mT + 3);
   //normals.insert(normals.end(), right.mT, right.mT + 3);
   //normals.insert(normals.end(), left.mT, left.mT + 3);
   //normals.insert(normals.end(), right.mT, right.mT + 3);

   //indices.push_back(0); indices.push_back(1); indices.push_back(2);
   //indices.push_back(3); indices.push_back(4); indices.push_back(5);

   // create the vao
   mEnterpriseE.mVAO.GenArray();
   mEnterpriseE.mVAO.Bind();

   // there should always be a vao here
   WGL_ASSERT(mEnterpriseE.mVAO);

   if (!mEnterpriseE.mVAO)
   {
      std::cout << "Error : Unable to generate VAO" << std::endl;
   }

   // create the vbos
   mEnterpriseE.mVertBuf.GenBuffer(GL_ARRAY_BUFFER);
   mEnterpriseE.mVertBuf.Bind();
   mEnterpriseE.mVertBuf.BufferData(vertices.size() * sizeof(decltype(vertices.front())), &vertices.front(), GL_STATIC_DRAW);
   mEnterpriseE.mVertBuf.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(decltype(vertices.front())) * 3, 0);
   glEnableVertexAttribArray(0);
   mEnterpriseE.mVertBuf.Unbind();

   mEnterpriseE.mClrBuf.GenBuffer(GL_ARRAY_BUFFER);
   mEnterpriseE.mClrBuf.Bind();
   mEnterpriseE.mClrBuf.BufferData(colors.size() * sizeof(decltype(colors.front())), &colors.front(), GL_STATIC_DRAW);
   mEnterpriseE.mClrBuf.VertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(decltype(colors.front())) * 3, 0);
   glEnableVertexAttribArray(1);
   mEnterpriseE.mClrBuf.Unbind();

   mEnterpriseE.mNormBuf.GenBuffer(GL_ARRAY_BUFFER);
   mEnterpriseE.mNormBuf.Bind();
   mEnterpriseE.mNormBuf.BufferData(normals.size() * sizeof(decltype(normals.front())), &normals.front(), GL_STATIC_DRAW);
   mEnterpriseE.mNormBuf.VertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(decltype(normals.front())) * 3, 0);
   glEnableVertexAttribArray(2);
   mEnterpriseE.mNormBuf.Unbind();

   // create the index buffer
   mEnterpriseE.mIdxBuf.GenBuffer(GL_ELEMENT_ARRAY_BUFFER);
   mEnterpriseE.mIdxBuf.Bind();
   mEnterpriseE.mIdxBuf.BufferData(indices.size() * sizeof(decltype(indices.front())), &indices.front(), GL_STATIC_DRAW);

   // disable the vao
   mEnterpriseE.mVAO.Unbind();

   // must unbind the index buffer after unbinding the vao
   mEnterpriseE.mIdxBuf.Unbind();
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

