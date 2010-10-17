#ifndef _OPENGL_EXTENSIONS_H_
#define _OPENGL_EXTENSIONS_H_

#include <windows.h>

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

extern HGLRC (__stdcall * wglCreateContextAttribsARB)(HDC hDC, HDC hShareContext, const int *attibList);

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

extern void (__stdcall * glProgramStringARB)(int target, int format, int len, const void *string); 
extern void (__stdcall * glBindProgramARB)(int target, unsigned int program);
extern void (__stdcall * glDeleteProgramsARB)(int n, const unsigned int *programs);
extern void (__stdcall * glGenProgramsARB)(int n, unsigned int *programs);
extern void (__stdcall * glProgramEnvParameter4dARB)(int target, unsigned int index, double x, double y, double z, double w);
extern void (__stdcall * glProgramEnvParameter4dvARB)(int target, unsigned int index, const double *params);
extern void (__stdcall * glProgramEnvParameter4fARB)(int target, unsigned int index, float x, float y, float z, float w);
extern void (__stdcall * glProgramEnvParameter4fvARB)(int target, unsigned int index, const float *params);
extern void (__stdcall * glProgramLocalParameter4dARB)(int target, unsigned int index, double x, double y, double z, double w);
extern void (__stdcall * glProgramLocalParameter4dvARB)(int target, unsigned int index, const double *params);
extern void (__stdcall * glProgramLocalParameter4fARB)(int target, unsigned int index, float x, float y, float z, float w);
extern void (__stdcall * glProgramLocalParameter4fvARB)(int target, unsigned int index, const float *params);
extern void (__stdcall * glGetProgramEnvParameterdvARB)(int target, unsigned int index, double *params);
extern void (__stdcall * glGetProgramEnvParameterfvARB)(int target, unsigned int index, float *params);
extern void (__stdcall * glGetProgramLocalParameterdvARB)(int target, unsigned int index, double *params);
extern void (__stdcall * glGetProgramLocalParameterfvARB)(int target, unsigned int index, float *params);
extern void (__stdcall * glGetProgramivARB)(int target, int pname, int *params);
extern void (__stdcall * glGetProgramStringARB)(int target, int pname, void *string);
extern unsigned char (__stdcall * glIsProgramARB)(unsigned int program);

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

extern void (__stdcall * glVertexAttrib1sARB)(unsigned int index, short x);
extern void (__stdcall * glVertexAttrib1fARB)(unsigned int index, float x);
extern void (__stdcall * glVertexAttrib1dARB)(unsigned int index, double x);
extern void (__stdcall * glVertexAttrib2sARB)(unsigned int index, short x, short y);
extern void (__stdcall * glVertexAttrib2fARB)(unsigned int index, float x, float y);
extern void (__stdcall * glVertexAttrib2dARB)(unsigned int index, double x, double y);
extern void (__stdcall * glVertexAttrib3sARB)(unsigned int index, short x, short y, short z);
extern void (__stdcall * glVertexAttrib3fARB)(unsigned int index, float x, float y, float z);
extern void (__stdcall * glVertexAttrib3dARB)(unsigned int index, double x, double y, double z);
extern void (__stdcall * glVertexAttrib4sARB)(unsigned int index, short x, short y, short z, short w);
extern void (__stdcall * glVertexAttrib4fARB)(unsigned int index, float x, float y, float z, float w);
extern void (__stdcall * glVertexAttrib4dARB)(unsigned int index, double x, double y, double z, double w);
extern void (__stdcall * glVertexAttrib4NubARB)(unsigned int index, unsigned char x, unsigned char y, unsigned char z, unsigned char w);
extern void (__stdcall * glVertexAttrib1svARB)(unsigned int index, const short *v);
extern void (__stdcall * glVertexAttrib1fvARB)(unsigned int index, const float *v);
extern void (__stdcall * glVertexAttrib1dvARB)(unsigned int index, const double *v);
extern void (__stdcall * glVertexAttrib2svARB)(unsigned int index, const short *v);
extern void (__stdcall * glVertexAttrib2fvARB)(unsigned int index, const float *v);
extern void (__stdcall * glVertexAttrib2dvARB)(unsigned int index, const double *v);
extern void (__stdcall * glVertexAttrib3svARB)(unsigned int index, const short *v);
extern void (__stdcall * glVertexAttrib3fvARB)(unsigned int index, const float *v);
extern void (__stdcall * glVertexAttrib3dvARB)(unsigned int index, const double *v);
extern void (__stdcall * glVertexAttrib4bvARB)(unsigned int index, const char *v);
extern void (__stdcall * glVertexAttrib4svARB)(unsigned int index, const short *v);
extern void (__stdcall * glVertexAttrib4ivARB)(unsigned int index, const int *v);
extern void (__stdcall * glVertexAttrib4ubvARB)(unsigned int index, const unsigned char *v);
extern void (__stdcall * glVertexAttrib4usvARB)(unsigned int index, const unsigned short *v);
extern void (__stdcall * glVertexAttrib4uivARB)(unsigned int index, const unsigned int *v);
extern void (__stdcall * glVertexAttrib4fvARB)(unsigned int index, const float *v);
extern void (__stdcall * glVertexAttrib4dvARB)(unsigned int index, const double *v);
extern void (__stdcall * glVertexAttrib4NbvARB)(unsigned int index, const char *v);
extern void (__stdcall * glVertexAttrib4NsvARB)(unsigned int index, const short *v);
extern void (__stdcall * glVertexAttrib4NivARB)(unsigned int index, const int *v);
extern void (__stdcall * glVertexAttrib4NubvARB)(unsigned int index, const unsigned char *v);
extern void (__stdcall * glVertexAttrib4NusvARB)(unsigned int index, const unsigned short *v);
extern void (__stdcall * glVertexAttrib4NuivARB)(unsigned int index, const unsigned int *v);
extern void (__stdcall * glVertexAttribPointerARB)(unsigned int index, int size, int type, unsigned char normalized, int stride, const void *pointer);
extern void (__stdcall * glEnableVertexAttribArrayARB)(unsigned int index);
extern void (__stdcall * glDisableVertexAttribArrayARB)(unsigned int index);
extern void (__stdcall * glGetVertexAttribdvARB)(unsigned int index, int pname, double *params);
extern void (__stdcall * glGetVertexAttribfvARB)(unsigned int index, int pname, float *params);
extern void (__stdcall * glGetVertexAttribivARB)(unsigned int index, int pname, int *params);
extern void (__stdcall * glGetVertexAttribPointervARB)(unsigned int index, int pname, void **pointer);

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

extern void (__stdcall * glDeleteObjectARB)(glHandleARB obj);
extern glHandleARB (__stdcall * glGetHandleARB)(int pname);
extern void (__stdcall * glDetachObjectARB)(glHandleARB containerObj, glHandleARB attachedObj);
extern glHandleARB (__stdcall * glCreateShaderObjectARB)(int shaderType);
extern void (__stdcall * glShaderSourceARB)(glHandleARB shaderObj, int count, const char **string, const int *length);
extern void (__stdcall * glCompileShaderARB)(glHandleARB shaderObj);
extern glHandleARB (__stdcall * glCreateProgramObjectARB)(void);
extern void (__stdcall * glAttachObjectARB)(glHandleARB containerObj, glHandleARB obj);
extern void (__stdcall * glLinkProgramARB)(glHandleARB programObj);
extern void (__stdcall * glUseProgramObjectARB)(glHandleARB programObj);
extern void (__stdcall * glValidateProgramARB)(glHandleARB programObj);
extern void (__stdcall * glUniform1fARB)(int location, float v0);
extern void (__stdcall * glUniform2fARB)(int location, float v0, float v1);
extern void (__stdcall * glUniform3fARB)(int location, float v0, float v1, float v2);
extern void (__stdcall * glUniform4fARB)(int location, float v0, float v1, float v2, float v3);
extern void (__stdcall * glUniform1iARB)(int location, int v0);
extern void (__stdcall * glUniform2iARB)(int location, int v0, int v1);
extern void (__stdcall * glUniform3iARB)(int location, int v0, int v1, int v2);
extern void (__stdcall * glUniform4iARB)(int location, int v0, int v1, int v2, int v3);
extern void (__stdcall * glUniform1fvARB)(int location, int count, const float *value);
extern void (__stdcall * glUniform2fvARB)(int location, int count, const float *value);
extern void (__stdcall * glUniform3fvARB)(int location, int count, const float *value);
extern void (__stdcall * glUniform4fvARB)(int location, int count, const float *value);
extern void (__stdcall * glUniform1ivARB)(int location, int count, const int *value);
extern void (__stdcall * glUniform2ivARB)(int location, int count, const int *value);
extern void (__stdcall * glUniform3ivARB)(int location, int count, const int *value);
extern void (__stdcall * glUniform4ivARB)(int location, int count, const int *value);
extern void (__stdcall * glUniformMatrix2fvARB)(int location, int count, unsigned char transpose, const float *value);
extern void (__stdcall * glUniformMatrix3fvARB)(int location, int count, unsigned char transpose, const float *value);
extern void (__stdcall * glUniformMatrix4fvARB)(int location, int count, unsigned char transpose, const float *value);
extern void (__stdcall * glGetObjectParameterfvARB)(glHandleARB obj, int pname, float *params);
extern void (__stdcall * glGetObjectParameterivARB)(glHandleARB obj, int pname, int *params);
extern void (__stdcall * glGetInfoLogARB)(glHandleARB obj, int maxLength, int *length, char *infoLog);
extern void (__stdcall * glGetAttachedObjectsARB)(glHandleARB containerObj, int maxCount, int *count, glHandleARB *obj);
extern int (__stdcall * glGetUniformLocationARB)(glHandleARB programObj, const char *name);
extern void (__stdcall * glGetActiveUniformARB)(glHandleARB programObj, unsigned int index, int maxLength, int *length, int *size, int *type, char *name);
extern void (__stdcall * glGetUniformfvARB)(glHandleARB programObj, int location, float *params);
extern void (__stdcall * glGetUniformivARB)(glHandleARB programObj, int location, int *params);
extern void (__stdcall * glGetShaderSourceARB)(glHandleARB obj, int maxLength, int *length, char *source);

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

extern void (__stdcall * glBindAttribLocationARB)(glHandleARB programObj, unsigned int index, const char *name);
extern void (__stdcall * glGetActiveAttribARB)(glHandleARB programObj, unsigned int index, int maxLength, int *length, int *size, int *type, char *name);
extern int (__stdcall * glGetAttribLocationARB)(glHandleARB programObj, const char *name);

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

extern void (__stdcall * glBindBufferARB)(unsigned int target, unsigned int buffer);
extern void (__stdcall * glDeleteBuffersARB)(int n, const unsigned int *buffers);
extern void (__stdcall * glGenBuffersARB)(int n, unsigned int *buffers);
extern unsigned char (__stdcall * glIsBufferARB)(unsigned int buffer);
extern void (__stdcall * glBufferDataARB)(unsigned int target, intptr size, const void *data, unsigned int usage);
extern void (__stdcall * glBufferSubDataARB)(unsigned int target, intptr offset, sizeiptr size, const void *data);
extern void (__stdcall * glGetBufferSubDataARB)(unsigned int target, intptr offset, sizeiptr size, void *data);
extern void * (__stdcall * glMapBufferARB)(unsigned int target, unsigned int access);
extern unsigned char (__stdcall * glUnmapBufferARB)(unsigned int target);
extern void (__stdcall * glGetBufferParameterivARB)(unsigned int target, unsigned int pname, int *params);
extern void (__stdcall * glGetBufferPointervARB)(unsigned int target, unsigned int pname, void **params);

// opengl debug out defines
enum
{
   DEBUG_OUTPUT_SYNCHRONOUS_ARB =      0x8242,

   MAX_DEBUG_MESSAGE_LENGTH_ARB =            0x9143,
   MAX_DEBUG_LOGGED_MESSAGES_ARB =           0x9144,
   DEBUG_LOGGED_MESSAGES_ARB =               0x9145,
   DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB =    0x8243,

   DEBUG_CALLBACK_FUNCTION_ARB =       0x8244,
   DEBUG_CALLBACK_USER_PARAM_ARB =     0x8245,

   DEBUG_SOURCE_API_ARB =              0x8246,
   DEBUG_SOURCE_WINDOW_SYSTEM_ARB =    0x8247,
   DEBUG_SOURCE_SHADER_COMPILER_ARB =  0x8248,
   DEBUG_SOURCE_THIRD_PARTY_ARB =      0x8249,
   DEBUG_SOURCE_APPLICATION_ARB =      0x824A,
   DEBUG_SOURCE_OTHER_ARB =            0x824B,

   DEBUG_TYPE_ERROR_ARB =                 0x824C,
   DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB =   0x824D,
   DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB =    0x824E,
   DEBUG_TYPE_PORTABILITY_ARB =           0x824F,
   DEBUG_TYPE_PERFORMANCE_ARB =           0x8250,
   DEBUG_TYPE_OTHER_ARB =                 0x8251,

   DEBUG_SEVERITY_HIGH_ARB =     0x9146,
   DEBUG_SEVERITY_MEDIUM_ARB =   0x9147,
   DEBUG_SEVERITY_LOW_ARB =      0x9148
};

typedef void (__stdcall * GLDEBUGPROCARB)(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char * message, void * userParam);

extern void (__stdcall * glDebugMessageControlARB)(unsigned int source, unsigned int type, unsigned int severity, int count, const unsigned int * ids, unsigned char enabled);
extern void (__stdcall * glDebugMessageInsertARB)(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char * buf);
extern void (__stdcall * glDebugMessageCallbackARB)(GLDEBUGPROCARB callback, void * userParam);
extern unsigned int (__stdcall * glGetDebugMessageLogARB)(unsigned int count, int bufsize, unsigned int * sources, unsigned int * types, unsigned int * ids, unsigned int * severities, int * lengths, char * messageLog);
extern void (__stdcall * glGetPointerv)(unsigned int pname, void ** params);

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

}; // namespace OpenGLExt

#endif // _OPENGL_EXTENSIONS_H_