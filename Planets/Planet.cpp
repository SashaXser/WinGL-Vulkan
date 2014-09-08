// project includes
#include "StdAfx.h"
#include "Planet.h"

// qgl project includes
#include "ImageLibrary.h"

Planet::Planet( ) :
m_pImage          ( 0 ),
m_fRadius         ( 0.0f ),
m_nDisplayList    ( 0 ),
m_nSurfaceImage   ( 0 )
{
   // it is up to the inheritted class to modify the
   // class variables to their own liking
}

Planet::Planet( const char * pSurfaceImg,
                const float & rRadius,
                const Angle & rSlices,
                const Angle & rStacks ) :
m_pImage          ( 0 ),
m_fRadius         ( rRadius ),
m_nDisplayList    ( 0 ),
m_nSurfaceImage   ( 0 )
{
   // load the surface image
   LoadSurfaceImage(pSurfaceImg);
   // construct the planet vertices
   ConstructPlanet(rSlices, rStacks);
}

Planet::~Planet( )
{
   // release the image
   ImageLibrary::Instance()->DeleteImage(m_nSurfaceImage);
}

void Planet::Render( )
{
   if (m_nDisplayList)
   {
      // render the display list
      glCallList(m_nDisplayList);
   }
   else
   {
      // create a new list
      m_nDisplayList = glGenLists(1);

      // start rendering the new list
      glNewList(m_nDisplayList, GL_COMPILE_AND_EXECUTE);

      // enable all the required attributes
      glEnable(GL_TEXTURE_2D);
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);

      // bind the texture for the planet
      glBindTexture(GL_TEXTURE_2D, m_nSurfaceImage);

      // send down the required vetex and text info
      glVertexPointer(3, GL_FLOAT, 0, &m_vVertices[0]);
      glTexCoordPointer(2, GL_FLOAT, 0, &m_vTexCoords[0]);

      // begin rendering all the information
      for (IndicesVec::iterator itBeg = m_vIndices.begin(),
                                itEnd = m_vIndices.end();
           itBeg != itEnd;
           ++itBeg)
      {
         glDrawElements(itBeg->front(),
                        itBeg->size() - 1,
                        GL_UNSIGNED_INT,
                        &itBeg->at(1));
      }

      // disable all the required attributes
      glDisable(GL_TEXTURE_2D);
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);

      // stop rendering the list
      glEndList();
   }
}

void Planet::Update( const double & rElapsedTime )
{
}

bool Planet::LoadSurfaceImage( const char * pSurfaceImg )
{
   // load the specified image
   if (const ImageLibrary::Image * pImage = ImageLibrary::Instance()->Load(pSurfaceImg, true, true, false))
   {
      // setup the variables
      m_pImage = pImage->m_pImage;
      m_nSurfaceImage = pImage->m_unImageID;

      // bind the texture
      glBindTexture(GL_TEXTURE_2D, m_nSurfaceImage);
      // set the texture attributes
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   }

   return m_nSurfaceImage != 0;
}

bool Planet::ConstructPlanet( Angle oSlice, Angle oStack )
{
   // x = r * sin(stack) * sin(slice)
   // y = r * cos(stack)
   // z = r * sin(stack) * cos(slice)

   // convert oSlice and oStack to radians
   oSlice.SetUnit(AngleType::RADIANS);
   oStack.SetUnit(AngleType::RADIANS);

   // save the angle info for delta changes
   Angle oSliceDelta = oSlice;
   Angle oStackDelta = oStack;

   // set the slice value to zero
   oSlice.SetValue(0.0);

   // push back the top vertex
   m_vVertices.push_back(0.0f);
   m_vVertices.push_back(m_fRadius);
   m_vVertices.push_back(0.0f);

   // push back the top texture coordinate
   m_vTexCoords.push_back(0.5f);
   m_vTexCoords.push_back(1.0f);

   // create an index array
   UIntVec vIndexVec;

   // the first element dictates the type of draw operation
   vIndexVec.push_back(GL_TRIANGLE_FAN);
   // push back the first index of 0 for the top
   vIndexVec.push_back(0);

   // keeps track of the stack iterations
   unsigned int nStackIndex = 0;

   while (oStack.GetValue(AngleType::DEGREES) < 180.0)
   {
      // keeps track of slice iterations
      unsigned int nSliceIndex = 0;
      // keeps track of the first vertex value
      float fFirstVertex[3] = { 0.0f };

      // calculate the trig values
      float fStackSin = (float)std::sin(oStack.GetValue());
      float fStackCos = (float)std::cos(oStack.GetValue());

      // calculate y
      float fY = m_fRadius * fStackCos;

      while (oSlice.GetValue(AngleType::DEGREES) < 360.0)
      {
         // calculate the trig values
         float fSliceSin = (float)std::sin(oSlice.GetValue());
         float fSliceCos = (float)std::cos(oSlice.GetValue());

         // calculate the values
         float fX = m_fRadius * fStackSin * fSliceSin;
         float fZ = m_fRadius * fStackSin * fSliceCos;

         if (!nSliceIndex)
         {
            fFirstVertex[0] = fX;
            fFirstVertex[1] = fY;
            fFirstVertex[2] = fZ;
         }

         // push back all the values between the top and bottom
         m_vVertices.push_back(fX);
         m_vVertices.push_back(fY);
         m_vVertices.push_back(fZ);

         // calculate the texture coordinate
         float fS = (float)(oSlice.GetValue(AngleType::DEGREES) / 360.0);
         float fT = (float)(1.0 - oStack.GetValue(AngleType::DEGREES) / 180.0);

         // push the texture coordinates into the list
         m_vTexCoords.push_back(fS);
         m_vTexCoords.push_back(fT);

         // determine how to introduce the next few indices
         switch (vIndexVec.at(0))
         {
         case GL_TRIANGLE_FAN:
               // push back the next index
               vIndexVec.push_back(vIndexVec.size() - 1);

               // push back one final vertex and texture coordinates
               if (oSlice.GetValue(AngleType::DEGREES) +
                   oSliceDelta.GetValue(AngleType::DEGREES) >= 360.0)
               {
                  // push back the first vertex again
                  m_vVertices.push_back(fFirstVertex[0]);
                  m_vVertices.push_back(fFirstVertex[1]);
                  m_vVertices.push_back(fFirstVertex[2]);

                  // push back the texture coordinate
                  m_vTexCoords.push_back(1.0f);
                  m_vTexCoords.push_back(fT);

                  // push back an extra index
                  vIndexVec.push_back(vIndexVec.size() - 1);
               }

            break;

         case GL_QUAD_STRIP:
            {
               // get a reference to the first index list
               UIntVec & rQuadStripVec = m_vIndices.at(0);
               // determine how many stack points we have
               unsigned int nStackPointSize = rQuadStripVec.size() - 2;
               // calculate the index points
               unsigned int nIndex1 = (nStackIndex - 1) * nStackPointSize + nSliceIndex + 1;
               unsigned int nIndex2 = nIndex1 + nStackPointSize;

               // push these values into the index list
               vIndexVec.push_back(nIndex1);
               vIndexVec.push_back(nIndex2);

               // connect the end to the beginning
               if (nStackPointSize - 2 == nSliceIndex)
               {
                  // push back the first vertex again
                  m_vVertices.push_back(fFirstVertex[0]);
                  m_vVertices.push_back(fFirstVertex[1]);
                  m_vVertices.push_back(fFirstVertex[2]);

                  // push back the texture coordinate
                  m_vTexCoords.push_back(1.0f);
                  m_vTexCoords.push_back(fT);

                  // push back the extra indices
                  vIndexVec.push_back(nIndex1 + 1);
                  vIndexVec.push_back(nIndex2 + 1);
               }
            }

            break;
         }

         // increase the slice
         oSlice.SetValue(oSlice.GetValue() + oSliceDelta.GetValue());
         // increase the slice index value
         ++nSliceIndex;
      }

      // increase the stack
      oStack.SetValue(oStack.GetValue() + oStackDelta.GetValue());
      // reset the slice to 0
      oSlice.SetValue(0.0);

      // push the index vector back
      m_vIndices.push_back(vIndexVec);
      // clear the local index vector
      vIndexVec.clear();

      // push a triangle trip onto the vector
      vIndexVec.push_back(GL_QUAD_STRIP);

      // increase the stack index value
      ++nStackIndex;
   }

   // push back the top vertex
   m_vVertices.push_back(0.0f);
   m_vVertices.push_back(-m_fRadius);
   m_vVertices.push_back(0.0f);

   // push the bottom texture coordinate
   m_vTexCoords.push_back(0.5f);
   m_vTexCoords.push_back(0.0f);

   // push back the first index vector
   m_vIndices.push_back(m_vIndices.at(0));
   // obtain the number of vertices
   unsigned int nVertSize = m_vVertices.size() / 3;
   // obtain a reference to the last index list
   UIntVec & rIndexVec = m_vIndices.back();
   // for each of the last indices substract by the total vertices
   for (UIntVec::iterator itBeg = rIndexVec.begin() + 1,
                          itEnd = rIndexVec.end();
        itBeg != itEnd;
        ++itBeg)
   {
      (*itBeg) = (nVertSize - 1) - (*itBeg);
   }

   return false;
}