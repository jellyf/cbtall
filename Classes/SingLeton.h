#ifndef _SingLeton_H
#define _SingLeton_H
#include <string>
#include <assert.h>  

template <typename T> class SingLeton
{
protected:
    
	static T* mSingleton;
    
public:
	SingLeton( void )
	{
		//assert( !ms_FMSingleton );
#if defined( _MSC_VER ) && _MSC_VER < 1200	 
		int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
		mSingleton = (T*)((int)this + offset);
#else
		mSingleton = static_cast< T* >( this );
#endif
	}
	~SingLeton( void )
	{  
		//assert( ms_FMSingleton );  
		mSingleton = 0;  
	}
	static T& getSingleton( void )
	{	
		//assert( ms_FMSingleton ); 
		return ( *mSingleton ); 
	}
	static T* getSingletonPtr( void )
	{ return mSingleton; }
};

#endif

