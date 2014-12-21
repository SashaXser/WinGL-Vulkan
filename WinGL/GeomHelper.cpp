// local includes
#include "GeomHelper.h"
//#include "Matrix.h"
#include "Vector4.h"
#include "WglAssert.h"
#include "MathHelper.h"
//#include "Quaternion.h"

// std includes
#include <cmath>
#include <limits>
#include <iterator>

namespace GeomHelper
{

template < typename T >
std::vector< Vector3< T > > ConstructNormals( const std::vector< Vector3< T > > & vertices,
                                              const std::vector< GLuint > & indices )
{
   // resize the normals
   std::vector< Vector3< T > > normals(vertices.size(), Vector3< T >(0, 0, 0));

   // construct the normals for all the indices
   auto index_beg = indices.cbegin();
   const auto index_end = indices.cend();

   for (; index_beg < index_end; index_beg += 3)
   {
      // get the first three indices
      const GLuint i0 = *(index_beg + 0);
      const GLuint i1 = *(index_beg + 1);
      const GLuint i2 = *(index_beg + 2);

      // construct the edge vectors
      const Vector3< T > e1 = vertices[i1] - vertices[i0];
      const Vector3< T > e2 = vertices[i2] - vertices[i0];

      // construct the normal
      const Vector3< T > n = (e1 ^ e2).UnitVector();

      // add the normal to the normal vector
      normals[i0] += n;
      normals[i1] += n;
      normals[i2] += n;
   }

   // normalize all the normals
   auto normal_beg = normals.begin();
   const auto normal_end = normals.end();

   for (; normal_beg != normal_end; ++normal_beg)
   {
      normal_beg->Normalize();
   }

   return normals;
}

template < typename T >
std::vector< Vector3< T > > ConstructNormals( const std::vector< T > & vertices,
                                              const std::vector< GLuint > & indices )
{
   // there should be 3 components per point
   WGL_ASSERT(vertices.size() % 3 == 0);

   return ConstructNormals(std::vector< Vector3< T > >(reinterpret_cast< const Vector3< T > * >(&(vertices[0])),
                                                       reinterpret_cast< const Vector3< T > * >(&(vertices[0]) + vertices.size())),
                           indices);
}

// floats and doubles are allowed, nothing else
template std::vector< Vec3f > ConstructNormals< float >( const std::vector< Vec3f > &, const std::vector< GLuint > & );
template std::vector< Vec3d > ConstructNormals< double >( const std::vector< Vec3d > &, const std::vector< GLuint > & );
template std::vector< Vec3f > ConstructNormals< float >( const std::vector< float > &, const std::vector< GLuint > & );
template std::vector< Vec3d > ConstructNormals< double >( const std::vector< double > &, const std::vector< GLuint > & );

// helper function to generate the normals
void ConstructNormals( Shape & shape )
{
   shape.normals = ConstructNormals(shape.vertices, shape.indices);
}

template < typename T >
std::pair< std::vector< Vector3< T > >, std::vector< Vector3< T > > >
ConstructTangentsAndBitangents( const std::vector< Vector3< T > > & vertices,
                                const std::vector< Vector3< T > > & normals,
                                const std::vector< T > & tex_coords,
                                const std::vector< GLuint > & indices )
{
   // there needs to be vertices and texture coords
   WGL_ASSERT(!vertices.empty());
   WGL_ASSERT(!tex_coords.empty());
   // the number of verts must match the number of tex coords
   WGL_ASSERT(vertices.size() == tex_coords.size() / 2);

   // resize the tangents
   std::pair< std::vector< Vector3< T > >, std::vector< Vector3< T > > > tangents_bitangents;
   tangents_bitangents.first.resize(vertices.size(), Vector3< T >(0, 0, 0));
   tangents_bitangents.second.resize(vertices.size(), Vector3< T >(0, 0, 0));

   // construct the tangents for all the indices
   auto index_beg = indices.cbegin();
   const auto index_end = indices.cend();

   for (; index_beg < index_end; index_beg += 3)
   {
      // just need to calculate the tangent relative to the surface of the face
      //      1      |  t2    -t1 | | P1x   P1y   P1z |   | T | << Tangent
      // ----------- |            | |                 | = |   |
      // s1t2 - s2t1 | -s2     s1 | | P2x   P2y   P2z |   | B | << Bitangent
      //
      //          1      /                                                   \
      // T = ----------- | t2 P1x - t1 P2x, t2 P1y - t1 P2y, t2 P1z - t1 P2z |
      //     s1t2 - s2t1 \                                                   /
      //
      //          1      /                                                   \
      // B = ----------- | s1 P2x - s2 P1x, s1 P2y - s2 P1y, s1 P2z - s2 P1z |
      //     s1t2 - s2t1 \                                                   /

      // get the first three indices
      const GLuint i0 = *(index_beg + 0);
      const GLuint i1 = *(index_beg + 1);
      const GLuint i2 = *(index_beg + 2);

      // construct the edge vectors
      const Vector3< T > e1 = vertices[i1] - vertices[i0];
      const Vector3< T > e2 = vertices[i2] - vertices[i0];

      // get the edge texture coordinates
      const T s1 = tex_coords[i1 * 2] - tex_coords[i0 * 2];
      const T t1 = tex_coords[i1 * 2 + 1] - tex_coords[i0 * 2 + 1];
      const T s2 = tex_coords[i2 * 2] - tex_coords[i0 * 2];
      const T t2 = tex_coords[i2 * 2 + 1] - tex_coords[i0 * 2 + 1];

      // calculate the determinat
      const T dividend = s1 * t2 - s2 * t1;
      const T det = dividend == 0 ? 1 : 1 / dividend;

      // calculate the tangent
      Vector3< T > t(det * (t2 * e1.X() - t1 * e2.X()),
                     det * (t2 * e1.Y() - t1 * e2.Y()),
                     det * (t2 * e1.Z() - t1 * e2.Z()));

      if (t.Length() == 0)
      {
         // use one of the edges to define the tangent vector
         if (e1.Length() != 0)
         {
            t = e1.UnitVector();
         }
         else if (e2.Length() != 0)
         {
            t = e2.UnitVector();
         }
         else
         {
            // todo: figure out what this case needs to be
            WGL_ASSERT(false);

            //// the tangent could not be calculated so take the three normals,
            //// average them, and normalize and use that as the tangent...
            //const Vector3< T > & n0 = normals[i0];
            //const Vector3< T > & n1 = normals[i1];
            //const Vector3< T > & n2 = normals[i2];

            //// average the normals
            //const Vector3< T > n_avg = (n0 + n1 + n2) * (T(1) / T(3));

            //// take a vertex and create 

            //// do the averaging of these vectors and rotate along the z-axis
            //// such that a normal of 0,1,0 will have a tangent of 1,0,0...
            //t = Matrix< T >::Rotate(T(-90), Vector3< T >(0, 0, 1)) * Vector4< T >(((n0 + n1 + n2) * (T(1) / T(3))), 0);
         }

         // issue a message to the console
         WGL_ASSERT_REPORT(false, "Creating tangent vector since tangent calculation produced zero length tangent vector");

         //const Vector3< T > b(det * (s1 * e2.X() - s2 * e1.X()),
         //                     det * (s1 * e2.Y() - s2 * e1.Y()),
         //                     det * (s1 * e2.Z() - s2 * e1.Z()));
         //
         //// the bitangent better have a length
         //WGL_ASSERT(b.Length() != 0);
         //
         //// the normals should already be normalized
         //WGL_ASSERT(MathHelper::Equals< T >(normals[*(index_beg + 0)].Length(), 1));
         //WGL_ASSERT(MathHelper::Equals< T >(normals[*(index_beg + 1)].Length(), 1));
         //WGL_ASSERT(MathHelper::Equals< T >(normals[*(index_beg + 2)].Length(), 1));
         //
         //// add the tangent to the tangent vector
         //tangents_bitangents.first[i0] += b ^ normals[*(index_beg + 0)];
         //tangents_bitangents.first[i1] += b ^ normals[*(index_beg + 1)];
         //tangents_bitangents.first[i2] += b ^ normals[*(index_beg + 2)];
      }

      // make sure the components are valid values
      WGL_ASSERT(!std::isnan(t.X()) && !std::isnan(t.Y()) && !std::isnan(t.Z()));

      // add the tangent to the tangent vector
      tangents_bitangents.first[i0] += t;
      tangents_bitangents.first[i1] += t;
      tangents_bitangents.first[i2] += t;
   }

   // normalize all the tangents and perform gram-schmidt to orthogonalize the tangent
   auto tangent_beg = tangents_bitangents.first.begin();
   const auto tangent_end = tangents_bitangents.first.end();

   for (; tangent_beg != tangent_end; ++tangent_beg)
   {
      // normalize the tangent
      tangent_beg->Normalize();

      // get the normal
      const auto index_n = std::distance(tangents_bitangents.first.begin(), tangent_beg);
      const Vector3< T > & n = normals[index_n];

      // the normal should already be normalized
      WGL_ASSERT(MathHelper::Equals< T >(n.Length(), 1));

      // perform grahm-schmidt on the normal and tangent,
      // as the tangent may not be orthogonal to the normal
      // T' = T - (N * T) * N
      *tangent_beg = (*tangent_beg - n * (n * *tangent_beg)).UnitVector();

      // the tangent should already be normalized
      WGL_ASSERT(MathHelper::Equals< T >(tangent_beg->Length(), 1, 2 * std::numeric_limits< T >::epsilon()));

      // bitangent is just n cross t
      tangents_bitangents.second[index_n] = (n ^ *tangent_beg).UnitVector();

      // the bitangent should be a unit vector
      WGL_ASSERT(MathHelper::Equals< T >(tangents_bitangents.second[index_n].Length(), 1));
   }

   return tangents_bitangents;
}

template < typename T >
std::pair< std::vector< Vector3< T > >, std::vector< Vector3< T > > >
ConstructTangentsAndBitangents( const std::vector< T > & vertices,
                                const std::vector< T > & normals,
                                const std::vector< T > & tex_coords,
                                const std::vector< GLuint > & indices )
{
   // there should be 3 components per point
   WGL_ASSERT(vertices.size() % 3 == 0);
   WGL_ASSERT(normals.size() % 3 == 0);
   // there should be 2 components per point
   WGL_ASSERT(tex_coords.size() % 2 == 0);

   return ConstructTangentsAndBitangents(std::vector< Vector3< T > >(reinterpret_cast< const Vector3< T > * >(&(vertices[0])),
                                                                     reinterpret_cast< const Vector3< T > * >(&(vertices[0]) + vertices.size())),
                                         std::vector< Vector3< T > >(reinterpret_cast< const Vector3< T > * >(&(normals[0])),
                                                                     reinterpret_cast< const Vector3< T > * >(&(normals[0]) + normals.size())),
                                         tex_coords,
                                         indices);
}

// floats and doubles are allowed, nothing else
template std::pair< std::vector< Vec3f >, std::vector< Vec3f > >
ConstructTangentsAndBitangents< float >( const std::vector< Vec3f > &, const std::vector< Vec3f > &,
                                         const std::vector< float > &, const std::vector< GLuint > & );
template std::pair< std::vector< Vec3d >, std::vector< Vec3d > >
ConstructTangentsAndBitangents< double >( const std::vector< Vec3d > &, const std::vector< Vec3d > &,
                                          const std::vector< double > &, const std::vector< GLuint > & );
template std::pair< std::vector< Vec3f >, std::vector< Vec3f > >
ConstructTangentsAndBitangents< float >( const std::vector< float > &, const std::vector< float > &,
                                         const std::vector< float > &, const std::vector< GLuint > & );
template std::pair< std::vector< Vec3d >, std::vector< Vec3d > >
ConstructTangentsAndBitangents< double >( const std::vector< double > &, const std::vector< double > &,
                                          const std::vector< double > &, const std::vector< GLuint > & );

// helper function to generate the tangents and bitangents
void ConstructTangentsAndBitangents( Shape & shape )
{
   auto tangents_bitangents = ConstructTangentsAndBitangents(shape.vertices, shape.normals, shape.tex_coords, shape.indices);

   shape.tangents = std::move(tangents_bitangents.first);
   shape.bitangents = std::move(tangents_bitangents.second);
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
