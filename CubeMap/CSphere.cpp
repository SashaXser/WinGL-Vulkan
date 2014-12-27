// includes
#include "CSphere.h"
#include "CPBuffer.h"
#include "CFrameBuffer.h"

#include "MathHelper.h"

#include <math.h>

// simple utility to quickly generate a texture
const auto GenTexture = [ ] ( const TextureData< uint8_t > & tex ) -> GLuint
{
   // load the image to the card
   GLuint image = 0;
   glGenTextures(1, &image);
   glBindTexture(GL_TEXTURE_2D, image);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, tex.pTexture.get());

   // set up the texture parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

   // no longer need the texture bound
   glBindTexture(GL_TEXTURE_2D, 0);

   return image;
};

struct CSphere::PBufferObjs
{
   // member variables
   HDC            m_hWndDC;
   HDC            m_hPBufDC;
   HWND           m_hWnd;
   HGLRC          m_hGLRC;
   HPBUFFERARB    m_hPBuffer;
};

struct CSphere::FrameBufferObjs
{
   // member variables
   unsigned int m_nFrameBuffer;
};

CSphere::CSphere( HWND hWnd,
                  float fRadius,
                  unsigned int nSlices,
                  unsigned int nStacks,
                  const char * pPosX,
                  const char * pNegX,
                  const char * pPosY,
                  const char * pNegY,
                  const char * pPosZ,
                  const char * pNegZ ) :
m_fRadius          ( fRadius ),
m_nSlices          ( nSlices ),
m_nStacks          ( nStacks ),
m_vEyeDir          ( 0.0f, 0.0f, 1.0f ),
m_pPBuffer         ( new CPBuffer ),
m_nDrawType        ( DRAW_FILL ),
m_pVertices        ( 0 ),
m_pPBufferObjs     ( new PBufferObjs ),
m_nReflection      ( REFLECT_STATIC ),
m_nIndicesSize     ( 0 ),
m_pFrameBuffer     ( new CFrameBuffer ),
m_pDrawFunctor     ( &CSphere::DrawSphere ),
m_nVerticesSize    ( 0 ),
m_bAutoGenNorms    ( false ),
m_nCubeMapTexture  ( 0 ),
m_nDynCubemapSize  ( 256 ),
m_pFrameBufferObjs ( new FrameBufferObjs )
{
   // material properties
   float fAmbient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
   float fDiffuse[]  = { 0.8f, 0.8f, 0.8f, 1.0f };
   float fSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
   float fEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };

   // setup the material properties
   m_oMaterial.nFace       = GL_FRONT;
   m_oMaterial.nFlags      = MF_NONE;
   m_oMaterial.fShininess  = 0.0f;

   memcpy(m_oMaterial.fAmbient, fAmbient, sizeof(fAmbient));
   memcpy(m_oMaterial.fDiffuse, fDiffuse, sizeof(fDiffuse));
   memcpy(m_oMaterial.fSpecular, fSpecular, sizeof(fSpecular));
   memcpy(m_oMaterial.fEmission, fEmission, sizeof(fEmission));

   // make sure there is 3 slice and 3 stack
   if (m_nSlices < 3) m_nSlices = 3;
   if (m_nStacks < 3) m_nStacks = 3;

   // load all the images
   const_cast< TextureData< uint8_t > & >(m_oImageAttribs[CM_POS_X]) = ReadTexture< uint8_t >(pPosX, GL_RGBA);
   const_cast< TextureData< uint8_t > & >(m_oImageAttribs[CM_NEG_X]) = ReadTexture< uint8_t >(pNegX, GL_RGBA);
   const_cast< TextureData< uint8_t > & >(m_oImageAttribs[CM_POS_Y]) = ReadTexture< uint8_t >(pPosY, GL_RGBA);
   const_cast< TextureData< uint8_t > & >(m_oImageAttribs[CM_NEG_Y]) = ReadTexture< uint8_t >(pNegY, GL_RGBA);
   const_cast< TextureData< uint8_t > & >(m_oImageAttribs[CM_POS_Z]) = ReadTexture< uint8_t >(pPosZ, GL_RGBA);
   const_cast< TextureData< uint8_t > & >(m_oImageAttribs[CM_NEG_Z]) = ReadTexture< uint8_t >(pNegZ, GL_RGBA);

   // save the image names
   m_sStaticImages[CM_POS_X] = new std::string(pPosX);
   m_sStaticImages[CM_NEG_X] = new std::string(pNegX);
   m_sStaticImages[CM_POS_Y] = new std::string(pPosY);
   m_sStaticImages[CM_NEG_Y] = new std::string(pNegY);
   m_sStaticImages[CM_POS_Z] = new std::string(pPosZ);
   m_sStaticImages[CM_NEG_Z] = new std::string(pNegZ);

   // generate a texture coordinate
   glGenTextures(1, &m_nCubeMapTexture);
   
   // construct the images
   ConstructStaticImages();

   // generate texture coordinates
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
   glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
   glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);

   // set the pbuffer variables
   m_pPBufferObjs->m_hWndDC = GetDC(hWnd);
   m_pPBufferObjs->m_hWnd = hWnd;
   m_pPBufferObjs->m_hGLRC = 0;
   m_pPBufferObjs->m_hPBuffer = 0;

   // construct the sphere
   ConstructSphere();
}

CSphere::~CSphere( )
{
   // release the resources
   delete [] m_pVertices;
   delete [] m_pIndices;

   // release the strings
   for (int i = 0; i < CM_MAX_TYPES; i++)
   {
      delete m_sStaticImages[i];
   }

   // release the memory from the cards
   Release();

   // release the pbuffer
   delete m_pPBuffer;
   delete m_pPBufferObjs;
   // release the frame buffer
   delete m_pFrameBuffer;
   delete m_pFrameBufferObjs;
}

void CSphere::ConstructSphere( float fRadius,
                               unsigned int nSlices,
                               unsigned int nStacks )
{
   // release resources
   delete [] m_pVertices;
   delete [] m_pIndices;

   // set the member variables
   m_fRadius = fRadius;
   m_nSlices = nSlices;
   m_nStacks = nStacks;

   // make sure there is 3 slice and 3 stack
   if (m_nSlices < 3) m_nSlices = 3;
   if (m_nStacks < 3) m_nStacks = 3;

   // construct the sphere
   ConstructSphere();
}

void CSphere::DrawSphere( const double & rElapsedTime ) const
{
   // set the material properties
   if (m_oMaterial.nFlags & MF_AMBIENT)   glMaterialfv(m_oMaterial.nFace, GL_AMBIENT,    m_oMaterial.fAmbient);
   if (m_oMaterial.nFlags & MF_DIFFUSE)   glMaterialfv(m_oMaterial.nFace, GL_DIFFUSE,    m_oMaterial.fDiffuse);
   if (m_oMaterial.nFlags & MF_SPECULAR)  glMaterialfv(m_oMaterial.nFace, GL_SPECULAR,   m_oMaterial.fSpecular);
   if (m_oMaterial.nFlags & MF_EMISSION)  glMaterialfv(m_oMaterial.nFace, GL_EMISSION,   m_oMaterial.fEmission);
   if (m_oMaterial.nFlags & MF_SHININESS) glMaterialfv(m_oMaterial.nFace, GL_SHININESS, &m_oMaterial.fShininess);

   // enable gl arttributes
   glEnable(GL_VERTEX_ARRAY);
   glEnable(GL_NORMAL_ARRAY);
   glEnable(GL_TEXTURE_CUBE_MAP_EXT);

   // determine how to enable the texture attributes
   if (m_bAutoGenNorms)
   {
      glEnable(GL_TEXTURE_GEN_S);
      glEnable(GL_TEXTURE_GEN_T);
      glEnable(GL_TEXTURE_GEN_R);
   }
   else
   {
      glEnable(GL_TEXTURE_COORD_ARRAY);
   }

   // bind the texture cube map
   glBindTexture(GL_TEXTURE_CUBE_MAP, m_nCubeMapTexture);

   // bind the texture correctly if a pbuffer is used
   if (m_nReflection == REFLECT_DYNAMIC_PBUFFER)
   {
      m_pPBuffer->wglBindTexImageARB(m_pPBufferObjs->m_hPBuffer, WGL_FRONT_LEFT_ARB);
   }

   // set the vertex pointer
   glVertexPointer(3, GL_FLOAT, sizeof(Vertex), m_pVertices);
   glNormalPointer(GL_FLOAT, sizeof(Vertex), &m_pVertices->fU);

   // determine if the texture coordinate pointer needs to be set
   if (!m_bAutoGenNorms)
   {
      glTexCoordPointer(3, GL_FLOAT, sizeof(Vertex), &m_pVertices->fS);
   }

   // begin drawing the lines
   glDrawElements(m_nDrawType == DRAW_FILL ? GL_TRIANGLES : GL_LINE_STRIP,
                  m_nIndicesSize,
                  GL_UNSIGNED_INT,
                  m_pIndices);

   // disable gl attributes
   glDisable(GL_VERTEX_ARRAY);
   glDisable(GL_NORMAL_ARRAY);
   glDisable(GL_TEXTURE_CUBE_MAP_EXT);

   // determine how to disable the texture attributes
   if (m_bAutoGenNorms)
   {
      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);
      glDisable(GL_TEXTURE_GEN_R);
   }
   else
   {
      glDisable(GL_TEXTURE_COORD_ARRAY);
   }

   // release the pbuffer texture
   if (m_nReflection == REFLECT_DYNAMIC_PBUFFER)
   {
      m_pPBuffer->wglReleaseTexImageARB(m_pPBufferObjs->m_hPBuffer, WGL_FRONT_LEFT_ARB);
   }
}

void CSphere::DrawQuads( const double & rElapsedTime ) const
{
   // vertex pointer
   float fVertPtr[] = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f, 0.5f, 0.0f };
   // tex coord pointer
   float fTexCoordPointer[6][12] = 
   {
      { -1.0f,-1.0f,-1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,-1.0f },  // neg x
      { -1.0f,-1.0f, 1.0f,   1.0f,-1.0f, 1.0f,   1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f },  // pos z
      {  1.0f,-1.0f, 1.0f,   1.0f,-1.0f,-1.0f,   1.0f, 1.0f,-1.0f,   1.0f, 1.0f, 1.0f },  // pos x
      {  1.0f,-1.0f,-1.0f,  -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,   1.0f, 1.0f,-1.0f },  // neg z
      { -1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,-1.0f,  -1.0f, 1.0f,-1.0f },  // pos y
      { -1.0f,-1.0f,-1.0f,   1.0f,-1.0f,-1.0f,   1.0f,-1.0f, 1.0f,  -1.0f,-1.0f, 1.0f },  // neg y
   };
   // translation units
   float fTranslate[6][3] =
   {
      { -1.575f, 0.0f, 0.0f },   // neg x
      { -0.525f, 0.0f, 0.0f },   // pos z
      {  0.525f, 0.0f, 0.0f },   // pos x
      {  1.575f, 0.0f, 0.0f },   // neg z
      { -0.525f, 1.05f, 0.0f },  // pos y
      { -0.525f, -1.05f, 0.0f }  // neg y
   };

   // disable cull facing
   glDisable(GL_CULL_FACE);

   // enable gl arttributes
   glEnable(GL_VERTEX_ARRAY);
   glEnable(GL_TEXTURE_COORD_ARRAY);
   glEnable(GL_TEXTURE_CUBE_MAP_EXT);

   // setup the texture unit
   glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, m_nCubeMapTexture);

   // bind the texture correctly if a pbuffer is used
   if (m_nReflection == REFLECT_DYNAMIC_PBUFFER)
   {
      m_pPBuffer->wglBindTexImageARB(m_pPBufferObjs->m_hPBuffer, WGL_FRONT_LEFT_ARB);
   }

   // draw the quads
   for (unsigned int i = 0; i < CM_MAX_TYPES; i++)
   {
      // setup the pointers
      glVertexPointer(3, GL_FLOAT, 0, fVertPtr);
      glTexCoordPointer(3, GL_FLOAT, 0, fTexCoordPointer[i]);

      // push a matrix
      glPushMatrix();

      // translate the quad
      glTranslatef(fTranslate[i][0],
                   fTranslate[i][1],
                   fTranslate[i][2]);

      // draw the quads
      glDrawArrays(GL_QUADS, 0, 4);

      // pop the matrix
      glPopMatrix();
   }

   // enable cull facing
   glEnable(GL_CULL_FACE);

   // disable gl attributes
   glDisable(GL_VERTEX_ARRAY);
   glDisable(GL_TEXTURE_COORD_ARRAY);
   glDisable(GL_TEXTURE_CUBE_MAP_EXT);

   // release the pbuffer texture
   if (m_nReflection == REFLECT_DYNAMIC_PBUFFER)
   {
      m_pPBuffer->wglReleaseTexImageARB(m_pPBufferObjs->m_hPBuffer, WGL_FRONT_LEFT_ARB);
   }
}

void CSphere::Update( const double & rElapsedTime,
                      const Vec3f & rEyePosition )
{
   // are hand generated normals set
   if (!m_bAutoGenNorms)
   {
      // obtain the beginning of the vertex pointer
      Vertex * pVertex = m_pVertices;

      // generate vectors for verts, norms, incidents, and reflections
      Vec3f vVert(0.0f, 0.0f, 0.0f);
      Vec3f vNorm(0.0f, 0.0f, 0.0f);
      Vec3f vReflect(0.0f, 0.0f, 0.0f);
      Vec3f vIncident(0.0f, 0.0f, 0.0f);

      for (unsigned int i = 0; i < m_nVerticesSize; i++)
      {
         // set the verts
         vVert.X() = pVertex->fX;
         vVert.Y() = pVertex->fY;
         vVert.Z() = pVertex->fZ;

         // set teh normals
         vNorm.X() = pVertex->fU;
         vNorm.Y() = pVertex->fV;
         vNorm.Z() = pVertex->fN;

         // determine the incident vector
         vIncident = vVert - rEyePosition;
         // determine the reflection
         Vec3f vReflect = vIncident - vNorm * (2 * (vNorm * vIncident));

         // set the texture coordinates
         pVertex->fS = vReflect.X();
         pVertex->fT = vReflect.Y();
         pVertex->fR = vReflect.Z();

         // increase the vertex pointer
         pVertex++;
      }
   }

   //// are dynamic textures set
   //if (m_nReflection & REFLECT_DYNAMIC)
   //{
   //   // set the eye position
   //   m_vEyeDir = rEyePosition.UnitVector();
   //}
}

void CSphere::ConstructSphere( )
{
   // determine the number of vertices
   m_nVerticesSize = (m_nStacks - 1) * m_nSlices + 2;

   // create the vertices
   m_pVertices = new Vertex[m_nVerticesSize];

   // determine the number of indices
   m_nIndicesSize = (2 * m_nSlices + 2 * m_nSlices * (m_nStacks - 2)) * 3;

   // create the indices
   m_pIndices = new Index[m_nIndicesSize];

   // set the first vertex
   m_pVertices->fX = 0.0f;
   m_pVertices->fY = m_fRadius;
   m_pVertices->fZ = 0.0f;

   // set the normal vertex
   m_pVertices->fU = 0.0f;
   m_pVertices->fV = 1.0f;
   m_pVertices->fN = 0.0f;

   // create a vertex pointer to the first vertex
   Vertex * pVertex = m_pVertices + 1;
   // create a index pointer to the first index
   Index * pIndex = m_pIndices;

   // determine stack and slice deltas
   const double slice_delta_rad = MathHelper::DegToRad(360.0 / static_cast< double >(m_nSlices));
   const double stack_delta_rad = MathHelper::DegToRad(180.0 / static_cast< double >(m_nStacks));

   // create the stack starting angle
   double stack_rad = stack_delta_rad;

   // create the first set of indices
   for (unsigned int i = 0; i < m_nSlices; i++)
   {
      *(pIndex++) = 0;
      *(pIndex++) = (i + 1) % m_nSlices ? i + 2 : 1;
      *(pIndex++) = i + 1;
   }

   // loop across all the stacks
   for (unsigned int nStack = 1; m_nStacks > nStack; nStack++)
   {
      // calculate the y coordinate
      float fY = (float)cos(stack_rad) * m_fRadius;

      // calculate the temp radius
      float fRadius = (float)sin(stack_rad) * m_fRadius;

      // create a slice starting angle
      double slice_rad = 0.0;

      // calculate the start index
      unsigned int nStartIndex = ((nStack - 1) * m_nSlices) + 1;

      // loop across all the slices
      for (unsigned int nSlice = 0; m_nSlices > nSlice; nSlice++)
      {
         // calculate and set the x, y, and z coordinates
         pVertex->fX = (float)cos(slice_rad) * fRadius;
         pVertex->fY = fY;
         pVertex->fZ = (float)sin(slice_rad) * fRadius;

         // create a vector
         Vec3f vNormal(pVertex->fX, pVertex->fY, pVertex->fZ);
         // normalize the vector
         vNormal.Normalize();

         // set the normals
         pVertex->fU = vNormal.X();
         pVertex->fV = vNormal.Y();
         pVertex->fN = vNormal.Z();

         // update the slice angle
         slice_rad = slice_rad + slice_delta_rad;

         // update the vertex pointer
         pVertex++;

         // make sure not to create indices for the bottom set
         if (nStack < m_nStacks - 1)
         {
            // set the indices
            *(pIndex++) = nStartIndex + nSlice;
            *(pIndex++) = (nSlice + 1) % m_nSlices ? nStartIndex + nSlice + 1 : nStartIndex;
            *(pIndex++) = nStartIndex + nSlice + m_nSlices;
            
            *(pIndex++) = nStartIndex + nSlice + m_nSlices;
            *(pIndex++) = (nSlice + 1) % m_nSlices ? nStartIndex + nSlice + 1 : nStartIndex;
            *(pIndex++) = (nSlice + 1) % m_nSlices ? nStartIndex + nSlice + m_nSlices + 1: nStartIndex + m_nSlices;
         }
      }

      // update the stack angle
      stack_rad = stack_rad + stack_delta_rad;
   }

   // set the last vertex
   pVertex->fX = 0.0f;
   pVertex->fY = -m_fRadius;
   pVertex->fZ = 0.0f;

   // set vertext normals
   pVertex->fU = 0.0f;
   pVertex->fV = -1.0f;
   pVertex->fN = 0.0f;

   // create the last set of indices
   for (unsigned int i = 0; i < m_nSlices; i++)
   {
      *(pIndex++) = m_nVerticesSize - m_nSlices - 1 + i;
      *(pIndex++) = (i + 1) % m_nSlices ? m_nVerticesSize - m_nSlices + i : m_nVerticesSize - m_nSlices - 1;
      *(pIndex++) = m_nVerticesSize - 1;
   }
}

void CSphere::SetReflectionType( ReflectionType nType )
{
   // remove the previous reflection type
   switch (m_nReflection)
   {
   case REFLECT_STATIC:
      // nothing to do here

      break;

   case REFLECT_DYNAMIC_COPY_PIX:
      // nothing to do here

      break;

   case REFLECT_DYNAMIC_PBUFFER:
      {
         // obtain the current rendering context
         HGLRC hCurrentContext = wglGetCurrentContext();
         // change the context to the pbuffer
         wglMakeCurrent(m_pPBufferObjs->m_hPBufDC, m_pPBufferObjs->m_hGLRC);
         // release the pbuffer
         m_pPBuffer->wglReleasePbufferDCARB(m_pPBufferObjs->m_hPBuffer, m_pPBufferObjs->m_hPBufDC);
         // delete the pbuffer
         m_pPBuffer->wglDestroyPbufferARB(m_pPBufferObjs->m_hPBuffer);

         // restore the rendering context
         wglMakeCurrent(m_pPBufferObjs->m_hWndDC, hCurrentContext);

         // release the render context
         wglDeleteContext(m_pPBufferObjs->m_hGLRC);

         // release the dc
         ReleaseDC(m_pPBufferObjs->m_hWnd, m_pPBufferObjs->m_hPBufDC);

         // clear out the stored values
         m_pPBufferObjs->m_hGLRC = 0;
         m_pPBufferObjs->m_hPBufDC = 0;
         m_pPBufferObjs->m_hPBuffer = 0;
      }

      break;

   case REFLECT_DYNAMIC_RBUFFER:
      // release the render buffer and frame buffer
      m_pFrameBuffer->glDeleteFramebuffersEXT(1, &m_pFrameBufferObjs->m_nFrameBuffer);

      // clear the render and frame buffers
      m_pFrameBufferObjs->m_nFrameBuffer = 0;

      break;
   }

   // release the cube map image
   glDeleteTextures(1, &m_nCubeMapTexture);
   // create a new cube map image
   glGenTextures(1, &m_nCubeMapTexture);

   // set the reflection type
   m_nReflection = nType;

   // setup the new reflection types
   switch (m_nReflection)
   {
   case REFLECT_STATIC:
      // construct the images
      ConstructStaticImages();
      // auto generate the texture coordinates
      m_bAutoGenNorms = false;

      break;

   case REFLECT_DYNAMIC_COPY_PIX:
      {
         // bind the cubemap texture
         glBindTexture(GL_TEXTURE_CUBE_MAP, m_nCubeMapTexture);

         // generate no mipmaps
         glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_FALSE);

         // send the temp image to the card
         for (int i = 0; i < CM_MAX_TYPES; i++)
         {
            // send the image to the graphics card
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT + i,
                         0,
                         GL_RGBA8,
                         m_nDynCubemapSize,
                         m_nDynCubemapSize,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         NULL);
         }

         // setup the cube map texture parameters
         glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
         glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

         // allow the driver to auto generate the texture coords
         m_bAutoGenNorms = true;
      }

      break;

   case REFLECT_DYNAMIC_PBUFFER:
      {
         // bind the cubemap texture
         glBindTexture(GL_TEXTURE_CUBE_MAP, m_nCubeMapTexture);

         // generate no mipmaps
         glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_FALSE);

         // send a temp image to the card
         for (int i = 0; i < CM_MAX_TYPES; i++)
         {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT + i,
                         0,
                         GL_RGBA8,
                         m_nDynCubemapSize,
                         m_nDynCubemapSize,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         NULL);
         }

         // setup the cube map texture parameters
         glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
         glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

         // allow the driver to auto generate the texture coords
         m_bAutoGenNorms = true;

         // generate the pixel format attributes
         int nPixelFormatAttrib[] =
         {
            WGL_SUPPORT_OPENGL_ARB,       TRUE,
            WGL_ACCELERATION_ARB,         WGL_FULL_ACCELERATION_ARB,
            WGL_DRAW_TO_PBUFFER_ARB,      TRUE,
            WGL_BIND_TO_TEXTURE_RGBA_ARB, TRUE,
            WGL_PIXEL_TYPE_ARB,           WGL_TYPE_RGBA_ARB,
            WGL_RED_BITS_ARB,             8,
            WGL_GREEN_BITS_ARB,           8,
            WGL_BLUE_BITS_ARB,            8,
            WGL_ALPHA_BITS_ARB,           8,
            WGL_DEPTH_BITS_ARB,           0,
            WGL_DOUBLE_BUFFER_ARB,        FALSE,
            0
         };

         // determine the pixel format
         int nPixelFormat = 0;
         unsigned int nCount = 0;

         m_pPBuffer->wglChoosePixelFormatARB(m_pPBufferObjs->m_hWndDC,
                                             nPixelFormatAttrib,
                                             NULL,
                                             1,
                                             &nPixelFormat,
                                             &nCount);

         // generate the pbuffer attributes
         int nPBufferAttrib[] =
         {
            WGL_TEXTURE_FORMAT_ARB, WGL_TEXTURE_RGBA_ARB,
            WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_CUBE_MAP_ARB,
            0
         };

         // create the pbuffer
         m_pPBufferObjs->m_hPBuffer = m_pPBuffer->wglCreatePbufferARB(m_pPBufferObjs->m_hWndDC,
                                                                      nPixelFormat,
                                                                      m_nDynCubemapSize,
                                                                      m_nDynCubemapSize,
                                                                      nPBufferAttrib);

         // obtain a new dc for the pbuffer
         m_pPBufferObjs->m_hPBufDC = m_pPBuffer->wglGetPbufferDCARB(m_pPBufferObjs->m_hPBuffer);

         // create the rendering context
         m_pPBufferObjs->m_hGLRC = wglCreateContext(m_pPBufferObjs->m_hPBufDC);

         // obtain the current context
         HGLRC hCurrent = wglGetCurrentContext();
         // share the display lists so texture information can be used
         // across both render contexts
         wglShareLists(hCurrent, m_pPBufferObjs->m_hGLRC);
         // obtain both the projection and modelview matrices
         double dProjection[16], dModelView[16];
         glGetDoublev(GL_PROJECTION_MATRIX, dProjection);
         glGetDoublev(GL_MODELVIEW_MATRIX, dModelView);

         // make the pbuffer current
         wglMakeCurrent(m_pPBufferObjs->m_hPBufDC, m_pPBufferObjs->m_hGLRC);

         // setup the projection matrix
         glMatrixMode(GL_PROJECTION);
         glLoadMatrixd(dProjection);
         // setup the modelview matrix
         glMatrixMode(GL_MODELVIEW);
         glLoadMatrixd(dModelView);
         // setup the viewport
         glViewport(0, 0, m_nDynCubemapSize, m_nDynCubemapSize);

         // enable depth testing
         glEnable(GL_DEPTH_TEST);

         // restore the gl context
         wglMakeCurrent(m_pPBufferObjs->m_hWndDC, hCurrent);
      }

      break;

   case REFLECT_DYNAMIC_RBUFFER:
      {
         // bind the cubemap texture
         glBindTexture(GL_TEXTURE_CUBE_MAP, m_nCubeMapTexture);

         // generate no mipmaps
         glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_FALSE);

         // send the temp image to the card
         for (int i = 0; i < CM_MAX_TYPES; i++)
         {
            // send the image to the graphics card
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT + i,
                         0,
                         GL_RGBA8,
                         m_nDynCubemapSize,
                         m_nDynCubemapSize,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         NULL);
         }

         // setup the cube map texture parameters
         glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
         glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

         // allow the driver to auto generate the texture coords
         m_bAutoGenNorms = true;

         // generate the frame buffer and render buffer objects
         m_pFrameBuffer->glGenFramebuffersEXT(1, &m_pFrameBufferObjs->m_nFrameBuffer);
         //m_pFrameBuffer->glGenRenderbuffersEXT(1, &m_pFrameBufferObjs->m_nRenderBuffer);

         // obtain references to the render and frame buffers
         unsigned int & rFrameBuffer  = m_pFrameBufferObjs->m_nFrameBuffer;

         // start the frame buffer settings
         m_pFrameBuffer->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, rFrameBuffer);

         // attach to a basic frame buffer
         m_pFrameBuffer->glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                                   GL_COLOR_ATTACHMENT0_EXT,
                                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT,
                                                   m_nCubeMapTexture,
                                                   0);

         // validate the render buffer
         if (m_pFrameBuffer->glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
         {
            // delete the render buffers
            m_pFrameBuffer->glDeleteFramebuffersEXT(1, &rFrameBuffer);

            // reset the render and frame buffers
            rFrameBuffer = 0;
         }
      }

      break;
   }
}

void CSphere::ConstructStaticImages( )
{
   // bind the cubemap texture
   glBindTexture(GL_TEXTURE_CUBE_MAP, m_nCubeMapTexture);

   // determine the image size
   unsigned int nImgWidth   = m_oImageAttribs[CM_POS_X].width;
   unsigned int nImgHeight  = m_oImageAttribs[CM_POS_X].height;
   unsigned int nImgSize    = nImgWidth * nImgHeight * 4;
   unsigned int nImgCpySize = nImgWidth * 4;

   // create a vertically flipped image
   unsigned char * pVrtFlipImgBuf = new unsigned char[nImgSize];

   // reload all the images with cube mapping applied
   for (int i = 0; i < CM_MAX_TYPES; i++)
   {
      // create a temp image buffer pointer to the end
      unsigned char * pTmpImgBuf = pVrtFlipImgBuf + (nImgWidth * (nImgHeight - 1) * 4);
      // obtain the image buffer from the library
      const unsigned char * pImage = static_cast< unsigned char * >(m_oImageAttribs[i].pTexture.get());

      // copy the image into the temp image
      for (unsigned int j = 0; j < nImgHeight; j++)
      {
         // copy the image
         memcpy(pTmpImgBuf, pImage, nImgCpySize);
         // increase the image pointers
         pImage     += nImgCpySize;
         pTmpImgBuf -= nImgCpySize;
      }

      // generate mipmaps
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);

      // send the image to the graphics card
      GLenum format = m_oImageAttribs[i].format == GL_RGBA ? GL_RGBA : GL_BGRA_EXT;

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT + i,
                   0,
                   GL_RGBA,
                   nImgWidth,
                   nImgHeight,
                   0,
                   format,
                   GL_UNSIGNED_BYTE,
                   pVrtFlipImgBuf);
   }

   // release the image buffer
   delete [] pVrtFlipImgBuf;

   // setup the cube map texture parameters
   glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void CSphere::RenderFace( CubeMapEnumType nFace )
{
   // determine the type of rendering
   switch (m_nReflection)
   {
   case REFLECT_DYNAMIC_COPY_PIX:
      // copy the background buffer to the texture
      glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT + nFace,
                          0,
                          0, 0,
                          0, 0,
                          m_nDynCubemapSize, m_nDynCubemapSize);

      break;

   case REFLECT_DYNAMIC_PBUFFER:
      {
         // flush the commands from this context
         // not quite sure why this makes it work
         glFlush();
         // clear the depth buffer
         glClear(GL_DEPTH_BUFFER_BIT);

         int nPBufferList[] =
         {
            WGL_CUBE_MAP_FACE_ARB, WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + nFace,
            0, 0
         };

         // bind the texture to the correct face
         m_pPBuffer->wglSetPbufferAttribARB(m_pPBufferObjs->m_hPBuffer, nPBufferList);
      }

      break;

   case REFLECT_DYNAMIC_RBUFFER:
      // render to the face using the frame buffer
      m_pFrameBuffer->glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                                GL_COLOR_ATTACHMENT0_EXT,
                                                GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT + nFace,
                                                m_nCubeMapTexture,
                                                0);

      break;
   }
}

void CSphere::BeginDynamicOperations( )
{
   switch (m_nReflection)
   {
   case REFLECT_DYNAMIC_PBUFFER:
      {
         // obtain both the projection and modelview matrices
         double dProjection[16], dModelView[16];
         glGetDoublev(GL_PROJECTION_MATRIX, dProjection);
         glGetDoublev(GL_MODELVIEW_MATRIX, dModelView);

         // obtain the current context
         HGLRC hCurrent = wglGetCurrentContext();
         // change the context to the pbuffer
         wglMakeCurrent(m_pPBufferObjs->m_hPBufDC, m_pPBufferObjs->m_hGLRC);
         // save the context
         m_pPBufferObjs->m_hGLRC = hCurrent;

         // setup the projection matrix
         glMatrixMode(GL_PROJECTION);
         glLoadMatrixd(dProjection);
         // setup the modelview matrix
         glMatrixMode(GL_MODELVIEW);
         glLoadMatrixd(dModelView);

         // clear the buffers
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }

      break;

   case REFLECT_DYNAMIC_RBUFFER:
      // attach the frame buffer
      m_pFrameBuffer->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_pFrameBufferObjs->m_nFrameBuffer);

      break;
   }
}

void CSphere::EndDynamicOperations( )
{
   switch (m_nReflection)
   {
   case REFLECT_DYNAMIC_PBUFFER:
      {
         // obtain the current context
         HGLRC hCurrent = wglGetCurrentContext();
         // restore the pbuffer object
         wglMakeCurrent(m_pPBufferObjs->m_hWndDC, m_pPBufferObjs->m_hGLRC);
         // save the context
         m_pPBufferObjs->m_hGLRC = hCurrent;
      }

      break;

   case REFLECT_DYNAMIC_RBUFFER:
      // detach the frame buffer
      m_pFrameBuffer->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

      break;
   }
}

void CSphere::Release( )
{
   // release the specific rendering type
   switch (m_nReflection)
   {
   case REFLECT_DYNAMIC_PBUFFER:
      {
         // obtain the current rendering context
         HGLRC hCurrentContext = wglGetCurrentContext();
         // change the context to the pbuffer
         wglMakeCurrent(m_pPBufferObjs->m_hPBufDC, m_pPBufferObjs->m_hGLRC);
         // release the pbuffer
         m_pPBuffer->wglReleasePbufferDCARB(m_pPBufferObjs->m_hPBuffer, m_pPBufferObjs->m_hPBufDC);
         // delete the pbuffer
         m_pPBuffer->wglDestroyPbufferARB(m_pPBufferObjs->m_hPBuffer);

         // restore the rendering context
         wglMakeCurrent(m_pPBufferObjs->m_hWndDC, hCurrentContext);

         // release the render context
         wglDeleteContext(m_pPBufferObjs->m_hGLRC);

         // release the dc
         ReleaseDC(m_pPBufferObjs->m_hWnd, m_pPBufferObjs->m_hPBufDC);
      }

      break;

   case REFLECT_DYNAMIC_RBUFFER:
      // release the render buffer and frame buffer
      m_pFrameBuffer->glDeleteFramebuffersEXT(1, &m_pFrameBufferObjs->m_nFrameBuffer);

      break;
   }

   // release the cube map image
   glDeleteTextures(1, &m_nCubeMapTexture);
}