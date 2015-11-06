// defines the glsl version to be used
#version 400 compatibility

// defines the per vertex attributes
layout (location = 0) in vec4 vertex_position;

// defines the per geometry attributes
uniform mat4 model_view_proj_mat;

// indicates the current control point id
// that the fragment shader will use...
flat out uint control_point_id;

void main( )
{
   // use the incoming vertex id as the control point id
   control_point_id = gl_VertexID;

   // make the points larger so users can grab them
   gl_PointSize = 6.0f;

   // calculate the new position of the vertex
   gl_Position = model_view_proj_mat * vertex_position;
}