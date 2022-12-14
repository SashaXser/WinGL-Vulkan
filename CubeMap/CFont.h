#ifndef _C_F0NT_H_   
#define _C_FONT_H_

// wingl includes
#include "Vector.h"
#include "ReadTexture.h"

// std includes
#include <string>
#include <vector>
#include <memory>
#include <cstdint>

// gl includes
#include <Windows.h>
#include <GL/glew.h>
#include <GL/GL.h>

class CFont
{
public:
   // public enumerations
   typedef enum AlignmentTag
   {
      ALIGN_LEFT,
      ALIGN_RIGHT,
      ALIGN_CENTER,
      MAX_ALIGNMENTS
   } Alignment;

   // constructor / deconstructor
   CFont( unsigned int unLayer,
          const Vec3f &rPos,
          const std::string &rFile,
          unsigned int unCharWidth,
          unsigned int unCharHeight,
          const std::string &rText = "",
          float fSize = 1,
          Alignment nAlign = ALIGN_LEFT );
   CFont( const CFont & font );
   virtual ~CFont( );

   // basic string operations
   void operator += ( const std::string &rText );
   void operator += ( const char *pText );
   void operator += ( char c );
   void operator = ( const std::string &rText );
   void operator = ( const char *pText );
   void operator = ( char c );

   // drawing functions
   void Draw( const double &rElapsedTime );
   void Update( const double &rElapsedTime );

   // accessors / modifiers
   void SetText( const std::string &rText );
   void SetSize( float fSize );
   void SetPosition( const Vec3f & pos );

   const std::string & GetText( ) const;
   float GetSize( ) const;
   const Vec3f & GetPosition( ) const;
   unsigned int GetPixelWidth( ) const;
   unsigned int GetPixelHeight( ) const;

private:
   // private data types
   typedef struct TextVerticesTag
   {
      unsigned int               m_unNumOfVerts;
      unsigned int               m_unNumOfTexCoords;
      std::shared_ptr< float >   m_pTexCoords;
      std::shared_ptr< float >   m_pVertices;
   } TextVertices;

   typedef std::vector< TextVertices > TexVertVector;
   typedef std::vector< std::string > Lines;

   // private member functions
   void ConstructText( );
   void ConstructHorizontal( );

   size_t FormLines( Lines & rLines );

   // private member data
   float                m_fSize;
   Vec3f                m_oPosition;
   Alignment            m_nAlignment;
   std::string          m_oText;
   unsigned int         m_unCharWidth;
   unsigned int         m_unCharHeight;
   size_t               m_nNumOfLines;
   TexVertVector        m_vText;

   GLuint                        m_oImage;
   const TextureData< uint8_t >  m_oImageAttrib;

};

inline void CFont::SetText( const std::string &rText )
{
   m_oText = rText;

   ConstructText();
}

inline const std::string & CFont::GetText( ) const
{
   return m_oText;
}

inline void CFont::SetSize( float fSize )
{
   if (fSize != m_fSize)
   {
      m_fSize = fSize;

      ConstructText();
   }
}

inline float CFont::GetSize( ) const
{
   return m_fSize;
}

inline void CFont::SetPosition( const Vec3f & pos )
{
   m_oPosition = pos;
}

inline const Vec3f & CFont::GetPosition( ) const
{
   return m_oPosition;
}

inline unsigned int CFont::GetPixelHeight( ) const
{
   return (unsigned int)((float)m_unCharHeight * m_fSize);
}

inline unsigned int CFont::GetPixelWidth( ) const
{
   return (unsigned int)((float)m_unCharWidth * m_fSize);
}

inline void CFont::operator += ( const char *pText )
{
   m_oText += pText;

   ConstructText();
}

inline void CFont::operator += ( const std::string &rText )
{
   m_oText += rText;

   ConstructText();
}

inline void CFont::operator += ( char c )
{
   m_oText += c;

   ConstructText();
}

inline void CFont::operator = ( const char *pText )
{
   if (m_oText != pText)
   {
      m_oText = pText;

      ConstructText();
   }
}

inline void CFont::operator = ( const std::string &rText )
{
   if (m_oText != rText)
   {
      m_oText = rText;

      ConstructText();
   }
}

inline void CFont::operator = ( char c )
{
   if (m_oText.size() > 1 ||
       m_oText.size() == 1 && m_oText.at(0) != c)
   {
      m_oText = c;

      ConstructText();
   }
}

#endif // _C_FONT_H_
