// local includes
#include "CFont.h"
#include "Matrix.h"

CFont::CFont( unsigned int unLayer,
              const Vec3f &rPos,
              const std::string &rFile,
              unsigned int unCharWidth,
              unsigned int unCharHeight,
              const std::string &rText,
              float fSize,
              Alignment nAlign ) :
m_fSize           ( fSize ),
m_oPosition       ( rPos ),
m_nAlignment      ( nAlign ),
m_oText           ( rText ),
m_unCharWidth     ( unCharWidth ),
m_unCharHeight    ( unCharHeight ),
m_nNumOfLines     ( 0 ),
m_oImage          ( 0 ),
m_oImageAttrib    ( ReadTexture< uint8_t >(rFile.c_str(), GL_RGBA) )
{
   // obtain the deception image
   if (m_oImageAttrib.pTexture)
   {
      // load the image to the card
      glGenTextures(1, &m_oImage);
      glBindTexture(GL_TEXTURE_2D, m_oImage);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_oImageAttrib.width, m_oImageAttrib.height, 0,
                   GL_RGBA, GL_UNSIGNED_BYTE, m_oImageAttrib.pTexture.get());

      // set up the texture parameters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

      // no longer need the texture bound
      glBindTexture(GL_TEXTURE_2D, 0);

      // create new text and vert data
      ConstructText();
   }
}

CFont::CFont( const CFont & font ) :
m_fSize           ( font.m_fSize ),
m_oPosition       ( font.m_oPosition ),
m_nAlignment      ( font.m_nAlignment ),
m_oText           ( font.m_oText ),
m_unCharWidth     ( font.m_unCharWidth ),
m_unCharHeight    ( font.m_unCharHeight ),
m_nNumOfLines     ( font.m_nNumOfLines ),
m_vText           ( font.m_vText ),
m_oImage          ( 0 ),
m_oImageAttrib    ( font.m_oImageAttrib )
{
   // obtain the deception image
   if (m_oImageAttrib.pTexture)
   {
      // load the image to the card
      glGenTextures(1, &m_oImage);
      glBindTexture(GL_TEXTURE_2D, m_oImage);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_oImageAttrib.width, m_oImageAttrib.height, 0,
                   GL_RGBA, GL_UNSIGNED_BYTE, m_oImageAttrib.pTexture.get());

      // set up the texture parameters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

      // no longer need the texture bound
      glBindTexture(GL_TEXTURE_2D, 0);

      // create new text and vert data
      ConstructText();
   }
}

CFont::~CFont( )
{
   // delete the texture
   glDeleteTextures(1, &m_oImage);
}

void CFont::Draw( const double &rElapsedTime )
{
   if (m_vText.size())
   {
      // enable textures
      glEnable(GL_BLEND);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_ALPHA_TEST);
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);

      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // push a new matrix on the stack
      glPushMatrix();

      // multiply the current matrix by the translated matrix
      glMultMatrixf(Matrixf::Translate(m_oPosition));

      // bind the correct texture
      glBindTexture(GL_TEXTURE_2D, m_oImage);

      // get the begin iterator to the text
      TexVertVector::iterator itTextBeg = m_vText.begin();
      TexVertVector::iterator itTextEnd = m_vText.end();

      // draw the text
      for (; itTextBeg != itTextEnd && (size_t)std::distance(m_vText.begin(), itTextBeg) < m_nNumOfLines; ++itTextBeg)
      {
         // enable vertex and texture coordinate pointers
         glTexCoordPointer(2, GL_FLOAT, 0, itTextBeg->m_pTexCoords.get() + 1);
         glVertexPointer(3, GL_FLOAT, 0, itTextBeg->m_pVertices.get() + 1);

         // draw the quads
         glDrawArrays(GL_TRIANGLES, 0, itTextBeg->m_unNumOfVerts);
      }

      // pop the matrix from the stack
      glPopMatrix();

      // disable textures
      glDisable(GL_BLEND);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_ALPHA_TEST);
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
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
   float dDeltaS = (float)m_unCharWidth / m_oImageAttrib.width;
   float dDeltaT = (float)m_unCharHeight / m_oImageAttrib.height;

   // form the lines
   m_nNumOfLines = FormLines(vLines);

   // setup the text index and size
   unsigned int nTextVecSize  = static_cast< unsigned int >(m_vText.size());
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
      unsigned int nDataSize = unTextLength * 6;

      // determine the number of points
      pData->m_unNumOfVerts = nDataSize;
      pData->m_unNumOfTexCoords = nDataSize;
      
      if (!pData->m_pVertices ||
          nDataSize > (unsigned int)pData->m_pVertices.get()[0])
      {
         // create the new data
         pData->m_pVertices.reset(
            new float[pData->m_unNumOfVerts * 3 + 1],
            [ ] ( const float * const p ) { delete [] p; });
         pData->m_pTexCoords.reset(
            new float[pData->m_unNumOfTexCoords * 2 + 1],
            [ ] ( const float * const p ) { delete [] p; });

         // set the data sizes
         pData->m_pVertices.get()[0] = (float)nDataSize;
         pData->m_pTexCoords.get()[0] = (float)nDataSize;
      }

      // determine a starting position
      Vec3f oPosition(0.0f, -(unLines * fCharHeight), 0.0f);

      switch (m_nAlignment)
      {
      case ALIGN_RIGHT:
         oPosition.X() = -(itLine->size() * fCharWidth);
         break;
      case ALIGN_CENTER:
         oPosition.X() = -((itLine->size() * fCharWidth) * 0.5f);
         break;
      }

      // set up data pointers
      float *pVerts = pData->m_pVertices.get() + 1;
      float *pTexCoords = pData->m_pTexCoords.get() + 1;

      // copy the data into the vectors
      for (unsigned int unPos = 0;
           unPos < unTextLength;
           unPos++)
      {
         // set point 1
         *pVerts        = oPosition.X();
         *(pVerts + 1)  = oPosition.Y();
         *(pVerts + 2)  = oPosition.Z();
         // set point 2
         *(pVerts + 3)  = oPosition.X();
         *(pVerts + 4)  = oPosition.Y() - fCharHeight;
         *(pVerts + 5)  = oPosition.Z();
         // set point 3
         *(pVerts + 6)  = oPosition.X() + fCharWidth;
         *(pVerts + 7)  = oPosition.Y() - fCharHeight;
         *(pVerts + 8)  = oPosition.Z();
         // set point 4
         *(pVerts + 9)  = oPosition.X();
         *(pVerts + 10) = oPosition.Y();
         *(pVerts + 11) = oPosition.Z();
         // set point 5
         *(pVerts + 12) = oPosition.X() + fCharWidth;
         *(pVerts + 13) = oPosition.Y() - fCharHeight;
         *(pVerts + 14) = oPosition.Z();
         // set point 6
         *(pVerts + 15) = oPosition.X() + fCharWidth;
         *(pVerts + 16) = oPosition.Y();
         *(pVerts + 17) = oPosition.Z();

         // update the position vector
         oPosition.X() += fCharWidth;

         // determine the character
         unsigned char unChar = itLine->at(unPos) - 32;

         // make sure the character is defined within the limits of the mapping
         if (unChar > 94)
         {
            unChar = 99;
         }

         // determine the texture coords
         float dS = (unChar % 10) * dDeltaS;
         float dT = 1.0f - ((unChar / 10) * dDeltaT);

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
         *(pTexCoords + 6) = dS;
         *(pTexCoords + 7) = dT;
         // set tex coord 5
         *(pTexCoords + 8) = dS + dDeltaS;
         *(pTexCoords + 9) = dT - dDeltaT;
         // set tex coord 6
         *(pTexCoords + 10) = dS + dDeltaS;
         *(pTexCoords + 11) = dT;

         // increase the data pointers
         pVerts += 18;
         pTexCoords += 12;
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

size_t CFont::FormLines( Lines & rLines )
{
   // local(s)
   size_t num_of_lines = 0;
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
         if (!sLine.empty()) ++num_of_lines;
         rLines.push_back(sLine);
         sLine = "";
      }
   }

   if (sLine.size())
   {
      rLines.push_back(sLine);
      ++num_of_lines;
   }

   return num_of_lines;
}