// defines the version of glsl to be used by the shader
#version 400 compatibility

// defines the per geometry attributes
uniform float depth_near_far[2];
uniform sampler2D depth_texture;

// defines the location of where the color should go
layout (location = 0) out vec4 frag_color_dest_0;

float LinearizeDepth( in vec2 uv, float near, float far )
{
    float depth = texture2D(depth_texture, uv).x;

    return (2.0f * near) / (far + near - depth * (far - near));
}

void main( )
{
   // obtain a color that removes the exponential nature of depth
   float ld = LinearizeDepth(gl_TexCoord[0].xy,
                             depth_near_far[0],
                             depth_near_far[1]);

   // assign the color to the destination output
   frag_color_dest_0 = vec4(ld, ld, ld, 1.0f);
}
