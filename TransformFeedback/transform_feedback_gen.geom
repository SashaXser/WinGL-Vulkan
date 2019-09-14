// defines the glsl version to be used
#version 400 core

// define the input and output topology
// the input topology must match the output topology
// for transform feedback operations to work correctly
layout (points) in;
layout (points, max_vertices = 256) out;

// defines the per geometry attributes
uniform mat4 model_view;
uniform mat4 model_view_proj_mat;

// todo: redo this as uniform block
uniform uint number_of_control_points;
uniform vec4 control_points[6];

// todo: add 5 and 6 point processing...

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

// usually splines are created from this point and onward...
vec3 quintic_bezier_curve( const vec3 pt1, const vec3 pt2, const vec3 pt3, const vec3 pt4, const vec3 pt5, const float t )
{
   vec3 pt6 = mix(pt1, pt2, t);
   vec3 pt7 = mix(pt2, pt3, t);
   vec3 pt8 = mix(pt3, pt4, t);
   vec3 pt9 = mix(pt4, pt5, t);

   return cubic_bezier_curve(pt6, pt7, pt8, pt9, t);
}

vec3 six_point_bezier_curve( const vec3 pt1, const vec3 pt2, const vec3 pt3, const vec3 pt4, const vec3 pt5, const vec3 pt6, const float t )
{
   vec3 pt7 = mix(pt1, pt2, t);
   vec3 pt8 = mix(pt2, pt3, t);
   vec3 pt9 = mix(pt3, pt4, t);
   vec3 pt10 = mix(pt4, pt5, t);
   vec3 pt11 = mix(pt5, pt6, t);

   return quintic_bezier_curve(pt7, pt8, pt9, pt10, pt11, t);
}

void main( )
{
   const float max_num_to_emit = 255.0f;

   if (number_of_control_points == 3)
   {
      for (float i = 0.0f; i <= 1.0f; i += (1.0f / max_num_to_emit))
      {
         gl_Position = vec4(quadratic_bezier_curve(control_points[0].xyz,
                                                   control_points[1].xyz,
                                                   control_points[2].xyz,
                                                   i),
                            1.0f);
         EmitVertex();
      }
   }
   else if (number_of_control_points == 4)
   {
      for (float i = 0.0f; i <= 1.0f; i += (1.0f / max_num_to_emit))
      {
         gl_Position = vec4(cubic_bezier_curve(control_points[0].xyz,
                                               control_points[1].xyz,
                                               control_points[2].xyz,
                                               control_points[3].xyz,
                                               i),
                            1.0f);
         EmitVertex();
      }
   }
   else if (number_of_control_points == 5)
   {
      for (float i = 0.0f; i <= 1.0f; i += (1.0f / max_num_to_emit))
      {
         gl_Position = vec4(quintic_bezier_curve(control_points[0].xyz,
                                                 control_points[1].xyz,
                                                 control_points[2].xyz,
                                                 control_points[3].xyz,
                                                 control_points[4].xyz,
                                                 i),
                            1.0f);
         EmitVertex();
      }
   }
   else if (number_of_control_points == 6)
   {
      for (float i = 0.0f; i <= 1.0f; i += (1.0f / max_num_to_emit))
      {
         gl_Position = vec4(six_point_bezier_curve(control_points[0].xyz,
                                                   control_points[1].xyz,
                                                   control_points[2].xyz,
                                                   control_points[3].xyz,
                                                   control_points[4].xyz,
                                                   control_points[5].xyz,
                                                   i),
                            1.0f);
         EmitVertex();
      }
   }

   // no longer emitting vertices
   EndPrimitive();
}
