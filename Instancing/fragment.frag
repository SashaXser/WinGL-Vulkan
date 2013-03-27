#version 420

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D BuildingTex;

in vec2 TexCoord0;

void main()
{
  FragColor = texture(BuildingTex, TexCoord0);
}
