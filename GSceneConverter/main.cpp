#include "GE_Prerequisites.h"
#include "GE_Math.h"
#include "Scene.h"
#include <vld.h>


int32_t main( int32_t i_argc, char *i_argv[ ] )
{

#ifdef DEBUG
	i_argc = 2;
	char *str[ 2 ] = { "" , "E:\\Gallardo.fbx" };
	i_argv = str;
#endif // DEBUG
	
	GE_SceneManager::getSingleton().m_shiftKeyDown = ( GetAsyncKeyState( VK_LSHIFT ) & 0x8001 ? true : false );
	GE_SceneManager::getSingleton( ).m_ctrlKeyDown = ( GetAsyncKeyState( VK_LCONTROL ) & 0x8001 ? true : false );
	
	i_argc--;
	if( !i_argc )
		return 0;
	cout << "\n.................................................................\n\n";
	for( uint32_t i = 0; i < ( uint32_t )i_argc; i++ )
	{
		GE_SceneManager::getSingleton().unpackScene( i_argv[ i + 1 ] );
		cout << "\n.................................................................\n\n";
	}

	if ( GE_SceneManager::getSingleton( ).m_shiftKeyDown || GE_SceneManager::getSingleton( ).m_ctrlKeyDown ||
		( GetAsyncKeyState( VK_LSHIFT ) & 0x8001 ) || ( GetAsyncKeyState( VK_LCONTROL ) & 0x8001 ) )
	{ 
		cout<<"\n";
		system( "pause" );
	}

	return 0;
}