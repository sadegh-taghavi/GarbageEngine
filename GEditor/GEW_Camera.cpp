#include "GEW_Camera.h"
#include "GE_Core.h"

GEW_FreeCamera::GEW_FreeCamera()
: GE_View()
{
	m_viewType = GE_View::Target;
	m_aspect = 4.0f / 3.0f;
	m_fovy = GE_ConvertToRadians(70.0f);
	m_near = GE_ConvertToUnit( 0.30f );
	m_far = GE_ConvertToUnit( 4000.0f );
	Pitch = ForwardUnits = SidewardUnits = UpwardUnits = 0.0f;
	m_position = GE_Vec3( 0.0f, 0.0f, GE_ConvertToUnit( 1.8f ) );
	Yaw = GE_ConvertToRadians(90.0f);
	m_uP = GE_Vec3( 0.0f, 0.0f, 1.0f );
}

void GEW_FreeCamera::update( GEW_Input::Data &i_inputData )
{
	if( m_viewType == GE_View::Free )
	{
		GE_View::update();
		return;
	}
	if (i_inputData.isMouseDown(GUM_BUTTON_MIDDLE))
	{
		UpwardUnits = ( float )i_inputData.Dy * 0.5f;
		SidewardUnits = ( float )-i_inputData.Dx * 0.5f;
	}
	if (i_inputData.isMouseDown(GUM_BUTTON_RIGHT))
	{
		float rot = GE_ConvertToRadians(14.0f) * GE_Core::getSingleton().getElapsedTime();
		Pitch -= i_inputData.Dy * rot;
		Yaw -= i_inputData.Dx * rot;

		if (i_inputData.isKeyDown(GUK_W))
		{
			ForwardUnits = GE_ConvertToUnit(1.0f) * GE_Core::getSingleton().getElapsedTime();
		}
		if (i_inputData.isKeyDown(GUK_S))
		{
			ForwardUnits = -GE_ConvertToUnit(1.0f) * GE_Core::getSingleton().getElapsedTime();
		}
		if (i_inputData.isKeyDown(GUK_A))
		{
			SidewardUnits = -GE_ConvertToUnit(1.0f) * GE_Core::getSingleton().getElapsedTime();
		}
		if (i_inputData.isKeyDown(GUK_D))
		{
			SidewardUnits = GE_ConvertToUnit(1.0f) * GE_Core::getSingleton().getElapsedTime();
		}			
	}

	if (i_inputData.Dw)
	{
		ForwardUnits = i_inputData.Dw * 0.1f ;
	}
	if (i_inputData.isKeyDown(GUK_SPACE))
	{
		ForwardUnits *= 700.f;
		SidewardUnits *= 700.f;
		UpwardUnits *= 700.0f;		
	}
	else if (i_inputData.isKeyDown(GUK_LSHIFT))
	{
		ForwardUnits *= 10.f;
		SidewardUnits *= 10.f;
		UpwardUnits *= 10.0f;
	}
			
	if (Pitch < -1.57f)
		Pitch = -1.57f;
	if (Pitch > 1.57f)
		Pitch = 1.57f;

	if (Yaw >= 6.28f)
		Yaw = 0.0f;
	if (Yaw <= -6.28f)
		Yaw = 0.0f;

	m_target = GE_Vec3( cosf( Pitch ) * cosf( Yaw ) * 10.0f, sinf( Yaw ) * cosf( Pitch ) * 10.0f, sinf( Pitch ) * 10.0f ) + m_position;
	LookVector.normalizeBy( &( m_target - m_position ) );
	GE_Vec3 SideVector;
	GE_Vec3 UpVector;
	LookVector.crossOut(&SideVector, &m_uP);
	SideVector.normalize();
	SideVector.crossOut(&UpVector, &LookVector);
	SideVector.normalize();
	GE_Vec3 pos = (LookVector * ForwardUnits) + (SideVector * SidewardUnits) + (UpVector * UpwardUnits);
	m_position += pos;
	m_target += pos;
	GE_View::update();
	ForwardUnits = 0;
	SidewardUnits = 0;
	UpwardUnits = 0;
}

void GEW_FreeCamera::lookAt( GE_Vec3 &i_target )
{
	GE_Vec3 direction = i_target - m_position;
	direction.normalize();
	Pitch = atan2f( direction.z, sqrtf( direction.y * direction.y + direction.x * direction.x ) );
	Yaw = atan2f( direction.y, direction.x );
}

void GEW_FreeCamera::lookAt( GE_Vec3 &i_target, float i_distance )
{
	lookAt( i_target );
	GE_Vec3 direction = m_position - i_target;
	direction.normalize();
	m_position = i_target + direction * i_distance /** ( GE_ConvertToRadians( 180.0f ) - m_fovy )*/;
	
}
