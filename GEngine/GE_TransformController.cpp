#include "GE_TransformController.h"
#include "GE_Model.h"
#include <vector>
using namespace std;

void GE_TransformController::loadFromMem( const uint8_t *i_memory )
{
	uint8_t *mem = ( uint8_t * )i_memory;
	TransformData *td = (TransformData *) i_memory;
	m_frames.resize( td->NumberOfFrames );
	mem += sizeof( TransformData );
	memcpy( m_frames.data(), mem, sizeof( Transform ) * m_frames.size() );
}


GE_TransformController::GE_TransformController( GE_Model *i_parent )
{
	m_parent = i_parent;
	
}

GE_TransformController::~GE_TransformController(void)
{

}

uint32_t GE_TransformController::getNumberOfFrames()
{
	return (uint32_t)m_frames.size();
}

void GE_TransformController::setFrame( float i_frame )
{
	if( i_frame == m_lastFrame || i_frame < 0 || ( uint32_t ) i_frame >= m_frames.size() - 1 )
		return;

	if( floor( i_frame ) == i_frame )
		m_transform = m_frames[ ( uint32_t ) i_frame ];
	else
	{
		 uint32_t startFrame = ( uint32_t )floor( i_frame );
		 uint32_t endFrame = ( uint32_t )ceil( i_frame );
		 float amount = i_frame - startFrame;

		 m_frames[ startFrame ].Position.lerpOut( &m_transform.Position, &m_frames[ endFrame ].Position, amount );
		 m_frames[ startFrame ].Rotation.lerpOut( &m_transform.Rotation, &m_frames[ endFrame ].Rotation, amount );
		 m_frames[ startFrame ].Scale.lerpOut( &m_transform.Scale, &m_frames[ endFrame ].Scale, amount );

		 m_transform.Matrix.srp( m_transform.Position, m_transform.Rotation, m_transform.Scale );
	}
	m_parent->m_worldNeedCalculate = true;
}

GE_TransformController::Transform * GE_TransformController::getTransform()
{
	return &m_transform;
}

std::vector< GE_TransformController::Transform > * GE_TransformController::getFrames()
{
	return &m_frames;
}

void GE_TransformController::loadAnimation( const std::string i_fileName )
{
	m_lastFrame = -1.0f;

	FILE *fp;

	fopen_s( &fp, i_fileName.c_str(), "rb" );

	if( fp )
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
}

void GE_TransformController::unload()
{
	m_frames.clear();
	m_lastFrame = -1;
}

