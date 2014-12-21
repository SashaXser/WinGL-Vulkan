// local includes
#include "ShadowMapWindow.h"

// wgl includes
#include "Matrix.h"
#include "Texture.h"
#include "Vector3.h"
#include "WglAssert.h"
#include "GeomHelper.h"
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
#include <cmath>
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
   typedef std::vector< std::shared_ptr< Texture > > TextureCtr;
   typedef std::multimap< GLuint, std::pair< GLuint, GLsizei > > RenderBucket;

   // vao id
   VAO      mVAO;
   // vbo ids
   VBO      mVertBuf;
   VBO      mIdxBuf;
   VBO      mTexBuf;
   VBO      mNormBuf;
   VBO      mTgtBuf;
   VBO      mBitgtBuf;
   VBO      mClrBuf;
   // texture containers
   TextureCtr     mDiffuse;
   TextureCtr     mHeight;
   TextureCtr     mNormal;
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
      int cur_x = static_cast< int >(lParam & 0xFFFF);
      int cur_y = static_cast< int >(lParam >> 16);

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


#include "Timer.h"
void ShadowMapWindow::RenderScene( )
{
   // clear the buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   

   // temp
   static float light_dir = 0.0f;
   mpEnterpriseE->mProgram.Enable();
   mpEnterpriseE->mProgram.SetUniformValue("light_dir", std::cos(light_dir), std::sin(light_dir), 0.0f);
   //mpEnterpriseE->mProgram.SetUniformValue("light_dir", 0.0f, -1.0f, 0.0f);
   mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("projection", proj);
   mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("model_view", mv);
   mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("model_view_normal", mvn);
   mpEnterpriseE->mProgram.SetUniformValue("light_per_pixel", light_per_pixel ? 1 : 0);

   Vec4f en = mvn * Vec4f(0.0f, 0.0f, 1.0f, 0.0f);

   light_dir += 0.0005f;

   mpEnterpriseE->mVAO.Bind();

   //glDrawArrays(GL_TRIANGLES, 0, static_cast< GLsizei >(mpEnterpriseE->mVertBuf.Size< float >() / 3));
   //glDrawElements(GL_TRIANGLES, static_cast< GLsizei >(mpEnterpriseE->mIdxBuf.Size< uint32_t >()), GL_UNSIGNED_INT, nullptr);

   Texture * current_tex = nullptr;
   auto rbucketBeg = mpEnterpriseE->mRenderBuckets.cbegin();
   const auto rbucketEnd = mpEnterpriseE->mRenderBuckets.cend();

   for (; rbucketBeg != rbucketEnd; ++rbucketBeg)
   {
      Texture * rbucket_tex = mpEnterpriseE->mDiffuse[rbucketBeg->first].get();                           

      if (current_tex != rbucket_tex)
      {
         if (current_tex && *current_tex)
         {
            current_tex->Unbind();
         }

         current_tex = rbucket_tex;

         if (!current_tex || !*current_tex)
         {
            mpEnterpriseE->mProgram.SetUniformValue("tex_unit_0_active", 0);
         }
         else
         {
            rbucket_tex->Bind(GL_TEXTURE0);

            mpEnterpriseE->mProgram.SetUniformValue("tex_unit_0_active", 1);
         }
      }

      glDrawElements(GL_TRIANGLES, rbucketBeg->second.second, GL_UNSIGNED_INT, reinterpret_cast< void * >(rbucketBeg->second.first * sizeof(rbucketBeg->second.first)));
   }

   if (current_tex && *current_tex)
   {
      current_tex->Unbind();
   }

   mpEnterpriseE->mVAO.Unbind();


   // swap the front and back
   SwapBuffers(GetHDC());
}

void ShadowMapWindow::GenerateSceneData( )
{
   GenerateEnterpriseE();
}
#include "MathHelper.h"
void ShadowMapWindow::GenerateEnterpriseE( )
{
   // a set of all the mesh data
   std::vector< float > colors;
   std::vector< float > normals;
   std::vector< float > vertices;
   std::vector< float > tex_coords;
   std::vector< float > tangents;
   std::vector< float > bitangents;
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

      if (ppChildrenEnd != ppNode)
      {
         for (const aiNode * pNode = *ppNode; pNode; pNode = pNode->mParent)
         {
            model_matrix = Matrixf(&pNode->mTransformation.a1).Transpose() * model_matrix;
         }
      }

      return model_matrix;
   };

   const auto ReadTextures = [ ] ( const aiScene * const pScene,
                                   const std::string & base_model_path,
                                   std::vector< std::shared_ptr< Texture > > & diffuse,
                                   std::vector< std::shared_ptr< Texture > > & height,
                                   std::vector< std::shared_ptr< Texture > > & normal )
   {
      // make sure we do not load the same texture twice
      std::map< std::string, std::shared_ptr< Texture > > texture_filenames;

      // iterate over all the materials collecting the diffuse texture
      const aiMaterial * const * pBeg = pScene->mMaterials;
      const aiMaterial * const * const pEnd = pScene->mMaterials + pScene->mNumMaterials;

      // an object that loads the appropriate texture
      const auto LoadTexture =
      [ &texture_filenames ] ( const std::string & filename, const GLenum internal_format, std::vector< std::shared_ptr< Texture > > & textures )
      {
         // make sure the length is valid
         if (!filename.empty())
         {
            // if the value has already been seen, then use that handle; otherwise create it
            const auto tex_filename = texture_filenames.find(filename);

            if (texture_filenames.find(filename) != texture_filenames.end())
            {
               // already seen, so just reuse the handle
               textures.back() = tex_filename->second;
            }
            else
            {
               if (textures.back()->Load2D(filename.c_str(), GL_RGBA, internal_format, true))
               {
                  // obtain the handle for this texture
                  // this is currently not supported on my HD 5850 with driver version 14.4 (14.100)
                  //const GLuint64 tex_handle = glGetTextureHandleARB(tex_id);

                  // save for later use
                  texture_filenames[filename] = textures.back();
               }
            }
         }
      };

      for (; pBeg != pEnd; ++pBeg)
      {
         // begin by inserting a null handle into the texture array
         diffuse.push_back(std::shared_ptr< Texture >(new Texture));
         height.push_back(std::shared_ptr< Texture >(new Texture));
         normal.push_back(std::shared_ptr< Texture >(new Texture));

         // if there is a diffuse texture, then set it up
         if ((*pBeg)->GetTextureCount(aiTextureType_DIFFUSE))
         {
            // have not found more than one texture so far
            WGL_ASSERT((*pBeg)->GetTextureCount(aiTextureType_DIFFUSE) <= 1);

            // get the texture associated with this material
            const std::string filename = [ &pBeg, &base_model_path ] ( ) -> std::string
            {
               aiString filename; (*pBeg)->GetTexture(aiTextureType_DIFFUSE, 0, &filename); return base_model_path + filename.C_Str();
            }();

            // load the diffuse texture
            LoadTexture(filename, GL_COMPRESSED_RGBA, diffuse);

            // this model is very incomplete in terms of the associated textures...
            // try to load height and bump data here as well...
            // the names of the other textures are very much the same except they have bump and norm in them...

            // determine the offset of the 'diff' in the filename
            const size_t diff_offset = filename.find("DIFF");
            
            if (diff_offset != std::string::npos)
            {
               // construct the bump file name
               const std::string filename_bump = std::string(filename).replace(filename.find("DIFF"), 4, "BUMP");

               // load the heigt map texture
               LoadTexture(filename_bump, GL_RGBA, height);

               // construct the normal file name
               const std::string filename_norm = std::string(filename_bump).insert(filename_bump.find_last_of("."), "_NORM");

               // load the normal texture
               LoadTexture(filename_norm, GL_RGBA, normal);
            }
         }
      }
   };

   const auto ReadModel = [ & ] ( const char * const pFilename )
   {
      // think about having assimp provide the tangents and bitangents too
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
         ReadTextures(pScene, GetBasePath(pFilename),
                      mpEnterpriseE->mDiffuse, mpEnterpriseE->mHeight, mpEnterpriseE->mNormal);

         for (size_t cur_mesh = 0; cur_mesh < pScene->mNumMeshes; ++cur_mesh)
         {
            // establish what the base index for this mesh is
            const uint32_t base_index = static_cast< uint32_t >(indices.size());

            // obatin all the required data for this mesh...
            const aiMesh * const pCurMesh = pScene->mMeshes[cur_mesh];
            const aiVector3D * const pVertices = pCurMesh->mVertices;
            const aiVector3D * const pNormals = pCurMesh->mNormals;
            const aiVector3D * const pTangents = pCurMesh->mTangents;
            const aiVector3D * const pBitangents = pCurMesh->mBitangents;
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

            if (pTexCoords)
            {
               // we have texture coordinates, so use them...
               for (auto ptex_coord_cur = pTexCoords; ptex_coord_cur != pTexCoords + num_verts; ++ptex_coord_cur)
               {
                  tex_coords.insert(tex_coords.end(), &ptex_coord_cur->x, &ptex_coord_cur->x + 2);
               }
            }
            else
            {
               // we do not have texture coordinates, so fill in the gaps with a zero to keep them in sync...
               tex_coords.insert(tex_coords.end(), num_verts * 2, 0.0f);

               // determine what the diffuse color is for this mesh
               const aiMaterial * const pMat = pScene->mMaterials[pCurMesh->mMaterialIndex];
               const aiColor3D diffuse_color = [ &pMat ] ( ) -> aiColor3D
               { aiColor3D clr; pMat->Get(AI_MATKEY_COLOR_DIFFUSE, clr); return clr; }();

               // overwrite the colors with this diffuse color
               std::for_each(reinterpret_cast< Vec3f * >(colors.data()) + base_index,
                             reinterpret_cast< Vec3f * >(colors.data()) + base_index + num_verts,
               [ &diffuse_color ] ( Vec3f & diffuse) { diffuse = Vec3f(&diffuse_color.r); });
            }
            
            for (size_t i = 0; num_verts > i; ++i)
            {
               // need to translate the vertices as they may be in the wrong place...
               const Vec3f vert = mesh_matrix * Vec3f(&((pVertices + i)->x));
               vertices.insert(vertices.end(), vert.mT, vert.mT + 3);
            }

            for (size_t i = 0; num_verts > i; ++i)
            {
               // need to translate the normals to the correct location as they too may be in the wrong place...
               Vec3f norm = Vec3f(&(normal_matrix * Vec4f((pNormals + i)->x, (pNormals + i)->y, (pNormals + i)->z, 0.0f)).X()).UnitVector();
               
               // this model has bad normal data in it, so just calculate it ourselves
               if (norm.Length() == 0 || std::isnan(norm.X()) || std::isnan(norm.Y()) || std::isnan(norm.Z()))
               {
                  // run across the faces until a matching face is found
                  const aiFace * const pFace =
                     std::find_if(pCurMesh->mFaces, pCurMesh->mFaces + pCurMesh->mNumFaces,
                     [ i ] ( const aiFace & cur_face )
                     {
                        // should always be three indices that make up this triangle
                        WGL_ASSERT(cur_face.mNumIndices == 3);

                        return cur_face.mIndices[0] == i ||
                               cur_face.mIndices[1] == i ||
                               cur_face.mIndices[2] == i;
                     });

                  // calculate the normal based on the face indices
                  const Vec3f e0(&(vertices[base_index * 3 + pFace->mIndices[0]]));
                  const Vec3f e1(&(vertices[base_index * 3 + pFace->mIndices[1]]));
                  const Vec3f e2(&(vertices[base_index * 3 + pFace->mIndices[2]]));

                  norm = ((e1 - e0) ^ (e2 - e0)).UnitVector();
               }
               
               normals.insert(normals.end(), norm.mT, norm.mT + 3);
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

            // determine how to handle the tangents and bitangents
            // if there are no texture coordinates, then the use of the tangent and bitangent is not needed
            if (!pTexCoords)
            {
               // no texture coordinates, so just fill in the tangent and bitangent data with defaults
               for (uint32_t i = 0; num_verts > i; ++i)
               {
                  tangents.push_back(1.0f); tangents.push_back(0.0f); tangents.push_back(0.0f);
                  bitangents.push_back(0.0f); bitangents.push_back(1.0f); bitangents.push_back(0.0f);
               }
            }
            else
            {
               // texture coordinates, so gather up the appropriate information
               // fill in temp vectors for what is just needed
               const std::vector< float > temp_vertices(vertices.cbegin() + base_index * 3, vertices.cend());
               const std::vector< float > temp_normals(normals.cbegin() + base_index * 3, normals.cend());
               const std::vector< float > temp_tex_coords(tex_coords.cbegin() + base_index * 2, tex_coords.cend());
               const std::vector< GLuint > temp_indices =
               [ & ] ( ) -> std::vector< GLuint >
               {
                  // need to transform the indices to use base 0
                  std::vector< GLuint > indices(indices.cbegin() + base_index, indices.cend());
                  for (auto & index : indices)
                  { 
                     index = index - base_index;
                  }

                  return indices;
               }();

               // calculate all the vector information
               const auto tangents_bitangents =
                  GeomHelper::ConstructTangentsAndBitangents(temp_vertices, temp_normals, temp_tex_coords, temp_indices);

               // add the tangents and bitangents
               tangents.insert(tangents.cend(),
                               static_cast< const float * >(*(tangents_bitangents.first.cbegin())),
                               static_cast< const float * >(*(tangents_bitangents.first.cbegin())) + tangents_bitangents.first.size() * Vec3f::NUM_COMPONENTS);
               bitangents.insert(bitangents.cend(),
                                 static_cast< const float * >(*(tangents_bitangents.second.cbegin())),
                                 static_cast< const float * >(*(tangents_bitangents.second.cbegin())) + tangents_bitangents.second.size() * Vec3f::NUM_COMPONENTS);
            }

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

   // read all the attributes of the model
   ReadModel(R"(.\enterprise\Enterp TOS - Arena.3DS)");

   // vertices size should match tangents and bitangents
   WGL_ASSERT(vertices.size() == tangents.size());
   WGL_ASSERT(vertices.size() == bitangents.size());

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
   mpEnterpriseE->mClrBuf.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(decltype(colors.front())) * 3, 0);
   glEnableVertexAttribArray(1);
   mpEnterpriseE->mClrBuf.Unbind();

   mpEnterpriseE->mNormBuf.GenBuffer(GL_ARRAY_BUFFER);
   mpEnterpriseE->mNormBuf.Bind();
   mpEnterpriseE->mNormBuf.BufferData(normals.size() * sizeof(decltype(normals.front())), &normals.front(), GL_STATIC_DRAW);
   mpEnterpriseE->mNormBuf.VertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(decltype(normals.front())) * 3, 0);
   glEnableVertexAttribArray(2);
   mpEnterpriseE->mNormBuf.Unbind();

   mpEnterpriseE->mTexBuf.GenBuffer(GL_ARRAY_BUFFER);
   mpEnterpriseE->mTexBuf.Bind();
   mpEnterpriseE->mTexBuf.BufferData(tex_coords.size() * sizeof(decltype(tex_coords.front())), &tex_coords.front(), GL_STATIC_DRAW);
   mpEnterpriseE->mTexBuf.VertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(decltype(tex_coords.front())) * 2, 0);
   glEnableVertexAttribArray(3);
   mpEnterpriseE->mTexBuf.Unbind();

   // create the index buffer
   mpEnterpriseE->mIdxBuf.GenBuffer(GL_ELEMENT_ARRAY_BUFFER);
   mpEnterpriseE->mIdxBuf.Bind();
   mpEnterpriseE->mIdxBuf.BufferData(indices.size() * sizeof(decltype(indices.front())), &indices.front(), GL_STATIC_DRAW);

   // make sure we save the render buckets to the model
   mpEnterpriseE->mRenderBuckets = std::move(render_buckets);

   // disable the vao
   mpEnterpriseE->mVAO.Unbind();

   // must unbind the index buffer after unbinding the vao
   mpEnterpriseE->mIdxBuf.Unbind();
}


