// local includes
#include "OpenGLExtensions.h"

// gl includes
#include <gl/gl.h>

// initializing helpers
#define GET_PROC_ADDRESS( func ) \
   func = reinterpret_cast< func##Type >(wglGetProcAddress(#func))

#define DEFINE_OGL_EXT( func ) func##Type func = 0

namespace OpenGLExt
{

void InitializeOpenGLExtensions( int (* (__stdcall * wglGetProcAddress)( const char * ))( void ) )
{
   // initialize extensions
   // context creation
   GET_PROC_ADDRESS(wglCreateContextAttribsARB);

   // fragment program extensions
   GET_PROC_ADDRESS(glProgramStringARB); 
   GET_PROC_ADDRESS(glBindProgramARB);
   GET_PROC_ADDRESS(glDeleteProgramsARB);
   GET_PROC_ADDRESS(glGenProgramsARB);
   GET_PROC_ADDRESS(glProgramEnvParameter4dARB);
   GET_PROC_ADDRESS(glProgramEnvParameter4dvARB);
   GET_PROC_ADDRESS(glProgramEnvParameter4fARB);
   GET_PROC_ADDRESS(glProgramEnvParameter4fvARB);
   GET_PROC_ADDRESS(glProgramLocalParameter4dARB);
   GET_PROC_ADDRESS(glProgramLocalParameter4dvARB);
   GET_PROC_ADDRESS(glProgramLocalParameter4fARB);
   GET_PROC_ADDRESS(glProgramLocalParameter4fvARB);
   GET_PROC_ADDRESS(glGetProgramEnvParameterdvARB);
   GET_PROC_ADDRESS(glGetProgramEnvParameterfvARB);
   GET_PROC_ADDRESS(glGetProgramLocalParameterdvARB);
   GET_PROC_ADDRESS(glGetProgramLocalParameterfvARB);
   GET_PROC_ADDRESS(glGetProgramivARB);
   GET_PROC_ADDRESS(glGetProgramStringARB);
   GET_PROC_ADDRESS(glIsProgramARB);

   // vertex program extensions
   GET_PROC_ADDRESS(glVertexAttrib1sARB);
   GET_PROC_ADDRESS(glVertexAttrib1fARB);
   GET_PROC_ADDRESS(glVertexAttrib1dARB);
   GET_PROC_ADDRESS(glVertexAttrib2sARB);
   GET_PROC_ADDRESS(glVertexAttrib2fARB);
   GET_PROC_ADDRESS(glVertexAttrib2dARB);
   GET_PROC_ADDRESS(glVertexAttrib3sARB);
   GET_PROC_ADDRESS(glVertexAttrib3fARB);
   GET_PROC_ADDRESS(glVertexAttrib3dARB);
   GET_PROC_ADDRESS(glVertexAttrib4sARB);
   GET_PROC_ADDRESS(glVertexAttrib4fARB);
   GET_PROC_ADDRESS(glVertexAttrib4dARB);
   GET_PROC_ADDRESS(glVertexAttrib4NubARB);
   GET_PROC_ADDRESS(glVertexAttrib1svARB);
   GET_PROC_ADDRESS(glVertexAttrib1fvARB);
   GET_PROC_ADDRESS(glVertexAttrib1dvARB);
   GET_PROC_ADDRESS(glVertexAttrib2svARB);
   GET_PROC_ADDRESS(glVertexAttrib2fvARB);
   GET_PROC_ADDRESS(glVertexAttrib2dvARB);
   GET_PROC_ADDRESS(glVertexAttrib3svARB);
   GET_PROC_ADDRESS(glVertexAttrib3fvARB);
   GET_PROC_ADDRESS(glVertexAttrib3dvARB);
   GET_PROC_ADDRESS(glVertexAttrib4bvARB);
   GET_PROC_ADDRESS(glVertexAttrib4svARB);
   GET_PROC_ADDRESS(glVertexAttrib4ivARB);
   GET_PROC_ADDRESS(glVertexAttrib4ubvARB);
   GET_PROC_ADDRESS(glVertexAttrib4usvARB);
   GET_PROC_ADDRESS(glVertexAttrib4uivARB);
   GET_PROC_ADDRESS(glVertexAttrib4fvARB);
   GET_PROC_ADDRESS(glVertexAttrib4dvARB);
   GET_PROC_ADDRESS(glVertexAttrib4NbvARB);
   GET_PROC_ADDRESS(glVertexAttrib4NsvARB);
   GET_PROC_ADDRESS(glVertexAttrib4NivARB);
   GET_PROC_ADDRESS(glVertexAttrib4NubvARB);
   GET_PROC_ADDRESS(glVertexAttrib4NusvARB);
   GET_PROC_ADDRESS(glVertexAttrib4NuivARB);
   GET_PROC_ADDRESS(glVertexAttribPointerARB);
   GET_PROC_ADDRESS(glEnableVertexAttribArrayARB);
   GET_PROC_ADDRESS(glDisableVertexAttribArrayARB);
   GET_PROC_ADDRESS(glGetVertexAttribdvARB);
   GET_PROC_ADDRESS(glGetVertexAttribfvARB);
   GET_PROC_ADDRESS(glGetVertexAttribivARB);
   GET_PROC_ADDRESS(glGetVertexAttribPointervARB);

   // shader object extensions
   GET_PROC_ADDRESS(glDeleteObjectARB);
   GET_PROC_ADDRESS(glGetHandleARB);
   GET_PROC_ADDRESS(glDetachObjectARB);
   GET_PROC_ADDRESS(glCreateShaderObjectARB);
   GET_PROC_ADDRESS(glShaderSourceARB);
   GET_PROC_ADDRESS(glCompileShaderARB);
   GET_PROC_ADDRESS(glCreateProgramObjectARB);
   GET_PROC_ADDRESS(glAttachObjectARB);
   GET_PROC_ADDRESS(glLinkProgramARB);
   GET_PROC_ADDRESS(glUseProgramObjectARB);
   GET_PROC_ADDRESS(glValidateProgramARB);
   GET_PROC_ADDRESS(glUniform1fARB);
   GET_PROC_ADDRESS(glUniform2fARB);
   GET_PROC_ADDRESS(glUniform3fARB);
   GET_PROC_ADDRESS(glUniform4fARB);
   GET_PROC_ADDRESS(glUniform1iARB);
   GET_PROC_ADDRESS(glUniform2iARB);
   GET_PROC_ADDRESS(glUniform3iARB);
   GET_PROC_ADDRESS(glUniform4iARB);
   GET_PROC_ADDRESS(glUniform1fvARB);
   GET_PROC_ADDRESS(glUniform2fvARB);
   GET_PROC_ADDRESS(glUniform3fvARB);
   GET_PROC_ADDRESS(glUniform4fvARB);
   GET_PROC_ADDRESS(glUniform1ivARB);
   GET_PROC_ADDRESS(glUniform2ivARB);
   GET_PROC_ADDRESS(glUniform3ivARB);
   GET_PROC_ADDRESS(glUniform4ivARB);
   GET_PROC_ADDRESS(glUniformMatrix2fvARB);
   GET_PROC_ADDRESS(glUniformMatrix3fvARB);
   GET_PROC_ADDRESS(glUniformMatrix4fvARB);
   GET_PROC_ADDRESS(glGetObjectParameterfvARB);
   GET_PROC_ADDRESS(glGetObjectParameterivARB);
   GET_PROC_ADDRESS(glGetInfoLogARB);
   GET_PROC_ADDRESS(glGetAttachedObjectsARB);
   GET_PROC_ADDRESS(glGetUniformLocationARB);
   GET_PROC_ADDRESS(glGetActiveUniformARB);
   GET_PROC_ADDRESS(glGetUniformfvARB);
   GET_PROC_ADDRESS(glGetUniformivARB);
   GET_PROC_ADDRESS(glGetShaderSourceARB);

   // vertex shader extensions
   GET_PROC_ADDRESS(glBindAttribLocationARB);
   GET_PROC_ADDRESS(glGetActiveAttribARB);
   GET_PROC_ADDRESS(glGetAttribLocationARB);

   // fragment shader extensions

   // vertex buffer object
   GET_PROC_ADDRESS(glBindBufferARB);
   GET_PROC_ADDRESS(glDeleteBuffersARB);
   GET_PROC_ADDRESS(glGenBuffersARB);
   GET_PROC_ADDRESS(glIsBufferARB);
   GET_PROC_ADDRESS(glBufferDataARB);
   GET_PROC_ADDRESS(glBufferSubDataARB);
   GET_PROC_ADDRESS(glGetBufferSubDataARB);
   GET_PROC_ADDRESS(glMapBufferARB);
   GET_PROC_ADDRESS(glUnmapBufferARB);
   GET_PROC_ADDRESS(glGetBufferParameterivARB);
   GET_PROC_ADDRESS(glGetBufferPointervARB);

   // draw instanced extensions
   GET_PROC_ADDRESS(glDrawArraysInstancedARB);
   GET_PROC_ADDRESS(glDrawElementsInstancedARB);

   // debug out extensions
   GET_PROC_ADDRESS(glDebugMessageControlARB);
   GET_PROC_ADDRESS(glDebugMessageInsertARB);
   GET_PROC_ADDRESS(glDebugMessageCallbackARB);
   GET_PROC_ADDRESS(glGetDebugMessageLogARB);
   GET_PROC_ADDRESS(glGetPointerv);

   // amd debug out extensions
   GET_PROC_ADDRESS(glDebugMessageEnableAMD);
   GET_PROC_ADDRESS(glDebugMessageInsertAMD);
   GET_PROC_ADDRESS(glDebugMessageCallbackAMD);
   GET_PROC_ADDRESS(glGetDebugMessageLogAMD);

   // framebuffer object
   GET_PROC_ADDRESS(glIsRenderbuffer);
   GET_PROC_ADDRESS(glBindRenderbuffer);
   GET_PROC_ADDRESS(glDeleteRenderbuffers);
   GET_PROC_ADDRESS(glGenRenderbuffers);
   GET_PROC_ADDRESS(glRenderbufferStorage);
   GET_PROC_ADDRESS(glRenderbufferStorageMultisample);
   GET_PROC_ADDRESS(glGetRenderbufferParameteriv);
   GET_PROC_ADDRESS(glIsFramebuffer);
   GET_PROC_ADDRESS(glBindFramebuffer);
   GET_PROC_ADDRESS(glDeleteFramebuffers);
   GET_PROC_ADDRESS(glGeglnFramebuffers);
   GET_PROC_ADDRESS(glCheckFramebufferStatus);
   GET_PROC_ADDRESS(glFramebufferTexture1D);
   GET_PROC_ADDRESS(glFramebufferTexture2D);
   GET_PROC_ADDRESS(glFramebufferTexture3D);
   GET_PROC_ADDRESS(glFramebufferTextureLayer);
   GET_PROC_ADDRESS(glFramebufferRenderbuffer);
   GET_PROC_ADDRESS(glGetFramebufferAttachmentParameteriv);
   GET_PROC_ADDRESS(glBlitFramebuffer);
   GET_PROC_ADDRESS(glGenerateMipmap);
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

// context creation
DEFINE_OGL_EXT(wglCreateContextAttribsARB);

// fragment program extensions
DEFINE_OGL_EXT(glProgramStringARB);
DEFINE_OGL_EXT(glBindProgramARB);
DEFINE_OGL_EXT(glDeleteProgramsARB);
DEFINE_OGL_EXT(glGenProgramsARB);
DEFINE_OGL_EXT(glProgramEnvParameter4dARB);
DEFINE_OGL_EXT(glProgramEnvParameter4dvARB);
DEFINE_OGL_EXT(glProgramEnvParameter4fARB);
DEFINE_OGL_EXT(glProgramEnvParameter4fvARB);
DEFINE_OGL_EXT(glProgramLocalParameter4dARB);
DEFINE_OGL_EXT(glProgramLocalParameter4dvARB);
DEFINE_OGL_EXT(glProgramLocalParameter4fARB);
DEFINE_OGL_EXT(glProgramLocalParameter4fvARB);
DEFINE_OGL_EXT(glGetProgramEnvParameterdvARB);
DEFINE_OGL_EXT(glGetProgramEnvParameterfvARB);
DEFINE_OGL_EXT(glGetProgramLocalParameterdvARB);
DEFINE_OGL_EXT(glGetProgramLocalParameterfvARB);
DEFINE_OGL_EXT(glGetProgramivARB);
DEFINE_OGL_EXT(glGetProgramStringARB);
DEFINE_OGL_EXT(glIsProgramARB);

// vertex program extensions
DEFINE_OGL_EXT(glVertexAttrib1sARB);
DEFINE_OGL_EXT(glVertexAttrib1fARB);
DEFINE_OGL_EXT(glVertexAttrib1dARB);
DEFINE_OGL_EXT(glVertexAttrib2sARB);
DEFINE_OGL_EXT(glVertexAttrib2fARB);
DEFINE_OGL_EXT(glVertexAttrib2dARB);
DEFINE_OGL_EXT(glVertexAttrib3sARB);
DEFINE_OGL_EXT(glVertexAttrib3fARB);
DEFINE_OGL_EXT(glVertexAttrib3dARB);
DEFINE_OGL_EXT(glVertexAttrib4sARB);
DEFINE_OGL_EXT(glVertexAttrib4fARB);
DEFINE_OGL_EXT(glVertexAttrib4dARB);
DEFINE_OGL_EXT(glVertexAttrib4NubARB);
DEFINE_OGL_EXT(glVertexAttrib1svARB);
DEFINE_OGL_EXT(glVertexAttrib1fvARB);
DEFINE_OGL_EXT(glVertexAttrib1dvARB);
DEFINE_OGL_EXT(glVertexAttrib2svARB);
DEFINE_OGL_EXT(glVertexAttrib2fvARB);
DEFINE_OGL_EXT(glVertexAttrib2dvARB);
DEFINE_OGL_EXT(glVertexAttrib3svARB);
DEFINE_OGL_EXT(glVertexAttrib3fvARB);
DEFINE_OGL_EXT(glVertexAttrib3dvARB);
DEFINE_OGL_EXT(glVertexAttrib4bvARB);
DEFINE_OGL_EXT(glVertexAttrib4svARB);
DEFINE_OGL_EXT(glVertexAttrib4ivARB);
DEFINE_OGL_EXT(glVertexAttrib4ubvARB);
DEFINE_OGL_EXT(glVertexAttrib4usvARB);
DEFINE_OGL_EXT(glVertexAttrib4uivARB);
DEFINE_OGL_EXT(glVertexAttrib4fvARB);
DEFINE_OGL_EXT(glVertexAttrib4dvARB);
DEFINE_OGL_EXT(glVertexAttrib4NbvARB);
DEFINE_OGL_EXT(glVertexAttrib4NsvARB);
DEFINE_OGL_EXT(glVertexAttrib4NivARB);
DEFINE_OGL_EXT(glVertexAttrib4NubvARB);
DEFINE_OGL_EXT(glVertexAttrib4NusvARB);
DEFINE_OGL_EXT(glVertexAttrib4NuivARB);
DEFINE_OGL_EXT(glVertexAttribPointerARB);
DEFINE_OGL_EXT(glEnableVertexAttribArrayARB);
DEFINE_OGL_EXT(glDisableVertexAttribArrayARB);
DEFINE_OGL_EXT(glGetVertexAttribdvARB);
DEFINE_OGL_EXT(glGetVertexAttribfvARB);
DEFINE_OGL_EXT(glGetVertexAttribivARB);
DEFINE_OGL_EXT(glGetVertexAttribPointervARB);

// shader object extensions
DEFINE_OGL_EXT(glDeleteObjectARB);
DEFINE_OGL_EXT(glGetHandleARB);
DEFINE_OGL_EXT(glDetachObjectARB);
DEFINE_OGL_EXT(glCreateShaderObjectARB);
DEFINE_OGL_EXT(glShaderSourceARB);
DEFINE_OGL_EXT(glCompileShaderARB);
DEFINE_OGL_EXT(glCreateProgramObjectARB);
DEFINE_OGL_EXT(glAttachObjectARB);
DEFINE_OGL_EXT(glLinkProgramARB);
DEFINE_OGL_EXT(glUseProgramObjectARB);
DEFINE_OGL_EXT(glValidateProgramARB);
DEFINE_OGL_EXT(glUniform1fARB);
DEFINE_OGL_EXT(glUniform2fARB);
DEFINE_OGL_EXT(glUniform3fARB);
DEFINE_OGL_EXT(glUniform4fARB);
DEFINE_OGL_EXT(glUniform1iARB);
DEFINE_OGL_EXT(glUniform2iARB);
DEFINE_OGL_EXT(glUniform3iARB);
DEFINE_OGL_EXT(glUniform4iARB);
DEFINE_OGL_EXT(glUniform1fvARB);
DEFINE_OGL_EXT(glUniform2fvARB);
DEFINE_OGL_EXT(glUniform3fvARB);
DEFINE_OGL_EXT(glUniform4fvARB);
DEFINE_OGL_EXT(glUniform1ivARB);
DEFINE_OGL_EXT(glUniform2ivARB);
DEFINE_OGL_EXT(glUniform3ivARB);
DEFINE_OGL_EXT(glUniform4ivARB);
DEFINE_OGL_EXT(glUniformMatrix2fvARB);
DEFINE_OGL_EXT(glUniformMatrix3fvARB);
DEFINE_OGL_EXT(glUniformMatrix4fvARB);
DEFINE_OGL_EXT(glGetObjectParameterfvARB);
DEFINE_OGL_EXT(glGetObjectParameterivARB);
DEFINE_OGL_EXT(glGetInfoLogARB);
DEFINE_OGL_EXT(glGetAttachedObjectsARB);
DEFINE_OGL_EXT(glGetUniformLocationARB);
DEFINE_OGL_EXT(glGetActiveUniformARB);
DEFINE_OGL_EXT(glGetUniformfvARB);
DEFINE_OGL_EXT(glGetUniformivARB);
DEFINE_OGL_EXT(glGetShaderSourceARB);

// vertex shader extensions
DEFINE_OGL_EXT(glBindAttribLocationARB);
DEFINE_OGL_EXT(glGetActiveAttribARB);
DEFINE_OGL_EXT(glGetAttribLocationARB);

// vertex buffer object extensions
DEFINE_OGL_EXT(glBindBufferARB);
DEFINE_OGL_EXT(glDeleteBuffersARB);
DEFINE_OGL_EXT(glGenBuffersARB);
DEFINE_OGL_EXT(glIsBufferARB);
DEFINE_OGL_EXT(glBufferDataARB);
DEFINE_OGL_EXT(glBufferSubDataARB);
DEFINE_OGL_EXT(glGetBufferSubDataARB);
DEFINE_OGL_EXT(glMapBufferARB);
DEFINE_OGL_EXT(glUnmapBufferARB);
DEFINE_OGL_EXT(glGetBufferParameterivARB);
DEFINE_OGL_EXT(glGetBufferPointervARB);

// draw instanced extensions
DEFINE_OGL_EXT(glDrawArraysInstancedARB);
DEFINE_OGL_EXT(glDrawElementsInstancedARB);

// debug out extensions
DEFINE_OGL_EXT(glDebugMessageControlARB);
DEFINE_OGL_EXT(glDebugMessageInsertARB);
DEFINE_OGL_EXT(glDebugMessageCallbackARB);
DEFINE_OGL_EXT(glGetDebugMessageLogARB);
DEFINE_OGL_EXT(glGetPointerv);

// amd debug out extensions
DEFINE_OGL_EXT(glDebugMessageEnableAMD);
DEFINE_OGL_EXT(glDebugMessageInsertAMD);
DEFINE_OGL_EXT(glDebugMessageCallbackAMD);
DEFINE_OGL_EXT(glGetDebugMessageLogAMD);

// framebuffer object
DEFINE_OGL_EXT(glIsRenderbuffer);
DEFINE_OGL_EXT(glBindRenderbuffer);
DEFINE_OGL_EXT(glDeleteRenderbuffers);
DEFINE_OGL_EXT(glGenRenderbuffers);
DEFINE_OGL_EXT(glRenderbufferStorage);
DEFINE_OGL_EXT(glRenderbufferStorageMultisample);
DEFINE_OGL_EXT(glGetRenderbufferParameteriv);
DEFINE_OGL_EXT(glIsFramebuffer);
DEFINE_OGL_EXT(glBindFramebuffer);
DEFINE_OGL_EXT(glDeleteFramebuffers);
DEFINE_OGL_EXT(glGeglnFramebuffers);
DEFINE_OGL_EXT(glCheckFramebufferStatus);
DEFINE_OGL_EXT(glFramebufferTexture1D);
DEFINE_OGL_EXT(glFramebufferTexture2D);
DEFINE_OGL_EXT(glFramebufferTexture3D);
DEFINE_OGL_EXT(glFramebufferTextureLayer);
DEFINE_OGL_EXT(glFramebufferRenderbuffer);
DEFINE_OGL_EXT(glGetFramebufferAttachmentParameteriv);
DEFINE_OGL_EXT(glBlitFramebuffer);
DEFINE_OGL_EXT(glGenerateMipmap);

} // namespace OpenGLExt