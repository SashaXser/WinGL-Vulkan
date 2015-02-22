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
mRadius           ( radius )
{
   // load the surface image
   LoadSurfaceImage(pSurfaceImg);
   // construct the planet vertices
   ConstructPlanet(slices_deg, stacks_deg);
}

Planet::~Planet( )
{
}

void Planet::Render( )
{
   // enable the texture
   mSurfaceImage.Bind();

   // enable all the required buffers
   glEnable(GL_TEXTURE_2D);
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);

   // setup the pointers
   mNormalArray.Bind();
   glNormalPointer(GL_FLOAT, 0, nullptr);

   mVertexArray.Bind();
   glVertexPointer(3, GL_FLOAT, 0, nullptr);

   mTexCoordArray.Bind();
   glTexCoordPointer(2, GL_FLOAT, 0, nullptr);

   // render with the index array
   mIndexArray.Bind();
   glDrawElements(mSphereShape.geom_type,
                  static_cast< GLsizei >(mSphereShape.indices.size()),
                  GL_UNSIGNED_INT,
                  nullptr);

   // unbind the pointers
   mIndexArray.Unbind();
   mVertexArray.Unbind();
   mNormalArray.Unbind();
   mTexCoordArray.Unbind();

   // disable all the required buffers
   glDisable(GL_TEXTURE_2D);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);

   // disable the texture
   mSurfaceImage.Unbind();
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
   if (mSurfaceImage.Load2D(pSurfaceImg, GL_RGBA, GL_COMPRESSED_RGBA, TRUE))
   {
      // bind the texture
      mSurfaceImage.Bind();

      // set extra texture attributes
      mSurfaceImage.SetParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      mSurfaceImage.SetParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      // no longer modifying this texture
      mSurfaceImage.Unbind();
   }

   return mSurfaceImage != 0;
}

void Planet::ConstructPlanet( const double slice_deg, const double stack_deg )
{
   mSphereShape = GeomHelper::ConstructSphere(static_cast< uint32_t >(360.0 / slice_deg),
                                              static_cast< uint32_t >(180.0 / stack_deg),
                                              mRadius);

   mVertexArray.GenBuffer(GL_ARRAY_BUFFER);
   mVertexArray.Bind();
   mVertexArray.BufferData(sizeof(mSphereShape.vertices.front()) * mSphereShape.vertices.size(), &mSphereShape.vertices[0], GL_STATIC_DRAW);
   mVertexArray.Unbind();

   mTexCoordArray.GenBuffer(GL_ARRAY_BUFFER);
   mTexCoordArray.Bind();
   mTexCoordArray.BufferData(sizeof(mSphereShape.tex_coords.front()) * mSphereShape.tex_coords.size(), &mSphereShape.tex_coords[0], GL_STATIC_DRAW);
   mTexCoordArray.Unbind();

   mNormalArray.GenBuffer(GL_ARRAY_BUFFER);
   mNormalArray.Bind();
   mNormalArray.BufferData(sizeof(mSphereShape.normals.front()) * mSphereShape.normals.size(), &mSphereShape.normals[0], GL_STATIC_DRAW);
   mNormalArray.Unbind();

   mIndexArray.GenBuffer(GL_ELEMENT_ARRAY_BUFFER);
   mIndexArray.Bind();
   mIndexArray.BufferData(sizeof(mSphereShape.indices.front()) * mSphereShape.indices.size(), &mSphereShape.indices[0], GL_STATIC_DRAW);
   mIndexArray.Unbind();
}