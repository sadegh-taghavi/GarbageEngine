#include "GE_View.h"
#include "GE_Math.h"

//GE_View----------------------------------------------------------------------------------
GE_View::GE_View()
{
	m_position = GE_Vec3( 0, 0, 0 );
	m_yaw = m_pitch = 0;
	m_target = GE_Vec3( 0, 0, 0 );
	m_uP = GE_Vec3( 0, 0, 1.0f );
	m_aspect = 4.0f / 3.0f;
	m_fovy = GE_ConvertToRadians( 70.0f );
	m_near = GE_ConvertToUnit( 0.3f );
	m_far = GE_ConvertToUnit( 2000.0f );
	m_viewType = Target;
	m_orthoWH = GE_Vec2( GE_ConvertToUnit( 100.0f ), GE_ConvertToUnit( 100.0f ) );
	m_isOrtho = false;
	m_isCenterOrtho = false;
	m_orthoLeft = -64;
	m_orthoRight = 64;
	m_orthoBottom = -64;
	m_orthoTop = 64;
	m_animation = new GE_ViewAnimation( this );
}

void GE_View::update()
{
	if ( m_viewType == Free )
	{
		if ( m_pitch < -6.28f )
			m_pitch = 0.0f;
		if ( m_pitch > 6.28f )
			m_pitch = 0.0f;
		if ( m_yaw >= 6.28f )
			m_yaw = 0.0f;
		if ( m_yaw <= -6.28f )
			m_yaw = 0.0f;
		m_target = GE_Vec3( cosf( m_pitch ) * cosf( m_yaw ), sinf( m_yaw ) * cosf( m_pitch ), sinf( m_pitch ) ) + m_position;
	}
	m_matView.lookAtRH( &m_position, &m_target, &m_uP );
	m_matViewInv.inverseBy( &m_matView );
	updateDepth();
}

void GE_View::updateDepth()
{
	m_matViewProjectionLast = m_matViewProjection;
	if ( m_isOrtho )
	{
		if (m_isCenterOrtho)
			m_matProjection.orthoCenterRH(m_orthoLeft, m_orthoRight, m_orthoBottom, m_orthoTop, m_near, m_far);
		else
			m_matProjection.orthoRH(m_orthoWH.x, m_orthoWH.y, m_near, m_far);
		m_matProjectionInv.inverseBy( &m_matProjection );
		GE_Mat4x4Multiply( &m_matViewProjection, &m_matView, &m_matProjection );
		m_matViewProjectionInv.inverseBy( &m_matViewProjection );
	}
	else
	{
		m_matProjection.perspectiveFovRH( m_fovy, m_aspect, m_near, m_far );
		m_matProjectionInv.inverseBy( &m_matProjection );
		GE_Mat4x4Multiply( &m_matViewProjection, &m_matView, &m_matProjection );
		m_matViewProjectionInv.inverseBy( &m_matViewProjection );
	}
	m_frustum.calculatePlanes( &m_matViewProjection );
}

GE_View::~GE_View()
{
	SAFE_DELETE( m_animation );
}

void GE_View::getRayFromScreen( const GE_Vec2 &i_cursorPos, const GE_Vec2 &i_widthHeight, GE_Vec3 *i_outOrigin, GE_Vec3 *i_outDirection )
{
	GE_Vec3 v;
	v.x = ( ( ( 2.0f * i_cursorPos.x ) / i_widthHeight.x ) - 1.0f ) / m_matProjection._00;
	v.y = -( ( ( 2.0f * i_cursorPos.y ) / i_widthHeight.y ) - 1.0f ) / m_matProjection._11;
	v.z = -1.0f;

	i_outDirection->x = v.x * m_matViewInv._00 + v.y * m_matViewInv._10 + v.z * m_matViewInv._20;
	i_outDirection->y = v.x * m_matViewInv._01 + v.y * m_matViewInv._11 + v.z * m_matViewInv._21;
	i_outDirection->z = v.x * m_matViewInv._02 + v.y * m_matViewInv._12 + v.z * m_matViewInv._22;
	i_outDirection->normalize();

	i_outOrigin->x = m_matViewInv._30;
	i_outOrigin->y = m_matViewInv._31;
	i_outOrigin->z = m_matViewInv._32;

	( *i_outOrigin ) += ( *i_outDirection );
}

//View Animation -----------------------------------------------------------------------------------------------------------------------------------------------------


void GE_ViewAnimation::loadFromMem( const uint8_t *i_memory )
{
	uint8_t *mem = ( uint8_t * )i_memory;
	FData *fd = ( FData * )mem;
	m_data.Name = fd->Name;
	m_data.IsTargetMode = fd->IsTargetMode;
	m_data.IsOrtho = fd->IsOrtho;
	m_data.Position = fd->Position;
	m_data.Rotation = fd->Rotation;
	m_data.Target = fd->Target;
	m_data.ZNear = fd->ZNear;
	m_data.ZFar = fd->ZFar;
	m_data.Fovy = fd->Fovy;
	m_data.FocusDistance = fd->FocusDistance;
	m_frames.resize( fd->NumberOfFrames );
	mem += sizeof( FData );
	memcpy_s( m_frames.data(), sizeof( Frame )* m_frames.size(), mem, sizeof( Frame )* m_frames.size() );
}

void GE_ViewAnimation::loadAnimation( std::string i_fileName )
{
	m_lastFrame = -1.0f;

	FILE *fp;

	fopen_s( &fp, i_fileName.c_str(), "rb" );

	if ( fp )
	{
		fseek( fp, 0, SEEK_END );
		uint32_t size = ftell( fp );
		uint8_t *chunk = new uint8_t[ size ];
		fseek( fp, 0, SEEK_SET );
		fread_s( chunk, size, sizeof( uint8_t ), size, fp );
		fclose( fp );
		loadFromMem( chunk );
		SAFE_DELETE_ARRAY( chunk );
	}

	m_parent->m_position = m_data.Position;
	m_parent->m_fovy = m_data.Fovy;
	m_parent->m_far = m_data.ZFar;
	m_parent->m_near = m_data.ZNear;
	m_parent->m_focusDistance = m_data.FocusDistance;
	m_parent->m_isOrtho = m_data.IsOrtho;
	m_parent->m_viewType = ( m_data.IsTargetMode ? GE_View::Target : GE_View::Free );
	m_parent->m_target = m_data.Target;
	m_data.Up = m_parent->m_uP;
	m_parent->update();
}

void GE_ViewAnimation::setFrame( float i_frame )
{
	if ( i_frame == m_lastFrame || i_frame < 0 || ( uint32_t )i_frame >= m_frames.size() - 1 )
		return;

	if ( floor( i_frame ) == i_frame )
		m_frame = m_frames[ ( uint32_t )i_frame ];
	else
	{
		uint32_t startFrame = ( uint32_t )floor( i_frame );
		uint32_t endFrame = ( uint32_t )ceil( i_frame );
		float amount = i_frame - startFrame;

		m_frames[ startFrame ].Position.lerpOut( &m_frame.Position, &m_frames[ endFrame ].Position, amount );
		m_frames[ startFrame ].Rotation.lerpOut( &m_frame.Rotation, &m_frames[ endFrame ].Rotation, amount );
		m_frames[ startFrame ].Target.lerpOut( &m_frame.Target, &m_frames[ endFrame ].Target, amount );
		m_frame.ZNear = GE_Lerp( m_frames[ startFrame ].ZNear, m_frames[ endFrame ].ZNear, amount );
		m_frame.ZFar = GE_Lerp( m_frames[ startFrame ].ZFar, m_frames[ endFrame ].ZFar, amount );
		m_frame.Fovy = GE_Lerp( m_frames[ startFrame ].Fovy, m_frames[ endFrame ].Fovy, amount );
		m_frame.FocusDistance = GE_Lerp( m_frames[ startFrame ].FocusDistance, m_frames[ endFrame ].FocusDistance, amount );

	}

	m_parent->m_position = m_frame.Position;
	m_parent->m_fovy = m_frame.Fovy;
	m_parent->m_far = m_frame.ZFar;
	m_parent->m_near = m_frame.ZNear;
	m_parent->m_focusDistance = m_frame.FocusDistance;
	m_parent->m_target = m_frame.Target;
	//GE_Mat4x4 mat;
	//mat.rsp( GE_Vec3( 0.0f, 0.0f, 0.0f ), m_frame.Rotation, GE_Vec3( 1.0f, 1.0f, 1.0f ) );
	//m_data.Up.transformOut( &m_parent->m_uP, &mat );
	m_parent->update();
}

GE_ViewAnimation::GE_ViewAnimation( GE_View *i_parent )
{
	m_parent = i_parent;
}

GE_ViewAnimation::~GE_ViewAnimation()
{

}

vector< GE_ViewAnimation::Frame > & GE_ViewAnimation::getFrames()
{
	return m_frames;
}

GE_ViewAnimation::Data & GE_ViewAnimation::getData()
{
	return m_data;
}

GE_ViewAnimation::Frame &GE_ViewAnimation::getFrame()
{
	return m_frame;
}

void GE_ViewAnimation::unload()
{
	m_frames.clear();
	m_lastFrame = -1;
}
