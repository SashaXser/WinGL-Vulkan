// defines the version of glsl to be used by the shader
#version 400 compatibility

// defines the incoming vertex attributes
//layout (location = 0) in vec3 vertex;
//layout (location = 1) in vec3 color;

// defines the per geometry attributes
uniform mat4 mvp_mat4;
uniform mat4 light_mvp_mat4;

// defines the attributes passed though the shader pipeline
smooth out vec3 frag_color;
smooth out vec4 frag_logo_tex_coord;

void main( )
{
   // allow the color to just pass through
   //frag_color = color;
   frag_color = gl_Color.rgb;

   // calculate the clip space vertex coordinate
   //gl_Position = mvp_mat4 * vec4(vertex, 1.0f);
   vec3 vertex = gl_Vertex.xyz; gl_Position = mvp_mat4 * gl_Vertex;

   // calculate the clip space vertex coord (homogenous space)
   // for the texture lookup of the logo
   frag_logo_tex_coord = mat4(0.5f, 0.0f, 0.0f, 0.0f,
                              0.0f, 0.5f, 0.0f, 0.0f,
                              0.0f, 0.0f, 0.5f, 0.0f,
                              0.5f, 0.5f, 0.5f, 1.0f) * light_mvp_mat4 * vec4(vertex, 1.0f);
}
