// defines the version of glsl to be used by the shader
#version 400 compatibility

// defines the incoming vertex attributes
//layout (location = 0) in vec3 vertex;
//layout (location = 1) in vec3 color;

// defines the per geometry attributes
uniform mat4 mvp_mat4;

void main( )
{
   // pass the texture coordinate off to the frag shader
   gl_TexCoord[0] = gl_MultiTexCoord0;

   // just use the exact coordinates defined
   gl_Position = mvp_mat4 * gl_Vertex;
}
