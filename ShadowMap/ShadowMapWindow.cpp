// local includes
#include "ShadowMapWindow.h"

// wgl includes
#include "Matrix.h"
#include "Vector.h"
#include "Texture.h"
#include "WglAssert.h"
#include "GeomHelper.h"
#include "ReadTexture.h"
#include "MatrixHelper.h"
#include "ShaderProgram.h"
#include "OpenGLExtensions.h"
#include "FrameBufferObject.h"
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
   typedef std::vector< std::shared_ptr< Texture > > TexturePtr;
   typedef std::multimap< GLuint, std::pair< GLuint, GLsizei > > RenderBucket;

   // vao id
   VAO      mVAO;
   VAO      mVAOEmpty;
   // vbo ids
   VBO      mVertBuf;
   VBO      mIdxBuf;
   VBO      mTexBuf;
   VBO      mNormBuf;
   VBO      mClrBuf;
   // texture containers
   TexturePtr     mDiffuse;
   // shader program
   ShaderProgram  mProgram;
   ShaderProgram  mProgramNormals;
   ShaderProgram  mProgramShadow;
   ShaderProgram  mProgramDisplayShadow;
   // defines the render order by texture
   // unit... group multiple items together...
   RenderBucket   mRenderBuckets;
};

ShadowMapWindow::ShadowMapWindow( ) :
mpEnterpriseE     ( new Renderable ),
mCamera           ( Vec3f(0.0f, 40.0f, 100.0f), Vec3f(0.0f, 40.0f, 0.0f) ),
mDisplayNormals   ( false ),
mpShadowMap       ( new FrameBufferObject )
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

   // cleanup the shadow map
   mpShadowMap = nullptr;

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
          !mpEnterpriseE->mProgram.Link() ||
          !mpEnterpriseE->mProgramNormals.AttachFile(GL_VERTEX_SHADER, "enterprise_normal.vert") ||
          !mpEnterpriseE->mProgramNormals.AttachFile(GL_GEOMETRY_SHADER, "enterprise_normal.geom") ||
          !mpEnterpriseE->mProgramNormals.AttachFile(GL_FRAGMENT_SHADER, "enterprise_normal.frag") ||
          !mpEnterpriseE->mProgramNormals.Link() ||
          !mpEnterpriseE->mProgramShadow.AttachFile(GL_VERTEX_SHADER, "enterprise_shadow.vert") ||
          !mpEnterpriseE->mProgramShadow.Link() ||
          !mpEnterpriseE->mProgramDisplayShadow.AttachFile(GL_VERTEX_SHADER, "enterprise_display_shadow.vert") ||
          !mpEnterpriseE->mProgramDisplayShadow.AttachFile(GL_FRAGMENT_SHADER, "enterprise_display_shadow.frag") ||
          !mpEnterpriseE->mProgramDisplayShadow.Link())
      {
         return false;
      }

      // enable specific state
      mPipeline.EnableCullFace(true);
      mPipeline.EnableDepthTesting(true);

      // set the initial view parameters
      mpEnterpriseE->mProgram.Enable();
      mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("model_view_mat", mCamera.GetViewMatrix());
      mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("model_view_normal", mCamera.GetViewMatrix().Inverse());
      mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("model_view_tinv_mat", mCamera.GetViewMatrix().Inverse().Transpose());

      // set some basic lighting parameters
      mpEnterpriseE->mProgram.SetUniformValue("directional_light.base.color", Vec3f(1.0f, 1.0f, 1.0f));
      mpEnterpriseE->mProgram.SetUniformValue("directional_light.base.ambient_intensity", 0.05f);
      mpEnterpriseE->mProgram.SetUniformValue("directional_light.base.diffuse_intensity", 1.0f);
      mpEnterpriseE->mProgram.Disable();

      // set some initial parameters for the shadow overlay
      mpEnterpriseE->mProgramDisplayShadow.Enable();
      mpEnterpriseE->mProgramDisplayShadow.SetUniformValue("shadow_texture", 0);
      mpEnterpriseE->mProgramDisplayShadow.Disable();

      // disable reading and drawing operations to
      // allow the attachment of the shadow map to take...
      mPipeline.ReadBuffer(GL_NONE);
      mPipeline.DrawBuffer(GL_NONE);

      // generate the shadow map to render the light from
      mpShadowMap->GenBuffer(2048*2, 2048*2);
      mpShadowMap->Bind(GL_FRAMEBUFFER);
      // attach a depth buffer texture
      mpShadowMap->Attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, GL_DEPTH_COMPONENT32F);

      // validate completeness of the attachment
      const bool frame_complete = mpShadowMap->IsComplete();
      mpShadowMap->Unbind();

      if (!frame_complete)
      {
         // issue an error from the application that it could not be created
         PostDebugMessage(GL_DEBUG_TYPE_ERROR, 2, GL_DEBUG_SEVERITY_HIGH, "Unable To Create Frame Buffer Depth Attachment");

         return false;
      }

      // restore the reading and drawing operations
      mPipeline.ReadBuffer(GL_BACK);
      mPipeline.DrawBuffer(GL_BACK);

      // force the view to resize again...
      SendMessage(GetHWND(), WM_SIZE, 0, nHeight << 16 | nWidth);
      
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

static float light_dir = 0.0f;
LRESULT ShadowMapWindow::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam )
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
                             0.01f,
                             3000.f);

      // apply the new params if created...
      if (mpEnterpriseE->mProgram && mpEnterpriseE->mProgramNormals)
      {
         const auto mvp = mCamera.GetProjectionMatrix() * mCamera.GetViewMatrix();

         mpEnterpriseE->mProgram.Enable();
         mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("projection", mCamera.GetProjectionMatrix());
         mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("model_view_proj_mat", mvp);
         mpEnterpriseE->mProgram.Disable();

         mpEnterpriseE->mProgramNormals.Enable();
         mpEnterpriseE->mProgramNormals.SetUniformMatrix< 1, 4, 4 >("model_view_proj_mat", mvp);
         mpEnterpriseE->mProgramNormals.Disable();
      }

      if (mpEnterpriseE->mProgramDisplayShadow)
      {
         mpEnterpriseE->mProgramDisplayShadow.Enable();
         mpEnterpriseE->mProgramDisplayShadow.SetUniformValue("window_width", static_cast< uint32_t >(x));
         mpEnterpriseE->mProgramDisplayShadow.SetUniformValue("window_height", static_cast< uint32_t >(y));
         mpEnterpriseE->mProgramDisplayShadow.Disable();
      }
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

         UpdateShaderCameraValues();
      }
      else if (wParam & MK_RBUTTON)
      {
         const int dx = static_cast< int >(cur_x - GetPreviousMousePosition().x);

         light_dir += 0.0005f * dx;
      }
   }

   break;

   case WM_CHAR:
   {
      bool update_mv = false;

      const float multiplier = GetAsyncKeyState(VK_SHIFT) & 0x8000 ? 0.5f : 1.0f;

      if (wParam == 'n' || wParam == 'N')
      {
         mDisplayNormals = !mDisplayNormals;
      }
      else if (wParam == 'a' || wParam == 'A')
      {
         mCamera.TranslateRight(-multiplier); update_mv = true;
      }
      else if (wParam == 'd' || wParam == 'D')
      {
         mCamera.TranslateRight(multiplier); update_mv = true;
      }
      else if (wParam == 'w' || wParam == 'W')
      {
         mCamera.TranslateForward(-multiplier); update_mv = true;
      }
      else if (wParam == 's' || wParam == 'S')
      {
         mCamera.TranslateForward(multiplier); update_mv = true;
      }

      if (update_mv)
      {
         UpdateShaderCameraValues();
      }
   }

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
   //static float light_dir = 0.0f;

   mpEnterpriseE->mProgram.Enable();
   //const float light_dir_cos = std::cos(light_dir);
   //const float light_dir_sin = std::sin(light_dir);
   const auto light_rotation = Matrixf::Rotate(math::RadToDeg(light_dir), Vec3f(0.0f, 0.0f, 1.0f));
   const auto light_dir_vec = light_rotation * Vec3f(0.0f, -1.0f, 0.0f);
   //mpEnterpriseE->mProgram.SetUniformValue("light_dir", light_dir_cos, light_dir_sin, 0.0f);
   mpEnterpriseE->mProgram.SetUniformValue("light_dir", light_dir_vec.X(), light_dir_vec.Y(), 0.0f);
   //mpEnterpriseE->mProgram.SetUniformValue("light_dir", 0.0f, -1.0f, 0.0f);
   
   //mpEnterpriseE->mProgram.SetUniformValue("directional_light.direction_world_space", light_dir_cos, light_dir_sin, 0.0f);
   mpEnterpriseE->mProgram.SetUniformValue("directional_light.direction_world_space", light_dir_vec.X(), light_dir_vec.Y(), 0.0f);
   //mpEnterpriseE->mProgram.SetUniformValue("directional_light.direction_world_space", 0.0f, -1.0f, 0.0f);
   mpEnterpriseE->mProgram.Disable();

   //light_dir += 0.0005f;

   WGL_ASSERT(!mpEnterpriseE->mRenderBuckets.empty());

   // enable the shadow program
   mpEnterpriseE->mProgramShadow.Enable();

   // bind the data to the program
   mpEnterpriseE->mVAO.Bind();

   // construct the model view matrix from the lights perspective
   const Matrixf mvp_light =
      Matrixf::Ortho(-80.0f, 80.0f, -80.0f, 80.0f, -50.0f, 50.0f) *
      Matrixf::LookAt(Vec3f(0.0f, 1.0f, 10.0f), Vec3f(0.0f, 0.0f, 10.0f), Vec3f(0.0f, 0.0f, 1.0f)) *
      light_rotation.InverseFromOrthogonal();

   // update the matrix for the lighting
   mpEnterpriseE->mProgramShadow.SetUniformMatrix< 1, 4, 4 >("model_view_proj_mat", mvp_light);

   // enable writing into the depth texture
   mpShadowMap->Bind(GL_FRAMEBUFFER);

   // update the viewport parameters to match the texture
   mPipeline.PushViewport(0, 0,
                          static_cast< GLint >(mpShadowMap->Width()),
                          static_cast< GLint >(mpShadowMap->Height()));

   // clear the depth texture
   glClear(GL_DEPTH_BUFFER_BIT);

   // disable reading and writing to the color buffers
   mPipeline.ReadBuffer(GL_NONE);
   mPipeline.DrawBuffers({ GL_NONE });

   // render to the shadow map buffer the scene
   typedef decltype( Renderable::mRenderBuckets ) RenderBucketType;
   RenderBucketType::const_iterator rbucketBeg = mpEnterpriseE->mRenderBuckets.lower_bound(mpEnterpriseE->mRenderBuckets.cbegin()->first);
   RenderBucketType::const_iterator rbucketEnd = mpEnterpriseE->mRenderBuckets.upper_bound(mpEnterpriseE->mRenderBuckets.cbegin()->first);

   while (mpEnterpriseE->mRenderBuckets.cend() != rbucketEnd)
   {
      for (; rbucketBeg != rbucketEnd; ++rbucketBeg)
      {
         glDrawElements(GL_TRIANGLES,
                        rbucketBeg->second.second,
                        GL_UNSIGNED_INT,
                        reinterpret_cast< void * >(rbucketBeg->second.first * sizeof(rbucketBeg->second.first)));
      }

      if (mpEnterpriseE->mRenderBuckets.cend() != rbucketEnd)
      {
         rbucketBeg = mpEnterpriseE->mRenderBuckets.lower_bound(rbucketEnd->first);
         rbucketEnd = mpEnterpriseE->mRenderBuckets.upper_bound(rbucketEnd->first);
      }
   }

   // restore the viewport
   mPipeline.PopViewport();

   // disable writing into the depth texture
   mpShadowMap->Unbind();

   // restore reading and writing to the default color buffers.
   mPipeline.ReadBuffer(GL_BACK);
   mPipeline.DrawBuffer(GL_BACK);

   // disable the shadow program
   mpEnterpriseE->mProgramShadow.Disable();

   // enable rendering to the main frame buffer
   mpEnterpriseE->mProgram.Enable();

   // bind the shadow texture
   mpShadowMap->GetAttachment(GL_DEPTH_ATTACHMENT)->Bind(GL_TEXTURE1);
   mpEnterpriseE->mProgram.SetUniformValue("shadow_texture",
                                           static_cast< GLint >(mpShadowMap->GetAttachment(GL_DEPTH_ATTACHMENT)->GetBoundSamplerID()));

   // update the shadow mvp matrix
   mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("shadow_mvp_mat", mvp_light);

   // render to the default view the actual scene
   rbucketBeg = mpEnterpriseE->mRenderBuckets.lower_bound(mpEnterpriseE->mRenderBuckets.cbegin()->first);
   rbucketEnd = mpEnterpriseE->mRenderBuckets.upper_bound(mpEnterpriseE->mRenderBuckets.cbegin()->first);

   while (mpEnterpriseE->mRenderBuckets.cend() != rbucketEnd)
   {
      // activate the texture
      Texture * diffuse_tex = mpEnterpriseE->mDiffuse[rbucketBeg->first].get();

      if (diffuse_tex && *diffuse_tex)
      {
         diffuse_tex->Bind(GL_TEXTURE0);
         mpEnterpriseE->mProgram.SetUniformValue("diffuse_texture", static_cast< GLint >(diffuse_tex->GetBoundSamplerID()));
      }

      for (; rbucketBeg != rbucketEnd; ++rbucketBeg)
      {
         glDrawElements(GL_TRIANGLES,
                        rbucketBeg->second.second,
                        GL_UNSIGNED_INT,
                        reinterpret_cast< void * >(rbucketBeg->second.first * sizeof(rbucketBeg->second.first)));
      }

      if (diffuse_tex && *diffuse_tex)
      {
         diffuse_tex->Unbind();
      }

      if (mpEnterpriseE->mRenderBuckets.cend() != rbucketEnd)
      {
         rbucketBeg = mpEnterpriseE->mRenderBuckets.lower_bound(rbucketEnd->first);
         rbucketEnd = mpEnterpriseE->mRenderBuckets.upper_bound(rbucketEnd->first);
      }
   }

   // unbind the shadow map
   mpShadowMap->GetAttachment(GL_DEPTH_ATTACHMENT)->Unbind();

   mpEnterpriseE->mProgram.Disable();

   if (mDisplayNormals)
   {
      mpEnterpriseE->mProgramNormals.Enable();

      rbucketBeg = mpEnterpriseE->mRenderBuckets.lower_bound(mpEnterpriseE->mRenderBuckets.cbegin()->first);
      rbucketEnd = mpEnterpriseE->mRenderBuckets.upper_bound(mpEnterpriseE->mRenderBuckets.cbegin()->first);

      while (mpEnterpriseE->mRenderBuckets.cend() != rbucketEnd)
      {
         // activate the texture
         Texture * diffuse_tex = mpEnterpriseE->mDiffuse[rbucketBeg->first].get();

         for (; rbucketBeg != rbucketEnd; ++rbucketBeg)
         {
            glDrawElements(GL_TRIANGLES,
                           rbucketBeg->second.second,
                           GL_UNSIGNED_INT,
                           reinterpret_cast< void * >(rbucketBeg->second.first * sizeof(rbucketBeg->second.first)));
         }

         if (mpEnterpriseE->mRenderBuckets.cend() != rbucketEnd)
         {
            rbucketBeg = mpEnterpriseE->mRenderBuckets.lower_bound(rbucketEnd->first);
            rbucketEnd = mpEnterpriseE->mRenderBuckets.upper_bound(rbucketEnd->first);
         }
      }

      mpEnterpriseE->mProgramNormals.Disable();
   }

   mpEnterpriseE->mVAO.Unbind();

   // render the shadow map to the lower left corner
   mpEnterpriseE->mProgramDisplayShadow.Enable();
   mpEnterpriseE->mVAOEmpty.Bind();

   // construct the mvp
   const Matrixf mvp = Matrixf::Ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
   mpEnterpriseE->mProgramDisplayShadow.SetUniformMatrix< 1, 4, 4 >("model_view_proj_mat", mvp);

   // bind the shadow map
   mpShadowMap->GetAttachment(GL_DEPTH_ATTACHMENT)->Bind(GL_TEXTURE0);
   
   // the shader does all the calculation work
   mPipeline.DrawArrays(GL_TRIANGLES, 0, 6);

   // unbind the shadow map
   mpShadowMap->GetAttachment(GL_DEPTH_ATTACHMENT)->Unbind();
   
   mpEnterpriseE->mVAOEmpty.Unbind();
   mpEnterpriseE->mProgramDisplayShadow.Disable();

   // swap the front and back
   SwapBuffers(GetHDC());
}

void ShadowMapWindow::GenerateSceneData( )
{
   GenerateEnterpriseE();

   // required by a core context to have at least a single
   // vao present even if there are no bound arrays...
   mpEnterpriseE->mVAOEmpty.GenArray();
}

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
               LoadTexture(filename_bump, GL_RGBA8, height);

               // construct the normal file name
               const std::string filename_norm = std::string(filename_bump).insert(filename_bump.find_last_of("."), "_NORM");

               // load the normal texture
               LoadTexture(filename_norm, GL_RGBA8, normal);
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

         // start off by reading the textures
         std::vector< std::shared_ptr< Texture > > unused_texs;
         ReadTextures(pScene, GetBasePath(pFilename), mpEnterpriseE->mDiffuse, unused_texs, unused_texs);

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

void ShadowMapWindow::UpdateShaderCameraValues()
{
   WGL_ASSERT(mpEnterpriseE->mProgram);
   WGL_ASSERT(mpEnterpriseE->mProgramNormals);

   mpEnterpriseE->mProgram.Enable();

   auto mv = mCamera.GetViewMatrix();
   mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("model_view_mat", mv);

   const auto mvp = mCamera.GetProjectionMatrix() * mv;
   mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("model_view_proj_mat", mvp);

   mv.MakeInverse();
   mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("model_view_normal", mv);

   mv.Transpose();
   mpEnterpriseE->mProgram.SetUniformMatrix< 1, 4, 4 >("model_view_tinv_mat", mv);

   mpEnterpriseE->mProgram.Disable();

   mpEnterpriseE->mProgramNormals.Enable();
   mpEnterpriseE->mProgramNormals.SetUniformMatrix< 1, 4, 4 >("model_view_proj_mat", mvp);
   mpEnterpriseE->mProgramNormals.Disable();
}
