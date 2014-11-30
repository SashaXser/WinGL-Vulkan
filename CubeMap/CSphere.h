#ifndef _C_SPHERE_H_
#define _C_SPHERE_H_

// includes
#include "Types.h"
#include "StdIncludes.h"

// wingl includes
#include "Vector3.h"
#include "ReadTexture.h"

#include <string>
#include <cstdint>

// forward declarations
class CPBuffer;
class CFrameBuffer;

class CSphere
{
public:
   // public enumeration types
   typedef enum Reflection
   {
      REFLECT_STATIC           = 0x01,  // six images will be generated and never updated
      REFLECT_DYNAMIC_COPY_PIX = 0x02,  // six images will be rendered every frame using copy pixels
      REFLECT_DYNAMIC_PBUFFER  = 0x04,  // six images will be rendered using the pbuffer
      REFLECT_DYNAMIC_RBUFFER  = 0x08   // six images will be rendered using a off-screen render buffer
   } ReflectionType;

   typedef enum CubeMapEnumType
   {
      CM_POS_X     = 0,
      CM_NEG_X     = 1,
      CM_POS_Y     = 2,
      CM_NEG_Y     = 3,
      CM_POS_Z     = 4,
      CM_NEG_Z     = 5,
      CM_MAX_TYPES = 6
   } CubeMapEnum;

   typedef enum Draw
   {
      DRAW_FILL,     // draws a filled sphere
      DRAW_WIRE,     // draws a wireframe sphere
      DRAW_TEX_QUAD, // draws the 6 texture quads
      DRAW_MAX_TYPE  // maximum draw types
   } DrawType;

   // constructor / deconstructor
               CSphere( HWND hWnd,
                        float fRadius        = 1.0f,
                        unsigned int nSlices = 32,
                        unsigned int nStacks = 32,
                        const char * pPosX   = NULL,
                        const char * pNegX   = NULL,
                        const char * pPosY   = NULL,
                        const char * pNegY   = NULL,
                        const char * pPosZ   = NULL,
                        const char * pNegZ   = NULL );
              ~CSphere( );

   // accessors
   float          GetRadius( ) const { return m_fRadius; };
   unsigned int   GetSlices( ) const { return m_nSlices; };
   unsigned int   GetStacks( ) const { return m_nStacks; };

   // returns the material parameter
   const Material &     GetMaterial( ) const { return m_oMaterial; };

   // sets the material properties
   void           SetMaterial( const Material & rMaterial ) { m_oMaterial = rMaterial; };

   // reconstructs the sphere
   void           ConstructSphere( float fRadius,
                                   unsigned int nSlices,
                                   unsigned int nStacks );

   // daws the object
   void           Draw( const double & rElapsedTime ) const { (this->*m_pDrawFunctor)(rElapsedTime); };

   // updates the sphere
   void           Update( const double & rElapsedTime,
                          const Vec3f & rEyePosition );
   // begins /ends special dynamic operations
   void           BeginDynamicOperations( );
   void           EndDynamicOperations( );

   // sets / gets the sphere draw type
   void           SetDrawType( DrawType nType );
   DrawType       GetDrawType( ) { return m_nDrawType; };

   // sets / gets the reflection type
   void           SetReflectionType( ReflectionType nType );
   ReflectionType GetReflectionType( ) { return m_nReflection; };

   // renders the specified face
   void           RenderFace( CubeMapEnumType nFace );

   // obtains the cubemap texture id
   unsigned int   GetCubeMapTexID( ) { return m_nCubeMapTexture; };

   // returns the dynamic cubemap texture size
   unsigned int   GetDynamicCubemapTexSize( ) { return m_nDynCubemapSize; };

   // changes the dynamic cubemap texture size
   void           IncreaseDynCubemapTexSize( );
   void           DecreaseDynCubemapTexSize( );

private:
   // private typedefs
   typedef void (CSphere::* DrawFunctor)( const double & rElapsedTime ) const;

   // private forward declarations
   struct PBufferObjs;
   struct FrameBufferObjs;

   // private member functions
   // constructs the sphere with the given parameters
   void           ConstructSphere( );

   // flips the normal image vertically and sends it to the card
   // make sure that the static images have been previously
   void           ConstructStaticImages( );

   // draws the sphere or quads
   void           DrawSphere( const double & rElapsedTime ) const;
   void           DrawQuads( const double & rElapsedTime ) const;

   // called to release the memory from the graphics card
   void     Release( );

   // private member variables
   float          m_fRadius;
   unsigned int   m_nSlices;
   unsigned int   m_nStacks;

   // private geometry
   bool                 m_bAutoGenNorms;
   Index *              m_pIndices;
   Vertex *             m_pVertices;
   Vec3f                m_vEyeDir;
   Material             m_oMaterial;
   DrawType             m_nDrawType;
   CPBuffer *           m_pPBuffer;
   PBufferObjs *        m_pPBufferObjs;
   DrawFunctor          m_pDrawFunctor;
   unsigned int         m_nVerticesSize;
   unsigned int         m_nIndicesSize;
   unsigned int         m_nCubeMapTexture;
   unsigned int         m_nDynCubemapSize;
   CFrameBuffer *       m_pFrameBuffer;
   ReflectionType       m_nReflection;
   FrameBufferObjs *    m_pFrameBufferObjs;

   // private cubemap static images
   const TextureData< uint8_t >  m_oImageAttribs[CM_MAX_TYPES];
   const std::string *           m_sStaticImages[CM_MAX_TYPES];

};

inline void CSphere::SetDrawType( DrawType nType )
{
   // set the type
   m_nDrawType = nType;
   // set the function ptr
   m_pDrawFunctor = m_nDrawType == DRAW_TEX_QUAD ? &CSphere::DrawQuads :
                                                   &CSphere::DrawSphere;
}

inline void CSphere::IncreaseDynCubemapTexSize( )
{
   if (m_nReflection != REFLECT_STATIC && m_nDynCubemapSize < 0x2000)
   {
      // increase the size
      m_nDynCubemapSize <<= 1;
      // acquire new images
      SetReflectionType(m_nReflection);
   }
}

inline void CSphere::DecreaseDynCubemapTexSize( )
{
   if (m_nReflection != REFLECT_STATIC && m_nDynCubemapSize > 0x0004)
   {
      // decrease the size
      m_nDynCubemapSize >>= 1;
      // acquire new images
      SetReflectionType(m_nReflection);
   }
}

#endif // _C_SPHERE_H_