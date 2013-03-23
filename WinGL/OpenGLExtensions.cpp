// local includes
#include "OpenGLExtensions.h"
#include "WglAssert.h"

// gl includes
#include <GL/glew.h>
#include <gl/gl.h>

namespace gl
{

void InitializeOpenGLExtensions( )
{
   // init the gl extensions through glew
   const GLenum init = glewInit();

   // make sure that it glew completed correctly
   WGL_ASSERT(init == GLEW_OK);
}

bool IsExtensionSupported( const char * const pExtension )
{
   // currently not supported
   bool supported = false;

   // obtain the extensions string
   const char * pExtensions =
      reinterpret_cast< const char * >(glGetString(GL_EXTENSIONS));

   // obtain the extensions str length
   const size_t extStrLen = strlen(pExtension);

   // continue until no extension is found
   while (pExtensions)
   {
      // substring into the list of extensions
      pExtensions = strstr(pExtensions, pExtension);

      // make sure an extension is found and that the
      // next character is a space or the null terminator
      if (pExtensions && (*(pExtensions + extStrLen) == ' ' ||
                          *(pExtensions + extStrLen) == '\0'))
      {
         supported = true; break;
      }
   }
   
   return supported;
}

} // namespace gl