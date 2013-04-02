#version 420

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord0;
layout (location = 2) in mat4 WorldMat;

uniform mat4 ProjViewMat;

out vec2 TexCoord;

void main()
{
  TexCoord = TexCoord0;
  gl_Position = ProjViewMat * WorldMat * vec4(Position, 1.0);
}
