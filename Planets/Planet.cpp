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
      glNormalPointer(GL_FLOAT, 0, &mNormals[0]);
      glVertexPointer(3, GL_FLOAT, 0, &mVertices[0]);
      glTexCoordPointer(2, GL_FLOAT, 0, &mTexCoords[0]);

      // begin rendering all the information
      for (IndicesVec::iterator itBeg = mIndices.begin(),
                                itEnd = mIndices.end();
           itBeg != itEnd;
           ++itBeg)
      {
         glDrawElements(itBeg->front(),
                        static_cast< GLsizei >(itBeg->size() - 1),
                        GL_UNSIGNED_INT,
                        &itBeg->at(1));
      }

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

bool Planet::ConstructPlanet( const double slice_deg, const double stack_deg )
{
   // x = r * sin(stack) * sin(slice)
   // y = r * cos(stack)
   // z = r * sin(stack) * cos(slice)

   // convert oSlice and oStack to radians
   double slice_rad = MathHelper::DegToRad(slice_deg);
   double stack_rad = MathHelper::DegToRad(stack_deg);

   // save the angle info for delta changes
   const double slice_rad_delta = slice_rad;
   const double stack_rad_delta = stack_rad;
   const double slice_deg_delta = slice_deg;

   // set the slice value to zero
   slice_rad = 0.0;

   // push back the top vertex
   mVertices.push_back(0.0f);
   mVertices.push_back(mRadius);
   mVertices.push_back(0.0f);

   // push back the top normal
   mNormals.push_back(0.0f);
   mNormals.push_back(1.0f);
   mNormals.push_back(0.0f);

   // push back the top texture coordinate
   mTexCoords.push_back(0.5f);
   mTexCoords.push_back(1.0f);

   // create an index array
   UIntVec vIndexVec;

   // the first element dictates the type of draw operation
   vIndexVec.push_back(GL_TRIANGLE_FAN);
   // push back the first index of 0 for the top
   vIndexVec.push_back(0);

   // keeps track of the stack iterations
   unsigned int nStackIndex = 0;

   while (stack_rad < MathHelper::DegToRad(180.0))
   {
      // keeps track of slice iterations
      unsigned int nSliceIndex = 0;
      // keeps track of the first vertex value
      float fFirstVertex[3] = { 0.0f };

      // calculate the trig values
      float fStackSin = (float)std::sin(stack_rad);
      float fStackCos = (float)std::cos(stack_rad);

      // calculate y
      float fY = mRadius * fStackCos;

      while (slice_rad < MathHelper::DegToRad(360.0))
      {
         // calculate the degrees for the slices and stacks
         const double slice_deg = MathHelper::RadToDeg(slice_rad);
         const double stack_deg = MathHelper::RadToDeg(stack_rad);

         // calculate the trig values
         float fSliceSin = (float)std::sin(slice_rad);
         float fSliceCos = (float)std::cos(slice_rad);

         // calculate the values
         float fX = mRadius * fStackSin * fSliceSin;
         float fZ = mRadius * fStackSin * fSliceCos;

         if (!nSliceIndex)
         {
            fFirstVertex[0] = fX;
            fFirstVertex[1] = fY;
            fFirstVertex[2] = fZ;
         }

         // push back all the values between the top and bottom
         mVertices.push_back(fX);
         mVertices.push_back(fY);
         mVertices.push_back(fZ);

         // calculate the normal
         const Vec3f normal = Vec3f(fX, fY, fZ).MakeUnitVector();
         mNormals.push_back(normal.X());
         mNormals.push_back(normal.Y());
         mNormals.push_back(normal.Z());

         // calculate the texture coordinate
         float fS = (float)(slice_deg / 360.0);
         float fT = (float)(1.0 - stack_deg / 180.0);

         // push the texture coordinates into the list
         mTexCoords.push_back(fS);
         mTexCoords.push_back(fT);

         // determine how to introduce the next few indices
         switch (vIndexVec.at(0))
         {
         case GL_TRIANGLE_FAN:
               // push back the next index
               vIndexVec.push_back(static_cast< uint32_t >(vIndexVec.size() - 1));

               // push back one final vertex and texture coordinates
               if (slice_deg + slice_deg_delta >= 360.0)
               {
                  // push back the first vertex again
                  mVertices.push_back(fFirstVertex[0]);
                  mVertices.push_back(fFirstVertex[1]);
                  mVertices.push_back(fFirstVertex[2]);

                  // calculate the normal
                  const Vec3f normal = Vec3f(fFirstVertex[0], fFirstVertex[1], fFirstVertex[2]).MakeUnitVector();
                  mNormals.push_back(normal.X());
                  mNormals.push_back(normal.Y());
                  mNormals.push_back(normal.Z());

                  // push back the texture coordinate
                  mTexCoords.push_back(1.0f);
                  mTexCoords.push_back(fT);

                  // push back an extra index
                  vIndexVec.push_back(static_cast< uint32_t >(vIndexVec.size() - 1));
               }

            break;

         case GL_QUAD_STRIP:
            {
               // get a reference to the first index list
               UIntVec & rQuadStripVec = mIndices.at(0);
               // determine how many stack points we have
               size_t nStackPointSize = rQuadStripVec.size() - 2;
               // calculate the index points
               size_t nIndex1 = (nStackIndex - 1) * nStackPointSize + nSliceIndex + 1;
               size_t nIndex2 = nIndex1 + nStackPointSize;

               // push these values into the index list
               vIndexVec.push_back(static_cast< uint32_t >(nIndex1));
               vIndexVec.push_back(static_cast< uint32_t >(nIndex2));

               // connect the end to the beginning
               if (nStackPointSize - 2 == nSliceIndex)
               {
                  // push back the first vertex again
                  mVertices.push_back(fFirstVertex[0]);
                  mVertices.push_back(fFirstVertex[1]);
                  mVertices.push_back(fFirstVertex[2]);

                  // calculate the normal
                  const Vec3f normal = Vec3f(fFirstVertex[0], fFirstVertex[1], fFirstVertex[2]).MakeUnitVector();
                  mNormals.push_back(normal.X());
                  mNormals.push_back(normal.Y());
                  mNormals.push_back(normal.Z());

                  // push back the texture coordinate
                  mTexCoords.push_back(1.0f);
                  mTexCoords.push_back(fT);

                  // push back the extra indices
                  vIndexVec.push_back(static_cast< uint32_t >(nIndex1 + 1));
                  vIndexVec.push_back(static_cast< uint32_t >(nIndex2 + 1));
               }
            }

            break;
         }

         // increase the slice
         slice_rad += slice_rad_delta;
         // increase the slice index value
         ++nSliceIndex;
      }

      // increase the stack
      stack_rad += stack_rad_delta;
      // reset the slice to 0
      slice_rad = 0;

      // push the index vector back
      mIndices.push_back(vIndexVec);
      // clear the local index vector
      vIndexVec.clear();

      // push a triangle trip onto the vector
      vIndexVec.push_back(GL_QUAD_STRIP);

      // increase the stack index value
      ++nStackIndex;
   }

   // push back the top vertex
   mVertices.push_back(0.0f);
   mVertices.push_back(-mRadius);
   mVertices.push_back(0.0f);

   // calculate the normal
   mNormals.push_back(0.0f);
   mNormals.push_back(-1.0f);
   mNormals.push_back(0.0f);

   // push the bottom texture coordinate
   mTexCoords.push_back(0.5f);
   mTexCoords.push_back(0.0f);

   // push back the first index vector
   mIndices.push_back(mIndices.at(0));
   // obtain the number of vertices
   size_t nVertSize = mVertices.size() / 3;
   // obtain a reference to the last index list
   UIntVec & rIndexVec = mIndices.back();
   // for each of the last indices substract by the total vertices
   for (UIntVec::iterator itBeg = rIndexVec.begin() + 1,
                          itEnd = rIndexVec.end();
        itBeg != itEnd;
        ++itBeg)
   {
      (*itBeg) = static_cast< uint32_t >(nVertSize - 1) - (*itBeg);
   }

   return false;
}