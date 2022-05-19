#include "GE_Model.h"
#include "GE_Core.h"

inline void GE_Model::setVisible( bool i_visible )
{
	m_visible = i_visible;
}

inline bool GE_Model::isVisible()
{
	return m_visible;
}

inline bool GE_Model::isOk()
{
	return m_isOk;
}

inline void GE_Model::setPosition( GE_Vec3 &i_position )
{
	m_position = i_position;
	m_worldNeedCalculate = true;
}

inline GE_Vec3 &GE_Model::getPosition()
{
	return m_position;
}

inline void GE_Model::setRotation( GE_Quat &i_rotation )
{
	m_rotation = i_rotation;
	m_worldNeedCalculate = true;
}

inline GE_Quat &GE_Model::getRotation()
{
	return m_rotation;
}

inline void GE_Model::setScale( GE_Vec3 &i_scale )
{
	m_scale = i_scale;
	m_worldNeedCalculate = true;
}

inline GE_Vec3 &GE_Model::getScale()
{
	return m_scale;
}

inline GE_Mat4x4 *GE_Model::getWorld()
{
	if ( m_transformForce )
		return m_transformForce;
	return &m_world;
}

inline void GE_Model::setLod( uint32_t i_lod )
{
	m_lod = i_lod;
}

inline void GE_Model::setLodDistance( uint32_t i_lod, float i_distance )
{
	m_lodDistance[ i_lod ] = i_distance;
}

inline void GE_Model::setVisibleDistance( float i_distance )
{
	m_visibleDistance = i_distance;
}

inline float GE_Model::getVisibleDistance()
{
	return m_visibleDistance;
}

inline uint32_t GE_Model::getLod()
{
	return m_lod;
}

inline float GE_Model::getLodDistance( uint32_t i_lod )
{
	return m_lodDistance[ i_lod ];
}

inline GE_Model::Subset *GE_Model::getSubset( uint32_t i_subset )
{
	return m_subsets[ i_subset ];
}

inline std::vector<GE_Model::Subset *> &GE_Model::getSubsets()
{
	return m_subsets;
}

inline GE_Mat4x4 *GE_Model::getTransformBy()
{
	return m_transformBy;
}

inline GE_Mat4x4 *GE_Model::getTransformForce()
{
	return m_transformForce;
}

inline void GE_Model::setTransformBy( const GE_Mat4x4 *i_transform )
{
	m_transformBy = ( GE_Mat4x4 * )i_transform;
}

inline void GE_Model::setTransformForce( const GE_Mat4x4 *i_transform )
{
	m_transformForce = ( GE_Mat4x4 * )i_transform;
}

inline GE_Bound &GE_Model::getBound()
{
	return m_bound;
}

inline void GE_Model::render( uint32_t i_subset, uint32_t i_lod )
{
	GE_Core::getSingleton().renderTriangleList( m_subsets[ i_subset ]->Lods[ i_lod ]->VB, sizeof( GE_Model::VertexBuffer ), m_subsets[ i_subset ]->Lods[ i_lod ]->IB
		, m_subsets[ i_subset ]->Lods[ i_lod ]->NumberOfIndices );
}

inline void GE_Model::renderInstance( uint32_t i_subset, uint32_t i_lod )
{
	GE_Core::getSingleton().renderTriangleList( m_subsets[ i_subset ]->Lods[ i_lod ]->VB, sizeof( GE_Model::VertexBuffer ),
		m_subsets[ i_subset ]->Lods[ i_lod ]->IB, m_subsets[ i_subset ]->Lods[ i_lod ]->NumberOfIndices, m_instanceBuffer.m_lenght );
}

inline void GE_Model::renderInstanceToStreamOut( uint32_t i_subset, uint32_t i_lod )
{
	SetUpStreamOut( i_subset, i_lod );
	render( i_subset, i_lod );
	SetDownStreamOut();
}

inline void GE_Model::renderInstanceByStreamOut( uint32_t i_subset, uint32_t i_lod )
{
	GE_Core::getSingleton().renderTriangle( m_subsets[ i_subset ]->Lods[ i_lod ]->StreamOutBuffer, sizeof( GE_Model::VertexBuffer ),
		m_subsets[ i_subset ]->Lods[ i_lod ]->NumberOfIndices, m_numberOfRenderInstance );
	ID3D11Buffer *b = NULL;
	uint32_t offset = 0, strid = 0;
	GE_Core::getSingleton().getImmediateContext()->IASetVertexBuffers( 0, 1, &b, &strid, &offset );
}

inline void GE_Model::SetUpStreamOut( uint32_t i_subset, uint32_t i_lod )
{
	uint32_t offset = 0;
	GE_Core::getSingleton().getImmediateContext()->SOSetTargets( 1, &m_subsets[ i_subset ]->Lods[ i_lod ]->StreamOutBuffer, &offset );
}

inline void GE_Model::SetDownStreamOut()
{
	uint32_t offset = 0;
	ID3D11Buffer *b = NULL;
	GE_Core::getSingleton().getImmediateContext()->SOSetTargets( 1, &b, &offset );
}

inline void GE_Model::CreateStreamOutBuffer( uint32_t i_subset, uint32_t i_lod )
{
	SAFE_RELEASE( m_subsets[ i_subset ]->Lods[ i_lod ]->StreamOutBuffer );
	D3D11_BUFFER_DESC vbdescSO =
	{
		m_subsets[ i_subset ]->Lods[ i_lod ]->NumberOfIndices * sizeof( GE_Model::VertexBuffer ),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT,
		0,
		0
	};
	GE_Core::getSingleton().getDevice()->CreateBuffer( &vbdescSO, NULL, &m_subsets[ i_subset ]->Lods[ i_lod ]->StreamOutBuffer );
}

inline uint32_t GE_Model::getNumberOfRenderInstances()
{
	return m_numberOfRenderInstance;
}

inline uint32_t GE_Model::getNumberOfLod()
{
	return m_numberOfLod;
}

inline bool GE_Model::isBufferDynamic()
{
	return m_bufferDynamic;
}

inline void GE_Model::setBufferDynamic( bool i_bufferDynamic )
{
	/*m_bufferDynamic = i_bufferDynamic;

	m_lodFilePath.push_back( m_lodFilePath- );

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
	}*/
}

inline bool GE_Model::hasAdditionTexcoord()
{
	return m_hasAdditionTexcoord;
}

inline uint32_t GE_Model::getNumberOfSubsets()
{
	return ( uint32_t ) m_subsets.size();
}

inline GE_Material * GE_Model::getMaterial( uint32_t i_subset )
{
	return m_subsets[ i_subset ]->Material;
}

inline GE_Texture	* GE_Model::getLightMap( uint32_t i_subset )
{
	return m_subsets[ i_subset ]->LightMap;
}

inline void GE_Model::setMaterial( uint32_t i_subset, const GE_Material *i_material )
{
	m_subsets[ i_subset ]->Material = ( GE_Material* )i_material;
}

inline void GE_Model::setLightMap( uint32_t i_subset, const GE_Texture *i_lightMap )
{
	m_subsets[ i_subset ]->LightMap = ( GE_Texture* )i_lightMap;
}

inline GE_Model::ComplexBound * GE_Model::getComplexBound()
{
	return &m_complexBound;
}

inline GE_MorphController * GE_Model::getMorphController()
{
	return m_morphController;
}

inline GE_SkeletonController * GE_Model::getSkeletonController()
{
	return m_skeletonController;
}

inline GE_TransformController * GE_Model::getTransformController()
{
	return m_transformController;
}

inline std::string	& GE_Model::getName()
{
	return m_name;
}

inline void GE_Model::setName( std::string i_name )
{
	m_name = i_name;
}
