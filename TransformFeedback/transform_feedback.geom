// defines the glsl version to be used
#version 400 core

// define the input and output topology
// the input topology must match the output topology
// for transform feedback operations to work correctly
layout (points) in;
layout (points, max_vertices = 512) out;

// defines the per geometry attributes
uniform mat4 model_view;
uniform mat4 model_view_proj_mat;
uniform vec3 control_points[4];

vec3 quadratic_bezier_curve( const vec3 pt1, const vec3 pt2, const vec3 pt3, const float t )
{
   vec3 pt4 = mix(pt1, pt2, t);
   vec3 pt5 = mix(pt2, pt3, t);

   return mix(pt4, pt5, t);
}

vec3 cubic_bezier_curve( const vec3 pt1, const vec3 pt2, const vec3 pt3, const vec3 pt4, const float t )
{
   vec3 pt5 = mix(pt1, pt2, t);
   vec3 pt6 = mix(pt2, pt3, t);
   vec3 pt7 = mix(pt3, pt4, t);

   return quadratic_bezier_curve(pt5, pt6, pt7, t);
}

void main( )
{
   for (float i = 0.0f; i <= 1.0f; i += (1.0f / 512.0f))
   {
      gl_Position = vec4(cubic_bezier_curve(control_points[0], control_points[1], control_points[2], control_points[3], i), 1.0f);
      EmitVertex();
   }

   // no longer emitting vertices
   EndPrimitive();
}
