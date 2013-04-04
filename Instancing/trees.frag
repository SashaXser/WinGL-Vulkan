#version 410

layout (location = 0) out vec4 FragColor;

layout (location = 0) uniform sampler2D TreeTex;

in vec2 TexCoord;

void main()
{
  FragColor = texture(TreeTex, TexCoord);

  uvec3 clr = uvec3(FragColor.r * 255, FragColor.g * 255, FragColor.b * 255);

  if (0 <= clr.r && clr.r <= 6 &&
      27 <= clr.g && clr.g <= 43 &&
      76 <= clr.b && clr.b <= 90)
  {
    discard;
  }
}
