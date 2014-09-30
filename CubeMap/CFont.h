#ifndef _C_F0NT_H_   
#define _C_FONT_H_

#include "ImageLibrary.h"

#include "Vector.h"

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
                               const Vector &rPos,
                               const std::string &rFile,
                               unsigned int unCharWidth,
                               unsigned int unCharHeight,
                               const std::string &rText = "",
                               float fSize = 1,
                               Alignment nAlign = ALIGN_LEFT );
                        CFont( const CFont * pFont );
   virtual             ~CFont( );

   // basic string operations
   void                 operator += ( const std::string &rText );
   void                 operator += ( const char *pText );
   void                 operator += ( char c );
   void                 operator = ( const std::string &rText );
   void                 operator = ( const char *pText );
   void                 operator = ( char c );

   // drawing functions
   void                 Draw( const double &rElapsedTime );
   void                 Update( const double &rElapsedTime );

   // accessors / modifiers
   void                 SetText( const std::string &rText );
   void                 SetSize( float fSize );

   const std::string &  GetText( ) const;
   float                GetSize( ) const;
   unsigned int         GetPixelWidth( ) const;
   unsigned int         GetPixelHeight( ) const;

private:
   // private data types
   typedef struct TextVerticesTag
   {
      unsigned int      m_unNumOfVerts;
      unsigned int      m_unNumOfTexCoords;
      double *          m_pTexCoords;
      double *          m_pVertices;
   } TextVertices;

   typedef std::vector< TextVertices > TexVertVector;
   typedef std::vector< std::string > Lines;

   // private member functions
   void                 ConstructText( );
   void                 ConstructHorizontal( );

   void                 FormLines( Lines & rLines );

   // private member data
   float                m_fSize;
   Vector               m_oPosition;
   Alignment            m_nAlignment;
   std::string          m_oText;
   unsigned int         m_unCharWidth;
   unsigned int         m_unCharHeight;
   unsigned int         m_nNumOfLines;
   TexVertVector        m_vText;
   ImageLibrary::Image  m_oImage;

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