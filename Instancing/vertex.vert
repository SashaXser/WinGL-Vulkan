#version 420

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

uniform mat4 gPVW;

out vec2 TexCoord0;

void main()
{
  TexCoord0 = TexCoord;
  gl_Position = gPVW * vec4(Position, 1.0);
}
