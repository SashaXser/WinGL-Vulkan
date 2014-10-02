// local includes
#include "CModel3DS.h"
#include "StdIncludes.h"

// wingl includes
#include "ReadTexture.h"

#include <direct.h>

// std includes
#include <cstdint>

// definitions
#define CHUNK_MAIN_3DS           0x4D4D
#define CHUNK_EDITOR_3DS         0x3D3D
#define CHUNK_OBJ_BLOCK          0x4000
#define CHUNK_TRIANGULAR_MESH    0x4100
#define CHUNK_VERTICES_LIST      0x4110
#define CHUNK_VERT_FACE_LIST     0x4120
#define CHUNK_FACE_SMOOTH        0x4150
#define CHUNK_FACE_MATERIAL      0x4130
#define CHUNK_MAP_COORD_LIST     0x4140
#define CHUNK_SMOOTH_GRP_LIST    0x4150
#define CHUNK_MATERIAL_BLK       0xAFFF
#define CHUNK_MATERIAL_NAME      0xA000
#define CHUNK_MAT_AMBIENT_CLR    0xA010
#define CHUNK_MAT_DIFFUSE_CLR    0xA020
#define CHUNK_MAT_SPECULAR_CLR   0xA030
#define CHUNK_MAT_SHININESS      0xA040
#define CHUNK_MAT_TRANSPARENCY   0xA050
#define CHUNK_MAT_TEXMAP         0xA200
#define CHUNK_MAT_TEXMAP_NAME    0xA300

#define CHUNK_COLOR_FLOAT        0x0010
#define CHUNK_COLOR_UCHAR        0x0011

// private forward declarations
struct CModel3DS::MaterialGroup
{
   char              strMatGrpName[20]; // name of material def
   unsigned short    nFaceMatSize;      // number of faces assigned material
   unsigned short *  pFaceMatList;      // a list of face materials assigned indices
};

struct CModel3DS::ObjectBlock
{
   char              strObjName[20];// object block name
   float *           pVertices;     // vertice array
   unsigned short *  pPolygons;     // polygon array
   float *           pTexCoords;    // texture coordinate array
   MaterialGroup *   pMatGroup;     // material group
   unsigned short    nVertSize;     // vertice array size
   unsigned short    nPolySize;     // polygon array size
   unsigned short    nTexCoordSize; // texture coord array size
};

struct CModel3DS::MaterialColor
{
   unsigned char nRed;
   unsigned char nGreen;
   unsigned char nBlue;
};

struct CModel3DS::MaterialTexMap
{
   char           strTexName[64]; // material texmap name
   unsigned int   nImage;         // image identifier
};

struct CModel3DS::MaterialBlock
{
   char              strMatName[64]; // material block name
   float             fShininess;     // material shininess
   float             fTransparent;   // material transparency
   MaterialTexMap *  pMatTexmap;     // material texture map
   MaterialColor     clrAmbient;     // ambient color
   MaterialColor     clrDiffuse;     // diffuse color
   MaterialColor     clrSpecular;    // specular color
};

CModel3DS::~CModel3DS( )
{
   ObjectBlock * pObjBlk;

   for (ObjBlkVec::iterator itBegin = m_vObjBlkVec.begin(),
                            itEnd   = m_vObjBlkVec.end();
        itBegin != itEnd;
        itBegin++)
   {
      // obtain the obj blk
      pObjBlk = *itBegin;
      // release the material group face list
      if (pObjBlk->pMatGroup) delete [] pObjBlk->pMatGroup->pFaceMatList;
      // delete the resources
      delete [] pObjBlk->pVertices;
      delete [] pObjBlk->pPolygons;
      delete [] pObjBlk->pTexCoords;
      delete pObjBlk->pMatGroup;
      delete pObjBlk;
   }

   MaterialBlock * pMatBlk;

   for (MatBlkMap::iterator itBegin = m_vMatBlkMap.begin(),
                            itEnd   = m_vMatBlkMap.end();
        itBegin != itEnd;
        itBegin++)
   {
      // obtain the mat blk
      pMatBlk = itBegin->second;
      // release texture resources
      if (pMatBlk->pMatTexmap) glDeleteTextures(1, &pMatBlk->pMatTexmap->nImage);
      // delete the resources
      delete pMatBlk->pMatTexmap;
      delete pMatBlk;
   }
}

void CModel3DS::Update( const double & rElapsedTime )
{
}

void CModel3DS::Draw( )
{
   glEnable(GL_INDEX_ARRAY);
   glEnable(GL_VERTEX_ARRAY);

   for (ObjBlkVec::iterator itBegin = m_vObjBlkVec.begin(),
                            itEnd   = m_vObjBlkVec.end();
        itBegin != itEnd;
        itBegin++)
   {
      if ((*itBegin)->pMatGroup)
      {
         MatBlkMap::iterator itMat = m_vMatBlkMap.find((*itBegin)->pMatGroup->strMatGrpName);
         if (itMat != m_vMatBlkMap.end() && itMat->second->pMatTexmap && itMat->second->pMatTexmap->nImage)
         {
            glEnable(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, (*itBegin)->pTexCoords);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, itMat->second->pMatTexmap->nImage);
         }
      }

      glVertexPointer(3, GL_FLOAT, 0, (*itBegin)->pVertices);
      glDrawElements(GL_TRIANGLES, (*itBegin)->nPolySize, GL_UNSIGNED_SHORT, (*itBegin)->pPolygons);

      glDisable(GL_TEXTURE_COORD_ARRAY);
      glDisable(GL_TEXTURE_2D);
   }

   glDisable(GL_INDEX_ARRAY);
   glDisable(GL_VERTEX_ARRAY);
}

bool CModel3DS::Read( const char * pFile )
{
   // local(s)
   bool bRead = false;

   // create a local inputs tream
   std::ifstream ifStream;
   // open the stream
   ifStream.open(pFile, std::ios_base::in | std::ios_base::binary);

   // validate the stream
   if (ifStream.is_open())
   {
      // local(s)
      unsigned short nChunkID = 0;
      unsigned long  nChunkLength = 0;

      if (strstr(pFile, ":") == NULL)
      {
         // clear the base directory
         m_strBaseDir.clear();
         // rebase the string
         m_strBaseDir.resize(MAX_PATH);

         // determine the base path by getting the current working directory
         _getcwd(&*m_strBaseDir.begin(), MAX_PATH);
         // determine the last character
         std::string::size_type nLast = m_strBaseDir.find_first_of('\0');
         // resize the string
         m_strBaseDir.resize(nLast); m_strBaseDir += "\\";
         // append the path to the file
         m_strBaseDir += pFile;
      }
      else
      {
         m_strBaseDir = pFile;
      }

      // remove the file name from the path
      std::string::size_type nLast = m_strBaseDir.find_last_of("\\");
      // remove the filename from the last pos
      m_strBaseDir.resize(nLast + 1);

      // read the first chunks and length
      ifStream.read((char *)&nChunkID, sizeof(nChunkID));
      ifStream.read((char *)&nChunkLength, sizeof(nChunkLength));

      // validate the chunk
      if (nChunkID == CHUNK_MAIN_3DS)
      {
         while (!ifStream.eof())
         {
            // read the chunk id
            ifStream.read((char *)&nChunkID, sizeof(nChunkID));
            ifStream.read((char *)&nChunkLength, sizeof(nChunkLength));

            // determine the type of chunck
            switch (nChunkID)
            {
            case CHUNK_EDITOR_3DS:
               ProcessEditorBlock(ifStream, nChunkLength - 6);

               break;

            default:
               // move the header along
               ifStream.seekg(nChunkLength - 6, std::ios_base::cur);

               break;
            }
         }
      }

      // close the stream
      ifStream.close();
      // indicate the file is read
      bRead = true;
   }

   return bRead;
}

void CModel3DS::ProcessEditorBlock( std::istream & iStream, unsigned int nChunkLength )
{
   // local(s)
   unsigned short nChunkID = 0;
   unsigned long nCurChunkLength = 0;

   while (nChunkLength)
   {
      // read the chunk and the length
      iStream.read((char *)&nChunkID, sizeof(nChunkID));
      iStream.read((char *)&nCurChunkLength, sizeof(nCurChunkLength));

      // determine the type of chunk
      switch (nChunkID)
      {
      case CHUNK_OBJ_BLOCK:
         ProcessObjectBlock(iStream, nCurChunkLength - 6);

         break;

      case CHUNK_MATERIAL_BLK:
         ProcessMaterialBlock(iStream, nCurChunkLength - 6);

         break;

      default:
         // move the chunk header
         iStream.seekg(nCurChunkLength - 6, std::ios_base::cur);

         break;
      }

      // decrease the chunklength
      nChunkLength -= nCurChunkLength;
   }
}

void CModel3DS::ProcessObjectBlock( std::istream & iStream, unsigned int nChunkLength )
{
   // local(s)
   unsigned short nChunkID = 0;
   unsigned long nCurChunkLength = 0;

   // create a new object block
   ObjectBlock * pObjBlk = new ObjectBlock;
   // add the object to the obj blk vector
   m_vObjBlkVec.push_back(pObjBlk);

   // clear the object block
   memset(pObjBlk, 0x00, sizeof(ObjectBlock));

   // point to the object name
   char * pObjBlkName = pObjBlk->strObjName;

   do
   {
      // read the object block name
      iStream.read(pObjBlkName++, sizeof(char));
   } while (*(pObjBlkName - 1) && pObjBlkName != (pObjBlk->strObjName + 20));

   // decrease the chunk length
   nChunkLength -= (pObjBlkName - pObjBlk->strObjName);

   while (nChunkLength)
   {
      // read the chunk and the length
      iStream.read((char *)&nChunkID, sizeof(nChunkID));
      iStream.read((char *)&nCurChunkLength, sizeof(nCurChunkLength));

      switch (nChunkID)
      {
      case CHUNK_TRIANGULAR_MESH:
         // process triangular meshes
         ProcessTriangularMesh(iStream, nCurChunkLength - 6, pObjBlk);

         break;

      default:
         // move the chunk header
         iStream.seekg(nCurChunkLength - 6, std::ios_base::cur);

         break;
      }

      // decrease the chunklength
      nChunkLength -= nCurChunkLength;
   }
}

void CModel3DS::ProcessMaterialBlock( std::istream & iStream, unsigned int nChunkLength )
{
   // local(s)
   unsigned short nChunkID = 0;
   unsigned long nCurChunkLength = 0;

   // create a new material block
   MaterialBlock * pMatBlk = new MaterialBlock;

   // clear the material block
   memset(pMatBlk, 0x00, sizeof(MaterialBlock));

   while (nChunkLength)
   {
      // read the chunk and the length
      iStream.read((char *)&nChunkID, sizeof(nChunkID));
      iStream.read((char *)&nCurChunkLength, sizeof(nCurChunkLength));

      switch (nChunkID)
      {
      case CHUNK_MATERIAL_NAME:
         // read the material name
         iStream.read(pMatBlk->strMatName, nCurChunkLength - 6);
         // add the object to the mat blk vector
         m_vMatBlkMap.insert(MatBlkMap::value_type(pMatBlk->strMatName, pMatBlk));

         break;

      case CHUNK_MAT_AMBIENT_CLR:
      case CHUNK_MAT_DIFFUSE_CLR:
      case CHUNK_MAT_SPECULAR_CLR:
         {
            // determine the color value to set
            MaterialColor & rColor = nChunkID == CHUNK_MAT_AMBIENT_CLR ? pMatBlk->clrAmbient :
                                                                         nChunkID == CHUNK_MAT_DIFFUSE_CLR ? pMatBlk->clrDiffuse :
                                                                                                             pMatBlk->clrSpecular;
            // read the next chunk id
            iStream.read((char *)&nChunkID, sizeof(nChunkID));
            // read the color values
            ReadColorValues(nChunkID, iStream, rColor);
         }

         break;

      case CHUNK_MAT_SHININESS:
      case CHUNK_MAT_TRANSPARENCY:
         {
            // locals(s)
            float *        pRatio = 0;
            unsigned short nRatio = 0;
            unsigned short nID = 0;
            unsigned long  nChunkLength = 0;

            // setup the ratio pointer
            switch (nChunkID)
            {
            case CHUNK_MAT_SHININESS:     pRatio = &pMatBlk->fShininess; break;
            case CHUNK_MAT_TRANSPARENCY:  pRatio = &pMatBlk->fTransparent; break;
            }

            // read the chunk id and length
            iStream.read((char *)&nID, sizeof(nID));
            iStream.read((char *)&nChunkLength, sizeof(nChunkLength));
            // read the ratio
            iStream.read((char *)&nRatio, sizeof(nRatio));
            // set the ratio
            *pRatio = nRatio / 100.0f;
         }

         break;

      case CHUNK_MAT_TEXMAP:
         // create a new texture map
         pMatBlk->pMatTexmap = new MaterialTexMap;
         // process the texture map
         ProcessMaterialTexmap(iStream, nCurChunkLength - 6, pMatBlk->pMatTexmap);

         break;

      default:
         // move the chunk header
         iStream.seekg(nCurChunkLength - 6, std::ios_base::cur);

         break;
      }

      // decrease the chunklength
      nChunkLength -= nCurChunkLength;
   }
}

void CModel3DS::ProcessTriangularMesh( std::istream & iStream,
                                       unsigned int nChunkLength,
                                       ObjectBlock * pObjBlk  )
{
   // local(s)
   unsigned short nChunkID = 0;
   unsigned long nCurChunkLength = 0;

   while (nChunkLength)
   {
      // read the chunk and the length
      iStream.read((char *)&nChunkID, sizeof(nChunkID));
      iStream.read((char *)&nCurChunkLength, sizeof(nCurChunkLength));

      switch (nChunkID)
      {
      case CHUNK_VERTICES_LIST:
         // read vertices size
         iStream.read((char *)&pObjBlk->nVertSize, sizeof(pObjBlk->nVertSize));
         // create the vertex array
         pObjBlk->pVertices = new float[pObjBlk->nVertSize * 3];
         // read the vertices
         iStream.read((char *)pObjBlk->pVertices, sizeof(float) * pObjBlk->nVertSize * 3);

         break;

      case CHUNK_VERT_FACE_LIST:
         {
            // read the polygon size
            iStream.read((char *)&pObjBlk->nPolySize, sizeof(pObjBlk->nPolySize));
            // create the poly array
            pObjBlk->pPolygons = new unsigned short[pObjBlk->nPolySize * 3];
            // point to the beginning of the polygons
            unsigned short * pPolygons = pObjBlk->pPolygons;
            // determine the number of items to read
            unsigned short nReadSize = sizeof(unsigned short) * 3;
            // read all the polygons
            for (unsigned short i = 0; i < pObjBlk->nPolySize; i++)
            {
               // read the polygons
               iStream.read((char *)pPolygons, nReadSize);
               // seek the stream two bytes
               iStream.seekg(sizeof(unsigned short), std::ios_base::cur);
               // increase the pointer by 3
               pPolygons += 3;
            }
            // determine if subchunks need processing
            unsigned int nFaceChunkSize = pObjBlk->nPolySize * sizeof(unsigned short) * 4 + sizeof(unsigned short) + 6;
            // multiply the poly size by 3
            // this value indicates the true number of indices
            pObjBlk->nPolySize *= 3;
            
            if (nFaceChunkSize != nCurChunkLength)
            {
               // process the face list subchunks
               ProcessTriangularFaceList(iStream, nCurChunkLength - nFaceChunkSize, pObjBlk);
            }
         }

         break;

      case CHUNK_MAP_COORD_LIST:
         {
            // read in the number of vertices
            iStream.read((char *)&pObjBlk->nTexCoordSize, sizeof(unsigned short));
            // create a new uv coordinate array
            pObjBlk->pTexCoords = new float[pObjBlk->nTexCoordSize * 2];
            // read in the number of texture coordinates
            iStream.read((char *)pObjBlk->pTexCoords, pObjBlk->nTexCoordSize * 2 * sizeof(float));
            // determine if subchunks need processing
            unsigned int nCoordChunkSize = pObjBlk->nTexCoordSize * 2 * sizeof(float) + sizeof(unsigned short) + 6;

            if (nCoordChunkSize != nCurChunkLength)
            {
               // process the coord map subchunks
               ProcessSmoothingGrpList(iStream, nChunkLength, pObjBlk);
            }
         }

         break;

      default:
         // move the chunk header
         iStream.seekg(nCurChunkLength - 6, std::ios_base::cur);

         break;
      }

      // decrease the chunklength
      nChunkLength -= nCurChunkLength;
   }
}

void CModel3DS::ProcessTriangularFaceList( std::istream & iStream,
                                           unsigned int nChunkLength,
                                           ObjectBlock * pObjBlk )
{
   // local(s)
   unsigned short nChunkID = 0;
   unsigned long nCurChunkLength = 0;

   while (nChunkLength)
   {
      // read the chunk and the length
      iStream.read((char *)&nChunkID, sizeof(nChunkID));
      iStream.read((char *)&nCurChunkLength, sizeof(nCurChunkLength));

      switch (nChunkID)
      {
      case CHUNK_FACE_MATERIAL:
         {
            // create a new material group object
            pObjBlk->pMatGroup = new MaterialGroup;
            // point to the beginning of the name
            char * pStrPtr = pObjBlk->pMatGroup->strMatGrpName;

            do
            {
               // read the material name
               iStream.read(pStrPtr, sizeof(char));
            } while (*(pStrPtr++) != '\0');

            // read the number of face material size
            iStream.read((char *)&pObjBlk->pMatGroup->nFaceMatSize, sizeof(pObjBlk->pMatGroup->nFaceMatSize));
            // create a new list
            pObjBlk->pMatGroup->pFaceMatList = new unsigned short[pObjBlk->pMatGroup->nFaceMatSize];
            // read the new list
            iStream.read((char *)pObjBlk->pMatGroup->pFaceMatList, pObjBlk->pMatGroup->nFaceMatSize * 2);
         }

         break;

      case CHUNK_FACE_SMOOTH:
         /* Nothing at this time */
         
      default:
         // move the chunk header
         iStream.seekg(nCurChunkLength - 6, std::ios_base::cur);

         break;
      }

      // decrease the chunklength
      nChunkLength -= nCurChunkLength;
   }
}

void CModel3DS::ProcessSmoothingGrpList( std::istream & iStream,
                                         unsigned int nChunkLength,
                                         ObjectBlock * pObjBlk )
{
   // local(s)
   unsigned short nChunkID = 0;
   unsigned long nCurChunkLength = 0;

   while (nChunkLength)
   {
      // read the chunk and the length
      iStream.read((char *)&nChunkID, sizeof(nChunkID));
      iStream.read((char *)&nCurChunkLength, sizeof(nCurChunkLength));

      switch (nChunkID)
      {
      case CHUNK_SMOOTH_GRP_LIST:
         /* Nothing at this time */
         
      default:
         // move the chunk header
         iStream.seekg(nCurChunkLength - 6, std::ios_base::cur);

         break;
      }

      // decrease the chunklength
      nChunkLength -= nCurChunkLength;
   }
}

void CModel3DS::ReadColorValues( unsigned short nChunkID,
                                 std::istream & iStream,
                                 MaterialColor & rColor )
{
   // local(s)
   unsigned long nChunkLength = 0;

   // read the chunk length
   iStream.read((char *)&nChunkLength, sizeof(unsigned long));

   switch (nChunkID)
   {
   case CHUNK_COLOR_FLOAT:
      {
         // create a floating point vector
         float fColor[3];
         // read the colors
         iStream.read((char *)fColor, sizeof(fColor));
         // convert the values to the material color
         rColor.nRed   = (unsigned char)(fColor[0] * 255.0f);
         rColor.nGreen = (unsigned char)(fColor[1] * 255.0f);
         rColor.nBlue  = (unsigned char)(fColor[2] * 255.0f);
      }

      break;

   case CHUNK_COLOR_UCHAR:
      // read the three bytes
      iStream.read((char *)&rColor, sizeof(unsigned char) * 3);

      break;
   }
}

void CModel3DS::ProcessMaterialTexmap( std::istream & iStream,
                                       unsigned int nChunkLength,
                                       MaterialTexMap * pTexMap )
{
   // local(s)
   unsigned short nChunkID = 0;
   unsigned long nCurChunkLength = 0;

   while (nChunkLength)
   {
      // read the chunk and the length
      iStream.read((char *)&nChunkID, sizeof(nChunkID));
      iStream.read((char *)&nCurChunkLength, sizeof(nCurChunkLength));

      switch (nChunkID)
      {
      case CHUNK_MAT_TEXMAP_NAME:
         {
            // start off by setting the image to 0
            pTexMap->nImage = 0;
            // copy the texture name
            iStream.read(pTexMap->strTexName, nCurChunkLength - 6);
            // create the path to the file
            std::string sTexFile = m_strBaseDir + pTexMap->strTexName;
            // load the texture to the card
            const auto texture = ReadTexture< uint8_t >(sTexFile.c_str(), GL_RGBA);

            if (texture.pTexture)
            {
               // generate the texture
               glGenTextures(1, &pTexMap->nImage);
               glBindTexture(GL_TEXTURE_2D, pTexMap->nImage);
               glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0,
                            GL_RGBA, GL_UNSIGNED_BYTE, texture.pTexture.get());

               // set up the texture parameters
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

               // no longer need the texture bound
               glBindTexture(GL_TEXTURE_2D, 0);
            }
         }

         break;
         
      default:
         // move the chunk header
         iStream.seekg(nCurChunkLength - 6, std::ios_base::cur);

         break;
      }

      // decrease the chunklength
      nChunkLength -= nCurChunkLength;
   }
}