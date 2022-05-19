#include "GE_SkeletonController.h"
#include <vector>
using namespace std;

GE_SkeletonController::GE_SkeletonController( GE_Model *i_parent )
{
	m_numberOfFrames = 0;
	m_boundBoneID = -1;
	m_parent = i_parent;
	m_bonesHierarchy = NULL;
	m_BoundTransform.identity();
}

GE_SkeletonController::~GE_SkeletonController( void )
{
	removeHierarchy( m_bonesHierarchy );
	if( m_parent )
		m_parent->m_bonesBuffer.release();
}

void GE_SkeletonController::addToHierarchy( BoneHierarchy *i_boneHierarchy, BonePC *i_bones )
{
	for( uint32_t i = 0; i < m_bones.size(); ++i )
	{
		if( i_boneHierarchy->ID == i_bones[ i ].ParentID )
		{
			BoneHierarchy *bh = new BoneHierarchy();
			i_boneHierarchy->Childs.push_back( bh );
			i_bones[ i ].ParentID = -2;
			bh->ID = i;
			bh->Parent = i_boneHierarchy;
			addToHierarchy( bh, i_bones );
		}
	}
}

void GE_SkeletonController::loadFromMem( const uint8_t *i_memory )
{
	uint8_t *mem = ( uint8_t * ) i_memory;
	BoneData *md = ( BoneData * ) mem;
	mem += sizeof( BoneData );
	m_numberOfFrames = md->NumberOfFrames;
	BonePC *bonesPC = ( BonePC * ) mem;
	m_boundBoneID = md->BoundBoneID;
	for( uint32_t i = 0; i < md->NumberOfBones; ++i )
	{
		Bone bone;
		bone.Name = bonesPC[ i ].Name;
		bone.InitializeMatrix = bonesPC[ i ].InitializeMatrix;
		bone.ParentID = bonesPC[ i ].ParentID;
		m_bones.push_back( bone );
	}
	m_frameValues.resize( m_bones.size() );
	m_frameValuesT1.resize( m_bones.size() );
	m_frameValuesT2.resize( m_bones.size() );

	m_bonesHierarchy = new BoneHierarchy();
	addToHierarchy( m_bonesHierarchy, bonesPC );
	mem += sizeof( BonePC ) * md->NumberOfBones;
	m_boneFrames.resize( md->NumberOfBones * md->NumberOfFrames );
	memcpy_s( m_boneFrames.data(), sizeof( GE_Mat4x4 ) * m_boneFrames.size(), mem, sizeof( GE_Mat4x4 ) * m_boneFrames.size() );
	m_parent->m_bonesBuffer.release();
	calculateFrame( m_bonesHierarchy, 0, &m_frameValues );
	m_parent->m_bonesBuffer.createBuffer( ( uint32_t ) m_frameValues.size(), sizeof( GE_Mat4x4 ), m_frameValues.data(), false, DXGI_FORMAT_R32G32B32A32_FLOAT, 4 );
}

void GE_SkeletonController::setFrame( float i_frame )
{
	if( i_frame == m_lastFrame || i_frame < 0 || ( uint32_t ) i_frame >= m_numberOfFrames - 1 )
		return;

	if( floor( i_frame ) == i_frame )
		calculateFrame( m_bonesHierarchy, ( uint32_t ) i_frame, &m_frameValues );
	else
	{
		uint32_t startFrame = ( uint32_t ) floor( i_frame );
		uint32_t endFrame = ( uint32_t ) ceil( i_frame );
		float amount = i_frame - startFrame;
		calculateFrame( m_bonesHierarchy, startFrame, &m_frameValuesT1 );
		calculateFrame( m_bonesHierarchy, endFrame, &m_frameValuesT2 );
		GE_Vec3 Position[ 2 ], Scale[ 2 ];
		GE_Quat Rotation[ 2 ];
		for( uint32_t i = 0; i < m_frameValues.size(); ++i )
		{
			m_frameValuesT1[ i ].decompose( &Position[ 0 ], &Rotation[ 0 ], &Scale[ 0 ] );
			m_frameValuesT2[ i ].decompose( &Position[ 1 ], &Rotation[ 1 ], &Scale[ 1 ] );
			Position[ 0 ].lerp( &Position[ 1 ], amount );
			Scale[ 0 ].lerp( &Scale[ 1 ], amount );
			Rotation[ 0 ].lerp( &Rotation[ 1 ], amount );
			m_frameValues[ i ].srp( Position[ 0 ], Rotation[ 0 ], Scale[ 0 ] );
		}
	}
	m_parent->m_bonesBuffer.fill();
	m_parent->m_worldNeedCalculate = true;
	if( m_boundBoneID >= 0 )
		m_BoundTransform = m_frameValues[ m_boundBoneID ];
}

void GE_SkeletonController::removeHierarchy( BoneHierarchy *i_hierarchy )
{
	if( !i_hierarchy )
		return;
	for( uint32_t i = 0; i < i_hierarchy->Childs.size(); ++i )
		removeHierarchy( i_hierarchy->Childs[ i ] );
	SAFE_DELETE( i_hierarchy );
}

std::vector< GE_Mat4x4 > * GE_SkeletonController::getFrames()
{
	return &m_boneFrames;
}

std::vector< GE_SkeletonController::Bone > * GE_SkeletonController::getBones()
{
	return &m_bones;
}

std::vector< GE_Mat4x4 > * GE_SkeletonController::getFrameValues()
{
	return &m_frameValues;
}

uint32_t GE_SkeletonController::getNumberOfFrames()
{
	return m_numberOfFrames;
}

void GE_SkeletonController::calculateFrame( BoneHierarchy *i_hierarchy, uint32_t i_frame, std::vector< GE_Mat4x4 > *i_bones )
{
	if( i_hierarchy->ID >= 0 )
	{
		uint32_t offset = i_frame * ( uint32_t ) m_frameValues.size();
		if( i_hierarchy->Parent->ID < 0 )
			i_hierarchy->Matrix = m_boneFrames[ offset + i_hierarchy->ID ];
		else
			i_hierarchy->Matrix = m_boneFrames[ offset + i_hierarchy->ID ];
		i_bones->data()[ i_hierarchy->ID ] = m_bones[ i_hierarchy->ID ].InitializeMatrix * i_hierarchy->Matrix;
	}
	for( uint32_t i = 0; i < i_hierarchy->Childs.size(); ++i )
		calculateFrame( i_hierarchy->Childs[ i ], i_frame, i_bones );
}

void GE_SkeletonController::loadAnimation( const std::string i_fileName )
{
	m_numberOfFrames = 0;
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

void GE_SkeletonController::unload()
{
	removeHierarchy( m_bonesHierarchy );
	if( m_parent )
		m_parent->m_bonesBuffer.release();
	m_frameValues.clear();
	m_frameValuesT1.clear();
	m_frameValuesT2.clear();
	m_bones.clear();
	m_boneFrames.clear();
	m_boundBoneID = -1;
	m_lastFrame = -1;
	m_numberOfFrames = 0;
}
