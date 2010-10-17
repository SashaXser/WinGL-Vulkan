// local includes
#include "OpenGLExtensions.h"

// initializing helpers
#define GET_PROC_ADDRESS( ret, params, func ) \
   func = reinterpret_cast< ret (__stdcall *) params >(wglGetProcAddress(#func))

namespace OpenGLExt
{

void InitializeOpenGLExtensions( int (* (__stdcall * wglGetProcAddress)( const char * ))( void ) )
{
   // initialize extensions
   // context creation
   GET_PROC_ADDRESS(HGLRC, (HDC, HDC, const int *), wglCreateContextAttribsARB);

   // fragment program extensions
   GET_PROC_ADDRESS(void, (int, int, int, const void *), glProgramStringARB); 
   GET_PROC_ADDRESS(void, (int, unsigned int), glBindProgramARB);
   GET_PROC_ADDRESS(void, (int, const unsigned int *), glDeleteProgramsARB);
   GET_PROC_ADDRESS(void, (int, unsigned int *), glGenProgramsARB);
   GET_PROC_ADDRESS(void, (int, unsigned int, double, double, double, double), glProgramEnvParameter4dARB);
   GET_PROC_ADDRESS(void, (int, unsigned int, const double *), glProgramEnvParameter4dvARB);
   GET_PROC_ADDRESS(void, (int, unsigned int, float, float, float, float), glProgramEnvParameter4fARB);
   GET_PROC_ADDRESS(void, (int, unsigned int, const float *), glProgramEnvParameter4fvARB);
   GET_PROC_ADDRESS(void, (int, unsigned int, double, double, double, double), glProgramLocalParameter4dARB);
   GET_PROC_ADDRESS(void, (int, unsigned int, const double *), glProgramLocalParameter4dvARB);
   GET_PROC_ADDRESS(void, (int, unsigned int, float, float, float, float), glProgramLocalParameter4fARB);
   GET_PROC_ADDRESS(void, (int, unsigned int, const float *), glProgramLocalParameter4fvARB);
   GET_PROC_ADDRESS(void, (int, unsigned int, double *), glGetProgramEnvParameterdvARB);
   GET_PROC_ADDRESS(void, (int, unsigned int, float *), glGetProgramEnvParameterfvARB);
   GET_PROC_ADDRESS(void, (int, unsigned int, double *), glGetProgramLocalParameterdvARB);
   GET_PROC_ADDRESS(void, (int, unsigned int, float *), glGetProgramLocalParameterfvARB);
   GET_PROC_ADDRESS(void, (int, int, int *), glGetProgramivARB);
   GET_PROC_ADDRESS(void, (int, int, void *), glGetProgramStringARB);
   GET_PROC_ADDRESS(unsigned char, (unsigned int), glIsProgramARB);

   // vertex program extensions
   GET_PROC_ADDRESS(void, (unsigned int, short), glVertexAttrib1sARB);
   GET_PROC_ADDRESS(void, (unsigned int, float), glVertexAttrib1fARB);
   GET_PROC_ADDRESS(void, (unsigned int, double), glVertexAttrib1dARB);
   GET_PROC_ADDRESS(void, (unsigned int, short, short), glVertexAttrib2sARB);
   GET_PROC_ADDRESS(void, (unsigned int, float, float), glVertexAttrib2fARB);
   GET_PROC_ADDRESS(void, (unsigned int, double, double), glVertexAttrib2dARB);
   GET_PROC_ADDRESS(void, (unsigned int, short, short, short), glVertexAttrib3sARB);
   GET_PROC_ADDRESS(void, (unsigned int, float, float, float), glVertexAttrib3fARB);
   GET_PROC_ADDRESS(void, (unsigned int, double, double, double), glVertexAttrib3dARB);
   GET_PROC_ADDRESS(void, (unsigned int, short, short, short, short), glVertexAttrib4sARB);
   GET_PROC_ADDRESS(void, (unsigned int, float, float, float, float), glVertexAttrib4fARB);
   GET_PROC_ADDRESS(void, (unsigned int, double, double, double, double), glVertexAttrib4dARB);
   GET_PROC_ADDRESS(void, (unsigned int, unsigned char, unsigned char, unsigned char, unsigned char), glVertexAttrib4NubARB);
   GET_PROC_ADDRESS(void, (unsigned int, const short *), glVertexAttrib1svARB);
   GET_PROC_ADDRESS(void, (unsigned int, const float *), glVertexAttrib1fvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const double *), glVertexAttrib1dvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const short *), glVertexAttrib2svARB);
   GET_PROC_ADDRESS(void, (unsigned int, const float *), glVertexAttrib2fvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const double *), glVertexAttrib2dvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const short *), glVertexAttrib3svARB);
   GET_PROC_ADDRESS(void, (unsigned int, const float *), glVertexAttrib3fvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const double *), glVertexAttrib3dvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const char *), glVertexAttrib4bvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const short *), glVertexAttrib4svARB);
   GET_PROC_ADDRESS(void, (unsigned int, const int *), glVertexAttrib4ivARB);
   GET_PROC_ADDRESS(void, (unsigned int, const unsigned char *), glVertexAttrib4ubvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const unsigned short *), glVertexAttrib4usvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const unsigned int *), glVertexAttrib4uivARB);
   GET_PROC_ADDRESS(void, (unsigned int, const float *), glVertexAttrib4fvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const double *), glVertexAttrib4dvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const char *), glVertexAttrib4NbvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const short *), glVertexAttrib4NsvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const int *), glVertexAttrib4NivARB);
   GET_PROC_ADDRESS(void, (unsigned int, const unsigned char *), glVertexAttrib4NubvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const unsigned short *), glVertexAttrib4NusvARB);
   GET_PROC_ADDRESS(void, (unsigned int, const unsigned int *), glVertexAttrib4NuivARB);
   GET_PROC_ADDRESS(void, (unsigned int, int, int, unsigned char, int, const void *), glVertexAttribPointerARB);
   GET_PROC_ADDRESS(void, (unsigned int), glEnableVertexAttribArrayARB);
   GET_PROC_ADDRESS(void, (unsigned int), glDisableVertexAttribArrayARB);
   GET_PROC_ADDRESS(void, (unsigned int, int, double *), glGetVertexAttribdvARB);
   GET_PROC_ADDRESS(void, (unsigned int, int, float *), glGetVertexAttribfvARB);
   GET_PROC_ADDRESS(void, (unsigned int, int, int *), glGetVertexAttribivARB);
   GET_PROC_ADDRESS(void, (unsigned int, int, void **), glGetVertexAttribPointervARB);

   // shader object extensions
   GET_PROC_ADDRESS(void, (glHandleARB), glDeleteObjectARB);
   GET_PROC_ADDRESS(glHandleARB, (int), glGetHandleARB);
   GET_PROC_ADDRESS(void, (glHandleARB, glHandleARB), glDetachObjectARB);
   GET_PROC_ADDRESS(glHandleARB, (int), glCreateShaderObjectARB);
   GET_PROC_ADDRESS(void, (glHandleARB, int, const char **, const int *), glShaderSourceARB);
   GET_PROC_ADDRESS(void, (glHandleARB), glCompileShaderARB);
   GET_PROC_ADDRESS(glHandleARB, (void), glCreateProgramObjectARB);
   GET_PROC_ADDRESS(void, (glHandleARB, glHandleARB), glAttachObjectARB);
   GET_PROC_ADDRESS(void, (glHandleARB), glLinkProgramARB);
   GET_PROC_ADDRESS(void, (glHandleARB), glUseProgramObjectARB);
   GET_PROC_ADDRESS(void, (glHandleARB), glValidateProgramARB);
   GET_PROC_ADDRESS(void, (int, float), glUniform1fARB);
   GET_PROC_ADDRESS(void, (int, float, float), glUniform2fARB);
   GET_PROC_ADDRESS(void, (int, float, float, float), glUniform3fARB);
   GET_PROC_ADDRESS(void, (int, float, float, float, float), glUniform4fARB);
   GET_PROC_ADDRESS(void, (int, int), glUniform1iARB);
   GET_PROC_ADDRESS(void, (int, int, int), glUniform2iARB);
   GET_PROC_ADDRESS(void, (int, int, int, int), glUniform3iARB);
   GET_PROC_ADDRESS(void, (int, int, int, int, int), glUniform4iARB);
   GET_PROC_ADDRESS(void, (int, int, const float *), glUniform1fvARB);
   GET_PROC_ADDRESS(void, (int, int, const float *), glUniform2fvARB);
   GET_PROC_ADDRESS(void, (int, int, const float *), glUniform3fvARB);
   GET_PROC_ADDRESS(void, (int, int, const float *), glUniform4fvARB);
   GET_PROC_ADDRESS(void, (int, int, const int *), glUniform1ivARB);
   GET_PROC_ADDRESS(void, (int, int, const int *), glUniform2ivARB);
   GET_PROC_ADDRESS(void, (int, int, const int *), glUniform3ivARB);
   GET_PROC_ADDRESS(void, (int, int, const int *), glUniform4ivARB);
   GET_PROC_ADDRESS(void, (int, int, unsigned char, const float *), glUniformMatrix2fvARB);
   GET_PROC_ADDRESS(void, (int, int, unsigned char, const float *), glUniformMatrix3fvARB);
   GET_PROC_ADDRESS(void, (int, int, unsigned char, const float *), glUniformMatrix4fvARB);
   GET_PROC_ADDRESS(void, (glHandleARB, int, float *), glGetObjectParameterfvARB);
   GET_PROC_ADDRESS(void, (glHandleARB, int, int *), glGetObjectParameterivARB);
   GET_PROC_ADDRESS(void, (glHandleARB, int, int *, char *), glGetInfoLogARB);
   GET_PROC_ADDRESS(void, (glHandleARB, int, int *, glHandleARB *), glGetAttachedObjectsARB);
   GET_PROC_ADDRESS(int, (glHandleARB, const char *), glGetUniformLocationARB);
   GET_PROC_ADDRESS(void, (glHandleARB, unsigned int, int, int *, int *, int *, char *), glGetActiveUniformARB);
   GET_PROC_ADDRESS(void, (glHandleARB, int, float *), glGetUniformfvARB);
   GET_PROC_ADDRESS(void, (glHandleARB, int, int *), glGetUniformivARB);
   GET_PROC_ADDRESS(void, (glHandleARB, int, int *, char *), glGetShaderSourceARB);

   // vertex shader extensions
   GET_PROC_ADDRESS(void, (glHandleARB, unsigned int, const char *), glBindAttribLocationARB);
   GET_PROC_ADDRESS(void, (glHandleARB, unsigned int, int, int *, int *, int *, char *), glGetActiveAttribARB);
   GET_PROC_ADDRESS(int, (glHandleARB, const char *), glGetAttribLocationARB);

   // fragment shader extensions

   // vertex buffer object
   GET_PROC_ADDRESS(void, (unsigned int, unsigned int), glBindBufferARB);
   GET_PROC_ADDRESS(void, (int, const unsigned int *), glDeleteBuffersARB);
   GET_PROC_ADDRESS(void, (int, unsigned int *), glGenBuffersARB);
   GET_PROC_ADDRESS(unsigned char, (unsigned int), glIsBufferARB);
   GET_PROC_ADDRESS(void, (unsigned int, intptr, const void *, unsigned int), glBufferDataARB);
   GET_PROC_ADDRESS(void, (unsigned int, intptr, sizeiptr, const void *), glBufferSubDataARB);
   GET_PROC_ADDRESS(void, (unsigned int, intptr, sizeiptr, void *), glGetBufferSubDataARB);
   GET_PROC_ADDRESS(void *, (unsigned int, unsigned int), glMapBufferARB);
   GET_PROC_ADDRESS(unsigned char, (unsigned int), glUnmapBufferARB);
   GET_PROC_ADDRESS(void, (unsigned int, unsigned int, int *), glGetBufferParameterivARB);
   GET_PROC_ADDRESS(void, (unsigned int, unsigned int, void **), glGetBufferPointervARB);

   // debug out extensions
   GET_PROC_ADDRESS(void, (unsigned int, unsigned int, unsigned int, int, const unsigned int *, unsigned char), glDebugMessageControlARB);
   GET_PROC_ADDRESS(void, (unsigned int, unsigned int, unsigned int, unsigned int, int, const char *), glDebugMessageInsertARB);
   GET_PROC_ADDRESS(void, (GLDEBUGPROCARB, void *), glDebugMessageCallbackARB);
   GET_PROC_ADDRESS(unsigned int, (unsigned int, int, unsigned int *, unsigned int *, unsigned int *, unsigned int *, int *, char *), glGetDebugMessageLogARB);
   GET_PROC_ADDRESS(void, (unsigned int, void **), glGetPointerv);
}

// context creation
HGLRC (__stdcall * wglCreateContextAttribsARB)(HDC hDC, HDC hShareContext, const int *attibList) = 0;

// fragment program extensions
void (__stdcall * glProgramStringARB)(int target, int format, int len, const void *string) = 0; 
void (__stdcall * glBindProgramARB)(int target, unsigned int program) = 0;
void (__stdcall * glDeleteProgramsARB)(int n, const unsigned int *programs) = 0;
void (__stdcall * glGenProgramsARB)(int n, unsigned int *programs) = 0;
void (__stdcall * glProgramEnvParameter4dARB)(int target, unsigned int index, double x, double y, double z, double w) = 0;
void (__stdcall * glProgramEnvParameter4dvARB)(int target, unsigned int index, const double *params) = 0;
void (__stdcall * glProgramEnvParameter4fARB)(int target, unsigned int index, float x, float y, float z, float w) = 0;
void (__stdcall * glProgramEnvParameter4fvARB)(int target, unsigned int index, const float *params) = 0;
void (__stdcall * glProgramLocalParameter4dARB)(int target, unsigned int index, double x, double y, double z, double w) = 0;
void (__stdcall * glProgramLocalParameter4dvARB)(int target, unsigned int index, const double *params) = 0;
void (__stdcall * glProgramLocalParameter4fARB)(int target, unsigned int index, float x, float y, float z, float w) = 0;
void (__stdcall * glProgramLocalParameter4fvARB)(int target, unsigned int index, const float *params) = 0;
void (__stdcall * glGetProgramEnvParameterdvARB)(int target, unsigned int index, double *params) = 0;
void (__stdcall * glGetProgramEnvParameterfvARB)(int target, unsigned int index, float *params) = 0;
void (__stdcall * glGetProgramLocalParameterdvARB)(int target, unsigned int index, double *params) = 0;
void (__stdcall * glGetProgramLocalParameterfvARB)(int target, unsigned int index, float *params) = 0;
void (__stdcall * glGetProgramivARB)(int target, int pname, int *params) = 0;
void (__stdcall * glGetProgramStringARB)(int target, int pname, void *string) = 0;
unsigned char (__stdcall * glIsProgramARB)(unsigned int program) = 0;

// vertex program extensions
void (__stdcall * glVertexAttrib1sARB)(unsigned int index, short x) = 0;
void (__stdcall * glVertexAttrib1fARB)(unsigned int index, float x) = 0;
void (__stdcall * glVertexAttrib1dARB)(unsigned int index, double x) = 0;
void (__stdcall * glVertexAttrib2sARB)(unsigned int index, short x, short y) = 0;
void (__stdcall * glVertexAttrib2fARB)(unsigned int index, float x, float y) = 0;
void (__stdcall * glVertexAttrib2dARB)(unsigned int index, double x, double y) = 0;
void (__stdcall * glVertexAttrib3sARB)(unsigned int index, short x, short y, short z) = 0;
void (__stdcall * glVertexAttrib3fARB)(unsigned int index, float x, float y, float z) = 0;
void (__stdcall * glVertexAttrib3dARB)(unsigned int index, double x, double y, double z) = 0;
void (__stdcall * glVertexAttrib4sARB)(unsigned int index, short x, short y, short z, short w) = 0;
void (__stdcall * glVertexAttrib4fARB)(unsigned int index, float x, float y, float z, float w) = 0;
void (__stdcall * glVertexAttrib4dARB)(unsigned int index, double x, double y, double z, double w) = 0;
void (__stdcall * glVertexAttrib4NubARB)(unsigned int index, unsigned char x, unsigned char y, unsigned char z, unsigned char w) = 0;
void (__stdcall * glVertexAttrib1svARB)(unsigned int index, const short *v) = 0;
void (__stdcall * glVertexAttrib1fvARB)(unsigned int index, const float *v) = 0;
void (__stdcall * glVertexAttrib1dvARB)(unsigned int index, const double *v) = 0;
void (__stdcall * glVertexAttrib2svARB)(unsigned int index, const short *v) = 0;
void (__stdcall * glVertexAttrib2fvARB)(unsigned int index, const float *v) = 0;
void (__stdcall * glVertexAttrib2dvARB)(unsigned int index, const double *v) = 0;
void (__stdcall * glVertexAttrib3svARB)(unsigned int index, const short *v) = 0;
void (__stdcall * glVertexAttrib3fvARB)(unsigned int index, const float *v) = 0;
void (__stdcall * glVertexAttrib3dvARB)(unsigned int index, const double *v) = 0;
void (__stdcall * glVertexAttrib4bvARB)(unsigned int index, const char *v) = 0;
void (__stdcall * glVertexAttrib4svARB)(unsigned int index, const short *v) = 0;
void (__stdcall * glVertexAttrib4ivARB)(unsigned int index, const int *v) = 0;
void (__stdcall * glVertexAttrib4ubvARB)(unsigned int index, const unsigned char *v) = 0;
void (__stdcall * glVertexAttrib4usvARB)(unsigned int index, const unsigned short *v) = 0;
void (__stdcall * glVertexAttrib4uivARB)(unsigned int index, const unsigned int *v) = 0;
void (__stdcall * glVertexAttrib4fvARB)(unsigned int index, const float *v) = 0;
void (__stdcall * glVertexAttrib4dvARB)(unsigned int index, const double *v) = 0;
void (__stdcall * glVertexAttrib4NbvARB)(unsigned int index, const char *v) = 0;
void (__stdcall * glVertexAttrib4NsvARB)(unsigned int index, const short *v) = 0;
void (__stdcall * glVertexAttrib4NivARB)(unsigned int index, const int *v) = 0;
void (__stdcall * glVertexAttrib4NubvARB)(unsigned int index, const unsigned char *v) = 0;
void (__stdcall * glVertexAttrib4NusvARB)(unsigned int index, const unsigned short *v) = 0;
void (__stdcall * glVertexAttrib4NuivARB)(unsigned int index, const unsigned int *v) = 0;
void (__stdcall * glVertexAttribPointerARB)(unsigned int index, int size, int type, unsigned char normalized, int stride, const void *pointer) = 0;
void (__stdcall * glEnableVertexAttribArrayARB)(unsigned int index) = 0;
void (__stdcall * glDisableVertexAttribArrayARB)(unsigned int index) = 0;
void (__stdcall * glGetVertexAttribdvARB)(unsigned int index, int pname, double *params) = 0;
void (__stdcall * glGetVertexAttribfvARB)(unsigned int index, int pname, float *params) = 0;
void (__stdcall * glGetVertexAttribivARB)(unsigned int index, int pname, int *params) = 0;
void (__stdcall * glGetVertexAttribPointervARB)(unsigned int index, int pname, void **pointer) = 0;

// shader object extensions
void (__stdcall * glDeleteObjectARB)(glHandleARB obj) = 0;
glHandleARB (__stdcall * glGetHandleARB)(int pname) = 0;
void (__stdcall * glDetachObjectARB)(glHandleARB containerObj, glHandleARB attachedObj) = 0;
glHandleARB (__stdcall * glCreateShaderObjectARB)(int shaderType) = 0;
void (__stdcall * glShaderSourceARB)(glHandleARB shaderObj, int count, const char **string, const int *length) = 0;
void (__stdcall * glCompileShaderARB)(glHandleARB shaderObj) = 0;
glHandleARB (__stdcall * glCreateProgramObjectARB)(void) = 0;
void (__stdcall * glAttachObjectARB)(glHandleARB containerObj, glHandleARB obj) = 0;
void (__stdcall * glLinkProgramARB)(glHandleARB programObj) = 0;
void (__stdcall * glUseProgramObjectARB)(glHandleARB programObj) = 0;
void (__stdcall * glValidateProgramARB)(glHandleARB programObj) = 0;
void (__stdcall * glUniform1fARB)(int location, float v0) = 0;
void (__stdcall * glUniform2fARB)(int location, float v0, float v1) = 0;
void (__stdcall * glUniform3fARB)(int location, float v0, float v1, float v2) = 0;
void (__stdcall * glUniform4fARB)(int location, float v0, float v1, float v2, float v3) = 0;
void (__stdcall * glUniform1iARB)(int location, int v0) = 0;
void (__stdcall * glUniform2iARB)(int location, int v0, int v1) = 0;
void (__stdcall * glUniform3iARB)(int location, int v0, int v1, int v2) = 0;
void (__stdcall * glUniform4iARB)(int location, int v0, int v1, int v2, int v3) = 0;
void (__stdcall * glUniform1fvARB)(int location, int count, const float *value) = 0;
void (__stdcall * glUniform2fvARB)(int location, int count, const float *value) = 0;
void (__stdcall * glUniform3fvARB)(int location, int count, const float *value) = 0;
void (__stdcall * glUniform4fvARB)(int location, int count, const float *value) = 0;
void (__stdcall * glUniform1ivARB)(int location, int count, const int *value) = 0;
void (__stdcall * glUniform2ivARB)(int location, int count, const int *value) = 0;
void (__stdcall * glUniform3ivARB)(int location, int count, const int *value) = 0;
void (__stdcall * glUniform4ivARB)(int location, int count, const int *value) = 0;
void (__stdcall * glUniformMatrix2fvARB)(int location, int count, unsigned char transpose, const float *value) = 0;
void (__stdcall * glUniformMatrix3fvARB)(int location, int count, unsigned char transpose, const float *value) = 0;
void (__stdcall * glUniformMatrix4fvARB)(int location, int count, unsigned char transpose, const float *value) = 0;
void (__stdcall * glGetObjectParameterfvARB)(glHandleARB obj, int pname, float *params) = 0;
void (__stdcall * glGetObjectParameterivARB)(glHandleARB obj, int pname, int *params) = 0;
void (__stdcall * glGetInfoLogARB)(glHandleARB obj, int maxLength, int *length, char *infoLog) = 0;
void (__stdcall * glGetAttachedObjectsARB)(glHandleARB containerObj, int maxCount, int *count, glHandleARB *obj) = 0;
int (__stdcall * glGetUniformLocationARB)(glHandleARB programObj, const char *name) = 0;
void (__stdcall * glGetActiveUniformARB)(glHandleARB programObj, unsigned int index, int maxLength, int *length, int *size, int *type, char *name) = 0;
void (__stdcall * glGetUniformfvARB)(glHandleARB programObj, int location, float *params) = 0;
void (__stdcall * glGetUniformivARB)(glHandleARB programObj, int location, int *params) = 0;
void (__stdcall * glGetShaderSourceARB)(glHandleARB obj, int maxLength, int *length, char *source) = 0;

// vertex shader extensions
void (__stdcall * glBindAttribLocationARB)(glHandleARB programObj, unsigned int index, const char *name) = 0;
void (__stdcall * glGetActiveAttribARB)(glHandleARB programObj, unsigned int index, int maxLength, int *length, int *size, int *type, char *name) = 0;
int (__stdcall * glGetAttribLocationARB)(glHandleARB programObj, const char *name) = 0;

// vertex buffer object extensions
void (__stdcall * glBindBufferARB)(unsigned int target, unsigned int buffer) = 0;
void (__stdcall * glDeleteBuffersARB)(int n, const unsigned int *buffers) = 0;
void (__stdcall * glGenBuffersARB)(int n, unsigned int *buffers) = 0;
unsigned char (__stdcall * glIsBufferARB)(unsigned int buffer) = 0;
void (__stdcall * glBufferDataARB)(unsigned int target, intptr size, const void *data, unsigned int usage) = 0;
void (__stdcall * glBufferSubDataARB)(unsigned int target, intptr offset, sizeiptr size, const void *data) = 0;
void (__stdcall * glGetBufferSubDataARB)(unsigned int target, intptr offset, sizeiptr size, void *data) = 0;
void * (__stdcall * glMapBufferARB)(unsigned int target, unsigned int access) = 0;
unsigned char (__stdcall * glUnmapBufferARB)(unsigned int target) = 0;
void (__stdcall * glGetBufferParameterivARB)(unsigned int target, unsigned int pname, int *params) = 0;
void (__stdcall * glGetBufferPointervARB)(unsigned int target, unsigned int pname, void **params) = 0;

// debug out extensions
void (__stdcall * glDebugMessageControlARB)(unsigned int source, unsigned int type, unsigned int severity, int count, const unsigned int * ids, unsigned char enabled) = 0;
void (__stdcall * glDebugMessageInsertARB)(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char * buf) = 0;
void (__stdcall * glDebugMessageCallbackARB)(GLDEBUGPROCARB callback, void * userParam) = 0;
unsigned int (__stdcall * glGetDebugMessageLogARB)(unsigned int count, int bufsize, unsigned int * sources, unsigned int * types, unsigned int * ids, unsigned int * severities, int * lengths, char * messageLog) = 0;
void (__stdcall * glGetPointerv)(unsigned int pname, void ** params) = 0;

} // namespace OpenGLExt