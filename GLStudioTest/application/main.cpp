// platform includes
#include <windows.h>

// glsc includes
#include "component.h"

// wingl includes
#include "Timer.h"
#include "Matrix.h"
#include "Texture.h"
#include "Pipeline.h"
#include "AllocConsole.h"
#include "OpenGLWindow.h"
#include "ShaderProgram.h"
#include "FrameBufferObject.h"
#include "VertexBufferObject.h"

// std includes
#include <vector>
#include <cstdint>

// gl includes
#include <GL/GL.h>

class GLStudioWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   GLStudioWindow( );

   // creates the application
   virtual bool Create( unsigned int nWidth,
                        unsigned int nHeight,
                        const char * pWndTitle,
                        const void * pInitParams = NULL );

   // basic run implementation
   // will process messages until app quit
   virtual int Run( );
   
protected:
   // destructor
   virtual ~GLStudioWindow( );

   // called when the window is about to be destroyed
   virtual void OnDestroy( ) override;

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam ) override;

private:
   // component to be rendered
   glsc::Component * _glsc_component;

   // frame buffer to render into
   float _rotation_deg;
   FrameBufferObject * _fbo;
   
};

GLStudioWindow::GLStudioWindow( ) :
_glsc_component( nullptr ),
_rotation_deg( 0.0f ),
_fbo( nullptr )
{
}

bool GLStudioWindow::Create( unsigned int nWidth,
                             unsigned int nHeight,
                             const char * pWndTitle,
                             const void * pInitParams )
{
   // initialize with a context else nothing
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 0, true, true, false },
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      std::cout << "Press 'o' to find texture and render to it" << std::endl;

      // make the context current
      MakeCurrent();

      // construct a new component to render
      _glsc_component = glsc::CreateComponent();

      return true;
   }

   return false;
}

int GLStudioWindow::Run( )
{
   // vars for the loop
   int appQuitVal = 0;
   bool bQuit = false;

   // basic message pump and render frame
   while (!bQuit)
   {
      double start_frame_ms = Timer().GetCurrentTimeMS();

      // process all the messages
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         double current_time_ms = Timer().GetCurrentTimeMS();
         double delta_time_ms = current_time_ms - start_frame_ms;
         double sleep_ms = 1.0 / 60.0 * 1000.0 - delta_time_ms;

         if (sleep_ms >= 1.0)
         {
            Timer().Wait(static_cast< uint32_t >(sleep_ms));
         }

         start_frame_ms = Timer().GetCurrentTimeMS();

         // clear the back buffer and the depth buffer
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         // draw the components
         _glsc_component->Draw();

         if (_fbo)
         {
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);

            Pipeline pipeline;
            pipeline.PushViewport(viewport[0], viewport[1],
                                  viewport[2], viewport[3]);

            _fbo->Bind(GL_DRAW_FRAMEBUFFER);

            pipeline.PushViewport(0, 0, _fbo->Width(), _fbo->Height());

            VertexBufferObject vbo;
            vbo.GenBuffer(GL_ARRAY_BUFFER);
            vbo.Bind();
            const float vertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f };
            vbo.BufferStorage(sizeof(vertices), vertices, 0);
            vbo.VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
            vbo.Unbind();

            ShaderProgram shader;
            shader.Attach(GL_VERTEX_SHADER,
                          "#version 400\n"
                           "layout (location = 0) in vec2 vertex_position;\n"
                           "uniform mat4 model_view_proj;\n"
                           "\n"
                           "void main( )\n"
                           "{\n"
                           "   gl_Position = model_view_proj * vec4(vertex_position, 0.0, 1.0f);\n"
                           "}\n");
            shader.Attach(GL_FRAGMENT_SHADER,
                          "#version 400\n"
                           "layout (location = 0) out vec4 frag_color_dest_0;\n"
                           "\n"
                           "void main( )\n"
                           "{\n"
                           "   frag_color_dest_0 = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
                           "}\n");
            shader.Link();

            shader.Enable();

            glClear(GL_COLOR_BUFFER_BIT);

            pipeline.EnableVertexAttribArray(true, 0);

            Matrixf proj;
            proj.MakeOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

            for (size_t i = 0; i <= 10; ++i)
            {
               const float scale = i != 0 ? i / 10.0f : 0.01f;
               Matrixf modelview = Matrixf::Translate(0.5f, 0.5f, 0.0f) *
                                   Matrixf::Rotate(_rotation_deg, 0.0f, 0.0f, -1.0f) *
                                   Matrixf::Scale(scale) *
                                   Matrixf::Translate(-0.5f, -0.5f, 0.0f);

               _rotation_deg += 0.36f / 60.0f * 2.0f;

               shader.SetUniformMatrix< 1, 4, 4 >("model_view_proj", proj * modelview);

               vbo.Bind();

               glDrawArrays(GL_LINE_STRIP, 0, sizeof(vertices) / (2 * sizeof(*vertices)));

               vbo.Unbind();
            }

            pipeline.EnableVertexAttribArray(false, 0);

            shader.Disable();

            _fbo->Unbind();

            pipeline.PopViewport();

            Texture * texture = _fbo->GetAttachment(GL_COLOR_ATTACHMENT0);

            if (texture->IsMipMapped())
            {
               texture->Bind(GL_TEXTURE0);
               glGenerateMipmap(GL_TEXTURE_2D);
               texture->Unbind();
            }
         }

         // swap the front and back buffers
         SwapBuffers(GetHDC());
      }
   }

   return appQuitVal;
}

GLStudioWindow::~GLStudioWindow( )
{
}

void GLStudioWindow::OnDestroy( )
{
   // release the components
   delete _fbo;
   delete _glsc_component;

   // call the base class destroy
   OpenGLWindow::OnDestroy();
}

LRESULT GLStudioWindow::MessageHandler( UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_SIZE:
   {
      // get the width and height
      const int32_t width = static_cast<int32_t>(lParam & 0x0000FFFF);
      const int32_t height = static_cast<int32_t>(lParam >> 16);

      // change the size of the viewport
      // this will be ignored until a valid gl context is created
      glViewport(0, 0,
                 static_cast<GLsizei>(width),
                 static_cast<GLsizei>(height));

      break;
   }

   case WM_CHAR:
   {
      if (wParam == 'o' || wParam == 'O')
      {
         for (GLuint i = 1; i <= 1'000'000; ++i)
         {
            if (glIsTexture(i))
            {
               glBindTexture(GL_TEXTURE_2D, i);

               GLint width, height;
               glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
               glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &height);

               std::vector< uint8_t > data(width * height * 4);

               // would be nice to be able to use glGetTextureImage or glGetnTexImage
               glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

               const uint32_t gsr_id = 0x00525347;
               const uint32_t mpd_id = 0x0044504D;

               if ((*reinterpret_cast< uint32_t * >(&data[0]) & 0x00FFFFFF) == gsr_id &&
                   (*reinterpret_cast< uint32_t * >(&data[4]) & 0x00FFFFFF) == mpd_id)
               {
                  // associate the texture
                  Texture texture(i, false);

                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                  // create the frame buffer
                  _fbo = new FrameBufferObject;
                  _fbo->GenBuffer(texture.GetWidth(), texture.GetHeight());
                  _fbo->Bind(GL_DRAW_FRAMEBUFFER);
                  _fbo->Attach(GL_COLOR_ATTACHMENT0, std::move(texture));
                  _fbo->Unbind();
               }

               glBindTexture(GL_TEXTURE_2D, 0);

               if (_fbo) break;
            }
         }
      }

      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);

      break;
   }

   default:
      // allow default processing to happen
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);

      break;
   }

   return result;
}

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nShowCmd )
{
   // indicates success or failure
   int error = -1;

   // allocate a console for the application
   AllocateDebugConsole();

   // create a new tesselation window
   GLStudioWindow * const pGLS = new GLStudioWindow();
   if (pGLS->Create(640, 640, "GLStudio Component Test"))
   {
      // run the program
      error = pGLS->Run();
   }

   return error;
}
