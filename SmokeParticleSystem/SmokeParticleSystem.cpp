// local includes
#include "SmokeParticleSystem.h"

#include "SimFrame.h"

// platform incudes
#include <windows.h>

// gl includes
#include <gl/gl.h>

// std includes
#include <fstream>

// templated functions
template < class T > T SwapData16( const T & rT )
{
   // create a local type
   T t;
   // create a pointer to the reference
   const char * pRefData = (const char *)&rT;
   // create a pointer to the local type
   char * pLocalData = (char *)&t;
   // move the data to the little endian format
   *pLocalData = *(pRefData + 1);
   *(pLocalData + 1) = *pRefData;
   // return the local type
   return t;
}

template < class T > T SwapData32( const T & rT )
{
   // create a local type
   T t;
   // create a pointer to the reference
   const char * pRefData = (const char *)&rT;
   // create a pointer to the local type
   char * pLocalData = (char *)&t;
   // move the data to the little endian format
   *pLocalData       = *(pRefData + 3);
   *(pLocalData + 1) = *(pRefData + 2);
   *(pLocalData + 2) = *(pRefData + 1);
   *(pLocalData + 3) = *pRefData;
   // return the local type
   return t;
}

SmokeParticleSystem::SmokeParticleSystem( ) :
mActive           ( true ),
mNextParticleDrop ( -1.0 )
{
   // translate to the edge of the grid
   mLocalTransMat.MakeTranslation(10.0, 2.0, 0.0);
}

SmokeParticleSystem::~SmokeParticleSystem( )
{
}

void SmokeParticleSystem::Update( const SimFrame & /*simFrame*/ )
{
   // update the position of the system...
   // this will be controlled by the shape it is
   // attached to, but since there are no shapes here,
   // just create a circular pattern by which to move the system...
   
   // create a rotation matrix that describes
   // a rotation around the y-axis...
   Matrixd yRotMat;
   yRotMat.MakeRotation(1.5, 0.0, 1.0, 0.0);

   // update the local rotation matrix
   mLocalRotMat *= yRotMat;
}

void SmokeParticleSystem::Render( const SimFrame & simFrame )
{
   ////////////////////////////////////////////////////////
   // this defines the buildup of everything in sdt and osg
   // before passing control off to the system

   // save the current matrix
   glPushMatrix();
   // update the local coordinate system
   glMultMatrixd(mLocalRotMat * mLocalTransMat);

   ////////////////////////////////////////////////////////

   // render a small point to indicate the systems location
   // this will be controlled by the current shapes body, but
   // this gives a way for the user to see the system in action...

   // obtain the current point size
   const float pointSize = [ ] ( )
   {
      float point_size = 0.0f;
      glGetFloatv(GL_POINT_SIZE, &point_size);

      return point_size;
   }();
   // set the new point size
   glPointSize(5.0f);
   // render the particle system body
   glBegin(GL_POINTS);
   glColor3f(1.0f, 0.0f, 0.0f);
   glVertex3f(0.0f, 0.0f, 0.0f);
   glEnd();
   // restore the point size
   glPointSize(pointSize);

   // reset the color to white
   glColor3f(1.0f, 1.0f, 1.0f);

   // this system will place a particle
   // into the system every 0.25 seconds
   if (simFrame.dCurTimeMS >= mNextParticleDrop)
   {
      // get the current modelview matrix
      Matrixd mv;
      glGetDoublev(GL_MODELVIEW_MATRIX, mv);

      // add a particle to the system
      Particle p;
      p.dPosition[0] = static_cast< float >(mv.mT[12]);
      p.dPosition[1] = static_cast< float >(mv.mT[13]);
      p.dPosition[2] = static_cast< float >(mv.mT[14]);

      mParticles.push_back(p);

      // update next particle drop
      mNextParticleDrop = simFrame.dCurTimeMS + 25.0;
   }

   // only render if particles available
   if (!mParticles.empty())
   {
      // make identity out of modelview
      glPushMatrix();
      glLoadIdentity();

      // enable vertex array
      glEnableClientState(GL_VERTEX_ARRAY);

      // setup vertex pointer
      glVertexPointer(3,
                      GL_FLOAT,
                      sizeof(Particle),
                      &(mParticles.front()));

      // render the particles
      glDrawArrays(GL_POINTS,
                   0,
                   static_cast< GLsizei >(mParticles.size()));

      // disable vertex array
      glDisableClientState(GL_VERTEX_ARRAY);

      // restore modelview matrix
      glPopMatrix();
   }

   ////////////////////////////////////////////////////////
   // this defines the buildup of everything in sdt and osg
   // before passing control off to the system

   // restore the previous matrix
   glPopMatrix();

   ////////////////////////////////////////////////////////
}

void SmokeParticleSystem::Release( )
{
}

bool SmokeParticleSystem::IsActive( )
{
   return mActive;
}

void SmokeParticleSystem::ReadRGBImage( const char * pFilename,
                                        unsigned char ** pImage,
                                        unsigned int & nWidth,
                                        unsigned int & nHeight )
{
   // local structure
   struct RGBHeader
   {
      short          nMagic;
      char           nStorage;
      char           nBpc;
      unsigned short nDimension;
      unsigned short nXSize;
      unsigned short nYSize;
      unsigned short nZSize;
      long           nPixMin;
      long           nPixMax;
      char           nDummy[4];
      char           nImageName[80];
      long           nColorMap;
      char           nDummy2[404];
   };

   // create a input file stream
   std::ifstream fInputStream;
   // open the stream
   fInputStream.open(pFilename, std::ios_base::in | std::ios_base::binary);

   // make sure the file is open
   if (fInputStream.is_open())
   {
      // create a local header
      RGBHeader rgbHeader;
      // read the local header
      fInputStream.read((char *)&rgbHeader, sizeof(rgbHeader));
      // need to convert the header to little endian
      rgbHeader.nMagic     = SwapData16(rgbHeader.nMagic);
      rgbHeader.nDimension = SwapData16(rgbHeader.nDimension);
      rgbHeader.nXSize     = SwapData16(rgbHeader.nXSize);
      rgbHeader.nYSize     = SwapData16(rgbHeader.nYSize);
      rgbHeader.nZSize     = SwapData16(rgbHeader.nZSize);
      rgbHeader.nPixMin    = SwapData32(rgbHeader.nPixMin);
      rgbHeader.nPixMax    = SwapData32(rgbHeader.nPixMax);
      rgbHeader.nColorMap  = SwapData32(rgbHeader.nColorMap);
      // dimensions of 3 or greater are valid
      // bpc is only valid if it is a 1
      // color map values of 0x00 are only valid
      if (rgbHeader.nDimension >= 0x03 &&
          rgbHeader.nBpc == 0x01       &&
          rgbHeader.nColorMap == 0x00)
      {
         // determine the type of image format
         if (rgbHeader.nStorage)
         {
            // not reading uncompressed rle rgb files
         }
         else
         {
            // determine the image size
            unsigned int nImgSize = rgbHeader.nXSize * rgbHeader.nYSize;

            // setup the image width and height
            nWidth  = rgbHeader.nXSize;
            nHeight = rgbHeader.nYSize;

            // create a local image format
            *pImage = new unsigned char[nImgSize * rgbHeader.nZSize];
            // read in all the image data
            fInputStream.read((char *)pImage, nImgSize * rgbHeader.nZSize);

            // point to the components
            unsigned char * pRed   = rgbHeader.nZSize >= 1 ? *pImage : NULL;
            unsigned char * pGreen = rgbHeader.nZSize >= 2 ? *pImage + nImgSize : NULL;
            unsigned char * pBlue  = rgbHeader.nZSize >= 3 ? *pImage + nImgSize * 2 : NULL;
            unsigned char * pAlpha = rgbHeader.nZSize >= 4 ? *pImage + nImgSize * 3 : NULL;

            // create a local pointer to the image
            unsigned char * pTempImage = static_cast< unsigned char * >(*pImage);

            // set the image data from the file
            for (unsigned int i = 0; i < nImgSize; i++)
            {
               // set the image attributes
               *(pTempImage++) = pBlue  ? *pBlue++  : 0xFF;
               *(pTempImage++) = pGreen ? *pGreen++ : 0xFF;
               *(pTempImage++) = pRed   ? *pRed++   : 0xFF;
               *(pTempImage++) = pAlpha ? *pAlpha++ : 0xFF;
            }
         }
      }

      // close the input stream
      fInputStream.close();
   }
}