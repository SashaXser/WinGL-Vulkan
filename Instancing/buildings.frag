#version 410

layout (location = 0) out vec4 FragColor;

uniform sampler2D BuildingTex;

in vec2 TexCoord;

void main()
{
  FragColor = texture(BuildingTex, TexCoord);
}
