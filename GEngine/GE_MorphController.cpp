#include "GE_MorphController.h"
#include <vector>
using namespace std;


GE_MorphController::GE_MorphController( GE_Model *i_parent )
{
	m_parent = i_parent;
}

GE_MorphController::~GE_MorphController(void)
{
	if( m_parent )
	{
		for( uint32_t i = 0; i < m_parent->getNumberOfSubsets(); ++i )
			m_parent->m_subsets[ i ]->MorphTargetsBuffer.release();
		m_parent->m_morphValuesBuffer.release();
	}
}

uint32_t GE_MorphController::getNumberOfFrames()
{
	return m_numberOfFrames;
}

void GE_MorphController::loadFromMem( const uint8_t *i_memory )
{
	uint8_t *mem = ( uint8_t * )i_memory;
	MorphData *md = ( MorphData *) mem;
	m_numberOfFrames = md->NumberOfFrames;
	m_morphNames.resize( md->NumberOfMorphTargets );
	m_values.resize( md->NumberOfMorphTargets );
	m_parent->m_morphValuesBuffer.release();
	m_parent->m_morphValuesBuffer.createBuffer( (uint32_t)m_values.size(), sizeof( float ), m_values.data(), false, DXGI_FORMAT_R32_FLOAT, 1 );

	mem += sizeof( MorphData );
	for( uint32_t targetIndex = 0; targetIndex < md->NumberOfMorphTargets; ++targetIndex )
	{
		m_morphNames[ targetIndex ] = ( char * )mem;
		mem += MAX_NAME_LENGTH;
	}
		
	for( uint32_t i = 0; i < m_parent->getNumberOfSubsets(); ++i )
	{
		m_parent->m_subsets[ i ]->MorphTargetsBuffer.release();
		m_parent->m_subsets[ i ]->MorphTargetsBuffer.createBuffer( md->NumberOfMorphTargets * m_parent->m_subsets[ i ]->Lods[ 0 ]->NumberOfVertices,
			sizeof( MorphTargetVertex ), mem, true, DXGI_FORMAT_R32G32B32_FLOAT, 3 );
		mem += sizeof( MorphTargetVertex ) * md->NumberOfMorphTargets * m_parent->m_subsets[ i ]->Lods[ 0 ]->NumberOfVertices;
	}
		
	m_frames.resize( md->NumberOfFrames * md->NumberOfMorphTargets );
	memcpy_s( m_frames.data(), sizeof( float ) * m_frames.size(), mem, sizeof( float ) * m_frames.size() );
	
}

std::vector< float > * GE_MorphController::getFrames()
{
	return &m_frames;
}

void GE_MorphController::setFrame( float i_frame )
{
	if( i_frame == m_lastFrame || i_frame < 0 || ( uint32_t ) i_frame >= m_numberOfFrames - 1 )
		return;

	if( floor( i_frame ) == i_frame )
		for( uint32_t i = 0 ; i < m_values.size(); ++i  )
			m_values[ i ] = m_frames[ ( uint32_t )i_frame * m_values.size() + i ];
	else
	{
		uint32_t startFrame = ( uint32_t )floor( i_frame );
		uint32_t endFrame = ( uint32_t )ceil( i_frame );
		float amount = i_frame - startFrame;
		for( uint32_t i = 0 ; i < m_values.size(); ++i  )
				m_values[ i ] = GE_Lerp( m_frames[ ( uint32_t )startFrame * m_values.size() + i ], m_frames[ ( uint32_t )endFrame * m_values.size() + i ], amount );
	}
	m_parent->m_morphValuesBuffer.fill();
}

std::vector< float > * GE_MorphController::getValues()
{
	return &m_values;
}

std::vector< std::string > * GE_MorphController::getNames()
{
	return &m_morphNames;
}

void GE_MorphController::loadAnimation( const std::string i_fileName )
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

void GE_MorphController::unload()
{
	if( m_parent )
	{
		for( uint32_t i = 0; i < m_parent->getNumberOfSubsets(); ++i )
			m_parent->m_subsets[ i ]->MorphTargetsBuffer.release();
		m_parent->m_morphValuesBuffer.release();
	}
	m_values.clear();
	m_frames.clear();
	m_numberOfFrames = 0;
	m_lastFrame = -1;
}
