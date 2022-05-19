#include "GE_Model.h"
#include "GE_Core.h"
#include "GE_BufferResource.h"
#include "GE_Math.h"
#include "GE_View.h"
#include "GE_Bound.h"
#include "GE_Texture.h"
#include "boost\foreach.hpp"

using namespace std;

//GE_Model::LodData ---------------------------------------------------------------------------------------------------------------------------------------------------
GE_Model::LodData::LodData()
: VB( NULL ), IB( NULL ), StreamOutBuffer( NULL )
{
}

GE_Model::LodData::~LodData()
{
	SAFE_RELEASE( VB );
	SAFE_RELEASE( IB );
	SAFE_RELEASE( StreamOutBuffer );
}

//GE_Model::Subset-----------------------------------------------------------------------------------------------------------------------------------------------------
GE_Model::Subset::Subset()
{
	LayerMultiply = 1.0f;
	LayerOffset = 0.0f;
	UseVertexColor = false;
	Material = NULL;
	LightMap = NULL;
	Lods.push_back( new LodData() );
}

GE_Model::Subset::~Subset()
{
	BOOST_FOREACH( LodData *lod, Lods )
		SAFE_DELETE( lod );
	Lods.clear();
}

//GE_Model::Instance---------------------------------------------------------------------------------------------------------------------------------------------------
void GE_Model::Instance::_updateBound()
{
	Bound.transform( &World );
}

void GE_Model::Instance::_update( )
{
	if( !NeedUpdate )
		return;
	World.srp( Position, Rotation, Scale );
	ParentModel->m_instanceData[ MyIndex ] = World;
	_updateBound();
	NeedUpdate = false;
	ParentModel->m_instanceNeedFill = true;
}

void GE_Model::Instance::setBoundRadiusMultiply( float i_boundRadiusMultiply )
{
	Bound.BoundRadiusMultiply = i_boundRadiusMultiply;
	Bound.transform( &World );
}
void GE_Model::Instance::setPosition( GE_Vec3 &i_position )
{
	Position = i_position;
	NeedUpdate = true;
	_update();
}
void GE_Model::Instance::setRotation( GE_Quat &i_rotation )
{
	Rotation = i_rotation;
	NeedUpdate = true;
	_update();
}
void GE_Model::Instance::setScale( GE_Vec3 &i_scale )
{
	Scale = i_scale;
	NeedUpdate = true;
	_update();
}

//GE_Model-----------------------------------------------------------------------------------------------------------------------------------------------------------
void GE_Model::update()
{
	if( m_transformForce )
		m_bound.transform( m_transformForce );
	else
	{
		if( m_worldNeedCalculate || m_transformBy )
		{
			m_world.srp( m_position, m_rotation, m_scale );
			if( m_transformBy )
				m_world.multiply( m_transformBy );
			if( m_transformController->getNumberOfFrames() )
				m_world.multiply( &m_transformController->getTransform()->Matrix );
			if( m_skeletonController->getBoundBoneID() < 0 )
				m_bound.transform( &m_world );
			else
				m_bound.transform( &( m_world * m_skeletonController->m_BoundTransform ) );
			m_worldNeedCalculate = false;
		}
	}
	updateInstances();
}
GE_Model::GE_Model( const std::string i_fileName )
{
	m_visible = true;
	m_isOk = false;
	m_position = GE_Vec3( 0.0f, 0.0f, 0.0f );
	m_rotation = GE_Quat( 0.0f, 0.0f, 0.0f, 0.0f );
	m_transformBy = NULL;
	m_transformForce = NULL;
	m_hasAdditionTexcoord = false;
	m_worldNeedCalculate = true;
	m_lod = 0;
	m_numberOfLod = 1;

	m_morphController = new GE_MorphController( this );
	m_transformController = new GE_TransformController( this );
	m_skeletonController = new GE_SkeletonController( this );

	m_lodFilePath.push_back( i_fileName );
	m_lodDistance.push_back( GE_ConvertToUnit( 1000000.0f ) );
	m_visibleDistance = GE_ConvertToUnit( 999999999.0f / 20.0f );

	FILE *filePtr;
	fopen_s( &filePtr, i_fileName.c_str(), "rb" );
	if( filePtr )
	{
		fseek( filePtr, 0, SEEK_END );
		uint32_t size = ftell( filePtr );
		uint8_t *chunk = new uint8_t[ size ];
		fseek( filePtr, 0, SEEK_SET );
		fread_s( chunk, size, sizeof( uint8_t ), size, filePtr );
		fclose( filePtr );
		loadFromMem( chunk );
		update();
		SAFE_DELETE_ARRAY( chunk );
	}

	if( m_subsets.size() > 0 )
		m_isOk = true;
}

GE_Model::~GE_Model()
{
	removeAllInstances();
	SAFE_DELETE( m_morphController );
	SAFE_DELETE( m_skeletonController );
	SAFE_DELETE( m_transformController );
	for( uint32_t i = 0; i < m_subsets.size(); ++i )
		SAFE_DELETE( m_subsets[ i ] );
}

void GE_Model::loadFromMem( const uint8_t *i_memory )
{
	uint8_t *mem = ( uint8_t * ) i_memory;
	ModelData *mdata = ( ModelData * ) mem;
	mem += sizeof( ModelData );
	m_name = mdata->Name;
	m_position = mdata->Position;
	m_rotation = mdata->Rotation;
	m_scale = mdata->Scale;
	m_bound = mdata->Bound;
	m_hasAdditionTexcoord = mdata->HasVerticesDataAdditionTexcoord;

	ComplexBoundData *complexBound = ( ComplexBoundData * ) mem;
	mem += sizeof( ComplexBoundData );

	m_complexBound.Boxes.resize( complexBound->NumberOfBoxes );
	m_complexBound.Spheres.resize( complexBound->NumberOfSpheres );
	m_complexBound.Capsules.resize( complexBound->NumberOfCapsules );
	m_complexBound.Convexes.resize( complexBound->NumberOfConvexes );
	m_complexBound.TriangleMeshes.resize( complexBound->NumberOfTriangleMeshes );

	for( uint32_t i = 0; i < complexBound->NumberOfBoxes; ++i )
	{
		CBBoxData *box = ( CBBoxData * ) mem;
		mem += sizeof( CBBoxData );
		m_complexBound.Boxes[ i ].Name = box->Name;
		m_complexBound.Boxes[ i ].HalfDimension = box->HalfDimension;
		m_complexBound.Boxes[ i ].Position = box->Position;
		m_complexBound.Boxes[ i ].Rotation = box->Rotation;
	}

	for( uint32_t i = 0; i < complexBound->NumberOfSpheres; ++i )
	{
		CBSphereData *sphere = ( CBSphereData * ) mem;
		mem += sizeof( CBSphereData );
		m_complexBound.Spheres[ i ].Name = sphere->Name;
		m_complexBound.Spheres[ i ].Radius = sphere->Radius;
		m_complexBound.Spheres[ i ].Position = sphere->Position;
		m_complexBound.Spheres[ i ].Rotation = sphere->Rotation;
	}

	for( uint32_t i = 0; i < complexBound->NumberOfCapsules; ++i )
	{
		CBCapsuleData *capsule = ( CBCapsuleData * ) mem;
		mem += sizeof( CBCapsuleData );
		m_complexBound.Capsules[ i ].Name = capsule->Name;
		m_complexBound.Capsules[ i ].Radius = capsule->Radius;
		m_complexBound.Capsules[ i ].HalfHeight = capsule->HalfHeight;
		m_complexBound.Capsules[ i ].Position = capsule->Position;
		m_complexBound.Capsules[ i ].Rotation = capsule->Rotation;
	}

	for( uint32_t i = 0; i < complexBound->NumberOfConvexes; ++i )
	{
		CBConvexData *convex = ( CBConvexData * ) mem;
		mem += sizeof( CBConvexData );
		m_complexBound.Convexes[ i ].Name = convex->Name;
		m_complexBound.Convexes[ i ].Position = convex->Position;
		m_complexBound.Convexes[ i ].Rotation = convex->Rotation;
		m_complexBound.Convexes[ i ].Vertices.resize( convex->NumberOfVertices );
		m_complexBound.Convexes[ i ].Indices.resize( convex->NumberOfIndices );
		memcpy_s( m_complexBound.Convexes[ i ].Vertices.data(), m_complexBound.Convexes[ i ].Vertices.size() * sizeof( GE_Vec3 ),
			mem, m_complexBound.Convexes[ i ].Vertices.size() * sizeof( GE_Vec3 ) );
		mem += sizeof( GE_Vec3 ) * m_complexBound.Convexes[ i ].Vertices.size();
		memcpy_s( m_complexBound.Convexes[ i ].Indices.data(), m_complexBound.Convexes[ i ].Indices.size() * sizeof( uint32_t ),
			mem, m_complexBound.Convexes[ i ].Indices.size() * sizeof( uint32_t ) );
		mem += sizeof( uint32_t ) * m_complexBound.Convexes[ i ].Indices.size();
	}

	for( uint32_t i = 0; i < complexBound->NumberOfTriangleMeshes; ++i )
	{
		CBTriangleMeshData *triangleMesh = ( CBTriangleMeshData * ) mem;
		mem += sizeof( CBTriangleMeshData );
		m_complexBound.TriangleMeshes[ i ].Name = triangleMesh->Name;
		m_complexBound.TriangleMeshes[ i ].Position = triangleMesh->Position;
		m_complexBound.TriangleMeshes[ i ].Rotation = triangleMesh->Rotation;
		m_complexBound.TriangleMeshes[ i ].Vertices.resize( triangleMesh->NumberOfVertices );
		m_complexBound.TriangleMeshes[ i ].Indices.resize( triangleMesh->NumberOfIndices );
		memcpy_s( m_complexBound.TriangleMeshes[ i ].Vertices.data(), m_complexBound.TriangleMeshes[ i ].Vertices.size() * sizeof( GE_Vec3 ),
			mem, m_complexBound.TriangleMeshes[ i ].Vertices.size() * sizeof( GE_Vec3 ) );
		mem += sizeof( GE_Vec3 ) * m_complexBound.TriangleMeshes[ i ].Vertices.size();
		memcpy_s( m_complexBound.TriangleMeshes[ i ].Indices.data(), m_complexBound.TriangleMeshes[ i ].Indices.size() * sizeof( uint32_t ),
			mem, m_complexBound.TriangleMeshes[ i ].Indices.size() * sizeof( uint32_t ) );
		mem += sizeof( uint32_t ) * m_complexBound.TriangleMeshes[ i ].Indices.size();
	}

	for( uint32_t i = 0; i < mdata->NumberOfSubsets; ++i )
	{
		Subset *sub = new Subset();
		m_subsets.push_back( sub );
		ModelSubsetLoader *sLoader = ( ModelSubsetLoader * ) mem;
		mem += sizeof( ModelSubsetLoader );
		sub->Lods[ 0 ]->IB = GE_Core::getSingleton().createIndexBuffer( ( uint32_t * ) mem, sizeof( uint32_t ), sLoader->NumberOfIndices );
		mem += sLoader->NumberOfIndices * sizeof( uint32_t );
		sub->Lods[ 0 ]->NumberOfIndices = sLoader->NumberOfIndices;
		sub->Lods[ 0 ]->VB = GE_Core::getSingleton().createVertexBuffer( ( VertexBuffer * ) mem, sizeof( VertexBuffer ), sLoader->NumberOfVertices );
		mem += sLoader->NumberOfVertices * sizeof( VertexBuffer );
		sub->Lods[ 0 ]->NumberOfVertices = sLoader->NumberOfVertices;
		if( mdata->HasVerticesDataTexcoord )
		{
			sub->Lods[ 0 ]->TexcoordBuffer.createBuffer( sLoader->NumberOfVertices, sizeof( GE_Vec4 ), mem );
			mem += sLoader->NumberOfVertices * sizeof( GE_Vec4 );
		}
		if( mdata->HasVerticesDataColor )
		{
			sub->Lods[ 0 ]->ColorBuffer.createBuffer( sLoader->NumberOfVertices, sizeof( VertexDataColor ), ( VertexDataColor * ) mem );
			mem += sLoader->NumberOfVertices * sizeof( VertexDataColor );
		}
		if( mdata->HasVerticesDataBoneIW )
		{
			sub->Lods[ 0 ]->BoneIndexBuffer.createBuffer( sLoader->NumberOfVertices * BONE_MAX_IW, sizeof( int32_t ), ( VertexDataBoneIndex * ) mem, true,
				DXGI_FORMAT_R32_SINT );
			mem += sLoader->NumberOfVertices * sizeof( VertexDataBoneIndex );
			sub->Lods[ 0 ]->BoneWeightBuffer.createBuffer( sLoader->NumberOfVertices * BONE_MAX_IW, sizeof( float ), ( VertexDataBoneWeight * ) mem, true,
				DXGI_FORMAT_R32_FLOAT );
			mem += sLoader->NumberOfVertices * sizeof( VertexDataBoneWeight );
		}
	}
}

void GE_Model::cleanUp()
{
	m_subsetData.clear();
}

void GE_Model::getSubsetVerticesData( uint32_t i_subset, std::vector<GE_Vec3> *i_vertices, std::vector<GE_Vec3> *i_normals, std::vector<GE_Vec4> *i_tangents,
	std::vector<GE_Vec4> *i_texcoords, std::vector<GE_Vec4> *i_colors, std::vector<GE_Model::VertexDataBoneIndex> *i_boneIndices,
	std::vector<GE_Model::VertexDataBoneWeight> *i_boneWeights, std::vector<uint32_t> *i_indices )
{
	if( m_subsetData.size() )
	{
		if( i_vertices )
			*i_vertices = m_subsetData[ i_subset ].Vertices;
		if( i_normals )
			*i_normals = m_subsetData[ i_subset ].Normals;
		if( i_tangents )
			*i_tangents = m_subsetData[ i_subset ].Tangents;
		if( i_texcoords )
			*i_texcoords = m_subsetData[ i_subset ].Texcoords;
		if( i_colors )
			*i_colors = m_subsetData[ i_subset ].Colors;
		if( i_boneIndices )
			*i_boneIndices = m_subsetData[ i_subset ].BoneIndices;
		if( i_boneWeights )
			*i_boneWeights = m_subsetData[ i_subset ].BoneWeights;
		if( i_indices )
			*i_indices = m_subsetData[ i_subset ].Indices;
		return;
	}
	FILE *fp;

	fopen_s( &fp, getLodFilePath( 0 ).c_str(), "rb" );

	if( fp )
	{
		fseek( fp, 0, SEEK_END );
		uint32_t size = ftell( fp );
		byte *chunk = new byte[ size ];
		fseek( fp, 0, SEEK_SET );
		fread_s( chunk, size, sizeof( byte ), size, fp );
		fclose( fp );
		byte *mem = ( byte * ) chunk;
		ModelData *mdata = ( ModelData * ) mem;
		mem += sizeof( ModelData );

		ComplexBoundData *complexBound = ( ComplexBoundData * ) mem;
		mem += sizeof( ComplexBoundData );
		mem += sizeof( CBBoxData ) * complexBound->NumberOfBoxes;
		mem += sizeof( CBSphereData ) * complexBound->NumberOfSpheres;
		mem += sizeof( CBCapsuleData ) * complexBound->NumberOfCapsules;

		for( uint32_t i = 0; i < complexBound->NumberOfConvexes; ++i )
		{
			mem += sizeof( CBConvexData );
			mem += sizeof( GE_Vec3 ) * m_complexBound.Convexes[ i ].Vertices.size();
			mem += sizeof( uint32_t ) * m_complexBound.Convexes[ i ].Indices.size();
		}

		for( uint32_t i = 0; i < complexBound->NumberOfTriangleMeshes; ++i )
		{
			mem += sizeof( CBTriangleMeshData );
			mem += sizeof( GE_Vec3 ) * m_complexBound.TriangleMeshes[ i ].Vertices.size();
			mem += sizeof( uint32_t ) * m_complexBound.TriangleMeshes[ i ].Indices.size();
		}

		for( uint32_t i = 0; i < mdata->NumberOfSubsets && i <= i_subset; ++i )
		{
			ModelSubsetLoader *sLoader = ( ModelSubsetLoader * ) mem;
			mem += sizeof( ModelSubsetLoader );

			if( i_subset != i )
			{
				mem += sLoader->NumberOfIndices * sizeof( uint32_t );
				mem += sLoader->NumberOfVertices * sizeof( VertexBuffer );
				if( mdata->HasVerticesDataTexcoord )
					mem += sLoader->NumberOfVertices * sizeof( GE_Vec4 );
				if( mdata->HasVerticesDataColor )
					mem += sLoader->NumberOfVertices * sizeof( VertexDataColor );
				if( mdata->HasVerticesDataBoneIW )
				{
					mem += sLoader->NumberOfVertices * sizeof( VertexDataBoneIndex );
					mem += sLoader->NumberOfVertices * sizeof( VertexDataBoneWeight );
				}
				continue;
			}

			if( i_indices )
			{
				i_indices->resize( sLoader->NumberOfIndices );
				memcpy_s( i_indices->data(), sizeof( uint32_t ) * i_indices->size(), mem, sizeof( uint32_t ) * i_indices->size() );
			}
			mem += sLoader->NumberOfIndices * sizeof( uint32_t );

			if( i_vertices )
				i_vertices->resize( sLoader->NumberOfVertices );

			if( i_normals )
				i_normals->resize( sLoader->NumberOfVertices );

			if( i_tangents )
				i_tangents->resize( sLoader->NumberOfVertices );

			for( uint32_t vertIndex = 0; vertIndex < sLoader->NumberOfVertices; vertIndex++ )
			{
				VertexBuffer *vb = ( VertexBuffer * ) mem;
				if( i_vertices )
					i_vertices->data()[ vertIndex ] = vb->Position;
				if( i_normals )
					i_normals->data()[ vertIndex ] = vb->Normal;
				if( i_tangents )
					i_tangents->data()[ vertIndex ] = vb->Tangent;
				mem += sizeof( VertexBuffer );
			}

			if( mdata->HasVerticesDataTexcoord )
			{
				if( i_texcoords )
				{
					i_texcoords->resize( sLoader->NumberOfVertices );
					memcpy_s( i_texcoords->data(), sizeof( GE_Vec4 ) * i_texcoords->size(), mem, sizeof( GE_Vec4 ) * i_texcoords->size() );
				}
				mem += sLoader->NumberOfVertices * sizeof( GE_Vec4 );
			}
			if( mdata->HasVerticesDataColor )
			{
				if( i_colors )
				{
					i_colors->resize( sLoader->NumberOfVertices );
					memcpy_s( i_texcoords->data(), sizeof( VertexDataColor ) * i_colors->size(), mem, sizeof( VertexDataColor ) * i_colors->size() );
				}
				mem += sLoader->NumberOfVertices * sizeof( VertexDataColor );
			}
			if( mdata->HasVerticesDataBoneIW )
			{
				if( i_boneIndices )
				{
					i_boneIndices->resize( sLoader->NumberOfVertices );
					memcpy_s( i_boneIndices->data(), sizeof( VertexDataBoneIndex ) * i_boneIndices->size(), mem, sizeof( VertexDataBoneIndex ) * i_boneIndices->size() );
				}
				mem += sLoader->NumberOfVertices * sizeof( VertexDataBoneIndex );
				if( i_boneWeights )
				{
					i_boneWeights->resize( sLoader->NumberOfVertices );
					memcpy_s( i_boneWeights->data(), sizeof( VertexDataBoneWeight ) * i_boneWeights->size(), mem, sizeof( VertexDataBoneWeight ) * i_boneWeights->size() );
				}
				mem += sLoader->NumberOfVertices * sizeof( VertexDataBoneWeight );
			}
		}

		SAFE_DELETE_ARRAY( chunk );
	}
}

void GE_Model::fillInstances()
{
	if( !m_instanceNeedFill || !m_instanceBuffer.m_lenght )
		return;
	m_instanceBuffer.fill();
	m_instanceNeedFill = false;
}

void GE_Model::removeAllInstances()
{
	BOOST_FOREACH( GE_Model::Instance *instance, m_instancesList )
		SAFE_DELETE( instance );
	m_instanceData.clear();
	m_instancesList.clear();
	m_instanceBuffer.release();
	for( uint32_t i = 0; i < m_subsets.size(); ++i )
		for( uint32_t j = 0; j < m_subsets[ i ]->Lods.size(); ++j )
			SAFE_RELEASE( m_subsets[ i ]->Lods[ j ]->StreamOutBuffer );
}

list<GE_Model::Instance *>::iterator GE_Model::createInstance( uint32_t i_count )
{
	removeAllInstances();
	m_instanceData.resize( i_count );
	for( uint32_t i = 0; i < i_count; i++ )
	{
		GE_Model::Instance *instance = new GE_Model::Instance();
		instance->ParentModel = this;
		instance->Position = m_position;
		instance->Rotation = m_rotation;
		instance->Scale = m_scale;
		instance->World = m_world;
		instance->Bound = m_bound;
		instance->NeedUpdate = true;
		instance->MyIndex = ( uint32_t ) m_instancesList.size();
		m_instancesList.push_back( instance );
		m_instanceData[ i ] = m_world;
	}
	list<GE_Model::Instance *>::iterator it = m_instancesList.end();
	for( ; i_count; --i_count )
		--it;

	m_numberOfRenderInstance = ( uint32_t ) m_instancesList.size();
	m_instanceBuffer.createBuffer( m_numberOfRenderInstance, sizeof( GE_Mat4x4 ), m_instanceData.data(), false, DXGI_FORMAT_R32G32B32A32_FLOAT, 4 );
	m_instanceNeedFill = false;

	for( uint32_t i = 0; i < m_subsets.size(); ++i )
	{
		for( uint32_t j = 0; j < m_numberOfLod; ++j )
		{
			if( ( m_subsets[ i ]->MorphTargetsBuffer.m_lenght || m_bonesBuffer.m_lenght ) && !m_subsets[ i ]->Lods[ j ]->StreamOutBuffer )
				CreateStreamOutBuffer( i, j );
		}
	}
	return it;
}

void GE_Model::removeInstance( GE_Model::Instance **i_instance )
{
	int i = 0;
	BOOST_FOREACH( GE_Model::Instance *instance, m_instancesList )
	{
		if( ( *i_instance ) == instance )
		{
			m_instancesList.remove( instance );
			SAFE_DELETE( instance );
			break;
		}
		++i;
	}

	m_instanceData.clear();
	i = 0;
	BOOST_FOREACH( GE_Model::Instance *instance, m_instancesList )
	{
		instance->MyIndex = i;
		m_instanceData.push_back( instance->World );
		++i;
	}

	m_numberOfRenderInstance = ( uint32_t ) m_instancesList.size();
	if( m_numberOfRenderInstance )
	{
		m_instanceBuffer.createBuffer( m_numberOfRenderInstance, sizeof( GE_Mat4x4 ), m_instanceData.data(), false, DXGI_FORMAT_R32G32B32A32_FLOAT, 4 );
		m_instanceNeedFill = false;
	} else
	{
		m_instanceBuffer.release();
		for( uint32_t i = 0; i < m_subsets.size(); ++i )
			for( uint32_t j = 0; j < m_subsets[ i ]->Lods.size(); ++j )
				SAFE_RELEASE( m_subsets[ i ]->Lods[ j ]->StreamOutBuffer );
	}
}

void GE_Model::addInstance()
{

}

std::string &GE_Model::getLodFilePath( uint32_t i_lod )
{
	auto it = m_lodFilePath.begin();
	std::advance( it, i_lod );
	return *it;
}

bool GE_Model::addLod( std::string &i_filePath )
{
	FILE *fp;
	fopen_s( &fp, i_filePath.c_str(), "rb" );
	if( !fp )
		return false;

	fseek( fp, 0, SEEK_END );
	uint32_t size = ftell( fp );
	uint8_t *chunk = new uint8_t[ size ];
	fseek( fp, 0, SEEK_SET );
	fread_s( chunk, size, sizeof( uint8_t ), size, fp );
	fclose( fp );
	bool ok = loadLodFromMem( chunk );
	if( ok )
	{
		m_lodFilePath.push_back( i_filePath );
		m_lodDistance.push_back( GE_ConvertToUnit( 1000000 ) );
		m_numberOfLod++;
	}
	SAFE_DELETE_ARRAY( chunk );
	return ok;
}

void GE_Model::removeLod( uint32_t i_lod )
{
	if( i_lod == 0 || i_lod >= m_numberOfLod )
		return;

	for( uint32_t i = 0; i < m_subsets.size(); ++i )
	{
		LodData *lod = m_subsets[ i ]->Lods[ i_lod ];
		m_subsets[ i ]->Lods.erase( m_subsets[ i ]->Lods.begin() + i_lod );
		SAFE_DELETE( lod );
	}

	auto it = m_lodFilePath.begin();
	std::advance( it, i_lod );
	m_lodFilePath.erase( it );
	auto itDistance = m_lodDistance.begin() + i_lod;
	m_lodDistance.erase( itDistance );
	m_numberOfLod--;
}

bool GE_Model::loadLodFromMem( const uint8_t *i_memory )
{
	uint8_t *mem = ( uint8_t * ) i_memory;
	ModelData *mdata = ( ModelData * ) mem;
	mem += sizeof( ModelData );

	ComplexBoundData *complexBound = ( ComplexBoundData * ) mem;
	mem += sizeof( ComplexBoundData );
	mem += sizeof( CBBoxData ) * complexBound->NumberOfBoxes;
	mem += sizeof( CBSphereData ) * complexBound->NumberOfSpheres;
	mem += sizeof( CBCapsuleData ) * complexBound->NumberOfCapsules;

	for( uint32_t i = 0; i < complexBound->NumberOfConvexes; ++i )
	{
		mem += sizeof( CBConvexData );
		mem += sizeof( GE_Vec3 ) * m_complexBound.Convexes[ i ].Vertices.size();
		mem += sizeof( uint32_t ) * m_complexBound.Convexes[ i ].Indices.size();
	}

	for( uint32_t i = 0; i < complexBound->NumberOfTriangleMeshes; ++i )
	{
		mem += sizeof( CBTriangleMeshData );
		mem += sizeof( GE_Vec3 ) * m_complexBound.TriangleMeshes[ i ].Vertices.size();
		mem += sizeof( uint32_t ) * m_complexBound.TriangleMeshes[ i ].Indices.size();
	}

	if( mdata->NumberOfSubsets != this->getNumberOfSubsets() )
		return false;

	for( uint32_t i = 0; i < mdata->NumberOfSubsets; ++i )
	{
		LodData *lod = new LodData();
		m_subsets[ i ]->Lods.push_back( lod );

		ModelSubsetLoader *sLoader = ( ModelSubsetLoader * ) mem;
		mem += sizeof( ModelSubsetLoader );
		lod->IB = GE_Core::getSingleton().createIndexBuffer( ( uint32_t * ) mem, sizeof( uint32_t ), sLoader->NumberOfIndices );
		mem += sLoader->NumberOfIndices * sizeof( uint32_t );
		lod->NumberOfIndices = sLoader->NumberOfIndices;
		lod->VB = GE_Core::getSingleton().createVertexBuffer( ( VertexBuffer * ) mem, sizeof( VertexBuffer ), sLoader->NumberOfVertices );
		mem += sLoader->NumberOfVertices * sizeof( VertexBuffer );
		lod->NumberOfVertices = sLoader->NumberOfVertices;
		if( mdata->HasVerticesDataTexcoord )
		{
			lod->TexcoordBuffer.createBuffer( sLoader->NumberOfVertices, sizeof( GE_Vec4 ), mem );
			mem += sLoader->NumberOfVertices * sizeof( GE_Vec4 );
		}
		if( mdata->HasVerticesDataColor )
		{
			lod->ColorBuffer.createBuffer( sLoader->NumberOfVertices, sizeof( VertexDataColor ), ( VertexDataColor * ) mem );
			mem += sLoader->NumberOfVertices * sizeof( VertexDataColor );
		}
		if( mdata->HasVerticesDataBoneIW )
		{

			lod->BoneIndexBuffer.createBuffer( sLoader->NumberOfVertices * BONE_MAX_IW, sizeof( int32_t ), ( VertexDataBoneIndex * ) mem, true,
				DXGI_FORMAT_R32_SINT );
			mem += sLoader->NumberOfVertices * sizeof( VertexDataBoneIndex );
			lod->BoneWeightBuffer.createBuffer( sLoader->NumberOfVertices * BONE_MAX_IW, sizeof( float ), ( VertexDataBoneWeight * ) mem, true,
				DXGI_FORMAT_R32_FLOAT );
			mem += sLoader->NumberOfVertices * sizeof( VertexDataBoneWeight );
		}
	}
	return true;
}

uint32_t GE_Model::getNumberOfVertices( uint32_t i_lod )
{
	uint32_t count = 0;
	for( uint32_t i = 0; i < getNumberOfSubsets(); i++ )
		count += m_subsets[ i ]->Lods[ i_lod ]->NumberOfVertices;
	return count;
}

uint32_t GE_Model::getNumberOfIndices( uint32_t i_lod )
{
	uint32_t count = 0;
	for( uint32_t i = 0; i < getNumberOfSubsets(); i++ )
		count += m_subsets[ i ]->Lods[ i_lod ]->NumberOfIndices;
	return count;
}

uint32_t GE_Model::getNumberOfSubsetVertices( uint32_t i_subset, uint32_t i_lod )
{
	return m_subsets[ i_subset ]->Lods[ i_lod ]->NumberOfVertices;
}

uint32_t GE_Model::getNumberOfSubsetIndices( uint32_t i_subset, uint32_t i_lod )
{
	return m_subsets[ i_subset ]->Lods[ i_lod ]->NumberOfIndices;
}

void GE_Model::updateInstances()
{
	fillInstances();
}
