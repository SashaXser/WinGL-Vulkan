#ifndef _C_MODEL_3DS_H_
#define _C_MODEL_3DS_H_

// includes
#include "CModel.h"

// std includes
#include <map>
#include <vector>
#include <string>
#include <fstream>

class CModel3DS : public CModel
{
public:
   // constructor / destructor
               CModel3DS( );
   virtual    ~CModel3DS( );

   // reads / writes the model
   virtual bool   Read( const char * pFile );
   virtual bool   Write( const char * pFile );

   // updates the model
   virtual void   Update( const double & rElapsedTime );
   
   // draws the model
   virtual void   Draw( );

private:
   // private forward declarations
   struct ObjectBlock;
   struct MaterialGroup;
   struct MaterialBlock;
   struct MaterialColor;
   struct MaterialTexMap;

   // private typedefs
   typedef std::vector< ObjectBlock * > ObjBlkVec;
   typedef std::map< std::string, MaterialBlock * > MatBlkMap;

   // processes the editor chunk
   void           ProcessEditorBlock( std::istream & iStream, unsigned int nChunkLength );
   // processes the object block
   void           ProcessObjectBlock( std::istream & iStream, unsigned int nChunkLength );
   // processes the material block
   void           ProcessMaterialBlock( std::istream & iStream, unsigned int nChunkLength );
   // processes the triangular chunk mesh
   void           ProcessTriangularMesh( std::istream & iStream,
                                         unsigned int nChunkLength,
                                         ObjectBlock * pObjBlk );
   // process the triangular face list
   void           ProcessTriangularFaceList( std::istream & iStream,
                                             unsigned int nChunkLength,
                                             ObjectBlock * pObjBlk );
   // process the smoothing list
   void           ProcessSmoothingGrpList( std::istream & iStream,
                                           unsigned int nChunkLength,
                                           ObjectBlock * pObjBlk );
   // process the texture map
   void           ProcessMaterialTexmap( std::istream & iStream,
                                         unsigned int nChunkLength,
                                         MaterialTexMap * pTexMap );

   // reads color values
   void           ReadColorValues( unsigned short nChunkID,
                                   std::istream & iStream,
                                   MaterialColor & rColor );

   // member variables
   ObjBlkVec      m_vObjBlkVec;
   MatBlkMap      m_vMatBlkMap;
   std::string    m_strBaseDir;

};

inline CModel3DS::CModel3DS( )
{
}

inline bool CModel3DS::Write( const char * /*pFile*/ )
{
   return false;
}

#endif // _C_MODEL_3DS_H_