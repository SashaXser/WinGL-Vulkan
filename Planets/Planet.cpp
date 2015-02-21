// project includes
#include "Planet.h"
#include "Vector.h"
#include "MathHelper.h"
#include "ReadTexture.h"

// std includes
#include <memory>

// plaform includes
#include <Windows.h>

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

Planet::Planet( const char * const pSurfaceImg,
                const float radius,
                const double slices_deg,
                const double stacks_deg ) :
mRadius           ( radius),
mDisplayList      ( 0 ),
mSurfaceImage     ( 0 )
{
   // load the surface image
   LoadSurfaceImage(pSurfaceImg);
   // construct the planet vertices
   ConstructPlanet(slices_deg, stacks_deg);
}

Planet::~Planet( )
{
   // release the image
   glDeleteTextures(1, &mSurfaceImage);

   // release the display list
   glDeleteLists(mDisplayList, 1);
}

void Planet::Render( )
{
   if (mDisplayList)
   {
      // render the display list
      glCallList(mDisplayList);
   }
   else
   {
      // create a new list
      mDisplayList = glGenLists(1);

      // start rendering the new list
      glNewList(mDisplayList, GL_COMPILE_AND_EXECUTE);

      // enable all the required attributes
      glEnable(GL_TEXTURE_2D);
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glEnableClientState(GL_NORMAL_ARRAY);

      // bind the texture for the planet
      glBindTexture(GL_TEXTURE_2D, mSurfaceImage);

      // send down the required vetex and text info
      glNormalPointer(GL_FLOAT, 0, &mSphereShape.normals[0]);
      glVertexPointer(3, GL_FLOAT, 0, &mSphereShape.vertices[0]);
      glTexCoordPointer(2, GL_FLOAT, 0, &mSphereShape.tex_coords[0]);

      // begin rendering all the information
      glDrawElements(mSphereShape.geom_type,
                     static_cast< GLsizei >(mSphereShape.indices.size()),
                     //4 * 3 + 4 * 3 * 2,
                     GL_UNSIGNED_INT,
                     &mSphereShape.indices[0]);

      // disable all the required attributes
      glDisable(GL_TEXTURE_2D);
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glDisableClientState(GL_NORMAL_ARRAY);

      // stop rendering the list
      glEndList();
   }
}

void Planet::Update( const double & rElapsedTime )
{
}

bool Planet::LoadSurfaceImage( const char * pSurfaceImg )
{
   uint32_t width = 0;
   uint32_t height = 0;
   std::shared_ptr< uint8_t > pImage;

   // load the specified image
   if (ReadTexture(pSurfaceImg, GL_BGRA, width, height, pImage))
   {
      // bind the texture
      glGenTextures(1, &mSurfaceImage);
      glBindTexture(GL_TEXTURE_2D, mSurfaceImage);
      // load the texture onto the card
      glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pImage.get());
      // set the texture attributes
      glGenerateMipmap(GL_TEXTURE_2D);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      // no longer modifying this texture
      glBindTexture(GL_TEXTURE_2D, 0);
   }

   return mSurfaceImage != 0;
}

void Planet::ConstructPlanet( const double slice_deg, const double stack_deg )
{
   mSphereShape = GeomHelper::ConstructSphere(static_cast< uint32_t >(360.0 / slice_deg),
                                              static_cast< uint32_t >(180.0 / stack_deg),
                                              mRadius);
}