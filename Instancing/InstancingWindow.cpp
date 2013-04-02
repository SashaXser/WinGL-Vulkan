// local includes
#include "InstancingWindow.h"
#include "Timer.h"
#include "Matrix.h"
#include "Shaders.h"
#include "Vector4.h"
#include "ReadTexture.h"

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
mVertShaderID        ( 0 ),
mFragShaderID        ( 0 )
{
   // clear out the instances
   memset(&mInstances, 0x00, sizeof(mInstances));
}

InstancingWindow::~InstancingWindow( )
{
}

bool InstancingWindow::Create( unsigned int nWidth, unsigned int nHeight,
                               const char * pWndTitle, const void ** pInitParams )
{
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
         ss << "Instancing - " << std::fixed << std::setprecision(3) << frame_rate << " fps";
         SetWindowText(GetHWND(), ss.str().c_str());
      }
   }

   return appQuitVal;
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
   for (uint32_t i = 0; i < NUM_INSTANCES; ++i)
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
      Instance & instance = mInstances[i];

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
   mProgramID = glCreateProgram();
   mVertShaderID = shader::LoadShaderFile(GL_VERTEX_SHADER, "vertex.vert");
   mFragShaderID = shader::LoadShaderFile(GL_FRAGMENT_SHADER, "fragment.frag");
   shader::LinkShaders(mProgramID, mVertShaderID, 0, mFragShaderID);
}

void InstancingWindow::RenderScene( )
{
   static float rot = 0;
   float len = 100.0f;
   float x = std::cos(rot) * len;
   float y = std::cos(rot) * 30.0f;
   float z = std::sin(rot) * len;
   rot += (15.0f / 30.0f * (M_PI / 180.0f));

   /*const Matrixf pvw =
      Matrixf::Perspective(45.0f, 800.0f / 600.0f, 1.0f, 50.0f) *
      Matrixf::LookAt(x, y, z, 0.0f, 2.5f, 0.0f, 0.0f, 1.0f, 0.0f) *
      Matrixf();*/
   const Matrixf pv =
      Matrixf::Perspective(45.0f, 800.0f / 600.0f, 1.0f, 1000.0f) *
      Matrixf::LookAt(x, y, z, 0.0f, 2.5f, 0.0f, 0.0f, 1.0f, 0.0f);
   //const Matrixf w = Matrixf::Rotate(rot * 180.0f / M_PI, 0, 1, 0);
   //const Matrixf w2 = Matrixf::Translate(3.0, 0, 0) * Matrixf::Rotate(rot * 180.0f / M_PI, 0, 1, 0);
   
   // clear the buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // enable textures
   glEnable(GL_TEXTURE_2D);

   // enable instanced building shaders
   glUseProgram(mProgramID);

   // update the location of the camera
   const GLint pvm_loc = glGetUniformLocation(mProgramID, "ProjViewMat");
   glUniformMatrix4fv(pvm_loc, 1, GL_FALSE, pv);

   // render the instances
   for (uint32_t i = 0; i < NUM_BUILDING_TYPES; ++i)
   {
      // get the instance
      const Instance & instance = mInstances[i];

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

   // disable instanced building shaders
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
        tex_rl, tex_rb, tex_rr, tex_rb, tex_rr, tex_rt, tex_rl, tex_rt }
   };

   // return a random texture set
   return tex_coords[set_id];
}


