#include "GE_Terrain.h"
#include "GE_View.h"
#include "GE_Texture.h"
#include "GE_RenderTarget.h"
#include "GE_BufferResource.h"
#include "GE_Core.h"
#include "GE_Utility.h"
#include "GE_StreamMap.h"
#include "GE_DeferredGeometry.h"
#include "GE_Material.h"
#include <DirectXTex.h>

#define	MASK_DIFFUSE_ADDRESS (6)
#define	MASK_NORMAL_ADDRESS  (MASK_DIFFUSE_ADDRESS + GE_MAX_TERRAIN_PAINT_LAYER)
#define	MASK_GSR_ADDRESS  (MASK_NORMAL_ADDRESS + GE_MAX_TERRAIN_PAINT_LAYER)

#define	PROCEDURAL_DIFFUSE_ADDRESS (MASK_GSR_ADDRESS + GE_MAX_TERRAIN_PAINT_LAYER)
#define	PROCEDURAL_NORMAL_ADDRESS  (PROCEDURAL_DIFFUSE_ADDRESS + GE_MAX_TERRAIN_PROCEDURAL_LAYER)
#define	PROCEDURAL_GSR_ADDRESS  (PROCEDURAL_NORMAL_ADDRESS + GE_MAX_TERRAIN_PROCEDURAL_LAYER)
#define	PROCEDURAL_MASK_ADDRESS  (PROCEDURAL_GSR_ADDRESS + GE_MAX_TERRAIN_PROCEDURAL_LAYER)

#define MASK_BASE_INDEX MASK_DIFFUSE_ADDRESS
#define PROC_BASE_INDEX PROCEDURAL_DIFFUSE_ADDRESS
#define MAX_PS_SRV (6 + ( GE_MAX_TERRAIN_PAINT_LAYER * 3 + GE_MAX_TERRAIN_PROCEDURAL_LAYER * 4 )  )

GE_Terrain::GE_Terrain()
{
	m_instances = NULL;
	m_vS = NULL;
	m_vS_StreamOut = NULL;
	m_gS_StreamOut = NULL;
	for ( uint32_t m = 0; m <= GE_MAX_TERRAIN_PAINT_LAYER; ++m )
	{
		for ( uint32_t p = 0; p <= GE_MAX_TERRAIN_PROCEDURAL_LAYER; ++p )
			m_pS[ m ][ p ] = NULL;
	}
	m_perFrame = NULL;
	m_perProcedural = NULL;
	m_perMask = NULL;
	m_vertexDeclaration = NULL;
	m_tree = NULL;
	m_lodMultiplier = 4.0f;
	m_cPerFrame.MaxHeight = GE_ConvertToUnit( 600.0f );
	m_cPerFrame.NormalAspect = 100.0f;
	m_cPerFrame.HeightBias = 0.0f;
	m_heightMap = NULL;
	m_maskMap = NULL;
}

GE_Terrain::~GE_Terrain()
{
	SAFE_DELETE_ARRAY( m_instances );
	SAFE_RELEASE( m_vertexDeclaration );
	SAFE_RELEASE( m_vS );
	SAFE_RELEASE( m_vS_StreamOut );
	SAFE_RELEASE( m_gS_StreamOut );
	for ( uint32_t m = 0; m <= GE_MAX_TERRAIN_PAINT_LAYER; ++m )
	{
		for ( uint32_t p = 0; p <= GE_MAX_TERRAIN_PROCEDURAL_LAYER; ++p )
			SAFE_RELEASE( m_pS[ m ][ p ] );
	}
	SAFE_RELEASE( m_perFrame );
	SAFE_RELEASE( m_perProcedural );
	SAFE_RELEASE( m_perMask );
	SAFE_DELETE( m_tree );
}

void GE_Terrain::render( const GE_View *i_view )
{
	for ( uint32_t i = 0; i < ( uint32_t )m_levels.size(); ++i )
	{
		m_instances[ i ].NumberOfRenderInstances[ Level::CENTER ] = 0;
		m_instances[ i ].NumberOfRenderInstances[ Level::DOWN ] = 0;
		m_instances[ i ].NumberOfRenderInstances[ Level::DOWN_LEFT ] = 0;
		m_instances[ i ].NumberOfRenderInstances[ Level::DOWN_LEFT_RIGHT ] = 0;
		m_instances[ i ].BaseNodes.clear();
	}

	m_numOut = m_numVisit = 0;
	check( m_tree, i_view );

	ID3D11ShaderResourceView *srv[ MAX_PS_SRV ];
	for ( uint32_t i = 0; i < ( MAX_PS_SRV ); ++i )
		srv[ i ] = NULL;

	srv[ 0 ] = m_heightMap;
	srv[ 1 ] = GE_DeferredGeometry::getSingleton().getBFNSRV();
	srv[ 2 ] = m_maskMap;
	uint32_t reflectMips = 0;
	if ( m_maps.NormalNoiseMap )
	{
		srv[ 3 ] = m_maps.NormalNoiseMap->getTextureSRV();
		reflectMips = m_maps.NormalNoiseMap->getNumberOfMips();
		m_cPerFrame.NormalNoiseEnable.x = 1.0f;
	}
	else
	{
		m_cPerFrame.NormalNoiseEnable.x = 0.0f;
		srv[ 3 ] = NULL;
	}

	if ( m_maps.ReflectMap )
	{
		srv[ 4 ] = m_maps.ReflectMap->getTextureSRV();
		reflectMips = m_maps.ReflectMap->getNumberOfMips();
	}
	else
		srv[ 4 ] = NULL;

	GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources( 1, 1, srv );

	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 1, 5, srv );

	GE_View *view = ( GE_View * )i_view;
	view->m_matViewProjection.transposeOut( &m_cPerFrame.ViewProjection );
	m_cPerFrame.ViewPosition.x = view->m_position.x;
	m_cPerFrame.ViewPosition.y = view->m_position.y;
	m_cPerFrame.ViewPosition.z = view->m_position.z;
	m_cPerFrame.FieldSize = m_tree->HalfSize * 2.0f;
	m_cPerFrame.Bumpiness = m_materialParameters.Bumpiness;
	m_cPerFrame.Reflectmask = m_materialParameters.Reflectmask;
	m_cPerFrame.Roughness = m_materialParameters.Roughness * reflectMips;
	m_cPerFrame.Glowness = m_materialParameters.getGlowIntensity();
	m_cPerFrame.MaterialID = ( float )GE_DeferredGeometry::getSingleton().getNumberOfMaterials() / 255.0f;
	GE_DeferredGeometry::getSingleton().addMaterial( ( GE_Material * )&m_materialParameters );
	GE_Core::getSingleton().getImmediateContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	GE_Core::getSingleton().getImmediateContext()->IASetInputLayout( m_vertexDeclaration );
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );

	uint32_t maskCount = 0;
	for ( uint32_t i = 0; i < GE_MAX_TERRAIN_PAINT_LAYER; ++i )
	{
		if ( m_materialParameters.Painting[ i ].Enable )
		{
			memcpy_s( &m_cPerMask[ maskCount ], sizeof( PerMask ), &m_materialParameters.Painting[ i ], sizeof( PerMask ) );

			PaintMaps *par = &m_maps.Painting[ i ];
			if ( par->DiffuseMap )
				srv[ maskCount ] = par->DiffuseMap->getTextureSRV();
			else
				srv[ maskCount ] = NULL;

			if ( par->NormalMap )
				srv[ maskCount + GE_MAX_TERRAIN_PAINT_LAYER ] = par->NormalMap->getTextureSRV();
			else
				srv[ maskCount + GE_MAX_TERRAIN_PAINT_LAYER ] = NULL;

			if ( par->GSRMap )
				srv[ maskCount + GE_MAX_TERRAIN_PAINT_LAYER * 2 ] = par->GSRMap->getTextureSRV();
			else
				srv[ maskCount + GE_MAX_TERRAIN_PAINT_LAYER * 2 ] = NULL;

			maskCount++;

		}
	}

	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( MASK_BASE_INDEX, GE_MAX_TERRAIN_PAINT_LAYER * 3, srv );


	uint32_t procCount = 0;
	for ( uint32_t i = 0; i < GE_MAX_TERRAIN_PROCEDURAL_LAYER; ++i )
	{
		if ( m_materialParameters.Procedural[ i ].Enable )
		{
			GE_Terrain::ProceduralParameters *perV = &m_materialParameters.Procedural[ i ];
			GE_Terrain::PerProcedural *perP = &m_cPerProcedural[ procCount ];
			*perP = *( GE_Terrain::PerProcedural * )perV;
			perP->AngleFalloff *= perV->AngleField;
			perP->HeightFalloff *= perV->HeightField;
			perP->MaskFalloff *= perV->MaskField;

			perP->MaskPosition.x = perV->MaskPosition.x + m_tree->HalfSize;
			perP->MaskPosition.y = m_cPerFrame.FieldSize - ( perV->MaskPosition.y + m_tree->HalfSize );
			perP->MaskUVTile = perP->MaskUVTile * m_cPerFrame.FieldSize / perP->MaskField * 0.5f;
			float invSize = 1.0f / m_cPerFrame.FieldSize;
			perP->MaskUVPosition.x = perP->MaskPosition.x * invSize * perP->MaskUVTile.x + perV->MaskUVPosition.x - 0.5f;
			perP->MaskUVPosition.y = perP->MaskPosition.y * invSize * perP->MaskUVTile.y + perV->MaskUVPosition.y - 0.5f;

			ProceduralMaps *par = &m_maps.Procedural[ i ];
			if ( par->DiffuseMap )
				srv[ procCount ] = par->DiffuseMap->getTextureSRV();
			else
				srv[ procCount ] = NULL;

			if ( par->NormalMap )
				srv[ procCount + GE_MAX_TERRAIN_PROCEDURAL_LAYER ] = par->NormalMap->getTextureSRV();
			else
				srv[ procCount + GE_MAX_TERRAIN_PROCEDURAL_LAYER ] = NULL;

			if ( par->GSRMap )
				srv[ procCount + GE_MAX_TERRAIN_PROCEDURAL_LAYER * 2 ] = par->GSRMap->getTextureSRV();
			else
				srv[ procCount + GE_MAX_TERRAIN_PROCEDURAL_LAYER * 2 ] = NULL;

			if ( par->MaskMap )
				srv[ procCount + GE_MAX_TERRAIN_PROCEDURAL_LAYER * 3 ] = par->MaskMap->getTextureSRV();
			else
				srv[ procCount + GE_MAX_TERRAIN_PROCEDURAL_LAYER * 3 ] = NULL;

			procCount++;
		}
	}

	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( PROC_BASE_INDEX, GE_MAX_TERRAIN_PROCEDURAL_LAYER * 4, srv );


	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perMask, 0, NULL, m_cPerMask, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perProcedural, 0, NULL, m_cPerProcedural, 0, 0 );

	GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers( 0, 1, &m_perFrame );
	ID3D11Buffer *cBuffer[ 3 ];
	cBuffer[ 0 ] = m_perFrame;
	cBuffer[ 1 ] = m_perMask;
	cBuffer[ 2 ] = m_perProcedural;
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 3, cBuffer );

	ID3D11SamplerState *ss[ 2 ];
	ss[ 0 ] = GE_Core::getSingleton().getRenderStates().SamplerLinearWrap;
	ss[ 1 ] = GE_Core::getSingleton().getRenderStates().SamplerPointClamp;

	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 2, ss );
	GE_Core::getSingleton().getImmediateContext()->VSSetSamplers( 0, 2, ss );

	GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS[ maskCount ][ procCount ], NULL, 0 );

	for ( int32_t i = 0; i < ( int32_t )m_levels.size(); ++i )
	{
		if ( !m_instances[ i ].BaseNodes.size() )
			continue;
		for ( uint32_t ib = 0; ib < ( uint32_t )m_instances[ i ].BaseNodes.size(); ++ib )
		{
			Quad *baseNode = m_instances[ i ].BaseNodes[ ib ];
			GE_Vec4 posAngle;
			posAngle.x = baseNode->Position.x;
			posAngle.y = baseNode->Position.y;
			posAngle.z = 0.0f;
			posAngle.w = 0.0f;
			if ( i == m_levels.size() - 1 )
			{
				uint32_t renderInstance = m_instances[ i ].NumberOfRenderInstances[ Level::CENTER ]++;
				m_instances[ i ].InstancesData[ Level::CENTER ].data()[ renderInstance ] = posAngle;
				continue;
			}
			bool isUpLow = false;
			bool isDownLow = false;
			bool isRightLow = false;
			bool isLeftLow = false;
			for ( uint32_t is = 0; is < m_instances[ i + 1 ].BaseNodes.size(); ++is )
			{
				Quad *searchNode = m_instances[ i + 1 ].BaseNodes[ is ];
				if ( searchNode->X == baseNode->X - 1 && searchNode->Y == baseNode->Y )
					isLeftLow = true;
				if ( searchNode->X == baseNode->X + 1 && searchNode->Y == baseNode->Y )
					isRightLow = true;
				if ( searchNode->X == baseNode->X && searchNode->Y == baseNode->Y - 1 )
					isUpLow = true;
				if ( searchNode->X == baseNode->X && searchNode->Y == baseNode->Y + 1 )
					isDownLow = true;
			}

			if ( isDownLow && isLeftLow && isRightLow )
			{
				uint32_t renderInstance = m_instances[ i ].NumberOfRenderInstances[ Level::DOWN_LEFT_RIGHT ]++;
				posAngle.w = GE_ConvertToRadians( 0.0f );
				m_instances[ i ].InstancesData[ Level::DOWN_LEFT_RIGHT ].data()[ renderInstance ] = posAngle;
			}
			else if ( isUpLow && isLeftLow && isRightLow )
			{
				uint32_t renderInstance = m_instances[ i ].NumberOfRenderInstances[ Level::DOWN_LEFT_RIGHT ]++;
				posAngle.w = GE_ConvertToRadians( 180.0f );
				m_instances[ i ].InstancesData[ Level::DOWN_LEFT_RIGHT ].data()[ renderInstance ] = posAngle;
			}
			else if ( isUpLow && isDownLow && isRightLow )
			{
				uint32_t renderInstance = m_instances[ i ].NumberOfRenderInstances[ Level::DOWN_LEFT_RIGHT ]++;
				posAngle.w = GE_ConvertToRadians( 90.0f );
				m_instances[ i ].InstancesData[ Level::DOWN_LEFT_RIGHT ].data()[ renderInstance ] = posAngle;
			}
			else if ( isUpLow && isDownLow && isLeftLow )
			{
				uint32_t renderInstance = m_instances[ i ].NumberOfRenderInstances[ Level::DOWN_LEFT_RIGHT ]++;
				posAngle.w = GE_ConvertToRadians( 270.0f );
				m_instances[ i ].InstancesData[ Level::DOWN_LEFT_RIGHT ].data()[ renderInstance ] = posAngle;
			}
			else if ( isDownLow && isLeftLow )
			{
				uint32_t renderInstance = m_instances[ i ].NumberOfRenderInstances[ Level::DOWN_LEFT ]++;
				posAngle.w = GE_ConvertToRadians( 0.0f );
				m_instances[ i ].InstancesData[ Level::DOWN_LEFT ].data()[ renderInstance ] = posAngle;
			}
			else if ( isDownLow && isRightLow )
			{
				uint32_t renderInstance = m_instances[ i ].NumberOfRenderInstances[ Level::DOWN_LEFT ]++;
				posAngle.w = GE_ConvertToRadians( 90.0f );
				m_instances[ i ].InstancesData[ Level::DOWN_LEFT ].data()[ renderInstance ] = posAngle;
			}
			else if ( isUpLow && isLeftLow )
			{
				uint32_t renderInstance = m_instances[ i ].NumberOfRenderInstances[ Level::DOWN_LEFT ]++;
				posAngle.w = GE_ConvertToRadians( 270.0f );
				m_instances[ i ].InstancesData[ Level::DOWN_LEFT ].data()[ renderInstance ] = posAngle;
			}
			else if ( isUpLow && isRightLow )
			{
				uint32_t renderInstance = m_instances[ i ].NumberOfRenderInstances[ Level::DOWN_LEFT ]++;
				posAngle.w = GE_ConvertToRadians( 180.0f );
				m_instances[ i ].InstancesData[ Level::DOWN_LEFT ].data()[ renderInstance ] = posAngle;
			}
			else if ( isDownLow )
			{
				uint32_t renderInstance = m_instances[ i ].NumberOfRenderInstances[ Level::DOWN ]++;
				posAngle.w = GE_ConvertToRadians( 0.0f );
				m_instances[ i ].InstancesData[ Level::DOWN ].data()[ renderInstance ] = posAngle;
			}
			else if ( isRightLow )
			{
				uint32_t renderInstance = m_instances[ i ].NumberOfRenderInstances[ Level::DOWN ]++;
				posAngle.w = GE_ConvertToRadians( 90.0f );
				m_instances[ i ].InstancesData[ Level::DOWN ].data()[ renderInstance ] = posAngle;
			}
			else if ( isUpLow )
			{
				uint32_t renderInstance = m_instances[ i ].NumberOfRenderInstances[ Level::DOWN ]++;
				posAngle.w = GE_ConvertToRadians( 180.0f );
				m_instances[ i ].InstancesData[ Level::DOWN ].data()[ renderInstance ] = posAngle;
			}
			else if ( isLeftLow )
			{
				uint32_t renderInstance = m_instances[ i ].NumberOfRenderInstances[ Level::DOWN ]++;
				posAngle.w = GE_ConvertToRadians( 270.0f );
				m_instances[ i ].InstancesData[ Level::DOWN ].data()[ renderInstance ] = posAngle;
			}
			else
			{
				uint32_t renderInstance = m_instances[ i ].NumberOfRenderInstances[ Level::CENTER ]++;
				posAngle.w = GE_ConvertToRadians( 0.0f );
				m_instances[ i ].InstancesData[ Level::CENTER ].data()[ renderInstance ] = posAngle;
			}
		}

		uint32_t offset = 0;
		uint32_t sov = sizeof( VertexBuffer );
		GE_Core::getSingleton().getImmediateContext()->IASetVertexBuffers( 0, 1, &m_levels[ i ].VB, &sov, &offset );

		for ( uint32_t renderIndex = 0; renderIndex < Level::MAX; ++renderIndex )
		{
			uint32_t nori = m_instances[ i ].NumberOfRenderInstances[ renderIndex ];
			if ( !nori )
				continue;
			m_instances[ i ].BufferResource[ renderIndex ]->fill( nori );

			srv[ 0 ] = m_instances[ i ].BufferResource[ renderIndex ]->m_sRV;
			GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources( 0, 1, srv );
			uint32_t ic = ( uint32_t )m_levels[ i ].Indices[ renderIndex ].size() * 3;
			GE_Core::getSingleton().renderTriangleList( m_levels[ i ].IB[ renderIndex ], ic,
				m_instances[ i ].NumberOfRenderInstances[ renderIndex ] );

		}
	}

	for ( uint32_t i = 0; i < ( MAX_PS_SRV ); ++i )
		srv[ i ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, ( MAX_PS_SRV ), srv );
	GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources( 0, 2, srv );
}

void GE_Terrain::init()
{
	if ( m_vS )
		return;

	const D3D11_INPUT_ELEMENT_DESC _vdl[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	_GE_FileChunk fc;
	fc.load( string( GE_SHADER_PATH ) + "GE_Terrain_VS.cso" );
	GE_Core::getSingleton().getDevice()->CreateInputLayout( _vdl, 1, fc.Data, fc.Len, &m_vertexDeclaration );
	GE_Core::getSingleton().getDevice()->CreateVertexShader( fc.Data, fc.Len, NULL, &m_vS );
	for ( uint32_t m = 0; m <= GE_MAX_TERRAIN_PAINT_LAYER; ++m )
	{
		for ( uint32_t p = 0; p <= GE_MAX_TERRAIN_PROCEDURAL_LAYER; ++p )
		{
			if ( m == 0 && p == 0 )
				continue;
			std::ostringstream buff;
			buff << string( GE_SHADER_PATH ) << "GE_Terrain_PS_Mask" << m << "_Proc" << p << ".cso";
			m_pS[ m ][ p ] = GE_Core::getSingleton().createPS( buff.str() );
		}
	}
	m_vS_StreamOut = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_Terrain_VS_StreamOut.cso" );

	const D3D11_SO_DECLARATION_ENTRY _vdlSO[] =
	{
		{ 0, "POSITION", 0, 0, 3, 0 },
	};
	m_perFrame = GE_Core::getSingleton().createConstBuffer( sizeof( PerFrame ) );
	m_perProcedural = GE_Core::getSingleton().createConstBuffer( sizeof( PerProcedural )* GE_MAX_TERRAIN_PROCEDURAL_LAYER );
	m_perMask = GE_Core::getSingleton().createConstBuffer( sizeof( PerMask )* GE_MAX_TERRAIN_PAINT_LAYER );
	fc.load( string( GE_SHADER_PATH ) + "GE_Terrain_GS_StreamOut.cso" );
	GE_Core::getSingleton().getDevice()->CreateGeometryShaderWithStreamOutput( fc.Data, fc.Len, _vdlSO, 1, NULL, 0, 0, NULL, &m_gS_StreamOut );
}

void GE_Terrain::create( float i_sizeWidth, uint32_t i_chunkQuadDensityWidth, uint32_t i_numberOfChunksWidth )
{
	m_width = i_sizeWidth;
	m_quadDensity = i_chunkQuadDensityWidth;
	m_cPerFrame.TexcoordViewProjection.lookAtRH( &GE_Vec3( 0.0f, 0.0f, 1000.0f ), &GE_Vec3( 0.0f, 0.0f, 0.0f ), &GE_Vec3( 0.0f, 1.0f, 0.0f ) );
	m_cPerFrame.TexcoordViewProjection *= GE_Mat4x4().orthoRH( i_sizeWidth, i_sizeWidth, 1.0f, 1000.0f );

	float chunkSize = ( float )i_sizeWidth / ( float )i_numberOfChunksWidth;

	m_levels.clear();
	m_levels.resize( ( uint32_t )( log( ( uint32_t )i_chunkQuadDensityWidth ) / log( 2 ) ) );
	uint32_t quadDensity = i_chunkQuadDensityWidth;
	for ( uint32_t i = 0; i < ( uint32_t )m_levels.size(); ++i )
	{
		m_levels[ i ].create( chunkSize, quadDensity );
		quadDensity /= 2;
	}

	GE_Vec2 startPos;
	startPos.x = startPos.y = ( float )i_sizeWidth * -0.5f;
	uint32_t i = 0;
	for ( uint32_t yy = 0; yy < i_numberOfChunksWidth; ++yy )
	{
		for ( uint32_t xx = 0; xx < i_numberOfChunksWidth; ++xx )
		{
			startPos.x += chunkSize;
			++i;
		}
		startPos.x = ( float )i_sizeWidth * -0.5f;
		startPos.y += chunkSize;
	}

	SAFE_DELETE( m_tree );
	m_tree = new Quad();
	m_tree->calculate( i_sizeWidth * 0.5f, chunkSize * 0.5f, m_tree );

	SAFE_DELETE_ARRAY( m_instances );
	m_instances = new Buffer[ ( uint32_t )m_levels.size() ];

	for ( uint32_t i = 0; i < ( uint32_t )m_levels.size(); ++i )
		m_instances[ i ].createBuffer( i_numberOfChunksWidth * 30 * ( i + 1 ) );
}

void GE_Terrain::check( Quad *i_quad, const GE_View *i_view )
{
	GE_View *view = ( GE_View * )i_view;
	++m_numVisit;

	if ( !i_quad )
		return;
	float HalfMaxHeight = m_cPerFrame.MaxHeight * 0.5f;
	GE_Vec3 size = GE_Vec3( i_quad->HalfSize, i_quad->HalfSize, HalfMaxHeight );
	GE_Vec3 pos = i_quad->Position;
	pos.z = pos.z + HalfMaxHeight + m_cPerFrame.HeightBias;
	if ( view->m_frustum.cubeInFrustum( &pos, &size ) == GE_Frustum::OutSide )
		return;
	if ( i_quad->IsChunk )
	{
		bool isAdd = false;
		for ( uint32_t i = 0; i < ( uint32_t )m_levels.size(); ++i )
		{
			if ( ( GE_Vec2( i_quad->Position.x, i_quad->Position.y ) - GE_Vec2( i_view->m_position.x, i_view->m_position.y ) ).length()
				<= m_lodMultiplier * i_quad->HalfSize * ( i + 1 ) )
			{
				m_instances[ i ].BaseNodes.push_back( i_quad );
				isAdd = true;
				break;
			}
		}
		if ( !isAdd )
			m_instances[ ( uint32_t )m_levels.size() - 1 ].BaseNodes.push_back( i_quad );
		++m_numOut;
	}

	check( i_quad->UpLeft, i_view );
	check( i_quad->UpRight, i_view );
	check( i_quad->DownLeft, i_view );
	check( i_quad->DownRight, i_view );
}

ID3D11ShaderResourceView * GE_Terrain::getHeightMap()
{
	return m_heightMap;
}

void GE_Terrain::setHeightMap( ID3D11ShaderResourceView * i_heightMap )
{
	m_heightMap = i_heightMap;
}

ID3D11ShaderResourceView * GE_Terrain::getMaskMap()
{
	return m_maskMap;
}

void GE_Terrain::setMaskMap( ID3D11ShaderResourceView * i_maskMap )
{
	m_maskMap = i_maskMap;
}

void GE_Terrain::setNormalAspect( float i_normalAspect )
{
	m_cPerFrame.NormalAspect = i_normalAspect;
}

float GE_Terrain::getNormalAspect()
{
	return m_cPerFrame.NormalAspect;
}

float GE_Terrain::getHeightBias()
{
	return m_cPerFrame.HeightBias;
}

void GE_Terrain::setMaxHeight( float i_maxHeight )
{
	m_cPerFrame.MaxHeight = i_maxHeight;
}

void GE_Terrain::setLodMultiplier( float i_lodMultiplier )
{
	m_lodMultiplier = i_lodMultiplier;
}

float GE_Terrain::getLodMultiplier()
{
	return m_lodMultiplier;
}

float GE_Terrain::getMaxHeight()
{
	return m_cPerFrame.MaxHeight;
}

void GE_Terrain::setHeightBias( float i_heightBias )
{
	m_cPerFrame.HeightBias = i_heightBias;
}

void GE_Terrain::getVerticesData( vector<GE_Vec3> *i_vertices, vector<uint32_t> *i_indices, GE_Vec2 &i_heightMinMax, uint32_t i_quadDensity )
{
	if ( i_quadDensity % 2 )
		i_quadDensity++;

	uint32_t numVertsWidth = i_quadDensity + 1;
	i_vertices->resize( numVertsWidth * numVertsWidth );
	ID3D11Buffer *VBStream, *Stage, *StreamOut;

	D3D11_BUFFER_DESC vbdesc =
	{
		( uint32_t )i_vertices->size() * sizeof( GE_Vec3 ),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_STREAM_OUTPUT,
		0,
		0
	};
	GE_Core::getSingleton().getDevice()->CreateBuffer( &vbdesc, NULL, &StreamOut );

	vbdesc.Usage = D3D11_USAGE_STAGING;
	vbdesc.BindFlags = 0;
	vbdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	GE_Core::getSingleton().getDevice()->CreateBuffer( &vbdesc, NULL, &Stage );

	float microSize = m_width / i_quadDensity;
	float halfSize = m_width * ( -0.5f );
	GE_Vec2 uv;
	uv.x = uv.y = 0.0f;
	uint32_t i = 0;
	for ( uint32_t yy = 0; yy < numVertsWidth; ++yy )
	{
		for ( uint32_t xx = 0; xx < numVertsWidth; ++xx )
		{
			i_vertices->data()[ i ] = GE_Vec3( microSize * xx + halfSize, microSize * yy + halfSize, 0.0f );
			++i;
		}
	}
	if ( i_indices )
	{
		i_indices->resize( i_quadDensity * i_quadDensity * 6 );
		uint32_t ic = 0;
		for ( uint32_t yy = 0; yy < i_quadDensity; ++yy )
		{
			for ( uint32_t xx = 0; xx < i_quadDensity; ++xx )
			{
				i_indices->data()[ ic + 0 ] = numVertsWidth * ( yy + 1 ) + ( xx );
				i_indices->data()[ ic + 1 ] = numVertsWidth * ( yy )+( xx );
				i_indices->data()[ ic + 2 ] = numVertsWidth * ( yy )+( xx + 1 );

				i_indices->data()[ ic + 3 ] = numVertsWidth * ( yy )+( xx + 1 );
				i_indices->data()[ ic + 4 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
				i_indices->data()[ ic + 5 ] = numVertsWidth * ( yy + 1 ) + ( xx );

				ic += 6;
			}
		}
	}

	VBStream = GE_Core::getSingleton().createVertexBuffer( i_vertices->data(), sizeof( VertexBuffer ), ( uint32_t )i_vertices->size() );

	ID3D11ShaderResourceView *srv[ 1 ];
	srv[ 0 ] = m_heightMap;

	m_cPerFrame.TexcoordViewProjection.transposeOut( &m_cPerFrame.ViewProjection );

	GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources( 1, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	GE_Core::getSingleton().getImmediateContext()->IASetInputLayout( m_vertexDeclaration );
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );

	GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers( 0, 1, &m_perFrame );

	ID3D11SamplerState *ss[ 2 ];
	ss[ 0 ] = GE_Core::getSingleton().getRenderStates().SamplerLinearWrap;
	ss[ 1 ] = GE_Core::getSingleton().getRenderStates().SamplerPointClamp;

	GE_Core::getSingleton().getImmediateContext()->VSSetSamplers( 0, 2, ss );

	GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_StreamOut, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->GSSetShader( m_gS_StreamOut, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( NULL, NULL, 0 );

	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );

	uint32_t offset = 0;
	GE_Core::getSingleton().getImmediateContext()->SOSetTargets( 1, &StreamOut, &offset );

	offset = 0;
	uint32_t sov = sizeof( GE_Vec3 );
	uint32_t vc = ( uint32_t )i_vertices->size();
	GE_Core::getSingleton().renderTriangle( VBStream, sov, vc );

	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources( 1, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );
	offset = 0;
	ID3D11Buffer *b = NULL;
	GE_Core::getSingleton().getImmediateContext()->SOSetTargets( 1, &b, &offset );

	GE_Core::getSingleton().getImmediateContext()->CopyResource( Stage, StreamOut );

	GE_Core::getSingleton().getImmediateContext()->VSSetShader( NULL, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );
	D3D11_MAPPED_SUBRESOURCE resource;
	i_heightMinMax.x = GE_FLOAT32_MAX;
	i_heightMinMax.y = GE_FLOAT32_MIN;
	if ( SUCCEEDED( GE_Core::getSingleton().getImmediateContext()->Map( Stage, 0, D3D11_MAP_READ, 0, &resource ) ) )
	{
		GE_Vec3 *ptr = ( GE_Vec3 * )resource.pData;
		for ( uint32_t i = 0; i < ( uint32_t )i_vertices->size(); ++i )
		{
			GE_Vec3 val = ptr[ i ];
			i_heightMinMax.x = GE_MIN( i_heightMinMax.x, val.z );
			i_heightMinMax.y = GE_MAX( i_heightMinMax.y, val.z );
			i_vertices->data()[ i ] = val;
		}
	}
	SAFE_RELEASE( VBStream );
	SAFE_RELEASE( StreamOut );
	SAFE_RELEASE( Stage );
}

GE_Terrain GE_Terrain::m_singleton;

GE_Terrain::Level::~Level()
{
	SAFE_RELEASE( VB );
	for ( uint32_t i = 0; i < Level::MAX; ++i )
		SAFE_RELEASE( IB[ i ] );
}

GE_Terrain::Level::Level()
{
	VB = NULL;
	for ( uint32_t i = 0; i < Level::MAX; ++i )
		IB[ i ] = NULL;
}

void GE_Terrain::Level::create( float i_sizeWidth, uint32_t i_quadDensityWidth )
{
	uint32_t numVertsWidth = i_quadDensityWidth + 1;
	Vertices.resize( numVertsWidth * numVertsWidth );
	Indices[ Level::CENTER ].resize( i_quadDensityWidth * i_quadDensityWidth * 2 );
	Indices[ Level::DOWN ].resize( i_quadDensityWidth * ( i_quadDensityWidth - 1 ) * 2 + i_quadDensityWidth / 2 * 3 );
	Indices[ Level::DOWN_LEFT ].resize( i_quadDensityWidth * ( i_quadDensityWidth - 2 ) * 2 + i_quadDensityWidth / 2 * 3 * 2 );
	Indices[ Level::DOWN_LEFT_RIGHT ].resize( i_quadDensityWidth * ( i_quadDensityWidth - 3 ) * 2 + i_quadDensityWidth / 2 * 3 * 3 );

	float microSize = i_sizeWidth / i_quadDensityWidth;
	float halfSize = i_sizeWidth * ( -0.5f );
	GE_Vec2 uv;
	uv.x = uv.y = 0.0f;
	uint32_t i = 0;
	for ( uint32_t yy = 0; yy < numVertsWidth; ++yy )
	{
		for ( uint32_t xx = 0; xx < numVertsWidth; ++xx )
		{
			Vertices[ i ].Position = GE_Vec3( microSize * xx + halfSize, microSize * yy + halfSize, 0.0f );
			++i;
		}
	}

	uint32_t ic = 0;
	uint32_t icDown = 0;
	uint32_t icDownLeft = 0;
	uint32_t icDownLeftRight = 0;
	for ( uint32_t yy = 0; yy < i_quadDensityWidth; ++yy )
	{
		for ( uint32_t xx = 0; xx < i_quadDensityWidth; ++xx )
		{
			if ( yy == 0 )
			{
				if ( ( xx % 2 ) == 0 )
				{
					Indices[ Level::DOWN ][ icDown ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx + 2 );
					Indices[ Level::DOWN ][ icDown ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 2 );
					Indices[ Level::DOWN ][ icDown ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );

					Indices[ Level::DOWN ][ icDown + 1 ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx );
					Indices[ Level::DOWN ][ icDown + 1 ].Triangle[ 1 ] = numVertsWidth * ( yy )+( xx + 2 );
					Indices[ Level::DOWN ][ icDown + 1 ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );

					Indices[ Level::DOWN ][ icDown + 2 ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx );
					Indices[ Level::DOWN ][ icDown + 2 ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
					Indices[ Level::DOWN ][ icDown + 2 ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx );

					icDown += 3;
				}
			}
			else
			{
				Indices[ Level::DOWN ][ icDown ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx );
				Indices[ Level::DOWN ][ icDown ].Triangle[ 1 ] = numVertsWidth * ( yy )+( xx );
				Indices[ Level::DOWN ][ icDown ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx + 1 );

				Indices[ Level::DOWN ][ icDown + 1 ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx + 1 );
				Indices[ Level::DOWN ][ icDown + 1 ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
				Indices[ Level::DOWN ][ icDown + 1 ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx );

				icDown += 2;
			}

			if ( yy == 0 || xx == 0 )
			{
				if ( xx == 0 && yy == 0 )
				{
					Indices[ Level::DOWN_LEFT ][ icDownLeft ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx + 2 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 2 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );

					Indices[ Level::DOWN_LEFT ][ icDownLeft + 1 ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx );
					Indices[ Level::DOWN_LEFT ][ icDownLeft + 1 ].Triangle[ 1 ] = numVertsWidth * ( yy )+( xx + 2 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft + 1 ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );

					Indices[ Level::DOWN_LEFT ][ icDownLeft + 2 ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft + 2 ].Triangle[ 1 ] = numVertsWidth * ( yy + 2 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft + 2 ].Triangle[ 2 ] = numVertsWidth * ( yy + 2 ) + ( xx );

					Indices[ Level::DOWN_LEFT ][ icDownLeft + 3 ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft + 3 ].Triangle[ 1 ] = numVertsWidth * ( yy + 2 ) + ( xx );
					Indices[ Level::DOWN_LEFT ][ icDownLeft + 3 ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx );

					icDownLeft += 4;
				}
				else if ( ( xx % 2 ) == 0 && yy == 0 )
				{
					Indices[ Level::DOWN_LEFT ][ icDownLeft ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx + 2 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 2 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );

					Indices[ Level::DOWN_LEFT ][ icDownLeft + 1 ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx );
					Indices[ Level::DOWN_LEFT ][ icDownLeft + 1 ].Triangle[ 1 ] = numVertsWidth * ( yy )+( xx + 2 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft + 1 ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );

					Indices[ Level::DOWN_LEFT ][ icDownLeft + 2 ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx );
					Indices[ Level::DOWN_LEFT ][ icDownLeft + 2 ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft + 2 ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx );
					icDownLeft += 3;
				}
				else if ( ( yy % 2 ) == 0 && xx == 0 )
				{
					Indices[ Level::DOWN_LEFT ][ icDownLeft ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft ].Triangle[ 1 ] = numVertsWidth * ( yy + 2 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft ].Triangle[ 2 ] = numVertsWidth * ( yy + 2 ) + ( xx );

					Indices[ Level::DOWN_LEFT ][ icDownLeft + 1 ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft + 1 ].Triangle[ 1 ] = numVertsWidth * ( yy + 2 ) + ( xx );
					Indices[ Level::DOWN_LEFT ][ icDownLeft + 1 ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx );

					Indices[ Level::DOWN_LEFT ][ icDownLeft + 2 ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx + 1 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft + 2 ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT ][ icDownLeft + 2 ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx );

					icDownLeft += 3;
				}
			}
			else
			{
				Indices[ Level::DOWN_LEFT ][ icDownLeft ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx );
				Indices[ Level::DOWN_LEFT ][ icDownLeft ].Triangle[ 1 ] = numVertsWidth * ( yy )+( xx );
				Indices[ Level::DOWN_LEFT ][ icDownLeft ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx + 1 );

				Indices[ Level::DOWN_LEFT ][ icDownLeft + 1 ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx + 1 );
				Indices[ Level::DOWN_LEFT ][ icDownLeft + 1 ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
				Indices[ Level::DOWN_LEFT ][ icDownLeft + 1 ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx );

				icDownLeft += 2;
			}

			if ( yy == 0 || xx == 0 || xx == i_quadDensityWidth - 1 )
			{
				if ( xx >= i_quadDensityWidth - 2 && yy == 0 )
				{
					if ( xx == i_quadDensityWidth - 2 )
					{
						Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
						Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx );
						Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx );

						Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
						Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 1 ] = numVertsWidth * ( yy )+( xx );
						Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx + 2 );

						Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 0 ] = numVertsWidth * ( yy + 2 ) + ( xx + 2 );
						Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
						Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx + 2 );

						Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 3 ].Triangle[ 0 ] = numVertsWidth * ( yy + 2 ) + ( xx + 2 );
						Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 3 ].Triangle[ 1 ] = numVertsWidth * ( yy + 2 ) + ( xx + 1 );
						Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 3 ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );

						icDownLeftRight += 4;
					}
				}
				else if ( xx == 0 && yy == 0 )
				{
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx + 2 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 2 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );

					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 1 ] = numVertsWidth * ( yy )+( xx + 2 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );

					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 1 ] = numVertsWidth * ( yy + 2 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 2 ] = numVertsWidth * ( yy + 2 ) + ( xx );

					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 3 ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 3 ].Triangle[ 1 ] = numVertsWidth * ( yy + 2 ) + ( xx );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 3 ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx );

					icDownLeftRight += 4;
				}
				else if ( ( xx % 2 ) == 0 && yy == 0 )
				{
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx + 2 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 2 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );

					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 1 ] = numVertsWidth * ( yy )+( xx + 2 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );

					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx );
					icDownLeftRight += 3;
				}
				else if ( ( yy % 2 ) == 0 && xx == 0 )
				{
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 1 ] = numVertsWidth * ( yy + 2 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 2 ] = numVertsWidth * ( yy + 2 ) + ( xx );

					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 1 ] = numVertsWidth * ( yy + 2 ) + ( xx );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx );

					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx + 1 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx );

					icDownLeftRight += 3;
				}
				else if ( ( yy % 2 ) == 0 && xx == i_quadDensityWidth - 1 )
				{
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 1 ] = numVertsWidth * ( yy )+( xx );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx + 1 );

					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 0 ] = numVertsWidth * ( yy + 2 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx + 1 );

					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 0 ] = numVertsWidth * ( yy + 2 ) + ( xx + 1 );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 1 ] = numVertsWidth * ( yy + 2 ) + ( xx );
					Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 2 ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx );

					icDownLeftRight += 3;
				}

			}
			else
			{
				Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx );
				Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 1 ] = numVertsWidth * ( yy )+( xx );
				Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx + 1 );

				Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx + 1 );
				Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
				Indices[ Level::DOWN_LEFT_RIGHT ][ icDownLeftRight + 1 ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx );

				icDownLeftRight += 2;
			}

			Indices[ Level::CENTER ][ ic ].Triangle[ 0 ] = numVertsWidth * ( yy + 1 ) + ( xx );
			Indices[ Level::CENTER ][ ic ].Triangle[ 1 ] = numVertsWidth * ( yy )+( xx );
			Indices[ Level::CENTER ][ ic ].Triangle[ 2 ] = numVertsWidth * ( yy )+( xx + 1 );

			Indices[ Level::CENTER ][ ic + 1 ].Triangle[ 0 ] = numVertsWidth * ( yy )+( xx + 1 );
			Indices[ Level::CENTER ][ ic + 1 ].Triangle[ 1 ] = numVertsWidth * ( yy + 1 ) + ( xx + 1 );
			Indices[ Level::CENTER ][ ic + 1 ].Triangle[ 2 ] = numVertsWidth * ( yy + 1 ) + ( xx );

			ic += 2;

		}
	}

	VB = GE_Core::getSingleton().createVertexBuffer( Vertices.data(), sizeof( VertexBuffer ), ( uint32_t )Vertices.size() );
	IB[ Level::CENTER ] = GE_Core::getSingleton().createIndexBuffer( Indices[ Level::CENTER ].data(), sizeof( IndexBuffer ), ( uint32_t )Indices[ Level::CENTER ].size() );
	IB[ Level::DOWN ] = GE_Core::getSingleton().createIndexBuffer( Indices[ Level::DOWN ].data(), sizeof( IndexBuffer ), ( uint32_t )Indices[ Level::DOWN ].size() );
	IB[ Level::DOWN_LEFT ] = GE_Core::getSingleton().createIndexBuffer( Indices[ Level::DOWN_LEFT ].data(), sizeof( IndexBuffer ), ( uint32_t )Indices[ Level::DOWN_LEFT ].size() );
	IB[ Level::DOWN_LEFT_RIGHT ] = GE_Core::getSingleton().createIndexBuffer( Indices[ Level::DOWN_LEFT_RIGHT ].data(), sizeof( IndexBuffer ), ( uint32_t )Indices[ Level::DOWN_LEFT_RIGHT ].size() );
}

GE_Terrain::Quad::Quad()
{
	UpLeft = NULL;
	UpRight = NULL;
	DownLeft = NULL;
	DownRight = NULL;
	Parrent = NULL;
	ChunkID = -1;
	X = -1;
	Y = -1;
	HalfSize = 0;
	Position = GE_Vec3( 0, 0, 0 );
}

bool GE_Terrain::Quad::calculate( float i_halfSize, float i_minHalfSize, const Quad *i_base )
{
	HalfSize = i_halfSize;
	i_halfSize *= 0.5f;
	IsChunk = false;
	if ( HalfSize < i_minHalfSize )
	{
		uint32_t rows = ( uint32_t )( i_base->HalfSize / i_minHalfSize );
		GE_Vec3 pos = Parrent->Position;
		pos.x += i_base->HalfSize;
		pos.y += i_base->HalfSize;
		float width = ( i_minHalfSize * 2.0f );
		int32_t x = ( int32_t )( pos.x / width );
		int32_t y = ( int32_t )( pos.y / width );
		y = ( rows - 1 ) - y;
		Parrent->ChunkID = y * rows + x;
		Parrent->X = x;
		Parrent->Y = y;
		Parrent->IsChunk = true;
		return false;
	}

	UpLeft = new Quad();
	UpLeft->Position.x = Position.x - i_halfSize;
	UpLeft->Position.y = Position.y + i_halfSize;
	UpLeft->Parrent = this;

	if ( !UpLeft->calculate( i_halfSize, i_minHalfSize, i_base ) )
		SAFE_DELETE( UpLeft );

	UpRight = new Quad();
	UpRight->Position.x = Position.x + i_halfSize;
	UpRight->Position.y = Position.y + i_halfSize;
	UpRight->Parrent = this;
	if ( !UpRight->calculate( i_halfSize, i_minHalfSize, i_base ) )
		SAFE_DELETE( UpRight );

	DownLeft = new Quad();
	DownLeft->Position.x = Position.x - i_halfSize;
	DownLeft->Position.y = Position.y - i_halfSize;
	DownLeft->Parrent = this;
	if ( !DownLeft->calculate( i_halfSize, i_minHalfSize, i_base ) )
		SAFE_DELETE( DownLeft );

	DownRight = new Quad();
	DownRight->Position.x = Position.x + i_halfSize;
	DownRight->Position.y = Position.y - i_halfSize;
	DownRight->Parrent = this;
	if ( !DownRight->calculate( i_halfSize, i_minHalfSize, i_base ) )
		SAFE_DELETE( DownRight );

	return true;
}

GE_Terrain::Quad::~Quad()
{
	SAFE_DELETE( UpLeft );
	SAFE_DELETE( UpRight );
	SAFE_DELETE( DownLeft );
	SAFE_DELETE( DownRight );
}

GE_Terrain::Buffer::Buffer()
{
	for ( uint32_t i = 0; i < Level::MAX; ++i )
		BufferResource[ i ] = new _GE_BufferResource();
}

GE_Terrain::Buffer::~Buffer()
{
	for ( uint32_t i = 0; i < Level::MAX; ++i )
		SAFE_DELETE( BufferResource[ i ] );
}

void GE_Terrain::Buffer::createBuffer( uint32_t i_size )
{
	for ( uint32_t i = 0; i < Level::MAX; ++i )
	{
		InstancesData[ i ].resize( i_size );
		BufferResource[ i ]->createBuffer( ( uint32_t )InstancesData[ i ].size(), sizeof( GE_Vec4 ), InstancesData[ i ].data(), false );
	}
}

GE_Terrain::MaterialParameters::MaterialParameters()
{
	BaseEmissive = Emissive = GE_Vec3( 0.0f, 0.0f, 0.0f );
	BaseAmbient = Ambient = GE_Vec3( 0.8f, 0.8f, 1.0f );
	BaseDiffuse = Diffuse = GE_Vec3( 1.0f, 1.0f, 1.0f );
	BaseGlow = Glow = GE_Vec3( 1.0f, 1.0f, 1.0f );
	BaseSpecular = Specular = GE_Vec3( 0.8f, 0.8f, 1.0f );

	EmissiveIntensity = 1.0f;
	AmbientIntensity = 1.0f;
	DiffuseIntensity = 1.0f;
	GlowIntensity = 0.0f;
	setGlowIntensity( GlowIntensity );
	SpecularIntensity = 1.0f;

	SpecularPower = 10.0f;
	Roughness = 0.0f;
	Reflectmask = 0.0f;
	Bumpiness = 1.0f;


}

GE_Terrain::MaterialParameters::~MaterialParameters()
{

}

GE_Terrain::ProceduralParameters::ProceduralParameters()
{
	Enable = false;
	Opacity = 1.0f;
	UVTile = GE_Vec2( 100.0f, 100.0f );
	UVPosition = GE_Vec2( 0.0f, 0.0f );
	Height = GE_ConvertToUnit( 0.0f );
	HeightField = GE_ConvertToUnit( 10000.0f );
	HeightFalloff = 1.0f;
	HeightHardness = 0.0f;
	Angle = GE_ConvertToRadians( 0.0f );
	AngleField = GE_ConvertToRadians( 90.0f );
	AngleFalloff = 1.0f;
	AngleHardness = 0.0f;
	MaskUVPosition = MaskPosition = GE_Vec2( 0.0f, 0.0f );
	MaskUVTile = GE_Vec2( 1.0f, 1.0f );
	MaskField = GE_ConvertToUnit( 100000.0f );
	MaskFalloff = 1.0f;
	MaskHardness = 0.0f;

}

GE_Terrain::ProceduralParameters::~ProceduralParameters()
{

}

GE_Terrain::PaintParameters::PaintParameters()
{
	Enable = false;
	Opacity = 0.5f;
	UVTile = GE_Vec2( 100.0f, 100.0f );
	UVPosition = GE_Vec2( 0.0f, 0.0f );


}

GE_Terrain::PaintParameters::~PaintParameters()
{

}

GE_Terrain::PaintMaps::PaintMaps()
{
	DiffuseMap = NULL;
	NormalMap = NULL;
	GSRMap = NULL;
}

GE_Terrain::PaintMaps::~PaintMaps()
{
	if ( DiffuseMap )
		GE_TextureManager::getSingleton().remove( &DiffuseMap );
	if ( NormalMap )
		GE_TextureManager::getSingleton().remove( &NormalMap );
	if ( GSRMap )
		GE_TextureManager::getSingleton().remove( &GSRMap );
}

GE_Terrain::ProceduralMaps::ProceduralMaps()
{
	DiffuseMap = NULL;
	NormalMap = NULL;
	GSRMap = NULL;
	MaskMap = NULL;
}

GE_Terrain::ProceduralMaps::~ProceduralMaps()
{
	if ( DiffuseMap )
		GE_TextureManager::getSingleton().remove( &DiffuseMap );
	if ( NormalMap )
		GE_TextureManager::getSingleton().remove( &NormalMap );
	if ( GSRMap )
		GE_TextureManager::getSingleton().remove( &GSRMap );
	if ( MaskMap )
		GE_TextureManager::getSingleton().remove( &MaskMap );
}

GE_Terrain::Maps::Maps()
{
	ReflectMap = NULL;
	NormalNoiseMap = NULL;
}

GE_Terrain::Maps::~Maps()
{
	if ( ReflectMap )
		GE_TextureManager::getSingleton().remove( &ReflectMap );
	if ( NormalNoiseMap )
		GE_TextureManager::getSingleton().remove( &NormalNoiseMap );
}
