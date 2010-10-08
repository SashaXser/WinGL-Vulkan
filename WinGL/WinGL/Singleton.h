#ifndef _SINGLETON_H_
#define _SINGLETON_H_

// crt includes
#include <stdlib.h>

template < typename T >
class Singleton
{
public:
   // access the basic member
   static T *  Instance( );

   // releases the basic member
   static void Release( );

private:
   // prohibit default and copy construction
   // prohibit destructor
            Singleton( );
            Singleton( const Singleton & );
           ~Singleton( );
   // prohibit copy operator
   Singleton & operator = ( const Singleton & );

   // at exit destructor
   static void AtExitCB( );

   // private static members
   static T *     mT;

};

// allocate space for the static instance
template < typename T > T * Singleton< T >::mT = NULL;

template < typename T >
T * Singleton< T >::Instance( )
{
   // validate instance
   if (!mT)
   {
      // create a new instance
      mT = new T;
      // register for at exit callback
      atexit(&AtExitCB);
   }

   // return current instance
   return mT;
}

template < typename T >
void Singleton< T >::Release( )
{
   AtExitCB();
}

template < typename T >
void Singleton< T >::AtExitCB( )
{
   // release the instance
   delete mT;
   // nullify the instance
   mT = NULL;
}

#endif // _SINGLETON_H_