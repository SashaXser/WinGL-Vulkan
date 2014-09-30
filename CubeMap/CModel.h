#ifndef _C_MODEL_H_
#define _C_MODEL_H_

class CModel
{
public:
   // constructor / destructor
   virtual       ~CModel( ) { };

   // reads / writes the model
   virtual bool   Read( const char * pFile ) = 0;
   virtual bool   Write( const char * pFile ) = 0;

   // updates the model
   virtual void   Update( const double & rElapsedTime ) = 0;
   
   // draws the model
   virtual void   Draw( ) = 0;

};

#endif // _C_MODEL_H_