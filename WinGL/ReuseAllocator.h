#ifndef _REUSE_ALLOCATOR_H_
#define _REUSE_ALLOCATOR_H_

#ifdef _DEBUG
#define VALIDATE_EQUAL_TO_ONE( val ) \
   if ((val) != 1) *((int *)(0x0)) = 0;
#else
#define VALIDATE_EQUAL_TO_ONE( val )
#endif

template < typename T >
class ReuseAllocator
{
public:
   // public typedefs
   typedef T                     value_type;
   typedef int                   difference_type;
   typedef size_t                size_type;
   typedef value_type &          reference;
   typedef value_type *          pointer;
   typedef const value_type &    const_reference;
   typedef const value_type *    const_pointer;

   // public structures
   template < typename O >
   struct rebind
   {
      typedef ReuseAllocator< O > other;
   };

   // constructor / destructor
    ReuseAllocator( );
    ReuseAllocator( const ReuseAllocator & allocator );
   ~ReuseAllocator( );

   // special constructor
   template < typename O >
   ReuseAllocator( const ReuseAllocator< O > & allocator );

   // copy operator 
   template < typename O >
   ReuseAllocator< T > & operator = ( const ReuseAllocator< O > & other );

   // allocates memory
   pointer allocate( size_type count, const void * hint = 0);

   // construct object
   void construct( pointer ptr, const_reference val );

   // destroy object
   void destroy( pointer ptr );

   // deallocates memory
   void deallocate( pointer ptr, size_type count );

   // maximum size
   size_type max_size( ) const;

private:
   // private member variables
   void *      _front;

};

template < typename T >
ReuseAllocator< T >::ReuseAllocator( ) :
_front   ( 0 )
{
}

template < typename T >
ReuseAllocator< T >::ReuseAllocator( const ReuseAllocator & allocator ) :
_front   ( 0 )
{
}

template < typename T >
template < typename O >
ReuseAllocator< T >::ReuseAllocator( const ReuseAllocator< O > & allocator ) :
_front   ( 0 )
{
}

template < typename T >
ReuseAllocator< T >::~ReuseAllocator( )
{
   while (_front)
   {
      void * next =  reinterpret_cast< void * >(
                    *reinterpret_cast< uintptr_t * >(_front));

      ::operator delete(_front);

      _front = next;
   }
}

template < typename T >
template < typename O >
ReuseAllocator< T > &
ReuseAllocator< T >::operator = ( const ReuseAllocator< O > & other )
{
   return *this;
}

template < typename T >
typename ReuseAllocator< T >::pointer
ReuseAllocator< T >::allocate( size_type count, const void * hint )
{
   // make sure that the count is equal to one.
   // if this crashes, basically this means that
   // the container being used is allocating memory
   // in continuous blocks (ie std::vector).
   // this allocator does not handle this case.
   VALIDATE_EQUAL_TO_ONE(count);

   void * mem = _front;

   if (!mem)
   {
      const size_t size =
         sizeof(T) >= sizeof(uintptr_t *) ?
         sizeof(T) : sizeof(uintptr_t *);

      mem = ::operator new(size);
   }
   else
   {
      _front =  reinterpret_cast< void * >(
               *reinterpret_cast< uintptr_t * >(_front));
   }

   return reinterpret_cast< pointer >(mem);
}

template < typename T >
void ReuseAllocator< T >::construct( pointer ptr, const_reference val )
{
   ::new (ptr) T(val);
}

template < typename T >
void ReuseAllocator< T >::destroy( pointer ptr )
{
   ptr->~T();
}

template < typename T >
void ReuseAllocator< T >::deallocate( pointer ptr, size_type count )
{
   if (!_front)
   {
      _front = reinterpret_cast< void * >(ptr);

      *reinterpret_cast< uintptr_t * >(ptr) = 0;
   }
   else
   {
      *reinterpret_cast< uintptr_t * >(ptr) =
         reinterpret_cast< uintptr_t >(_front);

      _front = reinterpret_cast< void * >(ptr);
   }
}

template < typename T >
typename ReuseAllocator< T >::size_type ReuseAllocator< T >::max_size( ) const
{
#if _WIN64
   return 0x8000000000000000 / sizeof(T);
#elif _WIN32
   return 0x80000000 / sizeof(T);
#endif
}

#endif // _REUSE_ALLOCATOR_H_