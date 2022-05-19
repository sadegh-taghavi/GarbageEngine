#include "GE_RenderUtility.h"
#include "GE_Core.h"
#include "GE_Math.h"
#include "GE_View.h"
#include <boost\foreach.hpp>

//GE_RenderUtilityObject----------------------------------------------------------------------------------------------------------------------------------------------

using namespace std;

void GE_RenderUtilityObject::render()
{
	GE_Core::getSingleton().getImmediateContext()->IASetPrimitiveTopology( m_topology );
	UINT il = ( UINT ) m_indicesData.size();
	UINT vl = ( UINT ) m_verticesData.size();
	if( il )
		GE_Core::getSingleton().renderTriangleList( m_vB, sizeof( GE_Vec3 ), m_iB, il );
	else
		GE_Core::getSingleton().renderTriangle( m_vB, sizeof( GE_Vec3 ), vl );
}

void GE_RenderUtilityObject::update()
{
	if( m_transformForce )
		m_bound.transform( m_transformForce );
	else
	{
		if( m_transformChange || m_transformBy )
		{
			m_world.srp( m_position, m_rotation, m_scale );
			if( m_transformBy )
				m_world.multiply( m_transformBy );
			m_bound.transform( &m_world );
			m_transformChange = false;
		}
	}

	if( m_verticesLen != m_verticesData.size() )
	{
		SAFE_RELEASE( m_vB );
		m_verticesLen = ( UINT ) m_verticesData.size();
		m_vB = GE_Core::getSingleton().createVertexBuffer( m_verticesData.data(), sizeof( GE_Vec3 ), m_verticesLen, false );
		m_verticesChange = false;
		calculateBound();
	}

	if( m_verticesChange && m_verticesLen )
	{
		D3D11_MAPPED_SUBRESOURCE mr;
		GE_Core::getSingleton().getImmediateContext()->Map( m_vB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr );
		memcpy( mr.pData, m_verticesData.data(), sizeof( GE_Vec3 ) * m_verticesLen );
		GE_Core::getSingleton().getImmediateContext()->Unmap( m_vB, 0 );
		m_verticesChange = false;
		calculateBound();
	}

	if( m_indicesLen != m_indicesData.size() )
	{
		SAFE_RELEASE( m_iB );
		m_indicesLen = ( UINT ) m_indicesData.size();
		m_iB = GE_Core::getSingleton().createIndexBuffer( m_indicesData.data(), sizeof( UINT ), m_indicesLen, false );
		m_indicesChange = false;
	}

	if( m_indicesChange && m_indicesLen )
	{
		D3D11_MAPPED_SUBRESOURCE mr;
		GE_Core::getSingleton().getImmediateContext()->Map( m_iB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr );
		memcpy( mr.pData, m_indicesData.data(), sizeof( UINT ) * m_indicesLen );
		GE_Core::getSingleton().getImmediateContext()->Unmap( m_iB, 0 );
		m_indicesChange = false;
	}

	if( m_colorsLen != m_colorsData.size() )
	{
		m_colorsLen = ( UINT ) m_colorsData.size();
		m_colorBuffer.createBuffer( m_colorsLen, sizeof( GE_Vec3 ), m_colorsData.data(), false, DXGI_FORMAT_R32G32B32_FLOAT );
		m_colorChange = false;
	}

	if( m_colorChange && m_colorsLen )
	{
		m_colorBuffer.fill();
		m_colorChange = false;
	}

	if( m_texcoordLen != m_texcoordData.size() )
	{
		m_texcoordLen = ( UINT ) m_texcoordData.size();
		m_texcoordBuffer.createBuffer( m_texcoordLen, sizeof( GE_Vec2 ), m_texcoordData.data(), false, DXGI_FORMAT_R32G32_FLOAT );
		m_texcoordChange = false;
	}

	if( m_texcoordChange && m_texcoordLen )
	{
		m_texcoordBuffer.fill();
		m_texcoordChange = false;
	}
}

void GE_RenderUtilityObject::setTexture( const char *i_fileName )
{
	if( m_texture )
		GE_TextureManager::getSingleton().remove( &m_texture );
	m_texture = GE_TextureManager::getSingleton().createTexture( i_fileName );
}

ID3D11ShaderResourceView *GE_RenderUtilityObject::getSRV()
{
	if( m_sRV )
		return m_sRV;
	if( m_texture )
		return m_texture->getTextureSRV();
	return NULL;
}

void GE_RenderUtilityObject::calculateBound()
{
	uint32_t s = ( uint32_t ) m_verticesData.size();
	if( !s )
		return;
	m_bound.RawBoundBoxMin = m_verticesData.data()[ 0 ];
	m_bound.RawBoundBoxMax = m_verticesData.data()[ 0 ];
	GE_Vec3 t;
	for( uint32_t i = 1; i < s; ++i )
	{
		t = m_verticesData.data()[ i ];
		if( t.x < m_bound.RawBoundBoxMin.x )
			m_bound.RawBoundBoxMin.x = t.x;
		if( t.y < m_bound.RawBoundBoxMin.y )
			m_bound.RawBoundBoxMin.y = t.y;
		if( t.z < m_bound.RawBoundBoxMin.z )
			m_bound.RawBoundBoxMin.z = t.z;

		if( t.x > m_bound.RawBoundBoxMax.x )
			m_bound.RawBoundBoxMax.x = t.x;
		if( t.y > m_bound.RawBoundBoxMax.y )
			m_bound.RawBoundBoxMax.y = t.y;
		if( t.z > m_bound.RawBoundBoxMax.z )
			m_bound.RawBoundBoxMax.z = t.z;
	}

	if( m_transformForce )
		m_bound.transform( m_transformForce );
	else
		m_bound.transform( &m_world );
}

GE_RenderUtilityObject::~GE_RenderUtilityObject()
{
	SAFE_RELEASE( m_vB );
	SAFE_RELEASE( m_iB );
	if( m_texture )
		GE_TextureManager::getSingleton().remove( &m_texture );
}

GE_RenderUtilityObject::GE_RenderUtilityObject()
{
	m_sRV = NULL;
	m_texture = NULL;
	m_bound.BoundRadiusMultiply = 1.0f;
	m_vB = NULL;
	m_iB = NULL;
	m_visible = true;
	m_depthDisable = true;
	m_verticesChange = false;
	m_colorChange = false;
	m_transformChange = false;
	m_indicesChange = false;
	m_texcoordChange = false;
	m_position = GE_Vec3( 0.0f, 0.0f, 0.0f );
	m_rotation.identity();
	m_color = m_scale = GE_Vec3( 1.0f, 1.0f, 1.0f );
	m_indicesLen = 0;
	m_verticesLen = 0;
	m_colorsLen = 0;
	m_texcoordLen = 0;
	m_billboard = false;
	m_alphaTest = 0.0f;
	m_world.identity();
	m_transformBy = NULL;
	m_transformForce = NULL;
	m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void GE_RenderUtilityObject::fillColorBuffer( GE_Vec3 &i_color )
{
	for( uint32_t i = 0; i < m_colorsData.size(); i++ )
		m_colorsData.data()[ i ] = i_color;
	m_colorChange = true;
}

void GE_RenderUtilityObject::createBox( GE_Vec3 &i_size, GE_Vec3 &i_offset )
{
	GE_Vec3 bMin = -i_size;
	GE_Vec3 bMax = i_size;

	std::vector<GE_Vec3> *vb = verticesDataBeginEdit();
	vb->resize( 8 );
	vb->data()[ 0 ] = bMin + i_offset;
	vb->data()[ 1 ] = GE_Vec3( bMax.x, bMin.y, bMin.z ) + i_offset;
	vb->data()[ 2 ] = GE_Vec3( bMin.x, bMin.y, bMax.z ) + i_offset;
	vb->data()[ 3 ] = GE_Vec3( bMax.x, bMin.y, bMax.z ) + i_offset;
	vb->data()[ 4 ] = GE_Vec3( bMin.x, bMax.y, bMin.z ) + i_offset;
	vb->data()[ 5 ] = GE_Vec3( bMax.x, bMax.y, bMin.z ) + i_offset;
	vb->data()[ 6 ] = GE_Vec3( bMin.x, bMax.y, bMax.z ) + i_offset;
	vb->data()[ 7 ] = bMax + i_offset;
	verticesDataEndEdit();

	std::vector<int> *ib = indicesDataBeginEdit();
	ib->resize( 16 );
	ib->data()[ 0 ] = 0;
	ib->data()[ 1 ] = 1;
	ib->data()[ 2 ] = 3;
	ib->data()[ 3 ] = 2;
	ib->data()[ 4 ] = 0;

	ib->data()[ 5 ] = 4;
	ib->data()[ 6 ] = 5;
	ib->data()[ 7 ] = 7;
	ib->data()[ 8 ] = 6;
	ib->data()[ 9 ] = 4;

	ib->data()[ 10 ] = 5;
	ib->data()[ 11 ] = 1;
	ib->data()[ 12 ] = 3;
	ib->data()[ 13 ] = 7;
	ib->data()[ 14 ] = 6;
	ib->data()[ 15 ] = 2;
	indicesDataEndEdit();

	setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );
}


void GE_RenderUtilityObject::createBoxBB( GE_Vec3 &i_bbMin, GE_Vec3 &i_bbMax )
{
	std::vector<GE_Vec3> *vb = verticesDataBeginEdit();
	vb->resize( 8 );
	vb->data()[ 0 ] = i_bbMin;
	vb->data()[ 1 ] = GE_Vec3( i_bbMax.x, i_bbMin.y, i_bbMin.z );
	vb->data()[ 2 ] = GE_Vec3( i_bbMin.x, i_bbMin.y, i_bbMax.z );
	vb->data()[ 3 ] = GE_Vec3( i_bbMax.x, i_bbMin.y, i_bbMax.z );
	vb->data()[ 4 ] = GE_Vec3( i_bbMin.x, i_bbMax.y, i_bbMin.z );
	vb->data()[ 5 ] = GE_Vec3( i_bbMax.x, i_bbMax.y, i_bbMin.z );
	vb->data()[ 6 ] = GE_Vec3( i_bbMin.x, i_bbMax.y, i_bbMax.z );
	vb->data()[ 7 ] = i_bbMax;
	verticesDataEndEdit();

	std::vector<int> *ib = indicesDataBeginEdit();
	ib->resize( 16 );
	ib->data()[ 0 ] = 0;
	ib->data()[ 1 ] = 1;
	ib->data()[ 2 ] = 3;
	ib->data()[ 3 ] = 2;
	ib->data()[ 4 ] = 0;

	ib->data()[ 5 ] = 4;
	ib->data()[ 6 ] = 5;
	ib->data()[ 7 ] = 7;
	ib->data()[ 8 ] = 6;
	ib->data()[ 9 ] = 4;

	ib->data()[ 10 ] = 5;
	ib->data()[ 11 ] = 1;
	ib->data()[ 12 ] = 3;
	ib->data()[ 13 ] = 7;
	ib->data()[ 14 ] = 6;
	ib->data()[ 15 ] = 2;
	indicesDataEndEdit();

	setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );
}


void GE_RenderUtilityObject::createSphere( float i_radius, GE_Vec3 &i_offset, uint32_t i_vSlice, uint32_t i_hSlice )
{
	uint32_t vSlice = i_vSlice * 2;
	uint32_t hSlice = i_hSlice;

	uint32_t nbVertices = vSlice * hSlice;
	std::vector<GE_Vec3> *vb = verticesDataBeginEdit();
	vb->resize( nbVertices );
	float vAngle = 360.0f / ( float ) vSlice;
	float angle = 0;
	for( uint32_t i = 0; i < vSlice; i++ )
	{
		vb->data()[ i ].x = sinf( GE_ConvertToRadians( angle ) ) * i_radius;
		vb->data()[ i ].y = 0.0f;
		vb->data()[ i ].z = cosf( GE_ConvertToRadians( angle ) ) * i_radius;
		angle += vAngle;
	}
	GE_Mat4x4	trans;
	GE_Vec3		axis( 0, 0, 1 );
	GE_Quat		rot;
	float		hAngle = 180.0f / ( float ) hSlice;
	rot.identity();
	trans.identity();
	angle = hAngle;
	for( uint32_t j = 1; j < hSlice; j++ )
	{
		rot.rotationAxis( &axis, GE_ConvertToRadians( angle ) );
		trans.RotationQuaternion( &rot );
		angle += hAngle;
		for( uint32_t i = 0; i < vSlice; i++ )
		{
			vb->data()[ i ].transformOut( &vb->data()[ j * vSlice + i ], &trans );
		}
	}
	for( uint32_t i = 0; i < nbVertices; i++ )
		vb->data()[ i ] += i_offset;
	verticesDataEndEdit();

	uint32_t nbIndices = ( vSlice + 1 ) * hSlice - ( hSlice - 1 );
	std::vector<int> *ib = indicesDataBeginEdit();
	ib->resize( nbIndices );
	for( uint32_t i = 0; i < nbIndices; i++ )
	{
		if( i%vSlice == 0 )
			ib->data()[ i ] = 0;
		else
			ib->data()[ i ] = i;
	}
	indicesDataEndEdit();

	setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );
}

void GE_RenderUtilityObject::createCylinder( float i_radius, float i_height, GE_Vec3 &i_offset, uint32_t i_slice )
{
	uint32_t nbVertices = 4 * i_slice;
	float height = i_height / 2.0f;
	std::vector<GE_Vec3> *vb = verticesDataBeginEdit();
	vb->resize( nbVertices );
	vb->data()[ 0 ] = GE_Vec3( i_radius, 0.0f, height );
	vb->data()[ 1 ] = GE_Vec3( i_radius, 0.0f, -height );
	vb->data()[ 2 ] = GE_Vec3( -i_radius, 0.0f, -height );
	vb->data()[ 3 ] = GE_Vec3( -i_radius, 0.0f, height );

	GE_Vec3		axis( 0, 0, 1 );
	GE_Quat		rot;
	GE_Mat4x4	trans;
	float		hAngle = 180.0f / ( float ) i_slice;
	rot.identity();
	trans.identity();
	float angle = hAngle;

	for( uint32_t j = 1; j < i_slice; j++ )
	{
		rot.rotationAxis( &axis, GE_ConvertToRadians( angle ) );
		trans.RotationQuaternion( &rot );
		angle += hAngle;

		vb->data()[ 0 ].transformOut( &vb->data()[ j * 4 ], &trans );
		vb->data()[ 1 ].transformOut( &vb->data()[ j * 4 + 1 ], &trans );
		vb->data()[ 2 ].transformOut( &vb->data()[ j * 4 + 2 ], &trans );
		vb->data()[ 3 ].transformOut( &vb->data()[ j * 4 + 3 ], &trans );
	}
	for( uint32_t i = 0; i < nbVertices; i++ )
		vb->data()[ i ] += i_offset;
	verticesDataEndEdit();

	uint32_t nbIndices = 8 * i_slice + 8 * i_slice;
	std::vector<int> *ib = indicesDataBeginEdit();
	ib->resize( nbIndices );
	uint32_t i = 0;
	for( uint32_t j = 0; j < i_slice; j++ )
	{
		ib->data()[ i++ ] = j * 4;
		ib->data()[ i++ ] = j * 4 + 1;

		ib->data()[ i++ ] = j * 4 + 1;
		ib->data()[ i++ ] = j * 4 + 2;

		ib->data()[ i++ ] = j * 4 + 2;
		ib->data()[ i++ ] = j * 4 + 3;

		ib->data()[ i++ ] = j * 4 + 3;
		ib->data()[ i++ ] = j * 4;
	}

	for( uint32_t j = 0; j < i_slice - 1; j++ )
	{
		ib->data()[ i++ ] = j * 4;
		ib->data()[ i++ ] = j * 4 + 4;

		ib->data()[ i++ ] = j * 4 + 1;
		ib->data()[ i++ ] = j * 4 + 5;

		ib->data()[ i++ ] = j * 4 + 3;
		ib->data()[ i++ ] = j * 4 + 7;

		ib->data()[ i++ ] = j * 4 + 2;
		ib->data()[ i++ ] = j * 4 + 6;
	}

	ib->data()[ i++ ] = ( i_slice - 1 ) * 4;
	ib->data()[ i++ ] = 3;

	ib->data()[ i++ ] = ( i_slice - 1 ) * 4 + 1;
	ib->data()[ i++ ] = 2;

	ib->data()[ i++ ] = ( i_slice - 1 ) * 4 + 3;
	ib->data()[ i++ ] = 0;

	ib->data()[ i++ ] = ( i_slice - 1 ) * 4 + 2;
	ib->data()[ i++ ] = 1;
	indicesDataEndEdit();

	setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
}


void GE_RenderUtilityObject::createCylinderNoCap( float i_radius, float i_height, GE_Vec3 &i_offset, uint32_t i_slice )
{
	uint32_t nbVertices = 4 * i_slice;
	float height = i_height / 2.0f;
	std::vector<GE_Vec3> *vb = verticesDataBeginEdit();
	vb->resize( nbVertices );
	vb->data()[ 0 ] = GE_Vec3( i_radius, 0.0f, height );
	vb->data()[ 1 ] = GE_Vec3( i_radius, 0.0f, -height );
	vb->data()[ 2 ] = GE_Vec3( -i_radius, 0.0f, -height );
	vb->data()[ 3 ] = GE_Vec3( -i_radius, 0.0f, height );

	GE_Vec3		axis( 0, 0, 1 );
	GE_Quat		rot;
	GE_Mat4x4	trans;
	float		hAngle = 180.0f / ( float ) i_slice;
	rot.identity();
	trans.identity();
	float angle = hAngle;

	for( uint32_t j = 1; j < i_slice; j++ )
	{
		rot.rotationAxis( &axis, GE_ConvertToRadians( angle ) );
		trans.RotationQuaternion( &rot );
		angle += hAngle;

		vb->data()[ 0 ].transformOut( &vb->data()[ j * 4 ], &trans );
		vb->data()[ 1 ].transformOut( &vb->data()[ j * 4 + 1 ], &trans );
		vb->data()[ 2 ].transformOut( &vb->data()[ j * 4 + 2 ], &trans );
		vb->data()[ 3 ].transformOut( &vb->data()[ j * 4 + 3 ], &trans );
	}
	for( uint32_t i = 0; i < nbVertices; i++ )
		vb->data()[ i ] += i_offset;
	verticesDataEndEdit();

	uint32_t nbIndices = 4 * i_slice + 4 * i_slice;
	std::vector<int> *ib = indicesDataBeginEdit();
	ib->resize( nbIndices );
	uint32_t i = 0;
	for( uint32_t j = 0; j < i_slice; j++ )
	{
		ib->data()[ i++ ] = j * 4;
		ib->data()[ i++ ] = j * 4 + 1;

		ib->data()[ i++ ] = j * 4 + 2;
		ib->data()[ i++ ] = j * 4 + 3;
	}

	for( uint32_t j = 0; j < i_slice - 1; j++ )
	{
		ib->data()[ i++ ] = j * 4;
		ib->data()[ i++ ] = j * 4 + 4;

		/*ib->data()[i++] = j * 4 + 1;
		ib->data()[i++] = j * 4 + 5;*/

		ib->data()[ i++ ] = j * 4 + 3;
		ib->data()[ i++ ] = j * 4 + 7;

		/*ib->data()[i++] = j * 4 + 2;
		ib->data()[i++] = j * 4 + 6;*/
	}

	ib->data()[ i++ ] = ( i_slice - 1 ) * 4;
	ib->data()[ i++ ] = 3;

	/*ib->data()[i++] = (i_slice - 1) * 4 + 1;
	ib->data()[i++] = 2;*/

	ib->data()[ i++ ] = ( i_slice - 1 ) * 4 + 3;
	ib->data()[ i++ ] = 0;

	/*ib->data()[i++] = (i_slice - 1) * 4 + 2;
	ib->data()[i++] = 1;*/
	indicesDataEndEdit();

	indicesDataEndEdit();

	setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
}


void GE_RenderUtilityObject::createCapsule( float i_radius, float i_halfHeight, GE_Vec3 &i_offset, uint32_t i_capSlice, uint32_t i_slice )
{
	float capHeight = i_radius;
	float height = i_halfHeight - capHeight * 2.0f;
	uint32_t sliceSize = ( i_capSlice + 1 ) * 2;
	uint32_t nbVertices = sliceSize * i_slice;
	std::vector<GE_Vec3> *vb = verticesDataBeginEdit();
	vb->resize( nbVertices );
	float vAngle = 180.0f / ( float ) i_capSlice;
	float angle = 0;
	uint32_t index = 0;
	for( uint32_t i = 0; i <= i_capSlice; i++ )
	{
		vb->data()[ index ].x = sinf( GE_ConvertToRadians( angle ) ) * i_radius + height + capHeight;
		vb->data()[ index ].y = 0.0f;
		vb->data()[ index ].z = cosf( GE_ConvertToRadians( angle ) ) * i_radius;
		angle += vAngle;
		index++;
	}
	angle = 180;
	for( uint32_t i = 0; i <= i_capSlice; i++ )
	{
		vb->data()[ index ].x = sinf( GE_ConvertToRadians( angle ) ) * i_radius - height - capHeight;
		vb->data()[ index ].y = 0.0f;
		vb->data()[ index ].z = cosf( GE_ConvertToRadians( angle ) ) * i_radius;
		angle += vAngle;
		index++;
	}

	GE_Vec3		axis( 1, 0, 0 );
	GE_Quat		rot;
	GE_Mat4x4	trans;
	float		hAngle = 180.0f / ( float ) i_slice;
	rot.identity();
	trans.identity();
	angle = hAngle;
	for( uint32_t j = 1; j < i_slice; j++ )
	{
		rot.rotationAxis( &axis, GE_ConvertToRadians( angle ) );
		trans.RotationQuaternion( &rot );
		angle += hAngle;
		for( uint32_t i = 0; i < sliceSize; i++ )
		{
			vb->data()[ i ].transformOut( &vb->data()[ j * sliceSize + i ], &trans );
		}
	}
	for( uint32_t i = 0; i < nbVertices; i++ )
		vb->data()[ i ] += i_offset;
	verticesDataEndEdit();

	uint32_t nbIndices = sliceSize * 2 * i_slice;
	std::vector<int> *ib = indicesDataBeginEdit();
	ib->resize( nbIndices );
	uint32_t i = 0;
	for( uint32_t j = 0; j < i_slice; j++ )
	{
		for( uint32_t k = 0; k < sliceSize; k++ )
		{
			ib->data()[ i++ ] = j * sliceSize + k;
			if( k == sliceSize - 1 )
				ib->data()[ i++ ] = j * sliceSize;
			else
				ib->data()[ i++ ] = j * sliceSize + k + 1;
		}
	}
	indicesDataEndEdit();

	setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
}

void GE_RenderUtilityObject::createTriangleMesh( GE_Vec3 *i_vertices, uint32_t i_nbVertices, uint32_t *i_indices, uint32_t i_nbIndices )
{
	std::vector<GE_Vec3> *vb = verticesDataBeginEdit();
	vb->resize( i_nbVertices );
	memcpy( ( void* ) vb->data(), ( void* ) i_vertices, sizeof( GE_Vec3 ) * i_nbVertices );
	verticesDataEndEdit();

	std::vector<int> *ib = indicesDataBeginEdit();
	ib->resize( i_nbIndices );
	memcpy( ( void* ) ib->data(), ( void* ) i_indices, sizeof( uint32_t ) * i_nbIndices );
	indicesDataEndEdit();

	setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );
}

void GE_RenderUtilityObject::createPlane( GE_Vec3 &i_offsetPos, GE_Quat &i_offsetRot, GE_Vec3 &i_offsetScale )
{
	GE_Mat4x4 trans;
	trans.srp(i_offsetPos, i_offsetRot, i_offsetScale);
	vector< GE_Vec3 > *vt = verticesDataBeginEdit( );
	vt->resize( 4 );
	vt->data( )[ 0 ] = GE_Vec3( -0.5f, 0.5f, 0.0f ).transform(&trans);
	vt->data( )[ 1 ] = GE_Vec3( -0.5f, -0.5f, 0.0f ).transform( &trans );
	vt->data( )[ 2 ] = GE_Vec3( 0.5f, 0.5f, 0.0f ).transform( &trans );
	vt->data( )[ 3 ] = GE_Vec3( 0.5f, -0.5f, 0.0f ).transform( &trans );
	verticesDataEndEdit();

	vector< GE_Vec2 > *vtc = texcoordDataBeginEdit();
	vtc->resize( 4 );
	vtc->data()[ 0 ] = GE_Vec2( 0.0f, 0.0f );
	vtc->data()[ 1 ] = GE_Vec2( 0.0f, 1.0f );
	vtc->data()[ 2 ] = GE_Vec2( 1.0f, 0.0f );
	vtc->data()[ 3 ] = GE_Vec2( 1.0f, 1.0f );
	texcoordDataEndEdit();
	setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
}

//GE_RenderUtility----------------------------------------------------------------------------------------------------------------------------------------------
GE_RenderUtility GE_RenderUtility::m_singleton;

void GE_RenderUtility::init()
{
	if( m_vS )
		return;

	m_vS = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_RU_VS.cso" );
	m_pS = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_RU_PS.cso" );
	m_pS_Depth = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_RU_PS_Depth.cso" );
	m_pS_Color = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_RU_PS_COLOR.cso" );
	m_pS_Texture = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_RU_PS_TEXTURE.cso" );
	m_pS_Color_Texture = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_RU_PS_COLOR_TEXTURE.cso" );
	m_pS_Color_Depth = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_RU_PS_COLOR_DEPTH.cso" );
	m_pS_Texture_Depth = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_RU_PS_TEXTURE_DEPTH.cso" );
	m_pS_Color_Texture_Depth = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_RU_PS_COLOR_TEXTURE_DEPTH.cso" );

	m_perObject = GE_Core::getSingleton().createConstBuffer( sizeof( PerObject ) );

}

void GE_RenderUtility::render( list<GE_RenderUtilityObject *> *i_objects, GE_View *i_view, ID3D11ShaderResourceView *i_depth )
{
	ID3D11ShaderResourceView *srv[ 4 ];

	GE_Core::getSingleton().setScreenAlignQuadVertexDeclarationToContext();

	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 1, &GE_Core::getSingleton().getRenderStates().SamplerLinearWrap );

	BOOST_FOREACH( GE_RenderUtilityObject *object, *i_objects )
	{
		if( object->m_depthDisable )
			GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );

		GE_Mat4x4Multiply( &m_temp.Matrix, object->getWorld(), &i_view->m_matViewProjection );
		if( object->m_billboard )
			m_temp.InvView = i_view->m_matViewInv;
		else
			m_temp.InvView.identity();

		m_temp.InvProjection.transposeBy( &i_view->m_matProjectionInv );

		m_temp.Color.x = object->m_color.x;
		m_temp.Color.y = object->m_color.y;
		m_temp.Color.z = object->m_color.z;
		m_temp.Color.w = object->m_alphaTest;

		GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perObject, 0, NULL, &m_temp, 0, 0 );

		GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers( 0, 1, &m_perObject );
		GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perObject );

		srv[ 0 ] = object->m_colorBuffer.m_sRV;
		srv[ 1 ] = object->m_texcoordBuffer.m_sRV;
		srv[ 2 ] = object->getSRV();
		srv[ 3 ] = i_depth;

		bool Color, Texture, Depth;

		Color = ( srv[ 0 ] != NULL );
		Texture = ( srv[ 1 ] != NULL && srv[ 2 ] != NULL );
		Depth = ( srv[ 3 ] != NULL && !object->m_depthDisable );

		GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 4, srv );
		GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources( 0, 4, srv );
		GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );

		if( !Color && !Texture && !Depth )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS, NULL, 0 );
		else if( Color && !Texture && !Depth )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Color, NULL, 0 );
		else if( !Color && Texture && !Depth )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Texture, NULL, 0 );
		else if( !Color && !Texture && Depth )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Depth, NULL, 0 );
		else if( Color && Texture && !Depth )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Color_Texture, NULL, 0 );
		else if( Color && !Texture && Depth )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Color_Depth, NULL, 0 );
		else if( !Color && Texture && Depth )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Texture_Depth, NULL, 0 );
		else if( Color && Texture && Depth )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Color_Texture_Depth, NULL, 0 );

		object->render();

		if( object->m_depthDisable )
			GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( 0, 0 );
	}
	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	srv[ 2 ] = NULL;
	srv[ 3 ] = NULL;

	GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources( 0, 4, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 4, srv );
	GE_Core::getSingleton().getImmediateContext()->RSSetState( GE_Core::getSingleton().getRenderStates().RasterStateDefault );
}

GE_RenderUtility::~GE_RenderUtility()
{
	SAFE_RELEASE( m_vS );
	SAFE_RELEASE( m_pS );
	SAFE_RELEASE( m_pS_Depth );
	SAFE_RELEASE( m_pS_Color );
	SAFE_RELEASE( m_pS_Texture );
	SAFE_RELEASE( m_pS_Color_Texture );
	SAFE_RELEASE( m_pS_Color_Depth );
	SAFE_RELEASE( m_pS_Texture_Depth );
	SAFE_RELEASE( m_pS_Color_Texture_Depth );
	SAFE_RELEASE( m_perObject );
	SAFE_RELEASE( m_pS_Texture );
}
