#version 330

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

uniform vec3 CameraPos;
uniform mat4 ProjViewMat;
uniform vec2 TexCoords[4];
uniform vec2 Size;

out vec2 TexCoord;

void main()
{
  vec3 pos = gl_in[0].gl_Position.xyz;

  vec3 to = normalize(CameraPos - pos);
  vec3 up = vec3(0.0f, 1.0f, 0.0f);
  vec3 right = cross(to, up);
  
  pos -= right * Size.x * 0.5f;
  gl_Position = ProjViewMat * vec4(pos, 1.0f);
  TexCoord = TexCoords[1];
  EmitVertex();

  pos.y += Size.y;
  gl_Position = ProjViewMat * vec4(pos, 1.0f);
  TexCoord = TexCoords[2];
  EmitVertex();

  pos.y -= Size.y;
  pos += right * Size.x;
  gl_Position = ProjViewMat * vec4(pos, 1.0f);
  TexCoord = TexCoords[0];
  EmitVertex();

  pos.y += Size.y;
  gl_Position = ProjViewMat * vec4(pos, 1.0f);
  TexCoord = TexCoords[3];
  EmitVertex();

  EndPrimitive();
}
