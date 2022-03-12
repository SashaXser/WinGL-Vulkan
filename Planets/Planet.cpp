// project includes
#include "Planet.h"
#include "WglAssert.h"
#include "MathHelper.h"
#include "ReadTexture.h"
#include "OpenGLExtensions.h"

// std includes
#include <cmath>
#include <memory>

// plaform includes
#include <Windows.h>

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

Planet::Planet( const char * const pSurfaceImg,
                const float radius,
                const double planet_tilts[2],
                const double planet_times[2],
                const double planet_major_minor_axes[3],
                const char * const pVertShader,
                const char * const pFragShader,
                const double slices_deg,
                const double stacks_deg ) :
mRadius           ( radius ),
mSurfaceImage     ( LoadSurfaceImage(pSurfaceImg) ),
mPlanetaryTime    ( 0.0 ),
PLANETARY_TILTS   ( { planet_tilts[0], planet_tilts[1] } ),
PLANETARY_TIME    ( { planet_times[0], planet_times[1] } ),
MAJ_MIN_AXES      ( { planet_major_minor_axes[0], planet_major_minor_axes[1], planet_major_minor_axes[2] } )
{
   // construct the planet vertices
   ConstructPlanet(slices_deg, stacks_deg);

   // generate the program
   // this is a virtual function, but the vtable is not yet setup
   // a calling class would need to create the program once again
   GenerateProgram(pVertShader, pFragShader);

   // construct the planets matrices
   // obtain references to each of the matrices
   Matrixf & ecliptic = mPlanetaryMatrix[0];
   Matrixf & tilt     = mPlanetaryMatrix[1];
   Matrixf & rotation = mPlanetaryMatrix[2];
   Matrixf & trans    = mPlanetaryMatrix[3];

   // setup the ecliptic
   ecliptic.MakeRotation(static_cast< float >(math::RadToDeg(PLANETARY_TILTS[0])), 0.0f, 0.0f, 1.0f);
   // setup the planet's tilt
   tilt.MakeRotation(math::RadToDeg(static_cast< float >(PLANETARY_TILTS[1])), 0.0f, 0.0f, 1.0f);
   // set the last matrix to the identity
   rotation.MakeIdentity();
   trans.MakeIdentity();
}

Planet::~Planet( )
{
}

void Planet::Render( )
{
   // lets make sure that we can use the glPushMatrix and other matrix calls
   WGL_ASSERT(gl::IsContextCompatible());

   // push the matrix to the stack
   glPushMatrix();
   
   // construct the world matrix for the planet
   const Matrixf world = mPlanetaryMatrix[0] * mPlanetaryMatrix[3];
   const Matrixf trans = Matrixf::Translate(Vec3f(static_cast< float >(MAJ_MIN_AXES[2]), 0.0f, 0.0f));
   const Matrixf planet_pos = world * trans;
   
   // construct the local matrix for the planet
   const Matrixf local = mPlanetaryMatrix[1] * mPlanetaryMatrix[2];
   
   // multiply the current matrix stack by the world then the local
   glMultMatrixf(planet_pos);
   glMultMatrixf(local);

   // determines if this function should disable the program
   // an inherited class may have enabled it and would expect
   // it to be still enabled coming out of the base class...
   const bool disable_program = !mPlanetPgm.IsEnabled();
   
   if (disable_program)
   {
      // enable the shader since it must be disabled
      mPlanetPgm.Enable();
   }
   
   // update the planet's position
   mPlanetPgm.SetUniformValue("planet_position_world_space", planet_pos * Vec3f(0.0f, 0.0f, 0.0f));

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

   // the program is currently active, so disable it to allow the
   // planet center dot to use the fixed function pipeline...
   mPlanetPgm.Disable();

   // render a dot at the middle of the planet
   // allows us to find the planet when really far back
   glPointSize(4);
   glBegin(GL_POINTS);
   glVertex3f(0.0f, 0.0f, 0.0f);
   glEnd();

   // if the program was not supposed to be disabled
   // then activate it again so the inheritted class
   // can have the pleasure of disabling it...
   if (!disable_program)
   {
      mPlanetPgm.Enable();
   }

   // remove the matrix from the stack
   glPopMatrix();
}

void Planet::Update( const double & /*true_elapsed_time_secs*/,
                     const double & sim_elapsed_time_secs )
{
   // update the time position
   mPlanetaryTime += (sim_elapsed_time_secs * PLANETARY_TIME[1]);

   // construct the translation matrix
   mPlanetaryMatrix[3] =
      Matrixf::Translate(static_cast< float >(std::cos(mPlanetaryTime) * MAJ_MIN_AXES[1]),
                         0.0f,
                         static_cast< float >(std::sin(mPlanetaryTime) * -MAJ_MIN_AXES[0]));

   // compute a planetary rotation
   const Matrixf mRotation =
      Matrixf::Rotate(static_cast< float >(math::RadToDeg(PLANETARY_TIME[0] * sim_elapsed_time_secs)),
                      Vec3f(0.0f, 1.0f, 0.0f));

   // update the local rotation
   mPlanetaryMatrix[2] = mPlanetaryMatrix[2] * mRotation;
}

void Planet::UpdateWorldToEyeSpaceLighting( const Matrixf & world_to_eye_light )
{
   WGL_ASSERT(!mPlanetPgm.IsEnabled())

   mPlanetPgm.Enable();
   mPlanetPgm.SetUniformMatrix< 1, 4, 4 >("light_world_to_eye_space_mat", world_to_eye_light);
   mPlanetPgm.Disable();
}

void Planet::UpdateSunWorldPosition( const Vec3f & world_position )
{
   WGL_ASSERT(!mPlanetPgm.IsEnabled())

   mPlanetPgm.Enable();
   mPlanetPgm.SetUniformValue("sun_position_world_space", world_position);
   mPlanetPgm.Disable();
}

Vec3f Planet::GetWorldPosition( ) const
{
   const Matrixf world = mPlanetaryMatrix[0] * mPlanetaryMatrix[3];
   const Matrixf trans = Matrixf::Translate(Vec3f(static_cast< float >(MAJ_MIN_AXES[2]), 0.0f, 0.0f));
   const Matrixf planet_pos = world * trans;

   return planet_pos * Vec3f(0.0f, 0.0f, 0.0f);
}

Texture Planet::LoadSurfaceImage( const char * const pSurfaceImg )
{
   Texture texture;

   // load the specified image
   if (texture.Load2D(pSurfaceImg, GL_RGBA, GL_COMPRESSED_RGBA, true))
   {
      // bind the texture
      texture.Bind();

      // set extra texture attributes
      texture.SetParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      texture.SetParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      // no longer modifying this texture
      texture.Unbind();
   }

   return texture;
}

void Planet::ConstructPlanet( const double slice_deg, const double stack_deg )
{
   // construct the sphere
   mSphereShape = GeomHelper::ConstructSphere(static_cast< uint32_t >(360.0 / slice_deg),
                                              static_cast< uint32_t >(180.0 / stack_deg),
                                              mRadius);

   // construct the vertex buffer
   mVertexArray.GenBuffer(GL_ARRAY_BUFFER);
   mVertexArray.Bind();
   mVertexArray.BufferData(sizeof(mSphereShape.vertices.front()) * mSphereShape.vertices.size(), &mSphereShape.vertices[0], GL_STATIC_DRAW);
   mVertexArray.Unbind();

   // construct the texture coordinate buffer
   mTexCoordArray.GenBuffer(GL_ARRAY_BUFFER);
   mTexCoordArray.Bind();
   mTexCoordArray.BufferData(sizeof(mSphereShape.tex_coords.front()) * mSphereShape.tex_coords.size(), &mSphereShape.tex_coords[0], GL_STATIC_DRAW);
   mTexCoordArray.Unbind();

   // construct the normal buffer
   mNormalArray.GenBuffer(GL_ARRAY_BUFFER);
   mNormalArray.Bind();
   mNormalArray.BufferData(sizeof(mSphereShape.normals.front()) * mSphereShape.normals.size(), &mSphereShape.normals[0], GL_STATIC_DRAW);
   mNormalArray.Unbind();

   // construct the index buffer
   mIndexArray.GenBuffer(GL_ELEMENT_ARRAY_BUFFER);
   mIndexArray.Bind();
   mIndexArray.BufferData(sizeof(mSphereShape.indices.front()) * mSphereShape.indices.size(), &mSphereShape.indices[0], GL_STATIC_DRAW);
   mIndexArray.Unbind();
}

bool Planet::GenerateProgram( const char * const pVertShader,
                              const char * const pFragShader )
{
   // compile and link the planet shader
   mPlanetPgm.AttachFile(GL_VERTEX_SHADER, pVertShader);
   mPlanetPgm.AttachFile(GL_FRAGMENT_SHADER, pFragShader);
   
   return mPlanetPgm.Link();
}
