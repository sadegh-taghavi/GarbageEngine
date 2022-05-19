#include "GE_DeferredPost.h"
#include "GE_DeferredGeometry.h"
#include "GE_Model.h"
#include "GE_RenderTarget.h"
#include "GE_View.h"
#include "GE_Core.h"
#include "GE_BufferResource.h"
#include "GE_Light.h"
#include "GE_Utility.h"

GE_DeferredPost GE_DeferredPost::m_singleton;

void GE_DeferredPost::init()
{
	if ( m_vS )
		return;
	GE_SphereVertex sv;
	sv.createVertices( 1 );
	m_indicesCount = ( uint32_t ) sv.Indices.size();
	m_sphereVB = GE_Core::getSingleton().createVertexBuffer( &sv.Vertices.at( 0 ), sizeof( float ) * 3, ( uint32_t ) sv.Vertices.size() );
	m_sphereIB = GE_Core::getSingleton().createIndexBuffer( &sv.Indices.at( 0 ), sizeof( uint32_t ), m_indicesCount );

	m_perFrame = GE_Core::getSingleton().createConstBuffer( sizeof( PerFrame ) );

	m_vS = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DP_VS.cso" );
	m_vS_Dir = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DP_VS_DIR.cso" );

	m_pS_Dir[ 0 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_DIR.cso" );
	m_pS_Dir[ 1 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_DIR1.cso" );
	m_pS_Dir[ 2 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_DIR2.cso" );
	m_pS_Dir[ 3 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_DIR3.cso" );
	m_pS_Dir[ 4 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_DIR4.cso" );

	m_pS_Dir_Visualize[ 0 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_DIR1_VISUALIZE.cso" );
	m_pS_Dir_Visualize[ 1 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_DIR2_VISUALIZE.cso" );
	m_pS_Dir_Visualize[ 2 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_DIR3_VISUALIZE.cso" );
	m_pS_Dir_Visualize[ 3 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_DIR4_VISUALIZE.cso" );

	m_pS_Spot_NoShadow_NoFilter = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_SPOT_NoShadow_NoFilter.cso" );
	m_pS_Spot_NoShadow_Filter = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_SPOT_NoShadow_Filter.cso" );
	m_pS_Spot_Shadow_NoFilter = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_SPOT_Shadow_NoFilter.cso" );
	m_pS_Spot_Shadow_Filter = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_SPOT_Shadow_Filter.cso" );

	m_pS_Point_Filter_Shadow = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_POINT_Filter_Shadow.cso" );
	m_pS_Point_NoFilter_NoShadow = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_POINT_NoFilter_NoShadow.cso" );
	m_pS_Point_Filter_NoShadow = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_POINT_Filter_NoShadow.cso" );
	m_pS_Point_NoFilter_Shadow = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DP_PS_POINT_NoFilter_Shadow.cso" );

}

void GE_DeferredPost::begin()
{
	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );
	m_useOne = true;
}

void GE_DeferredPost::end()
{
	ID3D11ShaderResourceView *srv[ 12 ];
	srv[ 0 ] = srv[ 1 ] = srv[ 2 ] = srv[ 3 ] = srv[ 4 ] = srv[ 5 ] = srv[ 6 ] = srv[ 7 ] = srv[ 8 ] = srv[ 9 ] = srv[ 10 ] = srv[ 11 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 12, srv );
	GE_Core::getSingleton().getImmediateContext()->OMSetBlendState( GE_Core::getSingleton().getRenderStates().BlendStateDisableAlpha, 0, 0xffffffff );
	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );
}

void GE_DeferredPost::render( GE_View *i_view, GE_Light *i_light )
{

	ID3D11ShaderResourceView *srv[ 6 ];

	GE_Light::Parameters *lp;
	lp = i_light->getParameters();
	if ( !lp->Enable )
		return;
	m_cPerFrame.LightRange = lp->Range;
	m_cPerFrame.LightAttenuation = lp->Attenuation;
	m_cPerFrame.LightSpotSpread = cos( lp->SpotSpread * 0.5f );
	m_cPerFrame.LightSpotSharpness = cos( lp->SpotSharpness * 0.5f );
	memcpy( &m_cPerFrame.LightPosition, &lp->Position, sizeof( GE_Vec3 ) );
	memcpy( &m_cPerFrame.LightAmbient, &lp->Ambient, sizeof( GE_Vec3 ) );
	memcpy( &m_cPerFrame.LightDiffuse, &lp->Diffuse, sizeof( GE_Vec3 ) );
	memcpy( &m_cPerFrame.LightSpecular, &lp->Specular, sizeof( GE_Vec3 ) );
	memcpy( &m_cPerFrame.LightDirection, &( lp->Direction * -1.0f ), sizeof( GE_Vec3 ) );
	memcpy( &m_cPerFrame.ViewPosition, &i_view->m_position, sizeof( GE_Vec3 ) );
	m_cPerFrame.LightAmbient *= lp->Intensity;
	m_cPerFrame.LightDiffuse *= lp->Intensity;
	m_cPerFrame.LightSpecular *= lp->Intensity;
	m_cPerFrame.ViewProjection.transposeBy( &i_view->m_matViewProjection );
	m_cPerFrame.ViewProjectionInv.transposeBy( &i_view->m_matViewProjectionInv );
	m_cPerFrame.CubeFilterRotate.transposeBy( i_light->getCubeFilterRotate() );

	srv[ 0 ] = GE_DeferredGeometry::getSingleton().getTargetSRV( GE_DeferredGeometry::NormalSpecular );
	srv[ 1 ] = GE_DeferredGeometry::getSingleton().getTargetSRV( GE_DeferredGeometry::DiffuseGlow );
	srv[ 2 ] = GE_DeferredGeometry::getSingleton().getTargetSRV( GE_DeferredGeometry::MaterialID );
	srv[ 3 ] = GE_DeferredGeometry::getSingleton().getTargetSRV( GE_DeferredGeometry::Depth );
	srv[ 4 ] = GE_DeferredGeometry::getSingleton().getMaterialBufferSRV();

	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 5, srv );

	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 1, &GE_Core::getSingleton().getRenderStates().SamplerLinearBorder );

	bool shadow = ( lp->HasShadow && lp->DoShadow );

	if ( shadow )
	{
		uint32_t smc = 0;
		if ( lp->LightType == GE_Light::Directional )
			smc = lp->NumberOfCascade;
		else if ( lp->LightType == GE_Light::Spot )
			smc = 1;
		else
			smc = LIGHT_PARAM_SIZE;

		for ( uint32_t i = 0; i < smc; i++ )
		{
			srv[ i ] = i_light->getShadowMap( i )->getDepthSRV();
			m_cPerFrame.LightViewProjection[ i ].transposeBy( i_light->getViewProjection( i ) );
			m_cPerFrame.ShadowOffsetBias[ i ].x = lp->ShadowOffsetBias[ i ].x;
			m_cPerFrame.ShadowOffsetBias[ i ].y = lp->ShadowOffsetBias[ i ].y;

		}
		GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 5, smc, srv );
	}
	else
		m_cPerFrame.LightViewProjection[ 0 ].transposeBy( i_light->getViewProjection( 0 ) );

	ID3D11ShaderResourceView *filterSRV = srv[ 0 ] = srv[ 1 ] = i_light->getFilterSRV();
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 11, 2, srv );

	GE_Mat4x4 mwvp;
	GE_Mat4x4Multiply( &mwvp, i_light->getWorld(), &i_view->m_matViewProjection );

	m_cPerFrame.WorldViewProjection.transposeBy( &mwvp );

	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers( 0, 1, &m_perFrame );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );

	if ( lp->LightType == GE_Light::Directional )
	{
		GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Dir, NULL, 0 );
		GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );
		if ( shadow )
		{
			if ( lp->VisualizeCascades )
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Dir_Visualize[ lp->NumberOfCascade - 1 ], NULL, 0 );
			else
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Dir[ lp->NumberOfCascade ], NULL, 0 );
		}
		else
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Dir[ 0 ], NULL, 0 );


		GE_Core::getSingleton().setScreenAlignQuadInputLayoutToContext();
		GE_Core::getSingleton().renderScreenAlignQuad();

	}
	else
	{
		GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );
		GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

		if ( lp->LightType == GE_Light::Spot )
		{
			if ( !shadow && !filterSRV )
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Spot_NoShadow_NoFilter, NULL, 0 );
			else if ( !shadow && filterSRV )
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Spot_NoShadow_Filter, NULL, 0 );
			else if ( shadow && !filterSRV )
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Spot_Shadow_NoFilter, NULL, 0 );
			else
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Spot_Shadow_Filter, NULL, 0 );
		}
		else
		{
			if ( !shadow && !filterSRV )
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Point_NoFilter_NoShadow, NULL, 0 );
			else if ( !shadow && filterSRV )
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Point_Filter_NoShadow, NULL, 0 );
			else if ( shadow && !filterSRV )
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Point_NoFilter_Shadow, NULL, 0 );
			else
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Point_Filter_Shadow, NULL, 0 );
		}
		GE_Core::getSingleton().setScreenAlignQuadVertexDeclarationToContext();
		GE_Core::getSingleton().getImmediateContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		GE_Core::getSingleton().renderTriangleList( m_sphereVB, sizeof( float )* 3, m_sphereIB, m_indicesCount );
	}

	if ( m_useOne )
	{
		GE_Core::getSingleton().getImmediateContext()->OMSetBlendState( GE_Core::getSingleton().getRenderStates().BlendStateAlphaOne, 0, 0xffffffff );
		m_useOne = false;
	}
}

GE_DeferredPost::GE_DeferredPost()
{
	m_perFrame = NULL;
	m_vS = NULL;
	m_vS_Dir = NULL;
	m_pS_Spot_NoShadow_NoFilter = NULL;
	m_pS_Spot_Shadow_NoFilter = NULL;
	m_pS_Spot_NoShadow_Filter = NULL;
	m_pS_Spot_Shadow_Filter = NULL;
	m_pS_Point_NoFilter_NoShadow = NULL;
	m_pS_Point_Filter_Shadow = NULL;
	m_pS_Point_Filter_NoShadow = NULL;
	m_pS_Point_NoFilter_Shadow = NULL;

	for ( uint32_t i = 0; i < 5; i++ )
		m_pS_Dir[ i ] = NULL;
	for ( uint32_t i = 0; i < 4; i++ )
		m_pS_Dir_Visualize[ i ] = NULL;

	m_sphereVB = NULL;
	m_sphereIB = NULL;
}

GE_DeferredPost::~GE_DeferredPost()
{
	SAFE_RELEASE( m_perFrame );
	SAFE_RELEASE( m_vS );
	SAFE_RELEASE( m_vS_Dir );
	SAFE_RELEASE( m_pS_Spot_NoShadow_NoFilter );
	SAFE_RELEASE( m_pS_Spot_Shadow_NoFilter );
	SAFE_RELEASE( m_pS_Spot_NoShadow_Filter );
	SAFE_RELEASE( m_pS_Spot_Shadow_Filter );
	SAFE_RELEASE( m_pS_Point_NoFilter_NoShadow );
	SAFE_RELEASE( m_pS_Point_Filter_Shadow );
	SAFE_RELEASE( m_pS_Point_NoFilter_Shadow );
	SAFE_RELEASE( m_pS_Point_Filter_NoShadow );
	for ( uint32_t i = 0; i < 5; i++ )
		SAFE_RELEASE( m_pS_Dir[ i ] );
	for ( uint32_t i = 0; i < 4; i++ )
		SAFE_RELEASE( m_pS_Dir_Visualize[ i ] );

	SAFE_RELEASE( m_sphereVB );
	SAFE_RELEASE( m_sphereIB );
}

