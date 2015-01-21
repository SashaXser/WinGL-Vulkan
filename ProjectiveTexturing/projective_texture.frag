// defines the version of glsl to be used by the shader
#version 400 compatibility

// defines the per geometry attributes
uniform sampler2D logo_texture;
uniform sampler2D depth_texture;
uniform float depth_bias;

// defines the attributes passed though the shader pipeline
smooth in vec3 frag_color;
smooth in vec4 frag_logo_tex_coord;

// defines the location of where the color should go
layout (location = 0) out vec4 frag_color_dest_0;

void main( )
{
   // convert from homogeneous coordinates to real texture coordinates
   vec2 frag_logo_tex_coord_real = frag_logo_tex_coord.st / frag_logo_tex_coord.q;

   // obtain the texture color and depth value as seen by the light
   vec4 color = texture(logo_texture, frag_logo_tex_coord_real);
   vec4 depth = texture(depth_texture, frag_logo_tex_coord_real);

   // obtain the depth value of the homogenous vertex position
   // this has already been calculated by the farg logo texture coordinate
   // just need to put the coordinate in the same NDC space as the depth value
   float frag_depth = frag_logo_tex_coord.p / frag_logo_tex_coord.q;

   // make sure the coordinate is clamped to [0, 1]
   // a q component less than 0 means that the projection is behind the camera
   // also make sure that this fragment is not being occluded by something
   if (frag_logo_tex_coord_real.s < 0.0f || frag_logo_tex_coord_real.s > 1.0f ||
       frag_logo_tex_coord_real.t < 0.0f || frag_logo_tex_coord_real.t > 1.0f ||
       frag_logo_tex_coord.q < 0.0f ||
       depth.z < frag_depth - depth_bias)
   {
      // just use the fragments assigned color
      color = vec4(frag_color, 1.0f) * 0.15f;
   }
   else
   {
      // peform the decal operation
      color.rgb = frag_color.rgb * (1.0f - color.a) + color.rgb * color.a;
   }

   // assign the color to the destination output
   frag_color_dest_0 = color;
}
