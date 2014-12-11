// local includes
#include "GeomHelper.h"
#include "Vector3.h"
#include "WglAssert.h"

// std includes
#include <iterator>

namespace GeomHelper
{

// helper function to generate the normals
void ConstructNormals( Shape & shape )
{
   // resize the normals
   shape.normals.resize(shape.vertices.size(), Vec3f(0.0f, 0.0f, 0.0f));

   // construct the normals for all the indices
   auto index_beg = shape.indices.cbegin();
   const auto index_end = shape.indices.cend();

   for (; index_beg < index_end; index_beg += 3)
   {
      // get the first three indices
      const GLuint i0 = *(index_beg + 0);
      const GLuint i1 = *(index_beg + 1);
      const GLuint i2 = *(index_beg + 2);

      // construct the edge vectors
      const Vec3f e1 = shape.vertices[i1] - shape.vertices[i0];
      const Vec3f e2 = shape.vertices[i2] - shape.vertices[i0];

      // construct the normal
      const Vec3f n = (e1 ^ e2).UnitVector();

      // add the normal to the normal vector
      shape.normals[i0] += n;
      shape.normals[i1] += n;
      shape.normals[i2] += n;
   }

   // normalize all the normals
   auto normal_beg = shape.normals.begin();
   const auto normal_end = shape.normals.end();

   for (; normal_beg != normal_end; ++normal_beg)
   {
      normal_beg->Normalize();
   }
}

// helper function to generate the tangents and bitangents
void ConstructTangentsAndBitangents( Shape & shape )
{
   // there needs to be vertices and texture coords
   WGL_ASSERT(!shape.vertices.empty());
   WGL_ASSERT(!shape.tex_coords.empty());
   // the number of verts must match the number of tex coords
   WGL_ASSERT(shape.vertices.size() == shape.tex_coords.size() / 2);

   // resize the tangents
   shape.tangents.resize(shape.vertices.size(), Vec3f(0.0f, 0.0f, 0.0f));
   shape.bitangents.resize(shape.vertices.size(), Vec3f(0.0f, 0.0f, 0.0f));

   // construct the tangents for all the indices
   auto index_beg = shape.indices.cbegin();
   const auto index_end = shape.indices.cend();

   for (; index_beg < index_end; index_beg += 3)
   {
      // just need to calculate the tangent relative to the surface of the face
      //      1      |  t2    -t1 | | P1x   P1y   P1z |   | T | << Tangent
      // ----------- |            | |                 | = |   |
      // s1t1 - s2t2 | -s2     s1 | | P2x   P2y   P2z |   | B | << Bitangent
      //
      //          1      /                                                   \
      // T = ----------- | t2 P1x - t1 P2x, t2 P1y - t1 P2y, t2 P1z - t1 P2z |
      //     s1t1 - s2t2 \                                                   /

      // get the first three indices
      const GLuint i0 = *(index_beg + 0);
      const GLuint i1 = *(index_beg + 1);
      const GLuint i2 = *(index_beg + 2);

      // construct the edge vectors
      const Vec3f e1 = shape.vertices[i1] - shape.vertices[i0];
      const Vec3f e2 = shape.vertices[i2] - shape.vertices[i0];

      // get the edge texture coordinates
      const float u1 = shape.tex_coords[i1 * 2] - shape.tex_coords[i0 * 2];
      const float v1 = shape.tex_coords[i1 * 2 + 1] - shape.tex_coords[i0 * 2 + 1];
      const float u2 = shape.tex_coords[i2 * 2] - shape.tex_coords[i0 * 2];
      const float v2 = shape.tex_coords[i2 * 2 + 1] - shape.tex_coords[i0 * 2 + 1];

      // calculate the determinat
      const float dividend = v1 * v2 - u2 - v1;
      const float det = dividend == 0.0f ? 1.0f : 1.0f / dividend;

      // calculate the tangent
      const Vec3f t(det * (v2 * e1.X() - v1 * e2.X()),
                    det * (v2 * e1.Y() - v1 * e2.Y()),
                    det * (v2 * e1.Z() - v1 * e2.Z()));

      // add the tangent to the tangent vector
      shape.tangents[i0] += t;
      shape.tangents[i1] += t;
      shape.tangents[i2] += t;
   }

   // normalize all the tangents and perform gram-schmidt to orthogonalize the tangent
   auto tangent_beg = shape.tangents.begin();
   const auto tangent_end = shape.tangents.end();

   for (; tangent_beg != tangent_end; ++tangent_beg)
   {
      // normalize the tangent
      tangent_beg->Normalize();

      // get the normal
      const auto index_n = std::distance(shape.tangents.begin(), tangent_beg);
      const Vec3f & n = shape.normals[index_n];

      // the normal should already be normalized
      WGL_ASSERT(n.Length() == 1.0f);

      // perform grahm-schmidt on the normal and tangent,
      // as the tangent may not be orthogonal to the normal
      // T' = T - (N * T) * N
      *tangent_beg = *tangent_beg - n * (n * *tangent_beg);

      // the tangent should already be normalized
      WGL_ASSERT(tangent_beg->Length() == 1.0f);

      // bitangent is just n cross t
      shape.bitangents[index_n] = n ^ *tangent_beg;

      // the bitangent should be a unit vector
      WGL_ASSERT(shape.bitangents[index_n].Length() == 1.0f);
   }
}

Shape ConstructPlane( const float width, const float height )
{
   Shape shape;

   // setup the type
   shape.geom_type = GL_TRIANGLES;

   // get half the width and height
   const float half_width = width * 0.5f;
   const float half_height = height * 0.5f;

   // construct all the vertices
   shape.vertices.push_back(Vec3f(-half_width, 0.0f,  half_height));
   shape.vertices.push_back(Vec3f( half_width, 0.0f,  half_height));
   shape.vertices.push_back(Vec3f( half_width, 0.0f, -half_height));
   shape.vertices.push_back(Vec3f(-half_width, 0.0f, -half_height));

   // construct the indices
   shape.indices = std::vector< GLuint > { 0, 2, 3, 2, 0, 1 };

   // construct the texture coords
   shape.tex_coords = std::vector< float > { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };

   // construct the normals
   ConstructNormals(shape);

   // construct the tangents and bitangents
   ConstructTangentsAndBitangents(shape);

   return shape;
}

Shape ConstructBox( const float width, const float height, const float depth )
{
   Shape shape;

   return shape;
}

} // namespace GeomHelper
