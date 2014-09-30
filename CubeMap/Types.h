#ifndef _TYPES_H_
#define _TYPES_H_

typedef struct VertexType
{
   float fX, fY, fZ;    // vertex position
   float fU, fV, fN;    // vertex normal
   float fS, fT, fR;    // vertex texture coordinates
} Vertex;

typedef unsigned int Index;

typedef enum MaterialFlagsType
{
   MF_NONE        = 0x00,
   MF_AMBIENT     = 0x01,
   MF_DIFFUSE     = 0x02,
   MF_SPECULAR    = 0x04,
   MF_EMISSION    = 0x08,
   MF_SHININESS   = 0x10
} MaterialFlags;

typedef struct MaterialType
{
   int            nFace;
   float          fAmbient[4];
   float          fDiffuse[4];
   float          fSpecular[4];
   float          fEmission[4];
   float          fShininess;
   unsigned int   nFlags;
} Material;

#endif // _TYPES_H_