#include "CFont.h"
#include "Matrix.h"

#include <Windows.h>
#include <GL\GL.h>

CFont::CFont( unsigned int unLayer,
              const Vector &rPos,
              const std::string &rFile,
              unsigned int unCharWidth,
              unsigned int unCharHeight,
              const std::string &rText,
              float fSize,
              Alignment nAlign ) :
m_oText           ( rText ),
m_unCharWidth     ( unCharWidth ),
m_unCharHeight    ( unCharHeight ),
m_nAlignment      ( nAlign ),
m_fSize           ( fSize ),
m_oPosition       ( rPos )
{
   // nullify the image info
   memset(&m_oImage, 0x00, sizeof(ImageLibrary::Image));

   // obtain the deception image
   if (const ImageLibrary::Image *pImage = ImageLibrary::Instance()->Load(rFile, true, false))
   {
      // copy the image info
      m_oImage = *pImage;

      // set up the texture parameters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

      // create new text and vert data
      ConstructText();
   }
}

CFont::CFont( const CFont * pFont ) :
m_oText           ( pFont->m_oText ),
m_unCharWidth     ( pFont->m_unCharWidth ),
m_unCharHeight    ( pFont->m_unCharHeight ),
m_nAlignment      ( pFont->m_nAlignment ),
m_fSize           ( pFont->m_fSize ),
m_oPosition       ( pFont->m_oPosition )
{
   // nullify the image info
   memset(&m_oImage, 0x00, sizeof(ImageLibrary::Image));

   // obtain the deception image
   if (const ImageLibrary::Image *pImage = ImageLibrary::Instance()->LoadTGA(pFont->m_oImage.m_pImage->GetFileName()))
   {
      // copy the image info
      m_oImage = *pImage;

      // create new tex and vert data
      ConstructText();
   }
}

CFont::~CFont( )
{
   // remove the texture from memory
   ImageLibrary::Instance()->UnloadImage(m_oImage.m_unImageID);

   for (TexVertVector::iterator itText = m_vText.begin();
        itText != m_vText.end();
        itText++)
   {
      delete [] itText->m_pVertices;
      delete [] itText->m_pTexCoords;
   }
}

void CFont::Draw( const double &rElapsedTime )
{
   if (m_vText.size())
   {
      // enable textures
      glEnable(GL_BLEND);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_ALPHA_TEST);
      glEnable(GL_VERTEX_ARRAY);
      glEnable(GL_TEXTURE_COORD_ARRAY);

      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // push a new matrix on the stack
      glPushMatrix();

      // create a translated matrix to the position of the text
      Matrix mTrans;

      MATRIX_TRANSLATE(mTrans,
                       m_oPosition.m_fU,
                       m_oPosition.m_fV,
                       m_oPosition.m_fN);

      // multiply the current matrix by the translated matrix
      glMultMatrixf(mTrans);

      // bind the correct texture
      glBindTexture(GL_TEXTURE_2D, m_oImage.m_unImageID);

      // get the begin iterator to the text
      TexVertVector::iterator itTextBeg = m_vText.begin();
      TexVertVector::iterator itTextEnd = m_vText.end();

      // draw the text
      for (; itTextBeg != itTextEnd; ++itTextBeg)
      {
         // enable vertex and texture coordinate pointers
         glTexCoordPointer(2, GL_DOUBLE, 0, itTextBeg->m_pTexCoords + 1);
         glVertexPointer(3, GL_DOUBLE, 0, itTextBeg->m_pVertices + 1);

         // draw the quads
         glDrawArrays(GL_QUADS, 0, itTextBeg->m_unNumOfVerts);
      }

      // pop the matrix from the stack
      glPopMatrix();

      // disable textures
      glDisable(GL_BLEND);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_ALPHA_TEST);
      glDisable(GL_VERTEX_ARRAY);
      glDisable(GL_TEXTURE_COORD_ARRAY);
   }
}

void CFont::Update( const double &rElapsedTime )
{
}

void CFont::ConstructText( )
{
   // delete the previous text holder
   //for (TexVertVector::iterator itText = m_vText.begin();
   //     itText != m_vText.end();
   //     itText++)
   //{
   //   delete [] itText->m_pVertices;
   //   delete [] itText->m_pTexCoords;
   //}

   //// clear the vector
   //m_vText.clear();

   // construct the new text
   ConstructHorizontal();
}

void CFont::ConstructHorizontal( )
{
   // local(s)
   Lines        vLines;
   unsigned int unLines = 0;

   // determine the character size
   float fCharWidth = (float)m_unCharWidth * m_fSize;
   float fCharHeight = (float)m_unCharHeight * m_fSize;

   // determine the delta s, t coords
   double dDeltaS = (double)m_unCharWidth / m_oImage.m_pImage->GetSize().m_nWidth;
   double dDeltaT = (double)m_unCharHeight / m_oImage.m_pImage->GetSize().m_nHeight;

   // form the lines
   FormLines(vLines);

   // set the number of lines
   m_nNumOfLines = vLines.size();

   // setup the text index and size
   unsigned int nTextVecSize  = m_vText.size();
   unsigned int nTextVecIndex = 0;

   // create a local text vertice data
   TextVertices oData;
   TextVertices * pData = &oData;

   for (Lines::iterator itLine = vLines.begin();
        itLine != vLines.end();
        itLine++, unLines++)
   {
      // local(s)
      unsigned int unTextLength = (unsigned int)itLine->size();

      // make sure there is some data
      if (!unTextLength)
      {
         unLines++;
         continue;
      }

      // determine how to address the data
      if (nTextVecIndex == nTextVecSize)
      {
         // set the reference to the data
         pData = &oData;
         // reset the pointers
         pData->m_pVertices = NULL;
         pData->m_pTexCoords = NULL;
      }
      else
      {
         // get the data from the vector
         pData = &m_vText.at(nTextVecIndex);
      }

      // determine the data sizes
      unsigned int nDataSize = unTextLength * 4;

      // determine the number of points
      pData->m_unNumOfVerts = nDataSize;
      pData->m_unNumOfTexCoords = nDataSize;
      
      if (!pData->m_pVertices || nDataSize > (unsigned int)pData->m_pVertices[0])
      {
         // release the previous resources
         delete [] pData->m_pVertices;
         delete [] pData->m_pTexCoords;

         // create the new data
         pData->m_pVertices = new double[pData->m_unNumOfVerts * 3 + 1];
         pData->m_pTexCoords = new double[pData->m_unNumOfTexCoords * 2 + 1];

         // set the data sizes
         pData->m_pVertices[0] = (double)nDataSize;
         pData->m_pTexCoords[0] = (double)nDataSize;
      }

      // determine a starting position
      Vector oPosition(0.0f, -(unLines * fCharHeight), 0.0f);

      switch (m_nAlignment)
      {
      case ALIGN_RIGHT:
         oPosition.m_fU = -(itLine->size() * fCharWidth);
         break;
      case ALIGN_CENTER:
         oPosition.m_fU = -((itLine->size() * fCharWidth) * 0.5f);
         break;
      }

      // set up data pointers
      double *pVerts = pData->m_pVertices + 1;
      double *pTexCoords = pData->m_pTexCoords + 1;

      // copy the data into the vectors
      for (unsigned int unPos = 0;
           unPos < unTextLength;
           unPos++)
      {
         // set point 1
         *pVerts        = oPosition.m_fU;
         *(pVerts + 1)  = oPosition.m_fV;
         *(pVerts + 2)  = oPosition.m_fN;
         // set point 2
         *(pVerts + 3)  = oPosition.m_fU;
         *(pVerts + 4)  = oPosition.m_fV - fCharHeight;
         *(pVerts + 5)  = oPosition.m_fN;
         // set point 3
         *(pVerts + 6)  = oPosition.m_fU + fCharWidth;
         *(pVerts + 7)  = oPosition.m_fV - fCharHeight;
         *(pVerts + 8)  = oPosition.m_fN;
         // set point 4
         *(pVerts + 9)  = oPosition.m_fU + fCharWidth;
         *(pVerts + 10) = oPosition.m_fV;
         *(pVerts + 11) = oPosition.m_fN;

         // update the position vector
         oPosition.m_fU += fCharWidth;

         // determine the character
         unsigned char unChar = itLine->at(unPos) - 32;

         // make sure the character is defined within the limits of the mapping
         if (unChar > 94)
         {
            unChar = 99;
         }

         // determine the texture coords
         double dS = (unChar % 10) * dDeltaS;
         double dT = 1.0 - ((unChar / 10) * dDeltaT);

         // set tex coord 1
         *pTexCoords       = dS;
         *(pTexCoords + 1) = dT;
         // set tex coord 2
         *(pTexCoords + 2) = dS;
         *(pTexCoords + 3) = dT - dDeltaT;
         // set tex coord 3
         *(pTexCoords + 4) = dS + dDeltaS;
         *(pTexCoords + 5) = dT - dDeltaT;
         // set tex coord 4
         *(pTexCoords + 6) = dS + dDeltaS;
         *(pTexCoords + 7) = dT;

         // increase the data pointers
         pVerts += 12;
         pTexCoords += 8;
      }

      // add to the vector
      if (nTextVecIndex == nTextVecSize)
      {
         m_vText.push_back(*pData);
      }
      else
      {
         nTextVecIndex++;
      }
   }
}

void CFont::FormLines( Lines & rLines )
{
   // local(s)
   std::string sLine = "";

   for (unsigned int unPos = 0;
        unPos < m_oText.size();
        unPos++)
   {
      char nChar = m_oText.at(unPos);

      if (nChar != '\n')
      {
         sLine += nChar;
      }
      else
      {
         rLines.push_back(sLine);
         sLine = "";
      }
   }

   if (sLine.size())
   {
      rLines.push_back(sLine);
   }
}