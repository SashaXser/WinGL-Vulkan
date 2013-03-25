#ifndef _SHADERS_H_
#define _SHADERS_H_

const char * const VERT_SHADER_CODE =
"layout (location = 0) in vec3 Position;"
""
"uniform mat4 gPVW;"
""
"void main()"
"{"
"  gl_Position = gPVW * vec4(Position, 1.0);"
"};"
;

const char * const FRAG_SHADER_CODE =
"out vec4 FragColor;"
""
"uniform sampler2D gSampler;"
""
"void main()"
"{"
"  FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f)"
"};"
;

#endif // _SHADERS_H_
