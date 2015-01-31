#ifndef _GEOM_HELPER_H_
#define _GEOM_HELPER_H_

// local includes
#include "Vector.h"

// gl includes
#include <GL/glew.h>
#include <GL/GL.h>

// std includes
#include <vector>
#include <utility>
#include <cstdint>

namespace GeomHelper
{

// structure that contains all the required attributes of a shape
// if indices is empty, then glDrawArrays is used.
struct Shape
{
   Shape( ) { }
   Shape( Shape && shape )
   {
      geom_type = shape.geom_type;
      vertices.swap(shape.vertices);
      indices.swap(shape.indices);
      tex_coords.swap(shape.tex_coords);
      normals.swap(shape.normals);
      tangents.swap(shape.tangents);
      bitangents.swap(shape.bitangents);
   }

   GLenum                  geom_type;
   std::vector< Vec3f >    vertices;
   std::vector< GLuint >   indices;
   std::vector< Vec2f >    tex_coords;
   std::vector< Vec3f >    normals;
   std::vector< Vec3f >    tangents;
   std::vector< Vec3f >    bitangents;
};

// construct the normals
// assumes indicies align to make triangles
template < typename T >
std::vector< Vector< T, 3 > > ConstructNormals( const std::vector< Vector< T, 3 > > & vertices,
                                                const std::vector< GLuint > & indices );
template < typename T >
std::vector< Vector< T, 3 > > ConstructNormals( const std::vector< T > & vertices,
                                                const std::vector< GLuint > & indices );

// construct the tangents and bitangents
// assumes indicies align to make triangles
// first = tangents, second = bitangents
template < typename T >
std::pair< std::vector< Vector< T, 3 > >, std::vector< Vector< T, 3 > > >
ConstructTangentsAndBitangents( const std::vector< Vector< T, 3 > > & vertices,
                                const std::vector< Vector< T, 3 > > & normals,
                                const std::vector< Vector< T, 2 > > & tex_coords,
                                const std::vector< GLuint > & indices );
template < typename T >
std::pair< std::vector< Vector< T, 3 > >, std::vector< Vector< T, 3 > > >
ConstructTangentsAndBitangents( const std::vector< T > & vertices,
                                const std::vector< T > & normals,
                                const std::vector< T > & tex_coords,
                                const std::vector< GLuint > & indices );

// constructs a plane
Shape ConstructPlane( const float width, const float height );

// constructs a box
Shape ConstructBox( const float width, const float height, const float depth );

// constructs a sphere
Shape ConstructSphere( const uint32_t slices, const uint32_t stacks, const float radius = 1.0f );

} // namespace GeomHelper

#endif // _GEOM_HELPER_H_
