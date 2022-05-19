#include "GE_EnvironmentProbe.h"
#include "GE_RenderTarget.h"
#include "GE_Model.h"
#include "GE_Light.h"
#include "GE_Sky.h"
#include "GE_BasicRenderer.h"
#include "GE_DeferredGeometry.h"
#include "GE_PostProcess.h"
#include "GE_DeferredPost.h"
#include "GE_Utility.h"
#include <DirectXTex.h>
#include <boost/foreach.hpp>

GE_EnvironmentProbe::GE_EnvironmentProbe()
{
	for ( int32_t i = 0; i < 6; ++i )
	{
		m_view[ i ].m_viewType = GE_View::Target;
		m_view[ i ].m_uP = GE_Vec3( 0.0f, 0.0f, 1.0f );
		m_view[ i ].m_aspect = m_mapSize.x / m_mapSize.y;
		m_view[ i ].m_fovy = GE_ConvertToRadians( 90.0f );
		m_view[ i ].m_near = GE_ConvertToUnit( 0.01f );
		m_view[ i ].m_far = GE_ConvertToUnit( 5000.0f );
		m_renderTarget[ i ] = NULL;
	}
	m_view[ 2 ].m_uP = GE_Vec3( 0.0f, -1.0f, 0.0f );
	m_view[ 3 ].m_uP = GE_Vec3( 0.0f, 1.0f, 0.0f );
	m_cubeTarget = NULL;
	m_scale.x = m_scale.y = m_scale.z = 1.0f;
	m_enable = true;
	m_cubeTarget = NULL;
	setPosition( GE_Vec3( 0.0f, 0.0f, 0.0f ) );
	setMapSize( GE_Vec2( 128.0f, 128.0f ) );
}

GE_EnvironmentProbe::~GE_EnvironmentProbe()
{
	for ( int32_t i = 0; i < 6; i++ )
		SAFE_DELETE( m_renderTarget[ i ] );
	SAFE_DELETE( m_cubeTarget );
}

GE_EnvironmentProbe::ShadowModels::~ShadowModels( )
{
	for ( auto it = ModelLists.begin( ); it != ModelLists.end( ); it++ )
	{
		auto *lst = *it;
		SAFE_DELETE( lst );
	}
	ModelLists.clear( );
}

void GE_EnvironmentProbe::render( std::list<GE_Model*> &i_modelList, std::list<GE_Light*> &i_lightList )
{
	/*for ( int32_t viewIndex = 0; viewIndex < 6; viewIndex++ )
	{
		std::list<GE_Model*>				renderModelList;
		std::list<GE_Light*>				renderLightList;
		list< ShadowModels > renderShadowModelsList;

		BOOST_FOREACH( GE_Model *model, i_modelList )
		{
			if ( m_view[ viewIndex ].m_frustum.sphereInFrustum(
				&model->getBound().BoundCenter,
				model->getBound().BoundRadius ) )
			{
				renderModelList.push_back( model );
			}
		}

		BOOST_FOREACH( GE_Light *light, i_lightList )
		{
			if ( m_view[ viewIndex ].m_frustum.sphereInFrustum(
				light->getPosition(),
				light->getRange() ) != GE_Frustum::OutSide ||
				light->getType() == GE_Light::Directional )
			{
				renderLightList.push_back( light );
				if ( light->hasShadow() )
				{
					renderShadowModelsList.push_back( ShadowModels() );
					auto shadowMdl = renderShadowModelsList.end();
					shadowMdl--;
					( *shadowMdl ).Light = light;

					GE_Frustum frustum;
					for ( uint32_t i = 0; i < ( ( light->getType() == GE_Light::Point ) ? LIGHT_PARAM_SIZE : light->getNumberOfCascade() ); ++i )
					{
						std::list<GE_Model *> *modelList = new std::list<GE_Model *>();
						( *shadowMdl ).ModelLists.push_back( modelList );
						frustum.calculatePlanes( light->getViewProjection( i ) );
						BOOST_FOREACH( GE_Model *model, i_modelList )
						{
							if ( frustum.sphereInFrustum(
								&model->getBound().BoundCenter,
								model->getBound().BoundRadius ) != GE_Frustum::OutSide )
							{
								modelList->push_back( model );
							}
						}
					}
				}
			}
		}

		BOOST_FOREACH( ShadowModels &shadowModel, renderShadowModelsList )
		{
			GE_BasicRenderer::getSingleton().renderToDepth( shadowModel.Light, &shadowModel.ModelLists );
		}

		GE_DeferredGeometry::getSingleton().begin();
		GE_DeferredGeometry::getSingleton().render( &renderModelList, &m_view[ viewIndex ] );
		GE_DeferredGeometry::getSingleton().end();

		if ( !GE_PostProcess::getSingleton().isEnableHDR() )
		{
			m_renderTarget[ viewIndex ]->setToDevice();
			m_renderTarget[ viewIndex ]->clearRenderTarget( 0 );
		}
			
		GE_PostProcess::getSingleton().beginHDR();
		GE_Sky::getSingleton().render( &m_view[ viewIndex ] );
		GE_DeferredPost::getSingleton().begin();
		BOOST_FOREACH( GE_Light *light, renderLightList )
		{
			GE_DeferredPost::getSingleton().render( &m_view[ viewIndex ], light );
		}
		GE_DeferredPost::getSingleton().end();
		m_renderTarget[ viewIndex ]->clearRenderTarget();
		GE_PostProcess::getSingleton( ).endHDR( m_renderTarget[ viewIndex] , false );
	}
	GE_EnvironmentProbeRenderer::getSingleton().generateCube( this );*/

}

void GE_EnvironmentProbe::saveToFile( std::string i_path, std::string i_name )
{
	m_cubeTarget->saveRTToFile( ( i_path + "\\" + i_name + ".dds" ).c_str(), 0 );
}

void GE_EnvironmentProbe::setPosition( GE_Vec3 &i_position )
{
	m_position = i_position;

	m_view[ 0 ].m_position = m_view[ 0 ].m_target = m_position; // right
	m_view[ 0 ].m_target.x += 1.0f;
	m_view[ 0 ].update();

	m_view[ 1 ].m_position = m_view[ 1 ].m_target = m_position; // left
	m_view[ 1 ].m_target.x -= 1.0f;
	m_view[ 1 ].update();

	m_view[ 2 ].m_position = m_view[ 2 ].m_target = m_position; // top
	m_view[ 2 ].m_target.z += 1.0f;
	m_view[ 2 ].update();

	m_view[ 3 ].m_position = m_view[ 3 ].m_target = m_position; // bottom
	m_view[ 3 ].m_target.z -= 1.0f;
	m_view[ 3 ].update();

	m_view[ 4 ].m_position = m_view[ 4 ].m_target = m_position; // front
	m_view[ 4 ].m_target.y += 1.0f;
	m_view[ 4 ].update();

	m_view[ 5 ].m_position = m_view[ 5 ].m_target = m_position; // back
	m_view[ 5 ].m_target.y -= 1.0f;
	m_view[ 5 ].update();

}

void GE_EnvironmentProbe::setMapSize( GE_Vec2 &i_mapSize )
{
	m_mapSize = i_mapSize;

	GE_RenderTarget::Parameters rtParam( 1 );
	rtParam.Width[ 0 ] = (uint32_t)m_mapSize.x;
	rtParam.Height[ 0 ] = ( uint32_t )m_mapSize.y;
	rtParam.HasDepth = false;

	for ( int32_t i = 0; i < 6; i++ )
	{
		SAFE_DELETE( m_renderTarget[ i ] );
		m_renderTarget[ i ] = new GE_RenderTarget( rtParam );
	}

	rtParam.MipLevel[ 0 ] = ( uint32_t )( log( ( uint32_t )i_mapSize.x ) / log( 2 ) );
	rtParam.Type = GE_RenderTarget::Parameters::CUBE;
	SAFE_DELETE( m_cubeTarget );
	m_cubeTarget = new GE_RenderTarget( rtParam );
}

ID3D11ShaderResourceView * GE_EnvironmentProbe::getSRV()
{
	return m_cubeTarget->getSRV( 0 );
}


//Renderer-----------------------------------------------------------------

GE_EnvironmentProbeRenderer::GE_EnvironmentProbeRenderer()
{
	m_vS = NULL;
	m_vS_Cube = NULL;
	m_pS_Cube = NULL;
	m_gS_Cube = NULL;
	m_pS = NULL;
	m_sphereVB = NULL;
	m_sphereIB = NULL;
}

GE_EnvironmentProbeRenderer::~GE_EnvironmentProbeRenderer()
{
	SAFE_RELEASE( m_vS );
	SAFE_RELEASE( m_gS_Cube );
	SAFE_RELEASE( m_vS_Cube );
	SAFE_RELEASE( m_pS );
	SAFE_RELEASE( m_pS_Cube );
	SAFE_RELEASE( m_sphereVB );
	SAFE_RELEASE( m_sphereIB );
	SAFE_RELEASE( m_perFrame );
	SAFE_RELEASE( m_perObject );
}

void GE_EnvironmentProbeRenderer::init()
{
	if ( m_pS )
		return;

	GE_SphereVertex sv;
	sv.createVertices( 3, GE_ConvertToUnit( 0.5f ) );
	m_indicesCount = ( uint32_t ) sv.Indices.size();
	m_sphereVB = GE_Core::getSingleton().createVertexBuffer( &sv.Vertices.at( 0 ), sizeof( float ) * 3, ( uint32_t ) sv.Vertices.size() );
	m_sphereIB = GE_Core::getSingleton().createIndexBuffer( &sv.Indices.at( 0 ), sizeof( uint32_t ), m_indicesCount );

	m_vS = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_EnvironmentProb_VS.cso" );
	m_pS = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_EnvironmentProb_PS.cso" );
	m_pS_Cube = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_EnvironmentProb_PS_Cube.cso" );
	m_vS_Cube = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_EnvironmentProb_VS_Cube.cso" );
	m_gS_Cube = GE_Core::getSingleton().createGS( string( GE_SHADER_PATH ) + "GE_EnvironmentProb_GS_Cube.cso" );
	m_perFrame = GE_Core::getSingleton().createConstBuffer( sizeof( PerFrame ) );
	m_perObject = GE_Core::getSingleton().createConstBuffer( sizeof( PerObject ) );
}

void GE_EnvironmentProbeRenderer::render( std::list< GE_EnvironmentProbe * > *i_probsList, const GE_View *i_view, ID3D11ShaderResourceView *i_depth )
{
	ID3D11ShaderResourceView *srv[ 2 ];
	
	if ( !i_probsList->size() )
		return;

	GE_Core::getSingleton().setScreenAlignQuadVertexDeclarationToContext();
	GE_Core::getSingleton().getImmediateContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	memcpy_s( &m_cPerFrame.ViewPosition, sizeof( GE_Vec3 ), &i_view->m_position, sizeof( GE_Vec3 ) );
	( ( GE_View * )i_view )->m_matProjectionInv.transposeOut( &m_cPerFrame.InverseProjection );
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers( 0, 1, &m_perFrame );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 1, &GE_Core::getSingleton().getRenderStates().SamplerLinearWrap );
	GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS, NULL, 0 );
	srv[ 1 ] = i_depth;
	GE_Quat   rot;
	rot.identity();

	BOOST_FOREACH( GE_EnvironmentProbe *prob, *i_probsList )
	{
		srv[ 0 ] = prob->getSRV();
		m_cPerObject.World.srp( prob->getPosition(), rot, prob->getScale() );
		m_cPerObject.WorldViewProjection = m_cPerObject.World * i_view->m_matViewProjection;
		m_cPerObject.WorldViewProjection.transpose();
		m_cPerObject.World.transpose();
		GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers( 1, 1, &m_perObject );
		GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 1, 1, &m_perObject );
		GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perObject, 0, NULL, &m_cPerObject, 0, 0 );
		GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
		GE_Core::getSingleton().renderTriangleList( m_sphereVB, sizeof( GE_Vec3 ), m_sphereIB, m_indicesCount );
	}
	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
}

void GE_EnvironmentProbeRenderer::generateCube( GE_EnvironmentProbe *i_prob )
{
	ID3D11ShaderResourceView *srv[ 6 ];

	i_prob->m_cubeTarget->clearRenderTarget();
	i_prob->m_cubeTarget->setToDevice();

	for ( uint32_t i = 0; i < 6; ++i )
		srv[ i ] = i_prob->m_renderTarget[ i ]->getSRV( 0 );
	
	GE_Core::getSingleton().setScreenAlignQuadInputLayoutToContext();
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 2, 6, srv );
	GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Cube, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->GSSetShader( m_gS_Cube, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Cube, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 1, &GE_Core::getSingleton().getRenderStates().SamplerLinearClamp );

	GE_Core::getSingleton().renderScreenAlignQuad();

	GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );

	for ( uint32_t i = 0; i < 6; ++i )
		srv[ i ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 2, 6, srv );

	GE_Core::getSingleton().getImmediateContext()->GenerateMips( i_prob->getSRV() );
}

GE_EnvironmentProbeRenderer GE_EnvironmentProbeRenderer::m_singleton;

