// local includes
#include "ShadowMapWindow.h"
#include "Matrix.h"
#include "Vector3.h"
#include "WglAssert.h"
#include "ReadTexture.h"
#include "MatrixHelper.h"
#include "ShaderProgram.h"
#include "OpenGLExtensions.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

// assimp 
#include "assimp/mesh.h"
#include "assimp/types.h"
#include "assimp/scene.h"
#include "assimp/vector3.h"
#include "assimp/material.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

// std includes
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <utility>
#include <algorithm>

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// defines a basic renderable object
struct ShadowMapWindow::Renderable
{
   // typedefs
   typedef std::vector< GLuint > TextureCtr;
   typedef std::multimap< GLuint, std::pair< GLuint, GLsizei > > RenderBucket;

   // vao id
   VAO      mVAO;
   // vbo ids
   VBO      mVertBuf;
   VBO      mIdxBuf;
   VBO      mTexBuf;
   VBO      mNormBuf;
   VBO      mClrBuf;
   // texture container
   TextureCtr     mTexIDs;
   // shader program
   ShaderProgram  mProgram;
   // defines the render order by texture
   // unit... group multiple items together...
   RenderBucket   mRenderBuckets;
};

ShadowMapWindow::ShadowMapWindow( ) :
mpEnterpriseE     ( new Renderable )
{
}

ShadowMapWindow::~ShadowMapWindow( )
{
   WGL_ASSERT(!mpEnterpriseE);
}

void ShadowMapWindow::OnDestroy( )
{
   // should still have a valid context
   WGL_ASSERT(ContextIsCurrent());

   // release all the textures...
   // need to move the textures into an object...
   std::for_each(mpEnterpriseE->mTexIDs.cbegin(),
                 mpEnterpriseE->mTexIDs.cend(),
   [ ] ( const GLuint tex_id )
   {
      if (glIsTexture(tex_id))
      {
         glDeleteTextures(1, &tex_id);
      }
   });

   // clean up the enterprise model
   delete mpEnterpriseE; mpEnterpriseE = nullptr;

   // call the base class to clean things up
   OpenGLWindow::OnDestroy();
}

bool ShadowMapWindow::Create( unsigned int nWidth,
                              unsigned int nHeight,
                              const char * pWndTitle,
                              const void * pInitParams )
{
   // initialize 40 first, then 32 second, else nothing
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

      // attach to the debug context
      AttachToDebugContext();

      // create the specific data
      GenerateSceneData();

      // setup the shaders
      if (!mpEnterpriseE->mProgram.AttachFile(GL_VERTEX_SHADER, "enterprise.vert") ||
          !mpEnterpriseE->mProgram.AttachFile(GL_FRAGMENT_SHADER, "enterprise.frag") ||
          !mpEnterpriseE->mProgram.Link())
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
      // issue an error from the application that it could not be created
      PostDebugMessage(GL_DEBUG_TYPE_ERROR, 1, GL_DEBUG_SEVERITY_HIGH, "Unable To Create 4.4 OpenGL Context");

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
   mpEnterpriseE->mProgram.Enable();
   mpEnterpriseE->mProgram.SetUniformValue("light_dir", std::sin(light_dir), 0.0f, std::cos(light_dir));
   //mpEnterpriseE->mProgram.SetUniformValue("light_dir", 0.0f, 0.0f, -1.0f);
   mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("projection", proj);
   mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("model_view", mv);
   mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("model_view_normal", mvn);
   mpEnterpriseE->mProgram.SetUniformValue("light_per_pixel", light_per_pixel ? 1 : 0);

   Vec4f en = mvn * Vec4f(0.0f, 0.0f, 1.0f, 0.0f);

   light_dir += 0.0005f;

   mpEnterpriseE->mVAO.Bind();

   //glDrawArrays(GL_TRIANGLES, 0, static_cast< GLsizei >(mpEnterpriseE->mVertBuf.Size< float >() / 3));
   //glDrawElements(GL_TRIANGLES, static_cast< GLsizei >(mpEnterpriseE->mIdxBuf.Size< uint32_t >()), GL_UNSIGNED_INT, nullptr);

   GLuint current_tex_id = 0;
   auto rbucketBeg = mpEnterpriseE->mRenderBuckets.cbegin();
   const auto rbucketEnd = mpEnterpriseE->mRenderBuckets.cend();

   for (; rbucketBeg != rbucketEnd; ++rbucketBeg)
   {
      GLuint rbucket_tex_id = mpEnterpriseE->mTexIDs[rbucketBeg->first];

      if (current_tex_id != rbucket_tex_id)
      {
         current_tex_id = rbucket_tex_id;

         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, rbucket_tex_id);

         if (!current_tex_id)
         {
            mpEnterpriseE->mProgram.SetUniformValue("tex_unit_0_active", 0);
         }
         else
         {
            mpEnterpriseE->mProgram.SetUniformValue("tex_unit_0_active", 1);
         }
      }

      glDrawElements(GL_TRIANGLES, rbucketBeg->second.second, GL_UNSIGNED_INT, reinterpret_cast< void * >(rbucketBeg->second.first * sizeof(rbucketBeg->second.first)));
   }

   glBindTexture(GL_TEXTURE_2D, 0);

   mpEnterpriseE->mVAO.Unbind();


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
   std::vector< float > tex_coords;
   std::vector< uint32_t > indices;
   std::multimap< GLuint, std::pair< GLuint, GLsizei > > render_buckets;

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

   const auto ReadDiffuseTextures = [ ] ( const aiScene * const pScene,
                                          const std::string & base_model_path,
                                          std::vector< GLuint > & textures )
   {
      // make sure we do not load the same texture twice
      std::map< std::string, GLuint > texture_filenames;

      // iterate over all the materials collecting the diffuse texture
      const aiMaterial * const * pBeg = pScene->mMaterials;
      const aiMaterial * const * const pEnd = pScene->mMaterials + pScene->mNumMaterials;

      for (; pBeg != pEnd; ++pBeg)
      {
         // begin by inserting a null handle into the texture array
         textures.push_back(0);

         // if there is a diffuse texture, then set it up
         if ((*pBeg)->GetTextureCount(aiTextureType_DIFFUSE))
         {
            // get the texture associated with this material
            const std::string filename = [ &pBeg, &base_model_path ] ( ) -> std::string
            {
               aiString filename; (*pBeg)->GetTexture(aiTextureType_DIFFUSE, 0, &filename); return base_model_path + filename.C_Str();
            }();

            // make sure the length is valid
            if (!filename.empty())
            {
               // if the value has already been seen, then use that handle; otherwise create it
               const auto tex_filename = texture_filenames.find(filename);

               if (texture_filenames.find(filename) != texture_filenames.end())
               {
                  // already seen, so just reuse the handle id
                  textures.back() = tex_filename->second;
               }
               else
               {
                  // read the texture
                  uint32_t tex_width = 0;
                  uint32_t tex_height = 0;
                  std::shared_ptr< uint8_t > pTexBuffer = nullptr;

                  if (ReadTexture(filename.c_str(), GL_BGRA, tex_width, tex_height, pTexBuffer))
                  {
                     // generate a new texture
                     const GLuint tex_id = [ ] ( ) -> GLuint { GLuint tex_id = 0; glGenTextures(1, &tex_id); return tex_id; }();

                     // load the texture into memory
                     glBindTexture(GL_TEXTURE_2D, tex_id);
                     glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, tex_width, tex_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pTexBuffer.get()); 
                     glGenerateMipmap(GL_TEXTURE_2D);
                     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                     glBindTexture(GL_TEXTURE_2D, 0);

                     // obtain the handle for this texture
                     // this is currently not supported on my HD 5850 with driver version 14.4 (14.100)
                     //const GLuint64 tex_handle = glGetTextureHandleARB(tex_id);

                     // save for later use
                     textures.back() = tex_id;
                     texture_filenames[filename] = tex_id;
                  }
               }
            }
         }
      }
   };

   const auto ReadModel = [ & ] ( const char * const pFilename )
   {
      Assimp::Importer model_import;
      const aiScene * const pScene = model_import.ReadFile(pFilename, 0);

      WGL_ASSERT(pScene);

      if (pScene)
      {
         // gets the base path for the model
         const auto GetBasePath = [ ] ( const char * const pFilename ) -> std::string
         {
            const char * const pLoc = std::strrchr(pFilename, '\\');

            return std::string(pFilename, pLoc ? pLoc + 1 : pFilename + std::strlen(pFilename));
         };

         // start off by reading the diffuse textures
         ReadDiffuseTextures(pScene, GetBasePath(pFilename), mpEnterpriseE->mTexIDs);

         for (size_t cur_mesh = 0; cur_mesh < pScene->mNumMeshes; ++cur_mesh)
         {
            // establish what the base index for this mesh is
            const uint32_t base_index = static_cast< uint32_t >(indices.size());

            // obatin all the required data for this mesh...
            const aiMesh * const pCurMesh = pScene->mMeshes[cur_mesh];
            const aiVector3D * const pVertices = pCurMesh->mVertices;
            const aiVector3D * const pNormals = pCurMesh->mNormals;
            const aiVector3D * const pTexCoords = pCurMesh->mTextureCoords[0];
            const size_t num_verts = pCurMesh->mNumVertices;

            // construct the child node matrix to translate all the vertices by
            const Matrixf mesh_matrix = ConstructChildNodeMatrix(pCurMesh->mName.C_Str(), pScene);

            // construct the normal matrix to translate all the normals by
            const Matrixf normal_matrix = mesh_matrix.Inverse().Transpose();

            // there should always be triangles in this model...
            WGL_ASSERT(pCurMesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE);

            // temp for now...
            GenColors(num_verts, colors);
            normals.insert(normals.end(), &pNormals->x, &pNormals->x + num_verts * 3);

            if (pTexCoords)
            {
               // we have texture coordinates, so use them...
               tex_coords.insert(tex_coords.end(), &pTexCoords->x, &pTexCoords->x + num_verts * 3);
            }
            else
            {
               // we do not have texture coordinates, so fill in the gaps with a zero to keep them in sync...
               tex_coords.insert(tex_coords.end(), num_verts * 3, 0.0f);

               // determine what the diffuse color is for this mesh
               const aiMaterial * const pMat = pScene->mMaterials[pCurMesh->mMaterialIndex];
               const aiColor3D diffuse_color = [ &pMat ] ( ) -> aiColor3D
               { aiColor3D clr; pMat->Get(AI_MATKEY_COLOR_DIFFUSE, clr); return clr; }();

               // overwrite the colors with this diffuse color
               std::for_each(reinterpret_cast< Vec3f * >(colors.data()) + base_index,
                             reinterpret_cast< Vec3f * >(colors.data()) + base_index + num_verts,
               [ &diffuse_color ] ( Vec3f & diffuse) { diffuse = Vec3f(&diffuse_color.r); });
            }

            for (int i = 0; num_verts > i; ++i)
            {
               // need to translate the vertices as they may be in the wrong place...
               const Vec3f vert = mesh_matrix * Vec3f(&((pVertices + i)->x));
               vertices.insert(vertices.end(), vert.mT, vert.mT + 3);

               // need to translate the normals to the correct location as they too may be in the wrong place...
               //const Vec3f norm = Vec3f(normal_matrix * Vec4f(pNormals->x, pNormals->y, pNormals->z, 0.0f)).MakeUnitVector();
               //normals.insert(normals.end(), norm.mT, norm.mT + 3);
            }

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

            // insert into the render buckets based on the texture index
            // this could be more efficient by pairing up the last bucket and the new bucket
            // to see if the indices could be combined, but we can leave that for another time...
            typedef std::remove_reference< decltype(render_buckets) >::type render_bucket_type;
            const render_bucket_type::mapped_type mesh_indices(base_index, static_cast< GLsizei >(indices.size() - base_index));
            render_buckets.insert(render_bucket_type::value_type(pCurMesh->mMaterialIndex, mesh_indices));
         }
      }
      else
      {
         PostDebugMessage(GL_DEBUG_TYPE_ERROR, 3, GL_DEBUG_SEVERITY_HIGH,
                          static_cast< std::stringstream & >(std::stringstream() << "Unable to read " << pFilename).str().c_str());
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
   mpEnterpriseE->mVAO.GenArray();
   mpEnterpriseE->mVAO.Bind();

   // there should always be a vao here
   WGL_ASSERT(mpEnterpriseE->mVAO);

   if (!mpEnterpriseE->mVAO)
   {
      PostDebugMessage(GL_DEBUG_TYPE_ERROR, 2, GL_DEBUG_SEVERITY_HIGH, "Unable to generate VAO!!!");
   }

   // create the vbos
   mpEnterpriseE->mVertBuf.GenBuffer(GL_ARRAY_BUFFER);
   mpEnterpriseE->mVertBuf.Bind();
   mpEnterpriseE->mVertBuf.BufferData(vertices.size() * sizeof(decltype(vertices.front())), &vertices.front(), GL_STATIC_DRAW);
   mpEnterpriseE->mVertBuf.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(decltype(vertices.front())) * 3, 0);
   glEnableVertexAttribArray(0);
   mpEnterpriseE->mVertBuf.Unbind();

   mpEnterpriseE->mClrBuf.GenBuffer(GL_ARRAY_BUFFER);
   mpEnterpriseE->mClrBuf.Bind();
   mpEnterpriseE->mClrBuf.BufferData(colors.size() * sizeof(decltype(colors.front())), &colors.front(), GL_STATIC_DRAW);
   mpEnterpriseE->mClrBuf.VertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(decltype(colors.front())) * 3, 0);
   glEnableVertexAttribArray(1);
   mpEnterpriseE->mClrBuf.Unbind();

   mpEnterpriseE->mNormBuf.GenBuffer(GL_ARRAY_BUFFER);
   mpEnterpriseE->mNormBuf.Bind();
   mpEnterpriseE->mNormBuf.BufferData(normals.size() * sizeof(decltype(normals.front())), &normals.front(), GL_STATIC_DRAW);
   mpEnterpriseE->mNormBuf.VertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(decltype(normals.front())) * 3, 0);
   glEnableVertexAttribArray(2);
   mpEnterpriseE->mNormBuf.Unbind();

   mpEnterpriseE->mTexBuf.GenBuffer(GL_ARRAY_BUFFER);
   mpEnterpriseE->mTexBuf.Bind();
   mpEnterpriseE->mTexBuf.BufferData(tex_coords.size() * sizeof(decltype(tex_coords.front())), &tex_coords.front(), GL_STATIC_DRAW);
   mpEnterpriseE->mTexBuf.VertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, sizeof(decltype(tex_coords.front())) * 3, 0);
   glEnableVertexAttribArray(3);
   mpEnterpriseE->mTexBuf.Unbind();

   // create the index buffer
   mpEnterpriseE->mIdxBuf.GenBuffer(GL_ELEMENT_ARRAY_BUFFER);
   mpEnterpriseE->mIdxBuf.Bind();
   mpEnterpriseE->mIdxBuf.BufferData(indices.size() * sizeof(decltype(indices.front())), &indices.front(), GL_STATIC_DRAW);

   // make sure we save the render buckets to the model
   mpEnterpriseE->mRenderBuckets = render_buckets;

   // disable the vao
   mpEnterpriseE->mVAO.Unbind();

   // must unbind the index buffer after unbinding the vao
   mpEnterpriseE->mIdxBuf.Unbind();
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

