#include "component_impl.h"

#include "Matrix.h"
#include "GeomHelper.h"

#include <gl/GL.h>

namespace glsc
{

void ComponentImpl::Draw( )
{
   float viewport[4];
   glGetFloatv(GL_VIEWPORT, viewport);

   const float vp_width = viewport[2];
   const float vp_height = viewport[3];

   if (vp_width >= vp_height)
   {
      float middle_x = vp_width / 2.0f;
      float start_x = middle_x - vp_height / 2.0f;
      glViewport(start_x, 0, vp_height, vp_height);
   }
   else
   {
      float middle_y = vp_height / 2.0f;
      float start_y = middle_y - vp_width / 2.0f;
      glViewport(0, start_y, vp_width, vp_width);
   }

   Matrixf projection;
   projection.MakeOrtho(-200.0f, 200.0f, -200.0f, 200.0f, 100.0f, -100.0f);

   Matrixf view;
   view.MakeLookAt(0.0f, 10.0f, 0.0f,
                   0.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, -1.0f);

   _basic_shader.Enable();

   _basic_shader.SetUniformValue("shape_color", Vec3f(1.0f, 0.0f, 0.0f));

   _basic_plane_verts.Bind();
   _basic_plane_verts.VertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
   
   _basic_plane_tex_coords.Bind();
   _basic_plane_tex_coords.VertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
   
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);

   _basic_plane_indices.Bind();

   Matrixf model;
   model.MakeTranslation(-100.0f, 0.0f, -100.0f);
   _basic_shader.SetUniformMatrix< 1, 4, 4 >("model_view_proj", projection * view * model);

   _texture1.Bind(GL_TEXTURE0);
   _basic_shader.SetUniformValue("image", static_cast< GLint >(_texture1.GetBoundSamplerID()));

   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

   _texture1.Unbind();

   model.MakeTranslation(100.0f, 0.0f, -100.0f);
   _basic_shader.SetUniformMatrix< 1, 4, 4 >("model_view_proj", projection * view * model);

   _texture2.Bind(GL_TEXTURE0);
   _basic_shader.SetUniformValue("image", static_cast< GLint >(_texture2.GetBoundSamplerID()));

   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

   _texture2.Unbind();

   model.MakeTranslation(100.0f, 0.0f, 100.0f);
   _basic_shader.SetUniformMatrix< 1, 4, 4 >("model_view_proj", projection * view * model);

   _texture3.Bind(GL_TEXTURE0);
   _basic_shader.SetUniformValue("image", static_cast< GLint >(_texture3.GetBoundSamplerID()));

   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

   _texture3.Unbind();

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);

   _basic_plane_indices.Unbind();
   _basic_plane_tex_coords.Unbind();
   _basic_plane_verts.Unbind();

   _basic_shader.Disable();

   glViewport(viewport[0], viewport[1],
              viewport[2], viewport[3]);
}

void ComponentImpl::Initialize( )
{
   _texture1.Load2D("glstudio-component-textures/bricks_diffuse.jpg", GL_RGBA, GL_RGBA8, false);
   _texture2.Load2D("glstudio-component-textures/cobblestone_diffuse.png", GL_RGBA, GL_RGBA8, false);
   _texture3.Load2D("glstudio-component-textures/lion_diffuse.png", GL_RGBA, GL_RGBA8, true);

   _basic_shader.Attach(GL_VERTEX_SHADER,
                        "#version 400\n"
                        "layout (location = 0) in vec3 vertex_position;\n"
                        "layout (location = 1) in vec2 texture_coords;\n"
                        "uniform mat4 model_view_proj;\n"
                        "smooth out vec2 texture_coords_frag;\n"
                        "\n"
                        "void main( )\n"
                        "{\n"
                        "   texture_coords_frag = texture_coords.st;\n"
                        "   gl_Position = model_view_proj * vec4(vertex_position, 1.0f);\n"
                        "}\n");
   _basic_shader.Attach(GL_FRAGMENT_SHADER,
                        "#version 400\n"
                        "layout (location = 0) out vec4 frag_color_dest_0;\n"
                        "uniform sampler2D image;\n"
                        "smooth in vec2 texture_coords_frag;\n"
                        "\n"
                        "void main( )\n"
                        "{\n"
                        "   frag_color_dest_0 = texture(image, texture_coords_frag);\n"
                        "}\n");

   _basic_shader.Link();

   const GeomHelper::Shape square = GeomHelper::ConstructPlane(128.0, 128.0);

   _basic_plane_verts.GenBuffer(GL_ARRAY_BUFFER);
   _basic_plane_verts.Bind();
   _basic_plane_verts.BufferData(square.vertices.size() * sizeof(Vec3f), square.vertices.at(0), GL_STATIC_DRAW);
   _basic_plane_verts.Unbind();

   _basic_plane_indices.GenBuffer(GL_ELEMENT_ARRAY_BUFFER);
   _basic_plane_indices.Bind();
   _basic_plane_indices.BufferData(square.indices.size() * sizeof(GLuint), &square.indices.at(0), GL_STATIC_DRAW);
   _basic_plane_indices.Unbind();

   _basic_plane_tex_coords.GenBuffer(GL_ARRAY_BUFFER);
   _basic_plane_tex_coords.Bind();
   _basic_plane_tex_coords.BufferData(square.tex_coords.size() * sizeof(Vec2f), &square.tex_coords.at(0), GL_STATIC_DRAW);
   _basic_plane_tex_coords.Unbind();
}

Component * CreateComponent( )
{
   ComponentImpl * component = new ComponentImpl;

   component->Initialize();

   return component;
}

} // namespace glsc
