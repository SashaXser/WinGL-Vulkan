// local includes
#include "InstancingWindow.h"
#include "Timer.h"
#include "Matrix.h"
#include "Shaders.h"
#include "Vector4.h"
#include "ReadTexture.h"
#include "MatrixHelper.h"

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// stl includes
#include <vector>
#include <sstream>
#include <iomanip>

// determines if the application should run wild
#define LET_APP_RUN_WILD 0

InstancingWindow::InstancingWindow( ) :
mBuildingsProgID        ( 0 ),
mBuildingsVertID        ( 0 ),
mBuildingsFragID        ( 0 ),
mTreesProgID            ( 0 ),
mTreesVertID            ( 0 ),
mTreesFragID            ( 0 ),
mTreesGeomID            ( 0 ),
mNumBuildingInstances   ( 100 ),
mNumTreeInstances       ( 100 ),
mPrevMouseX             ( 0 ),
mPrevMouseY             ( 0 )
{
   // give some meaning to random
   srand(static_cast< uint32_t >(time(nullptr)));

   // clear out the instances
   memset(mBuildingInstances, 0x00, sizeof(mBuildingInstances));
   memset(mTreeInstances, 0x00, sizeof(mTreeInstances));
}

InstancingWindow::~InstancingWindow( )
{
}

bool InstancingWindow::Create( unsigned int nWidth, unsigned int nHeight,
                               const char * pWndTitle, const void ** pInitParams )
{
   // initialize the camera and perspective
   mCamera.MakeLookAt(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
   mPerspective.MakePerspective(45.0f, static_cast< float >(nWidth) / static_cast< float >(nHeight), 1.0f, 1000.0f);

   // initialize gl context params
   const OpenGLWindow::OpenGLInit glInit41D =
   {
      4, 1, true, true, false
   };

   const OpenGLWindow::OpenGLInit glInit41 =
   {
      4, 1, true, false, false
   };

   const OpenGLWindow::OpenGLInit glInit40D =
   {
      4, 0, true, true, false
   };

   const OpenGLWindow::OpenGLInit glInit40 =
   {
      4, 0, true, false, false
   };

   const OpenGLWindow::OpenGLInit glInit33D =
   {
      3, 3, true, true, false
   };

   const OpenGLWindow::OpenGLInit glInit33 =
   {
      3, 3, true, false, false
   };

   const OpenGLWindow::OpenGLInit * glInit[] =
   {
      &glInit41D, &glInit41, &glInit40D, &glInit40, &glInit33D, &glInit33, nullptr
   };

   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle,
                            reinterpret_cast< const void ** >(glInit)))
   {
      // make the gl context active
      MakeCurrent();

      // create all the instances
      CreateInstances();

      // enable specific gl states
      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);

      return true;
   }

   return false;
}

int InstancingWindow::Run( )
{
   // create a local timer object...
   Timer localTimer;

   // setup the amount of timer per frame
#if LET_APP_RUN_WILD
   const double MS_PER_FRAME = 0.0;
#else
   const double MS_PER_FRAME = 1000.0 / 60.0;
#endif

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
         // render the scene
         RenderScene();

         // if there is time left, then do some waiting
         if (const double deltaMS = localTimer.DeltaMS(begTick) <= MS_PER_FRAME)
         {
            // wait for the remainder of the time
            localTimer.Wait(static_cast< unsigned long >(MS_PER_FRAME - deltaMS));
         }

         // determine the framrate of the application
         const double frame_rate = 1.0 / localTimer.DeltaSec(begTick);

         // construct a string to place on the title
         std::stringstream ss;
         ss << "Instancing - "
            << "Num Buildings: " << mNumBuildingInstances
            << " Num Trees: " << mNumTreeInstances << " - "
            << std::fixed << std::setprecision(3) << frame_rate << " fps";
         SetWindowText(GetHWND(), ss.str().c_str());
      }
   }

   return appQuitVal;
}

LRESULT InstancingWindow::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
   case WM_SIZE:
      {
      // update the viewport
      glViewport(0, 0, lParam & 0xFFFF, lParam >> 16);
      // update the perspective matrix
      float perspValues[4] = { 0 };
      mPerspective.GetPerspective(perspValues);
      mPerspective.MakePerspective(perspValues[0],
                                   static_cast< float >(lParam & 0xFFFF) / static_cast< float >(lParam >> 16),
                                   perspValues[2], perspValues[3]);
      }

      break;

   case WM_KEYDOWN:
      {
      // get the view and strafe vector
      const Vec3f strafe(mCamera[0], mCamera[4], mCamera[8]);
      const Vec3f view(mCamera[2] * -1.0f, mCamera[6] * -1.0f, mCamera[10] * -1.0f);
      
      switch (wParam)
      {
      case 'A': mCamera = mCamera * Matrixf::Translate(strafe); break;
      case 'D': mCamera = mCamera * Matrixf::Translate(strafe * -1.0f); break;
      case 'W': mCamera = mCamera * Matrixf::Translate(view * -1.0f); break;
      case 'S': mCamera = mCamera * Matrixf::Translate(view); break;

      case VK_OEM_PLUS:
      case VK_OEM_MINUS:
         // release all the instance data
         for (int i = 0; i < NUM_BUILDING_TYPES; ++i)
         {
            // release the texture
            glDeleteTextures(1, &mBuildingInstances[i].mTexID);
            // release the buffer data
            glDeleteBuffers(1, &mBuildingInstances[i].mIdxBufferID);
            glDeleteBuffers(1, &mBuildingInstances[i].mTexBufferID);
            glDeleteBuffers(1, &mBuildingInstances[i].mVertBufferID);
            glDeleteBuffers(1, &mBuildingInstances[i].mWorldBufferID);
            // release the vertex array
            glDeleteVertexArrays(1, &mBuildingInstances[i].mVertArrayID);
         }

         for (int i = 0; i < NUM_TREE_TYPES; ++i)
         {
            // release the texture
            glDeleteTextures(1, &mTreeInstances[i].mTexID);
            // release the buffer data
            glDeleteBuffers(1, &mTreeInstances[i].mVertBufferID);
         }

         // clear the instances
         memset(mBuildingInstances, 0x00, sizeof(mBuildingInstances));
         memset(mTreeInstances, 0x00, sizeof(mTreeInstances));

         // update the number of instances
         switch (wParam)
         {
         case VK_OEM_PLUS:
            // increase by half
            mNumBuildingInstances = static_cast< uint32_t >(mNumBuildingInstances * 1.5);
            
            break;
         case VK_OEM_MINUS:
            // reduce by half and cap at 100
            mNumBuildingInstances = std::max< uint32_t >(static_cast< uint32_t >(mNumBuildingInstances * 0.5), 100);
            
            break;
         }

         mNumTreeInstances = mNumBuildingInstances;

         // create the instances
         CreateInstances();

         break;
      }
      }

      break;

   case WM_MOUSEMOVE:
      {
      // obtain the current x and y locations
      const short curMouseX = (short)(lParam & 0x0000FFFF);
      const short curMouseY = (short)(lParam >> 16);

      if (wParam & MK_LBUTTON)
      {
         // delta values
         const short deltaX = curMouseX - mPrevMouseX;
         const short deltaY = curMouseY - mPrevMouseY;

         // decompose the camera into yaw and pitch
         float yaw = 0.0f, pitch = 0.0f;
         MatrixHelper::DecomposeYawPitchRollDeg< float >(mCamera, &yaw, &pitch, nullptr);

         // go from eye space to world space
         // make sure to multiply by -1 as the final matrix translates world to eye
         Vec3f eye = mCamera.InverseFromOrthogonal() * Vec3f(0.0f, 0.0f, 0.0f) * -1.0f;

         // construct the camera matrix
         mCamera = Matrixf::Rotate(pitch + deltaY, 1.0f, 0.0f, 0.0f) *
                   Matrixf::Rotate(yaw + deltaX, 0.0f, 1.0f, 0.0f) *
                   Matrixf::Translate(eye);
      }

      mPrevMouseX = curMouseX;
      mPrevMouseY = curMouseY;
      }

      break;
   }

   return OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
}

void InstancingWindow::CreateInstances( )
{
   // create a texture object
   uint8_t * pTexture = nullptr;
   uint32_t width = 0, height = 0;
   ReadRGB("buildings.rgb", width, height, &pTexture);

   GLuint texID = 0;
   glGenTextures(1, &texID);
   glBindTexture(GL_TEXTURE_2D, texID);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pTexture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glGenerateMipmap(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, 0);

   // release the texture
   delete [] pTexture;

   // defines a temp hold for the instanced data
   struct InstanceData
   {
      Matrixf  mWorld;
   };

   // start setting up the instanced data
   std::vector< std::vector< InstanceData > > instanceData(NUM_BUILDING_TYPES);

   // generate data for each instance
   for (uint32_t i = 0; i < mNumBuildingInstances; ++i)
   {
      // determine an instance to generate data for
      const uint32_t instance = rand() % NUM_BUILDING_TYPES;
      // setup the instance data
      const InstanceData data =
      {
         Matrixf::Translate(static_cast< float >((rand() % INSTANCE_AREA) * (rand() % 2 == 0 ? 1 : -1)),
                            0.0f,
                            static_cast< float >((rand() % INSTANCE_AREA) * (rand() % 2 == 0 ? 1 : -1)))  
      };
      // add to the list of instanced data
      instanceData[instance].push_back(data);
   }

   // generate gl data for each object
   for (uint32_t i = 0; i < NUM_BUILDING_TYPES; ++i)
   {
      // get the instance to work on...
      BuildingInstance & instance = mBuildingInstances[i];

      // generate the vertex array object
      glGenVertexArrays(1, &instance.mVertArrayID);
      glBindVertexArray(instance.mVertArrayID);

      // define the instance vertex positions
      const float vertices[] =
      {
         -1.0f, 0.0f,  1.0f,  1.0f, 0.0f,  1.0f,  1.0f, 5.0f,  1.0f, -1.0f, 5.0f,  1.0f,
          1.0f, 0.0f,  1.0f,  1.0f, 0.0f, -1.0f,  1.0f, 5.0f, -1.0f,  1.0f, 5.0f,  1.0f,
          1.0f, 0.0f, -1.0f, -1.0f, 0.0f, -1.0f, -1.0f, 5.0f, -1.0f,  1.0f, 5.0f, -1.0f,
         -1.0f, 0.0f, -1.0f, -1.0f, 0.0f,  1.0f, -1.0f, 5.0f,  1.0f, -1.0f, 5.0f, -1.0f,
         -1.0f, 5.0f,  1.0f,  1.0f, 5.0f,  1.0f,  1.0f, 5.0f, -1.0f, -1.0f, 5.0f, -1.0f
      };

      // create, fill, and define the vertex array data
      glGenBuffers(1, &instance.mVertBufferID);
      glBindBuffer(GL_ARRAY_BUFFER, instance.mVertBufferID);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

      // create, fill, and define the texture coord array data
      glGenBuffers(1, &instance.mTexBufferID);
      glBindBuffer(GL_ARRAY_BUFFER, instance.mTexBufferID);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 40, GetTextureCoords(i), GL_STATIC_DRAW);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

      // create, fill, and define the world matrix transform
      glGenBuffers(1, &instance.mWorldBufferID);
      glBindBuffer(GL_ARRAY_BUFFER, instance.mWorldBufferID);
      glBufferData(GL_ARRAY_BUFFER, sizeof(Matrixf) * instanceData[i].size(), &instanceData[i][0], GL_STATIC_DRAW);
      
      // setup all 4 vertex attribute locations
      for (GLuint j = 0; j < 4; ++j)
      {
         glEnableVertexAttribArray(2 + j);
         glVertexAttribPointer(2 + j, 4, GL_FLOAT, GL_FALSE, sizeof(Matrixf),
                               reinterpret_cast< const void * >(sizeof(Matrixf::type) * j * 4));
         glVertexAttribDivisor(2 + j, 1);
      }

      // define the number of instances
      instance.mNumInstances = instanceData[i].size();
      
      // define the instance index ids
      const uint32_t indices[] =
      {
         0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
      };

      // create, fill, and define the index array data
      glGenBuffers(1, &instance.mIdxBufferID);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance.mIdxBufferID);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

      // make sure to define the number of indices
      instance.mIdxBufferSize = sizeof(indices) / sizeof(*indices);

      // set the texture id for the instance
      instance.mTexID = texID;

      // done with the vertex array manipulation
      glBindVertexArray(0);
   }

   // create the shaders for this operation
   mBuildingsProgID = glCreateProgram();
   mBuildingsVertID = shader::LoadShaderFile(GL_VERTEX_SHADER, "buildings.vert");
   mBuildingsFragID = shader::LoadShaderFile(GL_FRAGMENT_SHADER, "buildings.frag");
   shader::LinkShaders(mBuildingsProgID, mBuildingsVertID, 0, mBuildingsFragID);

   // create a tree texture object
   ReadRGB("trees.rgb", width, height, &pTexture);

   GLuint treeTexID = 0;
   glGenTextures(1, &treeTexID);
   glBindTexture(GL_TEXTURE_2D, treeTexID);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pTexture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glGenerateMipmap(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, 0);

   // release the texture
   delete [] pTexture;

   // start setting up the instanced data for the trees
   std::vector< std::vector< Vec3f > > treeVerts(NUM_TREE_TYPES);

   // generate data for each instance
   for (uint32_t i = 0; i < mNumTreeInstances; ++i)
   {
      // determine an instance to generate data for
      const uint32_t instance = rand() % NUM_TREE_TYPES;
      // setup the instance data
      const float x = static_cast< float >((rand() % INSTANCE_AREA) * (rand() % 2 == 0 ? 1 : -1));
      const float z = static_cast< float >((rand() % INSTANCE_AREA) * (rand() % 2 == 0 ? 1 : -1));
      // add to the list of instanced data
      treeVerts[instance].push_back(Vec3f(x, 0.0f, z));
   }

   // generate gl data for each object
   for (uint32_t i = 0; i < NUM_TREE_TYPES; ++i)
   {
      // get the instance to work on...
      TreeInstance & instance = mTreeInstances[i];

      // create, fill, and define the vertex array data
      glGenBuffers(1, &instance.mVertBufferID);
      glBindBuffer(GL_ARRAY_BUFFER, instance.mVertBufferID);
      glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3f) * treeVerts[i].size(), &treeVerts[i][0], GL_STATIC_DRAW);

      // define the size of the tree
      const float size[][2] =
      {
         { 1.0f, 1.605f }, { 1.0f, 1.144f }, { 1.0f, 0.244f }
      };

      memcpy(instance.mSize, size[i], sizeof(instance.mSize));

      // reduce the width and height by one for the texture coordinates
      width -= 1; height -= 1;

      // define the texture coordinates
      const float coords[][8] =
      {
         { 0.0f, 343.0f / height, 413.0f / width, 343.0f / height, 413.0f / width, 1006.0f / height, 0.0f, 1006.0f / height },
         { 413.0f / width, 309.0f / height, 1.0f, 309.0f / height, 1.0f, 1007.0f / height, 413.0f / width, 1007.0f / height },
         { 130.0f / width, 36.0f / height, 895.0f / width, 36.0f / height, 895.0f / width, 223.0f / height, 130.0f / width, 223.0f / height }
      };

      memcpy(instance.mTexCoords, coords[i], sizeof(instance.mTexCoords));

      // define the number of instances
      instance.mNumInstances = treeVerts[i].size();
      
      // set the texture id for the instance
      instance.mTexID = treeTexID;
   }

   // create the shaders for this operation
   mTreesProgID = glCreateProgram();
   mTreesVertID = shader::LoadShaderFile(GL_VERTEX_SHADER, "trees.vert");
   mTreesFragID = shader::LoadShaderFile(GL_FRAGMENT_SHADER, "trees.frag");
   mTreesGeomID = shader::LoadShaderFile(GL_GEOMETRY_SHADER, "trees.geom");
   shader::LinkShaders(mTreesProgID, mTreesVertID, mTreesGeomID, mTreesFragID);
}

void InstancingWindow::RenderScene( )
{
   // create the projview matrix
   const Matrixf projview = mPerspective * mCamera;

   // clear the buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // enable textures
   glEnable(GL_TEXTURE_2D);

   // enable instanced building shaders
   glUseProgram(mBuildingsProgID);

   {
   // update the location of the camera
   const GLint pvm_loc = glGetUniformLocation(mBuildingsProgID, "ProjViewMat");
   glUniformMatrix4fv(pvm_loc, 1, GL_FALSE, projview);

   // render the instances
   for (uint32_t i = 0; i < NUM_BUILDING_TYPES; ++i)
   {
      // get the instance
      const BuildingInstance & instance = mBuildingInstances[i];

      // bind the texture to location 0
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, instance.mTexID);

      // setup buffered data
      glBindVertexArray(instance.mVertArrayID);

      // draw the instanced buildings
      glDrawElementsInstanced(GL_QUADS, instance.mIdxBufferSize, GL_UNSIGNED_INT, nullptr, instance.mNumInstances);

      // unbind buffered data
      glBindVertexArray(0);
   }
   }

   // enable instanced tree shaders
   glUseProgram(mTreesProgID);

   {
   // update the location of the camera
   const GLint pvm_loc = glGetUniformLocation(mTreesProgID, "ProjViewMat");
   glUniformMatrix4fv(pvm_loc, 1, GL_FALSE, projview);

   // update the camera location
   const GLint cam_loc = glGetUniformLocation(mTreesProgID, "CameraPos");
   glUniform3fv(cam_loc, 1, mCamera.InverseFromOrthogonal() * Vec3f(0.0f, 0.0f, 0.0f));

   // get the texture coordinates and size locations
   const GLint tex_loc = glGetUniformLocation(mTreesProgID, "TexCoords");
   const GLint size_loc = glGetUniformLocation(mTreesProgID, "Size");

   // render the instances
   for (uint32_t i = 0; i < NUM_TREE_TYPES; ++i)
   {
      // get the instance
      const TreeInstance & instance = mTreeInstances[i];

      // update the tex coords and size
      glUniform2fv(size_loc, 1, instance.mSize);
      glUniform2fv(tex_loc, 4, instance.mTexCoords);

      // bind the texture to location 0
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, instance.mTexID);

      // bind the vertex data and setup the stream of data
      glBindBuffer(GL_ARRAY_BUFFER, instance.mVertBufferID);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

      // render the data
      glDrawArrays(GL_POINTS, 0, instance.mNumInstances);

      // unbind the buffered data
      glBindBuffer(GL_ARRAY_BUFFER, 0);
   }
   }

   // disable instanced tree shaders
   glUseProgram(0);

   // disable texturing
   glDisable(GL_TEXTURE_2D);

   // swap the buffers to display
   SwapBuffers(GetHDC());
}

const float * InstancingWindow::GetTextureCoords( const uint32_t set_id )
{
   // define the texture coordinate values
   static const float tex0_l = 19.0f / 1023.0f;
   static const float tex0_r = 185.0f / 1023.0f;
   static const float tex0_b = (1023.0f - 239.0f) / 1023.0f;
   static const float tex0_t = (1023.0f - 18.0f) / 1023.0f;

   static const float tex1_l = 193.0f / 1023.0f;
   static const float tex1_r = 359.0f / 1023.0f;
   static const float tex1_b = (1023.0f - 239.0f) / 1023.0f;
   static const float tex1_t = (1023.0f - 18.0f) / 1023.0f;

   static const float tex2_l = 366.0f / 1023.0f;
   static const float tex2_r = 533.0f / 1023.0f;
   static const float tex2_b = (1023.0f - 239.0f) / 1023.0f;
   static const float tex2_t = (1023.0f - 18.0f) / 1023.0f;

   static const float tex3_l = 540.0f / 1023.0f;
   static const float tex3_r = 706.0f / 1023.0f;
   static const float tex3_b = (1023.0f - 239.0f) / 1023.0f;
   static const float tex3_t = (1023.0f - 18.0f) / 1023.0f;

   static const float tex4_l = 716.0f / 1023.0f;
   static const float tex4_r = 881.0f / 1023.0f;
   static const float tex4_b = (1023.0f - 239.0f) / 1023.0f;
   static const float tex4_t = (1023.0f - 18.0f) / 1023.0f;

   static const float tex5_l = 19.0f / 1023.0f;
   static const float tex5_r = 185.0f / 1023.0f;
   static const float tex5_b = (1023.0f - 534.0f) / 1023.0f;
   static const float tex5_t = (1023.0f - 312.0f) / 1023.0f;

   static const float tex6_l = 193.0f / 1023.0f;
   static const float tex6_r = 359.0f / 1023.0f;
   static const float tex6_b = (1023.0f - 534.0f) / 1023.0f;
   static const float tex6_t = (1023.0f - 312.0f) / 1023.0f;

   static const float tex7_l = 366.0f / 1023.0f;
   static const float tex7_r = 533.0f / 1023.0f;
   static const float tex7_b = (1023.0f - 534.0f) / 1023.0f;
   static const float tex7_t = (1023.0f - 312.0f) / 1023.0f;

   static const float tex8_l = 540.0f / 1023.0f;
   static const float tex8_r = 706.0f / 1023.0f;
   static const float tex8_b = (1023.0f - 534.0f) / 1023.0f;
   static const float tex8_t = (1023.0f - 312.0f) / 1023.0f;

   static const float tex9_l = 716.0f / 1023.0f;
   static const float tex9_r = 881.0f / 1023.0f;
   static const float tex9_b = (1023.0f - 534.0f) / 1023.0f;
   static const float tex9_t = (1023.0f - 312.0f) / 1023.0f;

   // define the common roof line for all buildings
   static const float tex_rl = 24.0f / 1023.0f;
   static const float tex_rr = 99.0f / 1023.0f;
   static const float tex_rb = (1023.0f - 634.0f) / 1023.0f;
   static const float tex_rt = (1023.0f - 558.0f) / 1023.0f;

   static const float tex_coords[][40] =
   {
      { tex0_l, tex0_b, tex0_r, tex0_b, tex0_r, tex0_t, tex0_l, tex0_t,
        tex0_l, tex0_b, tex0_r, tex0_b, tex0_r, tex0_t, tex0_l, tex0_t,
        tex0_l, tex0_b, tex0_r, tex0_b, tex0_r, tex0_t, tex0_l, tex0_t,
        tex0_l, tex0_b, tex0_r, tex0_b, tex0_r, tex0_t, tex0_l, tex0_t,
        tex_rl, tex_rb, tex_rr, tex_rb, tex_rr, tex_rt, tex_rl, tex_rt },

      { tex1_l, tex1_b, tex1_r, tex1_b, tex1_r, tex1_t, tex1_l, tex1_t,
        tex1_l, tex1_b, tex1_r, tex1_b, tex1_r, tex1_t, tex1_l, tex1_t,
        tex1_l, tex1_b, tex1_r, tex1_b, tex1_r, tex1_t, tex1_l, tex1_t,
        tex1_l, tex1_b, tex1_r, tex1_b, tex1_r, tex1_t, tex1_l, tex1_t,
        tex_rl, tex_rb, tex_rr, tex_rb, tex_rr, tex_rt, tex_rl, tex_rt },

      { tex2_l, tex2_b, tex2_r, tex2_b, tex2_r, tex2_t, tex2_l, tex2_t,
        tex2_l, tex2_b, tex2_r, tex2_b, tex2_r, tex2_t, tex2_l, tex2_t,
        tex2_l, tex2_b, tex2_r, tex2_b, tex2_r, tex2_t, tex2_l, tex2_t,
        tex2_l, tex2_b, tex2_r, tex2_b, tex2_r, tex2_t, tex2_l, tex2_t,
        tex_rl, tex_rb, tex_rr, tex_rb, tex_rr, tex_rt, tex_rl, tex_rt },

      { tex3_l, tex3_b, tex3_r, tex3_b, tex3_r, tex3_t, tex3_l, tex3_t,
        tex3_l, tex3_b, tex3_r, tex3_b, tex3_r, tex3_t, tex3_l, tex3_t,
        tex3_l, tex3_b, tex3_r, tex3_b, tex3_r, tex3_t, tex3_l, tex3_t,
        tex3_l, tex3_b, tex3_r, tex3_b, tex3_r, tex3_t, tex3_l, tex3_t,
        tex_rl, tex_rb, tex_rr, tex_rb, tex_rr, tex_rt, tex_rl, tex_rt },

      { tex4_l, tex4_b, tex4_r, tex4_b, tex4_r, tex4_t, tex4_l, tex4_t,
        tex4_l, tex4_b, tex4_r, tex4_b, tex4_r, tex4_t, tex4_l, tex4_t,
        tex4_l, tex4_b, tex4_r, tex4_b, tex4_r, tex4_t, tex4_l, tex4_t,
        tex4_l, tex4_b, tex4_r, tex4_b, tex4_r, tex4_t, tex4_l, tex4_t,
        tex_rl, tex_rb, tex_rr, tex_rb, tex_rr, tex_rt, tex_rl, tex_rt },

      { tex5_l, tex5_b, tex5_r, tex5_b, tex5_r, tex5_t, tex5_l, tex5_t,
        tex5_l, tex5_b, tex5_r, tex5_b, tex5_r, tex5_t, tex5_l, tex5_t,
        tex5_l, tex5_b, tex5_r, tex5_b, tex5_r, tex5_t, tex5_l, tex5_t,
        tex5_l, tex5_b, tex5_r, tex5_b, tex5_r, tex5_t, tex5_l, tex5_t,
        tex_rl, tex_rb, tex_rr, tex_rb, tex_rr, tex_rt, tex_rl, tex_rt },

      { tex6_l, tex6_b, tex6_r, tex6_b, tex6_r, tex6_t, tex6_l, tex6_t,
        tex6_l, tex6_b, tex6_r, tex6_b, tex6_r, tex6_t, tex6_l, tex6_t,
        tex6_l, tex6_b, tex6_r, tex6_b, tex6_r, tex6_t, tex6_l, tex6_t,
        tex6_l, tex6_b, tex6_r, tex6_b, tex6_r, tex6_t, tex6_l, tex6_t,
        tex_rl, tex_rb, tex_rr, tex_rb, tex_rr, tex_rt, tex_rl, tex_rt },

      { tex7_l, tex7_b, tex7_r, tex7_b, tex7_r, tex7_t, tex7_l, tex7_t,
        tex7_l, tex7_b, tex7_r, tex7_b, tex7_r, tex7_t, tex7_l, tex7_t,
        tex7_l, tex7_b, tex7_r, tex7_b, tex7_r, tex7_t, tex7_l, tex7_t,
        tex7_l, tex7_b, tex7_r, tex7_b, tex7_r, tex7_t, tex7_l, tex7_t,
        tex_rl, tex_rb, tex_rr, tex_rb, tex_rr, tex_rt, tex_rl, tex_rt },

      { tex8_l, tex8_b, tex8_r, tex8_b, tex8_r, tex8_t, tex8_l, tex8_t,
        tex8_l, tex8_b, tex8_r, tex8_b, tex8_r, tex8_t, tex8_l, tex8_t,
        tex8_l, tex8_b, tex8_r, tex8_b, tex8_r, tex8_t, tex8_l, tex8_t,
        tex8_l, tex8_b, tex8_r, tex8_b, tex8_r, tex8_t, tex8_l, tex8_t,
        tex_rl, tex_rb, tex_rr, tex_rb, tex_rr, tex_rt, tex_rl, tex_rt },

      { tex9_l, tex9_b, tex9_r, tex9_b, tex9_r, tex9_t, tex9_l, tex9_t,
        tex9_l, tex9_b, tex9_r, tex9_b, tex9_r, tex9_t, tex9_l, tex9_t,
        tex9_l, tex9_b, tex9_r, tex9_b, tex9_r, tex9_t, tex9_l, tex9_t,
        tex9_l, tex9_b, tex9_r, tex9_b, tex9_r, tex9_t, tex9_l, tex9_t,
        tex_rl, tex_rb, tex_rr, tex_rb, tex_rr, tex_rt, tex_rl, tex_rt }
   };

   // return a random texture set
   return tex_coords[set_id];
}


