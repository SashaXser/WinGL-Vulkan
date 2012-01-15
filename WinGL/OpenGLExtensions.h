#ifndef _OPENGL_EXTENSIONS_H_
#define _OPENGL_EXTENSIONS_H_

// platform includes
#include <windows.h>

// opengl ext defines
#define DECLARE_OGL_EXT( ret, params, name ) \
   typedef ret (__stdcall * name##Type) params; \
   extern name##Type name

namespace OpenGLExt
{
// defines ptr sizes
#ifdef _M_IX86
   typedef int sizeiptr;
   typedef int intptr;
#elif  _M_X64
   typedef long long sizeiptr;
   typedef long long intptr;
#else
   // needs to be defined for this system type
   #error Type defines need for this system type...
#endif

// wgl context creation
enum
{
   WGL_CONTEXT_MAJOR_VERSION_ARB =     0x2091,
   WGL_CONTEXT_MINOR_VERSION_ARB =     0x2092,
   WGL_CONTEXT_LAYER_PLANE_ARB =       0x2093,
   WGL_CONTEXT_FLAGS_ARB =             0x2094,
   WGL_CONTEXT_PROFILE_MASK_ARB =      0x9126,

   WGL_CONTEXT_DEBUG_BIT_ARB =               0x0001,
   WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB =  0x0002,

   WGL_CONTEXT_CORE_PROFILE_BIT_ARB =	           0x00001,
   WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB =    0x00002,

   GL_ERROR_INVALID_VERSION_ARB =     0x2095,
   GL_ERROR_INVALID_PROFILE_ARB =		0x2096
};

DECLARE_OGL_EXT(HGLRC, (HDC hDC, HDC hShareContext, const int * attibList), wglCreateContextAttribsARB);

// fragment program extensions
enum
{
   GL_FRAGMENT_PROGRAM_ARB = 0x8804,

   GL_PROGRAM_FORMAT_ASCII_ARB = 0x8875,

   GL_PROGRAM_LENGTH_ARB =                  0x8627,
   GL_PROGRAM_FORMAT_ARB =                  0x8876,
   GL_PROGRAM_BINDING_ARB =                 0x8677,
   GL_PROGRAM_INSTRUCTIONS_ARB =            0x88A0,
   GL_MAX_PROGRAM_INSTRUCTIONS_ARB =        0x88A1,
   GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB =     0x88A2,
   GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB = 0x88A3,
   GL_PROGRAM_TEMPORARIES_ARB =             0x88A4,
   GL_MAX_PROGRAM_TEMPORARIES_ARB =         0x88A5,
   GL_PROGRAM_NATIVE_TEMPORARIES_ARB =      0x88A6,
   GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB =  0x88A7,
   GL_PROGRAM_PARAMETERS_ARB =              0x88A8,
   GL_MAX_PROGRAM_PARAMETERS_ARB =          0x88A9,
   GL_PROGRAM_NATIVE_PARAMETERS_ARB =       0x88AA,
   GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB =   0x88AB,
   GL_PROGRAM_ATTRIBS_ARB =                 0x88AC,
   GL_MAX_PROGRAM_ATTRIBS_ARB =             0x88AD,
   GL_PROGRAM_NATIVE_ATTRIBS_ARB =          0x88AE,
   GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB =      0x88AF,
   GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB =    0x88B4,
   GL_MAX_PROGRAM_ENV_PARAMETERS_ARB =      0x88B5,
   GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB =     0x88B6,

   GL_PROGRAM_ALU_INSTRUCTIONS_ARB =            0x8805,
   GL_PROGRAM_TEX_INSTRUCTIONS_ARB =            0x8806,
   GL_PROGRAM_TEX_INDIRECTIONS_ARB =            0x8807,
   GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB =     0x8808,
   GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB =     0x8809,
   GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB =     0x880A,
   GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB =        0x880B,
   GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB =        0x880C,
   GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB =        0x880D,
   GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB = 0x880E,
   GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB = 0x880F,
   GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB = 0x8810,

   GL_PROGRAM_STRING_ARB = 0x8628,

   GL_PROGRAM_ERROR_POSITION_ARB =         0x864B,
   GL_CURRENT_MATRIX_ARB =                 0x8641,
   GL_TRANSPOSE_CURRENT_MATRIX_ARB =       0x88B7,
   GL_CURRENT_MATRIX_STACK_DEPTH_ARB =     0x8640,
   GL_MAX_PROGRAM_MATRICES_ARB =           0x862F,
   GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB = 0x862E,

   GL_MAX_TEXTURE_COORDS_ARB =      0x8871,
   GL_MAX_TEXTURE_IMAGE_UNITS_ARB = 0x8872,

   GL_PROGRAM_ERROR_STRING_ARB = 0x8874,

   GL_MATRIX0_ARB =  0x88C0,
   GL_MATRIX1_ARB =  0x88C1,
   GL_MATRIX2_ARB =  0x88C2,
   GL_MATRIX3_ARB =  0x88C3,
   GL_MATRIX4_ARB =  0x88C4,
   GL_MATRIX5_ARB =  0x88C5,
   GL_MATRIX6_ARB =  0x88C6,
   GL_MATRIX7_ARB =  0x88C7,
   GL_MATRIX8_ARB =  0x88C8,
   GL_MATRIX9_ARB =  0x88C9,
   GL_MATRIX10_ARB = 0x88CA,
   GL_MATRIX11_ARB = 0x88CB,
   GL_MATRIX12_ARB = 0x88CC,
   GL_MATRIX13_ARB = 0x88CD,
   GL_MATRIX14_ARB = 0x88CE,
   GL_MATRIX15_ARB = 0x88CF,
   GL_MATRIX16_ARB = 0x88D0,
   GL_MATRIX17_ARB = 0x88D1,
   GL_MATRIX18_ARB = 0x88D2,
   GL_MATRIX19_ARB = 0x88D3,
   GL_MATRIX20_ARB = 0x88D4,
   GL_MATRIX21_ARB = 0x88D5,
   GL_MATRIX22_ARB = 0x88D6,
   GL_MATRIX23_ARB = 0x88D7,
   GL_MATRIX24_ARB = 0x88D8,
   GL_MATRIX25_ARB = 0x88D9,
   GL_MATRIX26_ARB = 0x88DA,
   GL_MATRIX27_ARB = 0x88DB,
   GL_MATRIX28_ARB = 0x88DC,
   GL_MATRIX29_ARB = 0x88DD,
   GL_MATRIX30_ARB = 0x88DE,
   GL_MATRIX31_ARB = 0x88DF
};

DECLARE_OGL_EXT(void, (int target, int format, int len, const void * string), glProgramStringARB); 
DECLARE_OGL_EXT(void, (int target, unsigned int program), glBindProgramARB);
DECLARE_OGL_EXT(void, (int n, const unsigned int *programs), glDeleteProgramsARB);
DECLARE_OGL_EXT(void, (int n, unsigned int *programs), glGenProgramsARB);
DECLARE_OGL_EXT(void, (int target, unsigned int index, double x, double y, double z, double w), glProgramEnvParameter4dARB);
DECLARE_OGL_EXT(void, (int target, unsigned int index, const double *params), glProgramEnvParameter4dvARB);
DECLARE_OGL_EXT(void, (int target, unsigned int index, float x, float y, float z, float w), glProgramEnvParameter4fARB);
DECLARE_OGL_EXT(void, (int target, unsigned int index, const float *params), glProgramEnvParameter4fvARB);
DECLARE_OGL_EXT(void, (int target, unsigned int index, double x, double y, double z, double w), glProgramLocalParameter4dARB);
DECLARE_OGL_EXT(void, (int target, unsigned int index, const double *params), glProgramLocalParameter4dvARB);
DECLARE_OGL_EXT(void, (int target, unsigned int index, float x, float y, float z, float w), glProgramLocalParameter4fARB);
DECLARE_OGL_EXT(void, (int target, unsigned int index, const float *params), glProgramLocalParameter4fvARB);
DECLARE_OGL_EXT(void, (int target, unsigned int index, double *params), glGetProgramEnvParameterdvARB);
DECLARE_OGL_EXT(void, (int target, unsigned int index, float *params), glGetProgramEnvParameterfvARB);
DECLARE_OGL_EXT(void, (int target, unsigned int index, double *params), glGetProgramLocalParameterdvARB);
DECLARE_OGL_EXT(void, (int target, unsigned int index, float *params), glGetProgramLocalParameterfvARB);
DECLARE_OGL_EXT(void, (int target, int pname, int *params), glGetProgramivARB);
DECLARE_OGL_EXT(void, (int target, int pname, void *string), glGetProgramStringARB);
DECLARE_OGL_EXT(unsigned char, (unsigned int program), glIsProgramARB);

// vertex program extensions
enum
{
   GL_VERTEX_PROGRAM_ARB = 0x8620,

   GL_VERTEX_PROGRAM_POINT_SIZE_ARB = 0x8642,
   GL_VERTEX_PROGRAM_TWO_SIDE_ARB =   0x8643,
   GL_COLOR_SUM_ARB =                 0x8458,

   GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB =    0x8622,
   GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB =       0x8623,
   GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB =     0x8624,
   GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB =       0x8625,
   GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB = 0x886A,
   GL_CURRENT_VERTEX_ATTRIB_ARB =          0x8626,

   GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB = 0x8645,

   GL_PROGRAM_ADDRESS_REGISTERS_ARB =            0x88B0,
   GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB =        0x88B1,
   GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB =     0x88B2,
   GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB = 0x88B3,

   GL_MAX_VERTEX_ATTRIBS_ARB =             0x8869
};

DECLARE_OGL_EXT(void, (unsigned int index, short x), glVertexAttrib1sARB);
DECLARE_OGL_EXT(void, (unsigned int index, float x), glVertexAttrib1fARB);
DECLARE_OGL_EXT(void, (unsigned int index, double x), glVertexAttrib1dARB);
DECLARE_OGL_EXT(void, (unsigned int index, short x, short y), glVertexAttrib2sARB);
DECLARE_OGL_EXT(void, (unsigned int index, float x, float y), glVertexAttrib2fARB);
DECLARE_OGL_EXT(void, (unsigned int index, double x, double y), glVertexAttrib2dARB);
DECLARE_OGL_EXT(void, (unsigned int index, short x, short y, short z), glVertexAttrib3sARB);
DECLARE_OGL_EXT(void, (unsigned int index, float x, float y, float z), glVertexAttrib3fARB);
DECLARE_OGL_EXT(void, (unsigned int index, double x, double y, double z), glVertexAttrib3dARB);
DECLARE_OGL_EXT(void, (unsigned int index, short x, short y, short z, short w), glVertexAttrib4sARB);
DECLARE_OGL_EXT(void, (unsigned int index, float x, float y, float z, float w), glVertexAttrib4fARB);
DECLARE_OGL_EXT(void, (unsigned int index, double x, double y, double z, double w), glVertexAttrib4dARB);
DECLARE_OGL_EXT(void, (unsigned int index, unsigned char x, unsigned char y, unsigned char z, unsigned char w), glVertexAttrib4NubARB);
DECLARE_OGL_EXT(void, (unsigned int index, const short *v), glVertexAttrib1svARB);
DECLARE_OGL_EXT(void, (unsigned int index, const float *v), glVertexAttrib1fvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const double *v), glVertexAttrib1dvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const short *v), glVertexAttrib2svARB);
DECLARE_OGL_EXT(void, (unsigned int index, const float *v), glVertexAttrib2fvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const double *v), glVertexAttrib2dvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const short *v), glVertexAttrib3svARB);
DECLARE_OGL_EXT(void, (unsigned int index, const float *v), glVertexAttrib3fvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const double *v), glVertexAttrib3dvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const char *v), glVertexAttrib4bvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const short *v), glVertexAttrib4svARB);
DECLARE_OGL_EXT(void, (unsigned int index, const int *v), glVertexAttrib4ivARB);
DECLARE_OGL_EXT(void, (unsigned int index, const unsigned char *v), glVertexAttrib4ubvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const unsigned short *v), glVertexAttrib4usvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const unsigned int *v), glVertexAttrib4uivARB);
DECLARE_OGL_EXT(void, (unsigned int index, const float *v), glVertexAttrib4fvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const double *v), glVertexAttrib4dvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const char *v), glVertexAttrib4NbvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const short *v), glVertexAttrib4NsvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const int *v), glVertexAttrib4NivARB);
DECLARE_OGL_EXT(void, (unsigned int index, const unsigned char *v), glVertexAttrib4NubvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const unsigned short *v), glVertexAttrib4NusvARB);
DECLARE_OGL_EXT(void, (unsigned int index, const unsigned int *v), glVertexAttrib4NuivARB);
DECLARE_OGL_EXT(void, (unsigned int index, int size, int type, unsigned char normalized, int stride, const void *pointer), glVertexAttribPointerARB);
DECLARE_OGL_EXT(void, (unsigned int index), glEnableVertexAttribArrayARB);
DECLARE_OGL_EXT(void, (unsigned int index), glDisableVertexAttribArrayARB);
DECLARE_OGL_EXT(void, (unsigned int index, int pname, double *params), glGetVertexAttribdvARB);
DECLARE_OGL_EXT(void, (unsigned int index, int pname, float *params), glGetVertexAttribfvARB);
DECLARE_OGL_EXT(void, (unsigned int index, int pname, int *params), glGetVertexAttribivARB);
DECLARE_OGL_EXT(void, (unsigned int index, int pname, void **pointer), glGetVertexAttribPointervARB);

// shader object extensions
typedef unsigned int glHandleARB;

enum
{
   GL_PROGRAM_OBJECT_ARB =                   0x8B40,
   GL_OBJECT_TYPE_ARB =                      0x8B4E,
   GL_OBJECT_SUBTYPE_ARB =                   0x8B4F,
   GL_OBJECT_DELETE_STATUS_ARB =             0x8B80,
   GL_OBJECT_COMPILE_STATUS_ARB =            0x8B81,
   GL_OBJECT_LINK_STATUS_ARB =               0x8B82,
   GL_OBJECT_VALIDATE_STATUS_ARB =           0x8B83,
   GL_OBJECT_INFO_LOG_LENGTH_ARB =           0x8B84,
   GL_OBJECT_ATTACHED_OBJECTS_ARB =          0x8B85,
   GL_OBJECT_ACTIVE_UNIFORMS_ARB =           0x8B86,
   GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB = 0x8B87,
   GL_OBJECT_SHADER_SOURCE_LENGTH_ARB =      0x8B88,
   GL_SHADER_OBJECT_ARB =                    0x8B48,
   GL_FLOAT =                                0x1406,
   GL_FLOAT_VEC2_ARB =                       0x8B50,
   GL_FLOAT_VEC3_ARB =                       0x8B51,
   GL_FLOAT_VEC4_ARB =                       0x8B52,
   GL_INT =                                  0x1404,
   GL_INT_VEC2_ARB =                         0x8B53,
   GL_INT_VEC3_ARB =                         0x8B54,
   GL_INT_VEC4_ARB =                         0x8B55,
   GL_BOOL_ARB =                             0x8B56,
   GL_BOOL_VEC2_ARB =                        0x8B57,
   GL_BOOL_VEC3_ARB =                        0x8B58,
   GL_BOOL_VEC4_ARB =                        0x8B59,
   GL_FLOAT_MAT2_ARB =                       0x8B5A,
   GL_FLOAT_MAT3_ARB =                       0x8B5B,
   GL_FLOAT_MAT4_ARB =                       0x8B5C,
   GL_SAMPLER_1D_ARB =                       0x8B5D,
   GL_SAMPLER_2D_ARB =                       0x8B5E,
   GL_SAMPLER_3D_ARB =                       0x8B5F,
   GL_SAMPLER_CUBE_ARB =                     0x8B60,
   GL_SAMPLER_1D_SHADOW_ARB =                0x8B61,
   GL_SAMPLER_2D_SHADOW_ARB =                0x8B62,
   GL_SAMPLER_2D_RECT_ARB =                  0x8B63,
   GL_SAMPLER_2D_RECT_SHADOW_ARB =           0x8B64
};

DECLARE_OGL_EXT(void, (glHandleARB obj), glDeleteObjectARB);
DECLARE_OGL_EXT(glHandleARB, (int pname), glGetHandleARB);
DECLARE_OGL_EXT(void, (glHandleARB containerObj, glHandleARB attachedObj), glDetachObjectARB);
DECLARE_OGL_EXT(glHandleARB, (int shaderType), glCreateShaderObjectARB);
DECLARE_OGL_EXT(void, (glHandleARB shaderObj, int count, const char **string, const int *length), glShaderSourceARB);
DECLARE_OGL_EXT(void, (glHandleARB shaderObj), glCompileShaderARB);
DECLARE_OGL_EXT(glHandleARB, (void), glCreateProgramObjectARB);
DECLARE_OGL_EXT(void, (glHandleARB containerObj, glHandleARB obj), glAttachObjectARB);
DECLARE_OGL_EXT(void, (glHandleARB programObj), glLinkProgramARB);
DECLARE_OGL_EXT(void, (glHandleARB programObj), glUseProgramObjectARB);
DECLARE_OGL_EXT(void, (glHandleARB programObj), glValidateProgramARB);
DECLARE_OGL_EXT(void, (int location, float v0), glUniform1fARB);
DECLARE_OGL_EXT(void, (int location, float v0, float v1), glUniform2fARB);
DECLARE_OGL_EXT(void, (int location, float v0, float v1, float v2), glUniform3fARB);
DECLARE_OGL_EXT(void, (int location, float v0, float v1, float v2, float v3), glUniform4fARB);
DECLARE_OGL_EXT(void, (int location, int v0), glUniform1iARB);
DECLARE_OGL_EXT(void, (int location, int v0, int v1), glUniform2iARB);
DECLARE_OGL_EXT(void, (int location, int v0, int v1, int v2), glUniform3iARB);
DECLARE_OGL_EXT(void, (int location, int v0, int v1, int v2, int v3), glUniform4iARB);
DECLARE_OGL_EXT(void, (int location, int count, const float *value), glUniform1fvARB);
DECLARE_OGL_EXT(void, (int location, int count, const float *value), glUniform2fvARB);
DECLARE_OGL_EXT(void, (int location, int count, const float *value), glUniform3fvARB);
DECLARE_OGL_EXT(void, (int location, int count, const float *value), glUniform4fvARB);
DECLARE_OGL_EXT(void, (int location, int count, const int *value), glUniform1ivARB);
DECLARE_OGL_EXT(void, (int location, int count, const int *value), glUniform2ivARB);
DECLARE_OGL_EXT(void, (int location, int count, const int *value), glUniform3ivARB);
DECLARE_OGL_EXT(void, (int location, int count, const int *value), glUniform4ivARB);
DECLARE_OGL_EXT(void, (int location, int count, unsigned char transpose, const float *value), glUniformMatrix2fvARB);
DECLARE_OGL_EXT(void, (int location, int count, unsigned char transpose, const float *value), glUniformMatrix3fvARB);
DECLARE_OGL_EXT(void, (int location, int count, unsigned char transpose, const float *value), glUniformMatrix4fvARB);
DECLARE_OGL_EXT(void, (glHandleARB obj, int pname, float *params), glGetObjectParameterfvARB);
DECLARE_OGL_EXT(void, (glHandleARB obj, int pname, int *params), glGetObjectParameterivARB);
DECLARE_OGL_EXT(void, (glHandleARB obj, int maxLength, int *length, char *infoLog), glGetInfoLogARB);
DECLARE_OGL_EXT(void, (glHandleARB containerObj, int maxCount, int *count, glHandleARB *obj), glGetAttachedObjectsARB);
DECLARE_OGL_EXT(int, (glHandleARB programObj, const char *name), glGetUniformLocationARB);
DECLARE_OGL_EXT(void, (glHandleARB programObj, unsigned int index, int maxLength, int *length, int *size, int *type, char *name), glGetActiveUniformARB);
DECLARE_OGL_EXT(void, (glHandleARB programObj, int location, float *params), glGetUniformfvARB);
DECLARE_OGL_EXT(void, (glHandleARB programObj, int location, int *params), glGetUniformivARB);
DECLARE_OGL_EXT(void, (glHandleARB obj, int maxLength, int *length, char *source), glGetShaderSourceARB);

// vertex shader extensions
enum
{
   GL_VERTEX_SHADER_ARB = 0x8B31,

   GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB =    0x8B4A,
   GL_MAX_VARYING_FLOATS_ARB =               0x8B4B,
   GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB =   0x8B4C,
   GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB = 0x8B4D,

   GL_OBJECT_ACTIVE_ATTRIBUTES_ARB =           0x8B89,
   GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB = 0x8B8A
};

DECLARE_OGL_EXT(void, (glHandleARB programObj, unsigned int index, const char *name), glBindAttribLocationARB);
DECLARE_OGL_EXT(void, (glHandleARB programObj, unsigned int index, int maxLength, int *length, int *size, int *type, char *name), glGetActiveAttribARB);
DECLARE_OGL_EXT(int, (glHandleARB programObj, const char *name), glGetAttribLocationARB);

// fragment shader extensions
enum
{
   GL_FRAGMENT_SHADER_ARB = 0x8B30,

   GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB = 0x8B49,

   GL_FRAGMENT_SHADER_DERIVATIVE_HINT_ARB = 0x8B8B
};

// vertex buffer object extensions
enum
{
   GL_ARRAY_BUFFER_ARB =                            0x8892,
   GL_ELEMENT_ARRAY_BUFFER_ARB =                    0x8893,
   GL_ARRAY_BUFFER_BINDING_ARB =                    0x8894,
   GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB =            0x8895,
   GL_VERTEX_ARRAY_BUFFER_BINDING_ARB =             0x8896,
   GL_NORMAL_ARRAY_BUFFER_BINDING_ARB =             0x8897,
   GL_COLOR_ARRAY_BUFFER_BINDING_ARB =              0x8898,
   GL_INDEX_ARRAY_BUFFER_BINDING_ARB =              0x8899,
   GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB =      0x889A,
   GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB =          0x889B,
   GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB =    0x889C,
   GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB =     0x889D,
   GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB =             0x889E,
   GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB =      0x889F,
   GL_STREAM_DRAW_ARB =                             0x88E0,
   GL_STREAM_READ_ARB =                             0x88E1,
   GL_STREAM_COPY_ARB =                             0x88E2,
   GL_STATIC_DRAW_ARB =                             0x88E4,
   GL_STATIC_READ_ARB =                             0x88E5,
   GL_STATIC_COPY_ARB =                             0x88E6,
   GL_DYNAMIC_DRAW_ARB =                            0x88E8,
   GL_DYNAMIC_READ_ARB =                            0x88E9,
   GL_DYNAMIC_COPY_ARB =                            0x88EA,
   GL_READ_ONLY_ARB =                               0x88B8,
   GL_WRITE_ONLY_ARB =                              0x88B9,
   GL_READ_WRITE_ARB =                              0x88BA,
   GL_BUFFER_SIZE_ARB =                             0x8764,
   GL_BUFFER_USAGE_ARB =                            0x8765,
   GL_BUFFER_ACCESS_ARB =                           0x88BB,
   GL_BUFFER_MAPPED_ARB =                           0x88BC,
   GL_BUFFER_MAP_POINTER_ARB =                      0x88BD
};

DECLARE_OGL_EXT(void, (unsigned int target, unsigned int buffer), glBindBufferARB);
DECLARE_OGL_EXT(void, (int n, const unsigned int *buffers), glDeleteBuffersARB);
DECLARE_OGL_EXT(void, (int n, unsigned int *buffers), glGenBuffersARB);
DECLARE_OGL_EXT(unsigned char, (unsigned int buffer), glIsBufferARB);
DECLARE_OGL_EXT(void, (unsigned int target, intptr size, const void *data, unsigned int usage), glBufferDataARB);
DECLARE_OGL_EXT(void, (unsigned int target, intptr offset, sizeiptr size, const void *data), glBufferSubDataARB);
DECLARE_OGL_EXT(void, (unsigned int target, intptr offset, sizeiptr size, void *data), glGetBufferSubDataARB);
DECLARE_OGL_EXT(void *, (unsigned int target, unsigned int access), glMapBufferARB);
DECLARE_OGL_EXT(unsigned char, (unsigned int target), glUnmapBufferARB);
DECLARE_OGL_EXT(void, (unsigned int target, unsigned int pname, int *params), glGetBufferParameterivARB);
DECLARE_OGL_EXT(void, (unsigned int target, unsigned int pname, void **params), glGetBufferPointervARB);

// opengl draw instanced defines
DECLARE_OGL_EXT(void, (int mode, int first, int count, int primcount), glDrawArraysInstancedARB);
DECLARE_OGL_EXT(void, (int mode, int count, int type, const void * indices, int primcount), glDrawElementsInstancedARB);

// opengl debug out defines
enum
{
   GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB =      0x8242,

   GL_MAX_DEBUG_MESSAGE_LENGTH_ARB =            0x9143,
   GL_MAX_DEBUG_LOGGED_MESSAGES_ARB =           0x9144,
   GL_DEBUG_LOGGED_MESSAGES_ARB =               0x9145,
   GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB =    0x8243,

   GL_DEBUG_CALLBACK_FUNCTION_ARB =       0x8244,
   GL_DEBUG_CALLBACK_USER_PARAM_ARB =     0x8245,

   GL_DEBUG_SOURCE_API_ARB =              0x8246,
   GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB =    0x8247,
   GL_DEBUG_SOURCE_SHADER_COMPILER_ARB =  0x8248,
   GL_DEBUG_SOURCE_THIRD_PARTY_ARB =      0x8249,
   GL_DEBUG_SOURCE_APPLICATION_ARB =      0x824A,
   GL_DEBUG_SOURCE_OTHER_ARB =            0x824B,

   GL_DEBUG_TYPE_ERROR_ARB =                 0x824C,
   GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB =   0x824D,
   GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB =    0x824E,
   GL_DEBUG_TYPE_PORTABILITY_ARB =           0x824F,
   GL_DEBUG_TYPE_PERFORMANCE_ARB =           0x8250,
   GL_DEBUG_TYPE_OTHER_ARB =                 0x8251,

   GL_DEBUG_SEVERITY_HIGH_ARB =     0x9146,
   GL_DEBUG_SEVERITY_MEDIUM_ARB =   0x9147,
   GL_DEBUG_SEVERITY_LOW_ARB =      0x9148
};

typedef void (__stdcall * GLDEBUGPROCARB)(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char * message, void * userParam);

DECLARE_OGL_EXT(void, (unsigned int source, unsigned int type, unsigned int severity, int count, const unsigned int * ids, unsigned char enabled), glDebugMessageControlARB);
DECLARE_OGL_EXT(void, (unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char * buf), glDebugMessageInsertARB);
DECLARE_OGL_EXT(void, (GLDEBUGPROCARB callback, void * userParam), glDebugMessageCallbackARB);
DECLARE_OGL_EXT(unsigned int, (unsigned int count, int bufsize, unsigned int * sources, unsigned int * types, unsigned int * ids, unsigned int * severities, int * lengths, char * messageLog), glGetDebugMessageLogARB);
DECLARE_OGL_EXT(void, (unsigned int pname, void ** params), glGetPointerv);

// amd debug out defines
enum
{
   GL_MAX_DEBUG_MESSAGE_LENGTH_AMD =   0x9143,
   GL_MAX_DEBUG_LOGGED_MESSAGES_AMD =  0x9144,
   GL_DEBUG_LOGGED_MESSAGES_AMD =      0x9145,       

   GL_DEBUG_SEVERITY_HIGH_AMD =     0x9146,
   GL_DEBUG_SEVERITY_MEDIUM_AMD =   0x9147,
   GL_DEBUG_SEVERITY_LOW_AMD =      0x9148,

   GL_DEBUG_CATEGORY_API_ERROR_AMD =            0x9149,
   GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD =        0x914A,
   GL_DEBUG_CATEGORY_DEPRECATION_AMD =          0x914B,
   GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD =   0x914C,
   GL_DEBUG_CATEGORY_PERFORMANCE_AMD =          0x914D,
   GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD =      0x914E,
   GL_DEBUG_CATEGORY_APPLICATION_AMD =          0x914F,
   GL_DEBUG_CATEGORY_OTHER_AMD =                0x9150
};

typedef void (__stdcall * GLDEBUGPROCAMD)(unsigned int id, unsigned int category, unsigned int severity, int length, const char * message, void * userParam);

DECLARE_OGL_EXT(void, (unsigned int category, unsigned int severity, int count, const unsigned int * ids, unsigned char enabled), glDebugMessageEnableAMD);
DECLARE_OGL_EXT(void, (unsigned int category, unsigned int severity, unsigned int id, int length, const char * buf), glDebugMessageInsertAMD);
DECLARE_OGL_EXT(void, (GLDEBUGPROCAMD callback, void * userParam), glDebugMessageCallbackAMD);
DECLARE_OGL_EXT(unsigned int, (unsigned int count, int bufsize, unsigned int * categories, unsigned int * severities, unsigned int * ids, int * lengths, char * message), glGetDebugMessageLogAMD);

// framebuffer object
DECLARE_OGL_EXT(unsigned char, (unsigned int renderbuffer), glIsRenderbuffer);
DECLARE_OGL_EXT(void, (unsigned int target, unsigned int renderbuffer), glBindRenderbuffer);
DECLARE_OGL_EXT(void, (int n, const unsigned int * renderbuffers), glDeleteRenderbuffers);
DECLARE_OGL_EXT(void, (int n, unsigned int * renderbuffers), glGenRenderbuffers);
DECLARE_OGL_EXT(void, (unsigned int target, unsigned int internalformat, int width, int height), glRenderbufferStorage);
DECLARE_OGL_EXT(void, (unsigned int target, int samples, unsigned int internalformat, int width, int height), glRenderbufferStorageMultisample);
DECLARE_OGL_EXT(void, (unsigned int target, unsigned int pname, int * params), glGetRenderbufferParameteriv);
DECLARE_OGL_EXT(unsigned char, (unsigned int framebuffer), glIsFramebuffer);
DECLARE_OGL_EXT(void, (unsigned int target, unsigned int framebuffer), glBindFramebuffer);
DECLARE_OGL_EXT(void, (int n, const unsigned int * framebuffers), glDeleteFramebuffers);
DECLARE_OGL_EXT(void, (int n, unsigned int * framebuffers), glGenFramebuffers);
DECLARE_OGL_EXT(unsigned int, (unsigned int target), glCheckFramebufferStatus);
DECLARE_OGL_EXT(void, (unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level), glFramebufferTexture1D);
DECLARE_OGL_EXT(void, (unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level), glFramebufferTexture2D);
DECLARE_OGL_EXT(void, (unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level, int layer), glFramebufferTexture3D);
DECLARE_OGL_EXT(void, (unsigned int target, unsigned int attachment, unsigned int texture, int level, int layer), glFramebufferTextureLayer);
DECLARE_OGL_EXT(void, (unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer), glFramebufferRenderbuffer);
DECLARE_OGL_EXT(void, (unsigned int target, unsigned int attachment, unsigned int pname, int * params), glGetFramebufferAttachmentParameteriv);
DECLARE_OGL_EXT(void, (int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, unsigned int mask, unsigned int filter), glBlitFramebuffer);
DECLARE_OGL_EXT(void, (unsigned int target), glGenerateMipmap);

enum
{
   GL_FRAMEBUFFER =                    0x8D40,
   GL_READ_FRAMEBUFFER =               0x8CA8,
   GL_DRAW_FRAMEBUFFER =               0x8CA9,

   GL_RENDERBUFFER =                   0x8D41,

   GL_STENCIL_INDEX1 =                 0x8D46,
   GL_STENCIL_INDEX4 =                 0x8D47,
   GL_STENCIL_INDEX8 =                 0x8D48,
   GL_STENCIL_INDEX16 =                0x8D49,

   GL_RENDERBUFFER_WIDTH =             0x8D42,
   GL_RENDERBUFFER_HEIGHT =            0x8D43,
   GL_RENDERBUFFER_INTERNAL_FORMAT =   0x8D44,
   GL_RENDERBUFFER_RED_SIZE =          0x8D50,
   GL_RENDERBUFFER_GREEN_SIZE =        0x8D51,
   GL_RENDERBUFFER_BLUE_SIZE =         0x8D52,
   GL_RENDERBUFFER_ALPHA_SIZE =        0x8D53,
   GL_RENDERBUFFER_DEPTH_SIZE =        0x8D54,
   GL_RENDERBUFFER_STENCIL_SIZE =      0x8D55,
   GL_RENDERBUFFER_SAMPLES =           0x8CAB,

   GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE =           0x8CD0,
   GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME =           0x8CD1,
   GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL =         0x8CD2,
   GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE = 0x8CD3,
   GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER =         0x8CD4,
   GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING =        0x8210,
   GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE =        0x8211,
   GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE =              0x8212,
   GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE =            0x8213,
   GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE =             0x8214,
   GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE =            0x8215,
   GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE =            0x8216,
   GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE =          0x8217,

   GL_SRGB =                     0x8C40,
   GL_UNSIGNED_NORMALIZED =      0x8C17,
   GL_FRAMEBUFFER_DEFAULT =      0x8218,
   GL_INDEX =                    0x8222,

   GL_COLOR_ATTACHMENT0 =               0x8CE0,
   GL_COLOR_ATTACHMENT1 =               0x8CE1,
   GL_COLOR_ATTACHMENT2 =               0x8CE2,
   GL_COLOR_ATTACHMENT3 =               0x8CE3,
   GL_COLOR_ATTACHMENT4 =               0x8CE4,
   GL_COLOR_ATTACHMENT5 =               0x8CE5,
   GL_COLOR_ATTACHMENT6 =               0x8CE6,
   GL_COLOR_ATTACHMENT7 =               0x8CE7,
   GL_COLOR_ATTACHMENT8 =               0x8CE8,
   GL_COLOR_ATTACHMENT9 =               0x8CE9,
   GL_COLOR_ATTACHMENT10 =              0x8CEA,
   GL_COLOR_ATTACHMENT11 =              0x8CEB,
   GL_COLOR_ATTACHMENT12 =              0x8CEC,
   GL_COLOR_ATTACHMENT13 =              0x8CED,
   GL_COLOR_ATTACHMENT14 =              0x8CEE,
   GL_COLOR_ATTACHMENT15 =              0x8CEF,
   GL_DEPTH_ATTACHMENT =                0x8D00,
   GL_STENCIL_ATTACHMENT =              0x8D20,
   GL_DEPTH_STENCIL_ATTACHMENT =        0x821A,

   GL_MAX_SAMPLES =                     0x8D57,

   GL_FRAMEBUFFER_COMPLETE =                         0x8CD5,
   GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT =            0x8CD6,
   GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT =    0x8CD7,
   GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER =           0x8CDB,
   GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER =           0x8CDC,
   GL_FRAMEBUFFER_UNSUPPORTED =                      0x8CDD,
   GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE =           0x8D56,
   GL_FRAMEBUFFER_UNDEFINED =                        0x8219,

   GL_FRAMEBUFFER_BINDING =            0x8CA6,
   GL_DRAW_FRAMEBUFFER_BINDING =       0x8CA6,
   GL_READ_FRAMEBUFFER_BINDING =       0x8CAA,
   GL_RENDERBUFFER_BINDING =           0x8CA7,
   GL_MAX_COLOR_ATTACHMENTS =          0x8CDF,
   GL_MAX_RENDERBUFFER_SIZE =          0x84E8,

   GL_INVALID_FRAMEBUFFER_OPERATION =  0x0506,

   GL_DEPTH_STENCIL =            0x84F9,

   GL_UNSIGNED_INT_24_8 =        0x84FA,

   GL_DEPTH24_STENCIL8 =         0x88F0,

   GL_TEXTURE_STENCIL_SIZE =     0x88F1
};

// draw buffers
DECLARE_OGL_EXT(void, (int n, const unsigned int * bufs), glDrawBuffersARB);

enum
{
   MAX_DRAW_BUFFERS_ARB =     0x8824,
   DRAW_BUFFER0_ARB =         0x8825,
   DRAW_BUFFER1_ARB =         0x8826,
   DRAW_BUFFER2_ARB =         0x8827,
   DRAW_BUFFER3_ARB =         0x8828,
   DRAW_BUFFER4_ARB =         0x8829,
   DRAW_BUFFER5_ARB =         0x882A,
   DRAW_BUFFER6_ARB =         0x882B,
   DRAW_BUFFER7_ARB =         0x882C,
   DRAW_BUFFER8_ARB =         0x882D,
   DRAW_BUFFER9_ARB =         0x882E,
   DRAW_BUFFER10_ARB =        0x882F,
   DRAW_BUFFER11_ARB =        0x8830,
   DRAW_BUFFER12_ARB =        0x8831,
   DRAW_BUFFER13_ARB =        0x8832,
   DRAW_BUFFER14_ARB =        0x8833,
   DRAW_BUFFER15_ARB =        0x8834
};

// misc defines
enum
{
   // gl mipmap texture defines
   GL_GENERATE_MIPMAP =       0x8191,
   GL_GENERATE_MIPMAP_HINT =  0x8192
};

// initializes gl extensions
// the function passed in is a function pointer assigned the name wglGetProcAddress
// which accepts a const char *...  the returning value is another function pointer
// that returns int and take no arguments...
void InitializeOpenGLExtensions( int (* (__stdcall * wglGetProcAddress)( const char * ))( void ) );

// determines if an extension is supported
bool IsExtensionSupported( const char * const pExtension );

}; // namespace OpenGLExt

#endif // _OPENGL_EXTENSIONS_H_
