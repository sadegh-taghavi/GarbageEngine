#include "GEW_Input.h"
#include <memory.h>

bool GEW_Input::Data::isKeyDown( int i_gUK )
{
	return Keyboard[ i_gUK ];
}

bool GEW_Input::Data::isMouseDown( int i_mouseButton )
{
	return ( MButtons & i_mouseButton ) != 0;
}

GEW_Input::Data::Data()
{
	memset( this, 0, sizeof( Data ) );
}
