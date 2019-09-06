// includes
#include "SkyBox.h"

#include <Windows.h>
#include <GL\GL.h>
#include <Memory.h>

#define GL_CLAMP_TO_EDGE 0x812F

SkyBox::SkyBox( )

{
}

SkyBox::~SkyBox( )
{
   // release the resources
   glDeleteTextures(MAX_IMAGE_SIZE, m_TexIDs);
}

void SkyBox::InitImages( const char * pTop,
                         const char * pBottom,
                         const char * pFront,
                         const char * pBack,
                         const char * pLeft,
                         const char * pRight )
{
   // load all six images
   m_aImages[IMAGE_TOP_ENUM]    = ReadTexture< uint8_t >(pTop, GL_RGB);
   m_aImages[IMAGE_BOTTOM_ENUM] = ReadTexture< uint8_t >(pBottom, GL_RGB);
   m_aImages[IMAGE_FRONT_ENUM]  = ReadTexture< uint8_t >(pFront, GL_RGB);
   m_aImages[IMAGE_BACK_ENUM]   = ReadTexture< uint8_t >(pBack, GL_RGB);
   m_aImages[IMAGE_LEFT_ENUM]   = ReadTexture< uint8_t >(pLeft, GL_RGB);
   m_aImages[IMAGE_RIGHT_ENUM]  = ReadTexture< uint8_t >(pRight, GL_RGB);

   // enable the specific attributes
   for (int i = 0; i < MAX_IMAGE_SIZE; i++)
   {
      // bind the texture
      glGenTextures(1, &m_TexIDs[i]);
      glBindTexture(GL_TEXTURE_2D, m_TexIDs[i]);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_aImages[i].width, m_aImages[i].height, 0,
                   GL_RGB, GL_UNSIGNED_BYTE, m_aImages[i].pTexture.get());
      // set the texture attributes
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   }
}

void SkyBox::Render( const double & rElapsedTime )
{
   // create a vertex field
   // a LHS is assumed for the skybox
   float fVertices[][12] =
   { 
      { 5.0f, 5.0f, 5.0f, -5.0f, 5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f, 5.0f, -5.0f },     // Top
      { 5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, 5.0f, 5.0f, -5.0f, 5.0f }, // Bottom
      { 5.0f, -5.0f, 5.0f, -5.0f, -5.0f, 5.0f, -5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f },     // Front
      { -5.0f, -5.0f, -5.0f, 5.0f, -5.0f, -5.0f, 5.0f, 5.0f, -5.0f, -5.0f, 5.0f, -5.0f }, // Back
      { -5.0f, -5.0f, 5.0f, -5.0f, -5.0f, -5.0f, -5.0f, 5.0f, -5.0f, -5.0f, 5.0f, 5.0f }, // Right
      { 5.0f, -5.0f, -5.0f, 5.0f, -5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, -5.0f },     // Left
   };

   // create the texture coords
   float fTexCoords[][8] = 
   { 
      { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f }, // texture coordinates
   };

   // disable the depth test
   glDisable(GL_DEPTH_TEST);
   // enable specific attributes
   glEnable(GL_TEXTURE_2D);
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   // push a matrix onto the stack
   glPushMatrix();

   // create a local matrix
   float fModelView[16];
   // get the model view matrix
   glGetFloatv(GL_MODELVIEW_MATRIX, fModelView);

   // modify the model view translation
   fModelView[12] = 0.0f;
   fModelView[13] = 0.0f;
   fModelView[14] = 0.0f;

   // set the model view matrix
   glLoadMatrixf(fModelView);

   // draw all the vertices
   for (int i = 0; i < MAX_IMAGE_SIZE; i++)
   {
      // bind the texture
      glBindTexture(GL_TEXTURE_2D, m_TexIDs[i]);
      // associate the vertices and tex coords
      glVertexPointer(3, GL_FLOAT, 0, fVertices[i]);
      glTexCoordPointer(2, GL_FLOAT, 0, fTexCoords[0]);
      // draw the images
      glDrawArrays(GL_QUADS, 0, 4);
   }

   // pop the matrix from the stack
   glPopMatrix();

   // enable the depth test
   glEnable(GL_DEPTH_TEST);
   // disable specific attributes
   glDisable(GL_TEXTURE_2D);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void SkyBox::Update( const double & rElapsedTime )
{
   /* Do Nothing */
}