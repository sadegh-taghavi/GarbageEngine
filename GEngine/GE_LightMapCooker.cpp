#include "GE_LightMapCooker.h"
#include "GE_Core.h"
#include "GE_RenderTarget.h"
#include "GE_Model.h"
#include "GE_BasicRenderer.h"
#include "GE_Light.h"
#include "GE_Texture.h"
#include "GE_PostProcess.h"
#include <boost/foreach.hpp>
using namespace std;

GE_LightMapCooker GE_LightMapCooker::m_singleton;

GE_LightMapCooker::GE_LightMapCooker( void )
{
	m_overWriteLightMaps = true;
	m_pS_Dir = m_pS_Point = m_pS_Spot = NULL;
	m_vS = NULL;
	m_rt[ 0 ] = m_rt[ 1 ] = NULL;
	m_path = "GE_Data/GE_Texture/";
	m_size = 256;
	m_depthSize = 256;
}

void GE_LightMapCooker::init()
{
	if( m_vS )
		return;
	m_vS = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_LM_VS.cso" );
	m_pS_Dir = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_LM_PS_DIR.cso" );
	m_pS_Spot = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_LM_PS_SPOT.cso" );
	m_pS_Point = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_LM_PS_POINT.cso" );
	m_perObject = GE_Core::getSingleton().createConstBuffer( sizeof( PerObject ) );
}

void GE_LightMapCooker::cook( std::list<GE_Light *> &i_lights, std::list<GE_Model *> &i_models )
{
	if( !i_lights.size() || !i_models.size() )
		return;

	std::list<GE_Model *> zero_models;

	list<GE_Model *> listLightMap;
	vector<float>    maxIntensity;

	GE_RenderTarget::Parameters rtp;
	rtp.Count = 1;
	rtp.MipLevel[ 0 ] = 0;
	rtp.Width[ 0 ] = rtp.Height[ 0 ] = m_size;
	SAFE_DELETE( m_rt[ 0 ] );
	m_rt[ 0 ] = new GE_RenderTarget( rtp );
	m_rt[ 0 ]->setBackColor( 0.0f, 0.0f, 0.0f, 0.0f );
	SAFE_DELETE( m_rt[ 1 ] );
	rtp.MipLevel[ 0 ] = ( uint32_t ) ( log( m_size ) / log( 2 ) );
	m_rt[ 1 ] = new GE_RenderTarget( rtp );
	m_rt[ 1 ]->setBackColor( 0.0f, 0.0f, 0.0f, 0.0f );

	rtp.MipLevel[ 0 ] = 0;
	rtp.Count = 1;
	rtp.Width[ 0 ] = m_depthSize;
	rtp.Height[ 0 ] = m_depthSize;
	m_dirDepth = new GE_RenderTarget( rtp );

	BOOST_FOREACH( GE_Model *m, i_models )
	{
		if( m->hasAdditionTexcoord() )
			listLightMap.push_back( m );
	}

	GE_Core::getSingleton().getImmediateContext()->RSSetState( GE_Core::getSingleton().getRenderStates().RasterStateDisableCull );
	GE_Core::getSingleton().getImmediateContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	GE_Core::getSingleton().getImmediateContext()->IASetInputLayout( GE_BasicRenderer::getSingleton().getInputLayout() );
	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 1, &GE_Core::getSingleton().getRenderStates().SamplerLinearWrap );

	BOOST_FOREACH( GE_Light *light, i_lights )
	{
		float max = light->getIntensity();
		BOOST_FOREACH( GE_Light *lightP, i_lights )
		{
			DirectX::BoundingSphere bs, bs1;
			bs.Center.x = light->getPosition()->x;
			bs.Center.y = light->getPosition()->y;
			bs.Center.z = light->getPosition()->z;
			bs.Radius = light->getRange();

			bs1.Center.x = lightP->getPosition()->x;
			bs1.Center.y = lightP->getPosition()->y;
			bs1.Center.z = lightP->getPosition()->z;
			bs1.Radius = lightP->getRange();
			if( lightP->getIntensity() > max && ( lightP->getType() == GE_Light::Directional || light->getType() == GE_Light::Directional ) )
				max = lightP->getIntensity();
			else if( lightP->getIntensity() > max &&  bs.Intersects( bs1 ) )
				max = lightP->getIntensity();
		}
		maxIntensity.push_back( max );

		for( uint32_t i = 0; ( i < ( uint32_t ) ( light->getType() == GE_Light::Point ? 6 : 1 ) ) && light->getType() != GE_Light::Directional; i++ )
		{
			GE_RenderTarget *DRT = new GE_RenderTarget( rtp );
			m_rtLightDepth.push_back( DRT );
			GE_BasicRenderer::getSingleton().renderToDepth( *DRT, &i_models, &zero_models ,light->getViewProjection( i ) );
		}
	}

	BOOST_FOREACH( GE_Model *model, listLightMap )
	{
		ID3D11ShaderResourceView *srv[ 8 ];
		srv[ 0 ] = NULL;
		srv[ 1 ] = NULL;
		srv[ 2 ] = NULL;
		srv[ 3 ] = NULL;
		srv[ 4 ] = NULL;
		srv[ 5 ] = NULL;
		srv[ 6 ] = NULL;
		srv[ 7 ] = NULL;
		uint32_t subsetCount = model->getNumberOfSubsets();

		for( uint32_t i = 0; i < subsetCount; ++i )
		{
			bool useLightMap = false;
			GE_Model::Subset *subset = model->getSubset( i );

			if( m_overWriteLightMaps )
			{
				if( subset->LightMap )
					GE_TextureManager::getSingleton().remove( &subset->LightMap );
			} else if( subset->LightMap )
				continue;

			m_rt[ 1 ]->clearRenderTargetWithDepth( 0 );
			uint32_t lc = 0;
			uint32_t index = 0;
			BOOST_FOREACH( GE_Light *light, i_lights )
			{
				GE_Light::Parameters *lp = light->getParameters();
				useLightMap = true;

				if( light->getType() == GE_Light::Directional )
				{
					GE_Mat4x4 matView, matProjection, matViewProjection;
					GE_Vec3 pos = model->getPosition();
					if( pos == lp->Direction )
						pos.x += 0.01f;
					if( abs( lp->Direction.z ) == 1.0f )
						matView.lookAtRH( &( pos - lp->Direction * GE_ConvertToUnit( 1000.0f ) ),
						&( pos + lp->Direction * GE_ConvertToUnit( 2000.0f ) ),
						&GE_Vec3( 0, 1.0f, 0 ) );
					else
						matView.lookAtRH( &( pos - lp->Direction * GE_ConvertToUnit( 1000.0f ) ),
						&( pos + lp->Direction * GE_ConvertToUnit( 2000.0f ) ),
						&GE_Vec3( 0, 0, 1.0f ) );

					matProjection.orthoRH( model->getBound().BoundRadius * 2.0f,
						model->getBound().BoundRadius * 2.0f,
						GE_ConvertToUnit( 1.0f ),
						GE_ConvertToUnit( 4000.0f ) );
					GE_Mat4x4Multiply( &matViewProjection, &matView, &matProjection );
					GE_BasicRenderer::getSingleton().renderToDepth( *m_dirDepth, &i_models, &zero_models , &matViewProjection );
					srv[ 2 ] = m_dirDepth->getDepthSRV();
					m_cPerObject.LightViewProjection[ 0 ].transposeBy( &matViewProjection );
				} else
				{
					for( uint32_t i = 0; i < ( uint32_t ) ( light->getType() == GE_Light::Point ? 6 : 1 ); i++ )
					{
						m_cPerObject.LightViewProjection[ i ].transposeBy( light->getViewProjection( i ) );
						srv[ i + 2 ] = m_rtLightDepth[ lc + i ]->getDepthSRV();
					}
				}

				m_rt[ 0 ]->clearRenderTargetWithDepth( 0 );
				m_rt[ 0 ]->setToDevice();

				m_cPerObject.LightAmbient = GE_Vec4( lp->Ambient.x, lp->Ambient.y, lp->Ambient.z, 0.0f );
				memcpy( &m_cPerObject.LightDiffuse, &lp->Diffuse, sizeof( GE_Vec3 ) );
				m_cPerObject.LightDiffuse *= lp->Intensity / maxIntensity[ index ];
				m_cPerObject.LightAttenuation = lp->Attenuation;
				m_cPerObject.LightDirection = GE_Vec4( -lp->Direction.x, -lp->Direction.y, -lp->Direction.z, 0.0f );
				m_cPerObject.LightPosition = GE_Vec4( lp->Position.x, lp->Position.y, lp->Position.z, 0.0f );
				m_cPerObject.LightRange = lp->Range;
				m_cPerObject.LightSpotSharpness = cos( lp->SpotSharpness * 0.5f );
				m_cPerObject.LightSpotSpread = cos( lp->SpotSpread * 0.5f );
				m_cPerObject.ShadowOffsetBias = GE_Vec4( lp->ShadowOffsetBias[ 0 ].x, lp->ShadowOffsetBias[ 0 ].y, 0.0f, 0.0f );

				m_cPerObject.World.transposeBy( model->getWorld() );
				GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perObject, 0, NULL, &m_cPerObject, 0, 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers( 0, 1, &m_perObject );
				GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perObject );

				srv[ 0 ] = subset->Lods[ 0 ]->TexcoordBuffer.m_sRV;
				srv[ 1 ] = m_rt[ 1 ]->getSRV( 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );

				if( lp->LightType == GE_Light::Directional )
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Dir, NULL, 0 );
				else if( lp->LightType == GE_Light::Spot )
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Spot, NULL, 0 );
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Point, NULL, 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources( 0, 8, srv );
				GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 8, srv );

				model->render( i, 0 );

				if( light->getType() != GE_Light::Directional )
					lc += ( uint32_t ) ( light->getType() == GE_Light::Point ? 6 : 1 );

				srv[ 0 ] = NULL;
				srv[ 1 ] = NULL;
				srv[ 2 ] = NULL;
				srv[ 3 ] = NULL;
				srv[ 4 ] = NULL;
				srv[ 5 ] = NULL;
				srv[ 6 ] = NULL;
				srv[ 7 ] = NULL;
				GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources( 0, 8, srv );
				GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 8, srv );
				GE_Core::getSingleton().getImmediateContext()->CopyResource( m_rt[ 1 ]->getRTTexture( 0 ), m_rt[ 0 ]->getRTTexture( 0 ) );
				index++;
			}

			m_rt[ 0 ]->clearRenderTargetWithDepth( 0 );
			m_rt[ 0 ]->setToDevice();
			if( !useLightMap )
				continue;
			GE_PostProcess::getSingleton().begin();
			GE_PostProcess::getSingleton().renderGrowByAlpha( m_rt[ 1 ]->getSRV( 0 ) );
			m_rt[ 1 ]->clearRenderTargetWithDepth( 0 );
			m_rt[ 1 ]->setToDevice();
			GE_PostProcess::getSingleton().renderDown( m_rt[ 0 ]->getSRV( 0 ),
				( uint32_t ) m_rt[ 0 ]->getViewPort()->Width * 10,
				( uint32_t ) m_rt[ 0 ]->getViewPort()->Height * 10
				);
			GE_PostProcess::getSingleton().end();
			m_rt[ 0 ]->setToDevice();

			GE_Core::getSingleton().getImmediateContext()->RSSetState( GE_Core::getSingleton().getRenderStates().RasterStateDisableCull );
			GE_Core::getSingleton().getImmediateContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
			GE_Core::getSingleton().getImmediateContext()->IASetInputLayout( GE_BasicRenderer::getSingleton().getInputLayout() );
			GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 1, &GE_Core::getSingleton().getRenderStates().SamplerLinearWrap );

			GE_Core::getSingleton().getImmediateContext()->GenerateMips( m_rt[ 0 ]->getSRV( 0 ) );

			std::ostringstream buff;
			buff << m_path << "/" << model->getName().c_str() << "_LightMap" << i << ".dds";
			m_rt[ 1 ]->saveRTToFile( buff.str().c_str(), 0 );
			subset->LightMap = GE_TextureManager::getSingleton().createTexture( buff.str().c_str() );
		}
	}
	for( uint32_t i = 0; i < m_rtLightDepth.size(); i++ )
		SAFE_DELETE( m_rtLightDepth[ i ] );
	m_rtLightDepth.clear();

	SAFE_DELETE( m_rt[ 0 ] );
	SAFE_DELETE( m_rt[ 1 ] );
	SAFE_DELETE( m_dirDepth );

	GE_Core::getSingleton().getImmediateContext()->RSSetState( GE_Core::getSingleton().getRenderStates().RasterStateDefault );
	GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
}

GE_LightMapCooker::~GE_LightMapCooker( void )
{
	SAFE_RELEASE( m_pS_Dir );
	SAFE_RELEASE( m_pS_Spot );
	SAFE_RELEASE( m_pS_Point );
	SAFE_RELEASE( m_vS );
	SAFE_RELEASE( m_perObject );
}

std::string & GE_LightMapCooker::getPath()
{
	return m_path;
}

void GE_LightMapCooker::setOverWriteLightMaps( bool i_overWrite )
{
	m_overWriteLightMaps = i_overWrite;
}

void GE_LightMapCooker::setPath( const std::string &i_path )
{
	m_path = i_path;
}

uint32_t & GE_LightMapCooker::getDepthMapSize()
{
	return m_depthSize;
}

uint32_t & GE_LightMapCooker::getLightMapSize()
{
	return m_size;
}

void GE_LightMapCooker::setDepthMapSize( uint32_t i_size )
{
	m_depthSize = i_size;
}

void GE_LightMapCooker::setLightMapSize( uint32_t i_size )
{
	m_size = i_size;
}

GE_LightMapCooker & GE_LightMapCooker::getSingleton()
{
	return m_singleton;
}
