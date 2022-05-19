#include "GEW_Scene.h"
#include "GEW_Model.h"
#include "GEW_Camera.h"
#include "GEW_Light.h"
#include "GEW_Text.h"
#include "GEW_ItemList.h"
#include "GEW_Setting.h"
#include "GEW_Viewport.h"
#include "GEW_PostprocessProperty.h"
#include "GEW_Postprocess.h"
#include "GEW_Flare.h"
#include "GEW_Decal.h"
#include "GEW_Physics.h"
#include "GEW_Terrain.h"
#include "GEW_Screenshot.h"
#include "GEW_Triad.h"
#include "GEW_Prs.h"

#include <GE_Model.h>
#include <GE_Light.h>
#include <GE_Sprite.h>
#include <GE_Lens.h>
#include <GE_BasicRenderer.h>
#include <GE_DeferredGeometry.h>
#include <GE_DeferredPost.h>
#include <GE_PostProcess.h>
#include <GE_Sky.h>
#include <GE_Occluder.h>
#include <GE_Decal.h>
#include <GE_Terrain.h>
#include <GE_StreamMap.h>
#include <boost/foreach.hpp>

GEW_Scene GEW_Scene::m_singleton;

GEW_Scene::GEW_Scene( )
{
	m_camera = new GEW_FreeCamera( );

	m_text = NULL;
	m_counter = 0;
	m_frameRate = 0;
	m_lastRate = 0;
	m_time = 0;
	m_streamMap = NULL;
}

GEW_Scene::~GEW_Scene( )
{
	delete m_streamMap;
	delete m_camera;
	delete m_text;
}

void GEW_Scene::update( GEW_Input::Data &i_inputData )
{
	GE_Core::getSingleton( ).updateTime( );

	if ( !GEW_Setting::getSingleton( ).m_parameters.Render )
		return;

	GEW_Item *root = GEW_ItemList::getSingleton( ).getRoot( );

	m_renderEnvironmentProbList.clear( );
	m_renderModelList.clear( );
	m_renderTwoSideModelList.clear( );
	m_renderTransparentModelList.clear( );
	m_renderTransparentTwoSideModelList.clear( );
	m_renderRouList.clear( );
	m_renderLightList.clear( );
	m_renderSpriteList.clear( );
	m_renderOccluderList.clear( );
	m_shadowModelsList.clear( );
	m_renderDecalList.clear( );

	/*static bool wFlg = false;
	if ( i_inputData.isKeyDown( GUK_W ) && !wFlg )
	{
	wFlg = true;
	if( m_streamMap->isChunkReady( -1 ) )
	m_streamMap->read( -1, 0 );
	}
	else if ( !i_inputData.isKeyDown( GUK_W ) )
	wFlg = false;*/

	/*static int chunk = 0;
	if ( m_streamMap->isChunkReady( -1 ) )
	{
	m_streamMap->read( -1, chunk++ );
	if ( chunk == 10 )
	chunk = 0;
	}*/
	GE_Vec2 widthHeight;
	widthHeight.x = ( float )m_viewport->width( );
	widthHeight.y = ( float )m_viewport->height( );

	tagPOINT cursorPos;
	GetCursorPos( &cursorPos );
	QPoint p;
	p.setX( cursorPos.x );
	p.setY( cursorPos.y );
	QPoint pL = m_viewport->mapFromGlobal( p );

	GE_Vec2 curPos;
	curPos.x = ( float )pL.x( );
	curPos.y = ( float )pL.y( );

	GEW_Terrain::getSingleton( )->update( i_inputData, *m_camera, curPos, widthHeight );
	if ( !GEW_Setting::getSingleton( ).m_parameters.GameMode )
		GEW_Triad::getSingleton( )->update( i_inputData, *m_camera, curPos, widthHeight );

	GEW_Prs::getSingleton().updatePick( i_inputData, *m_camera, curPos, widthHeight );

	if ( GEW_Setting::getSingleton( ).m_parameters.PhysicsReset )
	{
		foreach( GEW_Model *model, GEW_Model::m_list )
		{
			if ( model->isPhysicsEnable( ) )
			{
				model->destroyPhysics( );
				model->createPhysics( );
			}
		}
		GEW_Setting::getSingleton( ).m_parameters.PhysicsReset = false;
	}

	if ( GEW_Setting::getSingleton( ).m_parameters.PhysicsSingleStep )
	{
		GEW_Physics::getSingleton( ).singleStep( GEW_Physics::getSingleton( ).getTimeStep( ) );
		GEW_Setting::getSingleton( ).m_parameters.PhysicsSingleStep = false;
	}
	else if ( GEW_Setting::getSingleton( ).m_parameters.PhysicsSimulation )
		GEW_Physics::getSingleton( ).singleStep( GE_Core::getSingleton( ).getElapsedTime( ) );

	//GEW_VehicleManager::getSingleton().updateInput( *m_inputData );
	//GEW_Physics::getSingleton().singleStep( GE_Core::getSingleton().getElapsedTime() );
	//if( !GEW_VehicleManager::getSingleton().isValid() )
	//	m_camera->update( *m_inputData );
	//GEW_VehicleManager::getSingleton().update();
	m_camera->update( i_inputData );

	//update hierarchy
	root->update( );

	//GEW_VehicleManager::getSingleton().updateView( m_camera );
	//if( GEW_Painter::getSingleton().m_ruoSize.isVisible() )
	//{
	//	m_renderRouList.push_back( &GEW_Painter::getSingleton().m_ruoSize );
	//	m_renderRouList.push_back( &GEW_Painter::getSingleton().m_ruoFalloff );
	//}

	foreach( GEW_Model *gewModel, GEW_Model::m_list )
	{
		//check model is in view
		if (
			gewModel->m_model->isVisible( ) &&
			gewModel->m_model->getVisibleDistance( ) >= ( gewModel->m_model->getBound( ).BoundCenter - m_camera->m_position ).length( ) &&
			m_camera->m_frustum.sphereInFrustum( &gewModel->m_model->getBound( ).BoundCenter, gewModel->m_model->getBound( ).BoundRadius ) != GE_Frustum::OutSide
			)
		{
			if ( !GEW_Setting::getSingleton( ).m_parameters.GameMode )
				gewModel->getRenderUtilityObjects( m_renderRouList );
			if ( gewModel->isTransparent( ) )
			{
				if ( gewModel->isTwoSideShade( ) )
					m_renderTransparentTwoSideModelList.push_back( gewModel->m_model );
				else
					m_renderTransparentModelList.push_back( gewModel->m_model );
			}
			else
			{
				if ( gewModel->isTwoSideShade( ) )
					m_renderTwoSideModelList.push_back( gewModel->m_model );
				else
					m_renderModelList.push_back( gewModel->m_model );
			}

			if ( GEW_Setting::getSingleton( ).m_parameters.GameMode )
			{
				float distance = ( gewModel->m_model->getBound( ).BoundCenter - m_camera->m_position ).length( );
				for ( uint32_t i = 0; i < gewModel->m_model->getNumberOfLod( ); ++i )
				{
					if ( distance <= gewModel->m_model->getLodDistance( i ) )
					{
						gewModel->m_model->setLod( i );
						break;
					}
				}
			}
		}

	}

	/*foreach( GEW_EnvironmentProbe *prob, GEW_EnvironmentProbe::m_list )
	{
	if( prob->m_probe->isVisible() &&
	m_camera.m_frustum.sphereInFrustum( &prob->m_probe->getPosition(), GE_ConvertToUnit( 0.5f ) ) != GE_Frustum::OutSide )
	{
	m_renderEnvironmentProbList.push_back( prob->m_probe );
	}
	}*/

	foreach( GEW_Light *gewLight, GEW_Light::m_list )
	{
		//check light is in view
		if ( !GEW_Setting::getSingleton( ).m_parameters.GameMode )
			gewLight->getRenderUtilityObjects( m_renderRouList );

		if ( gewLight->m_light->iSEnable( ) &&
			( m_camera->m_frustum.sphereInFrustum( gewLight->m_light->getPosition( ), gewLight->m_light->getRange( ) ) != GE_Frustum::OutSide ||
			gewLight->m_light->getType( ) == GE_Light::Directional )
			)
		{
			m_sunLight = gewLight->m_light;
			if ( gewLight->m_light->hasShadow( ) )
			{
				gewLight->m_light->setLinkToView( m_camera );

				m_shadowModelsList.push_back( ShadowModels( ) );
				auto shadowMdl = m_shadowModelsList.end( );
				shadowMdl--;
				( *shadowMdl ).Light = gewLight->m_light;

				GE_Frustum frustum;
				for ( uint32_t i = 0; i < ( ( gewLight->m_light->getType( ) == GE_Light::Point ) ? LIGHT_PARAM_SIZE : gewLight->m_light->getNumberOfCascade( ) ); ++i )
				{
					list<GE_Model *> *modelList = new list<GE_Model *>( );
					list<GE_Model *> *modelListTwoSide = new list<GE_Model *>( );
					( *shadowMdl ).ModelLists.push_back( modelList );
					( *shadowMdl ).TwoSideModelLists.push_back( modelListTwoSide );
					frustum.calculatePlanes( gewLight->m_light->getViewProjection( i ) );
					foreach( GEW_Model *gewModel, GEW_Model::m_list )
					{
						if ( gewModel->isShadowCaster( ) && gewModel->m_model->isVisible( ) && frustum.sphereInFrustum( &gewModel->m_model->getBound( ).BoundCenter,
							gewModel->m_model->getBound( ).BoundRadius ) != GE_Frustum::OutSide )
						{
							if ( gewModel->isTwoSideDepth( ) )
								modelListTwoSide->push_back( gewModel->m_model );
							else
								modelList->push_back( gewModel->m_model );

						}

					}
				}
			}

			m_renderLightList.push_back( gewLight->m_light );
		}
	}
	foreach( GEW_Flare *gewFlare, GEW_Flare::m_list )
	{
		if ( gewFlare->m_enable )
		{
			gewFlare->getRenderUtilityObjects( m_renderRouList );
			if ( gewFlare->m_flare->getSprite( )->isVisible( ) )
				m_renderSpriteList.push_back( gewFlare->m_flare->getSprite( ) );
			m_renderOccluderList.push_back( gewFlare->m_flare->getOccluder( ) );
		}
	}
	foreach( GEW_Decal *gewDecal, GEW_Decal::m_list )
	{
		if ( gewDecal->m_enable )
		{
			gewDecal->getRenderUtilityObjects( m_renderRouList );
			m_renderDecalList.push_back( gewDecal->m_decal );
		}
	}
	GEW_Terrain::getSingleton( )->getRenderUtilityObjects( m_renderRouList );
	GEW_Terrain::getSingleton( )->getDecalObjects( m_renderDecalList );
	if ( GEW_Setting::getSingleton( ).m_parameters.TextVisible && m_text && m_text->isVisible( ) )
	{
		m_counter++;
		m_lastRate += GE_Core::getSingleton( ).getFrameRate( );
		m_time += GE_Core::getSingleton( ).getElapsedTime( );
		if ( m_time > 0.2f )
		{
			m_frameRate = m_lastRate / m_counter;
			m_counter = 0;
			m_lastRate = 0;
			m_time = 0;
		}

		std::ostringstream buff;
		buff << "FPS: " << ( uint32_t )round( m_frameRate )
			<< "\nPolys: " << GE_Core::getSingleton( ).getPolygonCount( )
			<< "\nDraw calls: " << GE_Core::getSingleton( ).getDrawCallsCount( )
			<< "\nCamera pos X:" << GE_ConvertToMeter( m_camera->m_position.x )
			<< "\nCamera pos Y:" << GE_ConvertToMeter( m_camera->m_position.y )
			<< "\nCamera pos Z:" << GE_ConvertToMeter( m_camera->m_position.z )
			<< "\nCamera rot yaw:" << GE_ConvertToDegrees( m_camera->Yaw )
			<< "\nCamera rot pitch:" << GE_ConvertToDegrees( m_camera->Pitch );
		if ( GEW_Terrain::getSingleton( )->m_baseData.Enable )
		{
			buff << "\nTerrain visited nodes:" << GE_Terrain::getSingleton( ).getNumVisit( )
				<< "\nTerrain render nodes:" << GE_Terrain::getSingleton( ).getNumOut( );
		}
		GE_Core::getSingleton( ).clearDrawCallAndPolygonCount( );
		m_text->setText( buff.str( ).data( ) );
		m_text->update( );
		m_renderSpriteList.push_back( m_text->getSprite( ) );
	}

	GEW_Postprocess::getSingleton( )->update( );
}

void GEW_Scene::render( )
{

	if ( !GEW_Setting::getSingleton( ).m_parameters.Render )
		return;

	BOOST_FOREACH( ShadowModels &shdMdl, m_shadowModelsList )
	{
		GE_BasicRenderer::getSingleton( ).renderToDepth( shdMdl.Light, &shdMdl.ModelLists, &shdMdl.TwoSideModelLists );
	}

	GE_SwapChain *swapChain = GE_Core::getSingleton( ).getCurrentSwapchain( );

	GE_DeferredGeometry::getSingleton( ).begin( );
	if ( GEW_Terrain::getSingleton( )->m_baseData.Enable )
		GE_DeferredGeometry::getSingleton( ).render( &m_renderModelList, &m_renderTwoSideModelList, &GE_Terrain::getSingleton( ), m_camera );
	else
		GE_DeferredGeometry::getSingleton( ).render( &m_renderModelList, &m_renderTwoSideModelList, NULL, m_camera );
	GE_DeferredGeometry::getSingleton( ).copyRenderTarget( );
	GE_DeferredGeometry::getSingleton( ).end( );

	GE_DecalRenderer::getSingleton( ).render( &m_renderDecalList, m_camera );

	GE_Core::getSingleton( ).setClearSwapChain( *swapChain );

	GE_PostProcess::getSingleton( ).beginHDR( );

	if ( GEW_Postprocess::getSingleton( )->m_sky.Enable )
		GE_Sky::getSingleton( ).render( m_camera );

	GE_DeferredPost::getSingleton( ).begin( );

	auto it = m_renderLightList.begin( );
	for ( uint32_t i = 0; i < m_renderLightList.size( ); ++i )
	{
		GE_DeferredPost::getSingleton( ).render( m_camera, ( *it ) );
		it++;
	}

	GE_DeferredPost::getSingleton( ).end( );

	if ( m_sunLight )
	{
		swapChain->copyBackBufferToSRV( );
		GE_DeferredGeometry::getSingleton( ).renderTransparent( &m_renderTransparentModelList,
			&m_renderTransparentTwoSideModelList, m_camera, m_sunLight, swapChain->RenderTargetViewSRV, GE_PostProcess::getSingleton( ).isEnableHDR( ) );
	}

	GE_PostProcess::getSingleton( ).endHDR( );

	GE_PostProcess::getSingleton( ).begin( );

	if ( GEW_Postprocess::getSingleton( )->m_aO.Enable )
	{
		swapChain->copyBackBufferToSRV( );
		GE_PostProcess::getSingleton( ).renderSSAO( swapChain->RenderTargetViewSRV,
			GE_DeferredGeometry::getSingleton( ).getTargetSRV( GE_DeferredGeometry::Depth ),
			GE_DeferredGeometry::getSingleton( ).getTargetSRV( GE_DeferredGeometry::NormalSpecular ),
			m_camera->m_matView, m_camera->m_matViewProjectionInv, ( uint32_t )swapChain->ViewPort.Width, ( uint32_t )swapChain->ViewPort.Height,
			GEW_Postprocess::getSingleton( )->m_aO.Radius, GEW_Postprocess::getSingleton( )->m_aO.Depth, GEW_Postprocess::getSingleton( )->m_aO.Normal,
			GEW_Postprocess::getSingleton( )->m_aO.Intensity, GEW_Postprocess::getSingleton( )->m_aO.DistanceStart, GEW_Postprocess::getSingleton( )->m_aO.DistanceEnd );
	}

	if ( GEW_Postprocess::getSingleton( )->m_lightScatter.Enable && GEW_Postprocess::getSingleton( )->m_skyLight )
	{
		swapChain->copyBackBufferToSRV( );
		GE_Vec4 l2DPos;
		l2DPos.To2DLeftUpPosition( GEW_Postprocess::getSingleton( )->m_skyLight->getPosition( ), m_camera->m_matViewProjection );

		GE_PostProcess::getSingleton( ).renderLightScatter(
			GE_Core::getSingleton( ).getCurrentSwapchain( )->RenderTargetViewSRV,
			GE_Core::getSingleton( ).getCurrentSwapchain( )->ViewPort.Width,
			GE_Core::getSingleton( ).getCurrentSwapchain( )->ViewPort.Height,
			l2DPos,
			GEW_Postprocess::getSingleton( )->m_lightScatter.Density,
			GEW_Postprocess::getSingleton( )->m_lightScatter.Decay,
			GEW_Postprocess::getSingleton( )->m_lightScatter.Weight,
			GEW_Postprocess::getSingleton( )->m_lightScatter.Exp,
			GEW_Postprocess::getSingleton( )->m_lightScatter.Luminance,
			GEW_Postprocess::getSingleton( )->m_lightScatter.Cutoff );
	}

	if ( GEW_Postprocess::getSingleton( )->m_atmosphereFog.Enable )
	{
		swapChain->copyBackBufferToSRV( );
		GE_PostProcess::getSingleton( ).renderAtmosphereFog( swapChain->RenderTargetViewSRV,
			GE_DeferredGeometry::getSingleton( ).getTargetSRV( GE_DeferredGeometry::Depth ),
			m_camera->m_matView, m_camera->m_matViewProjectionInv,
			GEW_Postprocess::getSingleton( )->m_atmosphereFog.Depth_PositionFieldFalloffHardness,
			GEW_Postprocess::getSingleton( )->m_atmosphereFog.Height_PositionFieldFalloffHardness,
			GEW_Postprocess::getSingleton( )->m_atmosphereFog.Color,
			GEW_Postprocess::getSingleton( )->m_atmosphereFog.Intensity );
	}

	if ( GEW_Postprocess::getSingleton( )->m_glowMapped.Enable )
	{
		swapChain->copyBackBufferToSRV( );
		GE_PostProcess::getSingleton( ).renderGlowMask( swapChain->RenderTargetViewSRV, 
			GE_DeferredGeometry::getSingleton( ).getTargetSRV( GE_DeferredGeometry::DiffuseGlow ),
			( uint32_t )swapChain->ViewPort.Width, ( uint32_t )swapChain->ViewPort.Height,
			GEW_Postprocess::getSingleton( )->m_glowMapped.HScale, GEW_Postprocess::getSingleton( )->m_glowMapped.VScale,
			GEW_Postprocess::getSingleton( )->m_glowMapped.Luminance, GEW_Postprocess::getSingleton( )->m_glowMapped.Cutoff );
	}

	if ( GEW_Postprocess::getSingleton( )->m_glow.Enable )
	{
		swapChain->copyBackBufferToSRV( );
		GE_PostProcess::getSingleton( ).renderGlow( swapChain->RenderTargetViewSRV, ( uint32_t )swapChain->ViewPort.Width, 
			( uint32_t )swapChain->ViewPort.Height,
			GEW_Postprocess::getSingleton( )->m_glow.HScale, GEW_Postprocess::getSingleton( )->m_glow.VScale, 
			GEW_Postprocess::getSingleton( )->m_glow.Luminance, GEW_Postprocess::getSingleton( )->m_glow.Cutoff );
	}

	if ( GEW_Postprocess::getSingleton( )->m_lensDirt.Enable && GEW_Postprocess::getSingleton( )->m_dirtMap )
	{
		swapChain->copyBackBufferToSRV( );
		GE_PostProcess::getSingleton( ).renderLensDirt(
			GE_Core::getSingleton( ).getCurrentSwapchain( )->RenderTargetViewSRV,
			GEW_Postprocess::getSingleton( )->m_dirtMap->getTextureSRV( ),
			GE_Core::getSingleton( ).getCurrentSwapchain( )->ViewPort.Width,
			GE_Core::getSingleton( ).getCurrentSwapchain( )->ViewPort.Height,
			GEW_Postprocess::getSingleton( )->m_lensDirt.Luminance,
			GEW_Postprocess::getSingleton( )->m_lensDirt.Cutoff,
			GEW_Postprocess::getSingleton( )->m_lensDirt.Exp,
			GEW_Postprocess::getSingleton( )->m_lensDirt.CoreExp, GEW_Postprocess::getSingleton( )->m_lensDirt.DirtExp );
	}

	static GE_Mat4x4 preViewProjection;
	if ( GEW_Postprocess::getSingleton( )->m_motionBlur.Enable )
	{
		swapChain->copyBackBufferToSRV( );
		GE_PostProcess::getSingleton( ).renderMotionBlur( swapChain->RenderTargetViewSRV,
			GE_DeferredGeometry::getSingleton( ).getTargetSRV( GE_DeferredGeometry::Depth ),
			m_camera->m_matViewProjectionInv, preViewProjection, GEW_Postprocess::getSingleton( )->m_motionBlur.Damp,
				GEW_Postprocess::getSingleton( )->m_motionBlur.Max );
	}
	preViewProjection = m_camera->m_matViewProjection;

	if ( GEW_Setting::getSingleton( ).m_parameters.FXAAQuality )
	{
		swapChain->copyBackBufferToSRV( );
		GE_PostProcess::getSingleton( ).renderFXAA( swapChain->RenderTargetViewSRV, ( uint32_t )swapChain->ViewPort.Width,
			( uint32_t )swapChain->ViewPort.Height, GEW_Setting::getSingleton( ).m_parameters.FXAAQuality + 1 );
	}


	GE_PostProcess::getSingleton( ).end( );


	//GE_BasicRenderer::getSingleton().render( &m_renderModelList, &m_camera->m_matViewProjection );

	//swapChain->copyDepthToSRV();

	if ( !GEW_Setting::getSingleton( ).m_parameters.GameMode )
		GE_RenderUtility::getSingleton( ).render( &m_renderRouList, m_camera,
			GE_DeferredGeometry::getSingleton( ).getTargetSRV( GE_DeferredGeometry::Depth ) );

	GE_OccluderTester::getSingleton( ).begin( GE_DeferredGeometry::getSingleton( ).getRenderTarget( ) );
	GE_OccluderTester::getSingleton( ).test( m_renderOccluderList, m_camera );
	GE_OccluderTester::getSingleton( ).end( );

	GE_SpriteRenderer::getSingleton( ).begin( );
	GE_SpriteRenderer::getSingleton( ).render( &m_renderSpriteList );
	GE_SpriteRenderer::getSingleton( ).end( );

	if ( !GEW_Setting::getSingleton( ).m_parameters.GameMode )
		GEW_Triad::getSingleton( )->render( );

	GE_Core::getSingleton( ).getCurrentSwapchain( )->SwapChain->Present( GEW_Setting::getSingleton( ).m_parameters.SyncLevel, 0 );

	GEW_Screenshot::getSingleton( )->update( m_viewport, this );
}

void GEW_Scene::init( )
{
	GE_Core::getSingleton( ).createRenderStates( 0 );
	GE_OccluderTester::getSingleton( ).init( );
	GE_SpriteRenderer::getSingleton( ).init( );
	GE_BasicRenderer::getSingleton( ).init( );
	GE_RenderUtility::getSingleton( ).init( );
	GE_DeferredGeometry::getSingleton( ).init( );
	GE_DeferredPost::getSingleton( ).init( );
	GE_PostProcess::getSingleton( ).init( );
	GE_Sky::getSingleton( ).init( );
	GE_DecalRenderer::getSingleton( ).init( );
	GE_Terrain::getSingleton( ).init( );
	GEW_Terrain::getSingleton( )->init( );
	GEW_PostprocessProperty::getSingleton( )->init( );
	GEW_Triad::getSingleton( )->init( );
	m_text = new GEW_Text( std::string( "../../GData/Texture/Glyph_Tahoma.dds" ) );
	m_text->setDiffuse( GE_Vec4( 1.0f, 1.0f, 0.0f, 1.0f ) );
	m_text->setScale( GE_Vec2( 1.0f, 1.0f ) );
	m_sunLight = NULL;
}

void GEW_Scene::resize( int i_width, int i_height )
{
	m_camera->m_aspect = ( float )i_width / i_height;
	GE_DeferredGeometry::getSingleton( ).resize( );
	GE_PostProcess::getSingleton( ).resize( );
	GE_Vec2 wh;
	wh.x = i_width;
	wh.y = i_height;
	m_text->setPosition( GE_Vec2( 5.0f - wh.x * 0.5f, wh.y * 0.5f - 8.0f ) );
}

void GEW_Scene::setViewPort( GEW_Viewport *i_viewport )
{
	m_viewport = i_viewport;
}
