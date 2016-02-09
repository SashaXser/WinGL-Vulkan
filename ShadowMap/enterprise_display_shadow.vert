// defines the glsl version to be used
#version 440 core

// defines the per geometry attributes
uniform mat4 model_view_proj_mat;
uniform sampler2D shadow_texture;
uniform uint window_width;
uniform uint window_height;

// defines the attributes passed along through the shader pipeline
smooth out vec2 frag_tex_coords;

vec2 calculate_shadow_verts( const float vp_width, const float vp_height )
{
   // obtain the size of the texture
   const ivec2 shadow_size = textureSize(shadow_texture, 0);

   // determine the new relative size keeping aspect of the texture
   vec2 shadow_vertex_size;
   if (window_width > window_height)
   {
      shadow_vertex_size.y = vp_height * 0.25f;
      const float pixels_height = shadow_vertex_size.y * window_height;
      shadow_vertex_size.x = (shadow_size.x / shadow_size.y * pixels_height) / window_width * vp_width;
   }
   else
   {
      shadow_vertex_size.x = vp_width * 0.25f;
      const float pixels_width = shadow_vertex_size.x * window_width;
      shadow_vertex_size.y = (shadow_size.y / shadow_size.x * pixels_width) / window_height * vp_height;
   }

   return shadow_vertex_size;
}

void main( )
{
   // determine the bounds of the viewing volume
   const mat4 inverse_model_view_proj_mat = inverse(model_view_proj_mat);
   const float vp_left = (inverse_model_view_proj_mat * vec4(-1.0f, 0.0f, 0.0f, 1.0f)).x;
   const float vp_right = (inverse_model_view_proj_mat * vec4(1.0f, 0.0f, 0.0f, 1.0f)).x;
   const float vp_top = (inverse_model_view_proj_mat * vec4(0.0f, 1.0f, 0.0f, 1.0f)).y;
   const float vp_bottom = (inverse_model_view_proj_mat * vec4(0.0f, -1.0f, 0.0f, 1.0f)).y;
   const float vp_width = vp_right - vp_left;
   const float vp_height = vp_top - vp_bottom;

   // determine the vertex size for the shadow area
   const vec2 shadow_size = calculate_shadow_verts(vp_width, vp_height);

   // calculate the vertices to be 25% relative screen space
   const vec4 vertices[6] =
   {
      vec4(vp_left, vp_bottom, 0.0f, 1.0f), vec4(shadow_size.x, shadow_size.y, 0.0f, 1.0f), vec4(vp_left, shadow_size.y, 0.0f, 1.0f),
      vec4(vp_left, vp_bottom, 0.0f, 1.0f), vec4(shadow_size.x, vp_bottom, 0.0f, 1.0f), vec4(shadow_size.x, shadow_size.y, 0.0f, 1.0f)
   };

   // calculate the texture coordinates to be used to display the shadow map
   const vec2 tex_coords[6] =
   {
      vec2(0.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 1.0f),
      vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f)
   };

   // allow the position to just pass through
   gl_Position = model_view_proj_mat * vertices[gl_VertexID];

   // just copy over the texture coords to be interp in the frag shader
   frag_tex_coords = tex_coords[gl_VertexID];
}
