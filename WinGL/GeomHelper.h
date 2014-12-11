#ifndef _GEOM_HELPER_H_
#define _GEOM_HELPER_H_

// local includes
#include "Vector3.h"

// gl includes
#include <GL/glew.h>
#include <GL/GL.h>

// std includes
#include <vector>

namespace GeomHelper
{

// structure that contains all the required attributes of a shape
// if indices is empty, then glDrawArrays is used.
struct Shape
{
   GLenum                  geom_type;
   std::vector< Vec3f >    vertices;
   std::vector< GLuint >   indices;
   std::vector< float >    tex_coords;
   std::vector< Vec3f >    normals;
   std::vector< Vec3f >    tangents;
   std::vector< Vec3f >    bitangents;
};

// constructs a plane
Shape ConstructPlane( const float width, const float height );

// constructs a box
Shape ConstructBox( const float width, const float height, const float depth );

// constructs a sphere

} // namespace GeomHelper

#endif // _GEOM_HELPER_H_
