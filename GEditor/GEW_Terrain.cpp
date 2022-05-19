#include "GEW_Terrain.h"
#include <GE_Terrain.h>
#include <GE_Texture.h>
#include <GE_Core.h>
#include <GE_View.h>
#include <GE_RenderUtility.h>
#include <GE_RenderTarget.h>
#include <GE_Decal.h>
#include <QDir>
#include <QFile>
#include <QString>
#include <QMessageBox>
#include "GEW_Path.h"
#include "GEW_Utility.h"
#include <vector>
using namespace physx;

GEW_Terrain::GEW_Terrain()
{
	m_maskMapRT[ 0 ] = NULL;
	m_heightMapRT[ 0 ] = NULL;
	m_maskMapRT[ 1 ] = NULL;
	m_heightMapRT[ 1 ] = NULL;
	m_brushDecalMapRT = NULL;
	m_brushDecal = NULL;
	m_vS = NULL;
	m_pS_HeightCopy = NULL;
	m_pS_HeightSculptPositive = NULL;
	m_pS_HeightSculptNegative = NULL;
	m_pS_HeightSculptPositiveMask = NULL;
	m_pS_HeightSculptNegativeMask = NULL;
	m_pS_HeightSculptSmooth = NULL;
	m_pS_HeightSculptSmoothMask = NULL;
	m_pS_HeightSculptFlat = NULL;
	m_pS_MaskCopy = NULL;
	m_pS_MaskSculptPositive = NULL;
	m_pS_MaskSculptNegative = NULL;
	m_pS_MaskSculptPositiveMask = NULL;
	m_pS_MaskSculptNegativeMask = NULL;
	m_pS_MaskSculptSmooth = NULL;
	m_pS_MaskSculptSmoothMask = NULL;
	m_pS_MaskSculptFlat = NULL;
	m_pS_BrushDecal = NULL;
	m_horizonRUO = NULL;
	m_brushRUO = NULL;
	m_perFrameBrushDecal = NULL;
	m_perFrameBrushHeight = NULL;
	m_lastHit.y = m_lastHit.x = D3D11_FLOAT32_MAX;
	m_heightFieldActor = NULL;
}


GEW_Terrain::~GEW_Terrain()
{
	SAFE_RELEASE( m_vS );
	SAFE_RELEASE( m_pS_HeightCopy );
	SAFE_RELEASE( m_pS_HeightSculptPositive );
	SAFE_RELEASE( m_pS_HeightSculptNegative );
	SAFE_RELEASE( m_pS_HeightSculptPositiveMask );
	SAFE_RELEASE( m_pS_HeightSculptNegativeMask );
	SAFE_RELEASE( m_pS_HeightSculptSmooth );
	SAFE_RELEASE( m_pS_HeightSculptSmoothMask );
	SAFE_RELEASE( m_pS_HeightSculptFlat );
	SAFE_RELEASE( m_pS_MaskCopy );
	SAFE_RELEASE( m_pS_MaskSculptPositive );
	SAFE_RELEASE( m_pS_MaskSculptNegative );
	SAFE_RELEASE( m_pS_MaskSculptPositiveMask );
	SAFE_RELEASE( m_pS_MaskSculptNegativeMask );
	SAFE_RELEASE( m_pS_MaskSculptSmooth );
	SAFE_RELEASE( m_pS_MaskSculptSmoothMask );
	SAFE_RELEASE( m_pS_MaskSculptFlat );
	SAFE_RELEASE( m_pS_BrushDecal );
	SAFE_RELEASE( m_perFrameBrushDecal );
	SAFE_RELEASE( m_perFrameBrushHeight );
	SAFE_DELETE( m_brushDecalMapRT );
	SAFE_DELETE( m_heightMapRT[ 0 ] );
	SAFE_DELETE( m_heightMapRT[ 1 ] );
	SAFE_DELETE( m_maskMapRT[ 0 ] );
	SAFE_DELETE( m_maskMapRT[ 1 ] );
	SAFE_DELETE( m_horizonRUO );
	SAFE_DELETE( m_brushRUO );
	SAFE_DELETE( m_brushDecal );

}

void GEW_Terrain::setHeightMap()
{
	if ( m_heightMapRT[ 0 ] )
		GE_Terrain::getSingleton().setHeightMap( m_heightMapRT[ 0 ]->getSRV( 0 ) );
	else
		GE_Terrain::getSingleton().setHeightMap( NULL );
}

void GEW_Terrain::setMaskMap()
{
	if ( m_maskMapRT[ 0 ] )
		GE_Terrain::getSingleton().setMaskMap( m_maskMapRT[ 0 ]->getSRV( 0 ) );
	else
		GE_Terrain::getSingleton().setMaskMap( NULL );
}

void GEW_Terrain::init()
{
	if ( m_vS )
		return;
	m_vS = GE_Core::getSingleton().createVS( string( GEW_SHADER_PATH ) + "GEW_T_VS.cso" );
	m_pS_HeightCopy = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_HeightCopy.cso" );
	m_pS_HeightSculptPositive = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_HeightSculptPositive.cso" );
	m_pS_HeightSculptNegative = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_HeightSculptNegative.cso" );
	m_pS_HeightSculptPositiveMask = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_HeightSculptPositiveMask.cso" );
	m_pS_HeightSculptNegativeMask = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_HeightSculptNegativeMask.cso" );
	m_pS_HeightSculptSmooth = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_HeightSculptSmooth.cso" );
	m_pS_HeightSculptSmoothMask = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_HeightSculptSmoothMask.cso" );
	m_pS_HeightSculptFlat = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_HeightSculptFlat.cso" );
	m_pS_MaskCopy = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_MaskCopy.cso" );
	m_pS_MaskSculptPositive = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_MaskSculptPositive.cso" );
	m_pS_MaskSculptNegative = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_MaskSculptNegative.cso" );
	m_pS_MaskSculptPositiveMask = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_MaskSculptPositiveMask.cso" );
	m_pS_MaskSculptNegativeMask = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_MaskSculptNegativeMask.cso" );
	m_pS_MaskSculptSmooth = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_MaskSculptSmooth.cso" );
	m_pS_MaskSculptSmoothMask = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_MaskSculptSmoothMask.cso" );
	m_pS_MaskSculptFlat = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_MaskSculptFlat.cso" );
	m_pS_BrushDecal = GE_Core::getSingleton().createPS( string( GEW_SHADER_PATH ) + "GEW_T_PS_BrushDecal.cso" );
	m_perFrameBrushDecal = GE_Core::getSingleton().createConstBuffer( sizeof( PerFrameBrushDecal ) );
	m_perFrameBrushHeight = GE_Core::getSingleton().createConstBuffer( sizeof( PerFrameBrushHeight ) );
	m_baseData.set();
	m_baseParam.set();

	m_brushDecal = new GE_Decal();
	m_brushDecal->setNumberOfDecal( 1 );
	m_brushDecal->setDiffuseTexture( ( string( GEW_TEXTURE_PATH ) + "TerrainDecal.dds" ).c_str() );
	m_horizonRUO = new GE_RenderUtilityObject();
	m_horizonRUO->setColor( GE_Vec3( 1.0f, 1.0f, 0.0f ) );
	m_horizonRUO->setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	vector<GE_Vec3> *vb = m_horizonRUO->verticesDataBeginEdit();
	vb->resize( 4 );
	vb->data()[ 0 ] = GE_Vec3( -0.5f, -0.5f, 0.0f );
	vb->data()[ 1 ] = GE_Vec3( 0.5f, -0.5f, 0.0f );
	vb->data()[ 2 ] = GE_Vec3( -0.5f, 0.5f, 0.0f );
	vb->data()[ 3 ] = GE_Vec3( 0.5f, 0.5f, 0.0f );
	m_horizonRUO->verticesDataEndEdit();
	vector< int > *ib = m_horizonRUO->indicesDataBeginEdit();
	ib->resize( 6 );
	ib->data()[ 0 ] = 0;
	ib->data()[ 1 ] = 1;
	ib->data()[ 2 ] = 3;
	ib->data()[ 3 ] = 2;
	ib->data()[ 4 ] = 0;
	ib->data()[ 5 ] = 3;
	m_horizonRUO->indicesDataEndEdit();
	m_horizonRUO->setDepthDisable( false );
	m_horizonRUO->setVisible( false );

	m_brushRUO = new GE_RenderUtilityObject();
	m_brushRUO->setColor( GE_Vec3( 1.0f, 1.0f, 0.0f ) );
	m_brushRUO->setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );
	vb = m_brushRUO->verticesDataBeginEdit();
	vb->resize( 2 );
	vb->data()[ 0 ] = GE_Vec3( 0.0f, 0.0f, 0.0f );
	vb->data()[ 1 ] = GE_Vec3( 0.0f, 0.0f, 1.0f );
	m_brushRUO->verticesDataEndEdit();
	m_brushRUO->setDepthDisable( false );
	m_brushRUO->setVisible( true );
}

void GEW_Terrain::setSculptEnableChange()
{
	if ( m_sculptData.Enable )
		m_heightMaskMap = GE_TextureManager::getSingleton().createTexture( m_sculptCurrentParam.MaskFileName );
	else
		GE_TextureManager::getSingleton().remove( &m_heightMaskMap );
	if ( m_heightMapRT[ 0 ] )
	{
		SAFE_DELETE( m_heightMapRT[ 1 ] );
		if ( m_baseData.Enable )
		{
			uint32_t size = ( uint32_t )m_heightMapRT[ 0 ]->getViewPort()->Width;
			GE_RenderTarget::Parameters rtp;
			D3D11_TEXTURE2D_DESC desc;
			m_heightMapRT[ 0 ]->getRenderTargetTextureDescriptor( 0, desc );
			rtp.Format[ 0 ] = desc.Format;
			rtp.Width[ 0 ] = rtp.Height[ 0 ] = size;
			//rtp.MipLevel[0] = log(size) / log(2);
			rtp.Count = 1;
			rtp.HasDepth = false;
			m_heightMapRT[ 1 ] = new GE_RenderTarget( rtp );
		}
	}
	if ( m_maskMapRT[ 0 ] )
	{
		SAFE_DELETE( m_maskMapRT[ 1 ] );
		if ( m_baseData.Enable )
		{
			uint32_t size = ( uint32_t )m_maskMapRT[ 0 ]->getViewPort()->Width;
			GE_RenderTarget::Parameters rtp;
			D3D11_TEXTURE2D_DESC desc;
			m_maskMapRT[ 0 ]->getRenderTargetTextureDescriptor( 0, desc );
			rtp.Format[ 0 ] = desc.Format;
			rtp.Width[ 0 ] = rtp.Height[ 0 ] = size;
			//rtp.MipLevel[0] = log(size) / log(2);
			rtp.Count = 1;
			rtp.HasDepth = false;
			m_maskMapRT[ 1 ] = new GE_RenderTarget( rtp );
		}
	}
}

void GEW_Terrain::createHeightMap( QString i_filename, uint32_t i_size, bool i_copyFromLast, bool i_justCopy )
{
	GE_Texture *tempTexture = NULL;
	if ( i_justCopy )
	{
		tempTexture = GE_TextureManager::getSingleton().createTexture( i_filename.toStdString().c_str() );
		i_size = tempTexture->getWidth();
	}
	GE_RenderTarget::Parameters rtp;
	rtp.Format[ 0 ] = DXGI_FORMAT_R16_FLOAT;
	rtp.Width[ 0 ] = rtp.Height[ 0 ] = i_size;
	//rtp.MipLevel[0] = log(i_size) / log(2);
	rtp.Count = 1;
	rtp.HasDepth = false;
	SAFE_DELETE( m_heightMapRT[ 0 ] );
	m_heightMapRT[ 0 ] = new GE_RenderTarget( rtp );
	m_heightMapRT[ 0 ]->clearRenderTarget();
	if ( i_copyFromLast || i_justCopy )
	{
		if ( !tempTexture )
			tempTexture = GE_TextureManager::getSingleton().createTexture( m_baseParam.HeightMapFileName );
		m_heightMapRT[ 0 ]->setToDevice();
		GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );
		GE_Core::getSingleton().setScreenAlignQuadInputLayoutToContext();
		GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );
		GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_HeightCopy, NULL, 0 );
		ID3D11SamplerState *sam[ 3 ];
		sam[ 0 ] = GE_Core::getSingleton().getRenderStates().SamplerPointClamp;
		sam[ 1 ] = GE_Core::getSingleton().getRenderStates().SamplerLinearClamp;
		sam[ 2 ] = GE_Core::getSingleton().getRenderStates().SamplerLinearBorder;
		GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 3, sam );

		ID3D11ShaderResourceView *srv[ 1 ];
		if ( tempTexture )
			srv[ 0 ] = tempTexture->getTextureSRV();
		else
			srv[ 0 ] = NULL;
		GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );

		GE_Core::getSingleton().renderScreenAlignQuad();

		srv[ 0 ] = NULL;
		GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );

		GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
	}
	//GE_Core::getSingleton().getImmediateContext()->GenerateMips(m_heightMapRT[0]->getSRV(0));
	strcpy_s( m_baseParam.HeightMapFileName, i_filename.toStdString().c_str() );
	if ( !i_justCopy )
		m_heightMapRT[ 0 ]->saveRTToFile( i_filename.toStdString().c_str(), 0 );
	GE_TextureManager::getSingleton().remove( &tempTexture );
}

void GEW_Terrain::createMaskMap( QString i_filename, uint32_t i_size, bool i_copyFromLast, bool i_justCopy )
{
	GE_Texture *tempTexture = NULL;
	if ( i_justCopy )
	{
		tempTexture = GE_TextureManager::getSingleton().createTexture( i_filename.toStdString().c_str() );
		i_size = tempTexture->getWidth();
	}
	GE_RenderTarget::Parameters rtp;
	rtp.Format[ 0 ] = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtp.Width[ 0 ] = rtp.Height[ 0 ] = i_size;
	//rtp.MipLevel[0] = log(i_size) / log(2);
	rtp.Count = 1;
	rtp.HasDepth = false;
	SAFE_DELETE( m_maskMapRT[ 0 ] );
	m_maskMapRT[ 0 ] = new GE_RenderTarget( rtp );
	m_maskMapRT[ 0 ]->clearRenderTarget();
	if ( i_copyFromLast || i_justCopy )
	{
		if ( !tempTexture )
			tempTexture = GE_TextureManager::getSingleton().createTexture( m_baseParam.MaskMapFileName );
		m_maskMapRT[ 0 ]->setToDevice();
		GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );
		GE_Core::getSingleton().setScreenAlignQuadInputLayoutToContext();
		GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );
		GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MaskCopy, NULL, 0 );
		ID3D11SamplerState *sam[ 3 ];
		sam[ 0 ] = GE_Core::getSingleton().getRenderStates().SamplerPointClamp;
		sam[ 1 ] = GE_Core::getSingleton().getRenderStates().SamplerLinearClamp;
		sam[ 2 ] = GE_Core::getSingleton().getRenderStates().SamplerLinearBorder;
		GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 3, sam );

		ID3D11ShaderResourceView *srv[ 1 ];
		if ( tempTexture )
			srv[ 0 ] = tempTexture->getTextureSRV();
		else
			srv[ 0 ] = NULL;
		GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );

		GE_Core::getSingleton().renderScreenAlignQuad();

		srv[ 0 ] = NULL;
		GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );

		GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
	}
	//GE_Core::getSingleton().getImmediateContext()->GenerateMips(m_maskMapRT[0]->getSRV(0));
	strcpy_s( m_baseParam.MaskMapFileName, i_filename.toStdString().c_str() );
	if ( !i_justCopy )
		m_maskMapRT[ 0 ]->saveRTToFile( i_filename.toStdString().c_str(), 0 );
	GE_TextureManager::getSingleton().remove( &tempTexture );
}

void GEW_Terrain::updateBrushMap()
{
	if ( !m_baseData.Enable || !m_baseData.Enable )
	{
		SAFE_DELETE( m_brushDecalMapRT );
		return;
	}

	if ( !m_brushDecalMapRT )
	{
		GE_RenderTarget::Parameters rtp;
		rtp.Format[ 0 ] = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtp.Width[ 0 ] = rtp.Height[ 0 ] = 512.0f;
		rtp.Count = 1;
		rtp.MipLevel[ 0 ] = log( rtp.Height[ 0 ] ) / log( 2 );
		rtp.HasDepth = false;
		m_brushDecalMapRT = new GE_RenderTarget( rtp );
	}

	m_brushDecalMapRT->setToDevice();
	m_cPerFrameBrushDecal.DecalFalloff = m_sculptCurrentParam.Falloff;
	m_cPerFrameBrushDecal.DecalHardness = m_sculptCurrentParam.Hardness;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrameBrushDecal, 0, NULL, &m_cPerFrameBrushDecal, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrameBrushDecal );

	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );
	GE_Core::getSingleton().setScreenAlignQuadInputLayoutToContext();
	GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_BrushDecal, NULL, 0 );
	ID3D11SamplerState *sam[ 3 ];
	sam[ 0 ] = GE_Core::getSingleton().getRenderStates().SamplerPointClamp;
	sam[ 1 ] = GE_Core::getSingleton().getRenderStates().SamplerLinearClamp;
	sam[ 2 ] = GE_Core::getSingleton().getRenderStates().SamplerLinearBorder;
	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 3, sam );

	GE_Core::getSingleton().renderScreenAlignQuad();

	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );
	GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
	GE_Core::getSingleton().getImmediateContext()->GenerateMips( m_brushDecalMapRT->getSRV( 0 ) );
	m_brushDecal->getDiffuseTexture()->setOutSrv( m_brushDecalMapRT->getSRV( 0 ) );
}

void GEW_Terrain::update( GEW_Input::Data &i_inputData, const GE_View &i_view, const GE_Vec2 &i_cursorPos, const GE_Vec2 &i_widthHeight )
{
	if ( !m_baseData.Enable )
		return;

	if ( m_horizonRUO->isVisible() )
	{
		m_horizonRUO->setScale( GE_Vec3( m_baseData.Size, m_baseData.Size, 1.0f ) );
		m_horizonRUO->update();
	}
	if ( m_sculptData.Enable )
	{
		if ( !i_inputData.isMouseDown( GUM_BUTTON_LEFT ) )
		{
			m_lastHit.y = m_lastHit.x = D3D11_FLOAT32_MAX;
			if ( m_sculptCurrentParam.Mode > 0 )
				undoMaskMapSave();
			else
				undoHeightMapSave();
		}

		GE_Vec3 org;
		GE_Vec3 dir;
		( ( GE_View * )&i_view )->getRayFromScreen( i_cursorPos, i_widthHeight, &org, &dir );
		GE_Vec3 verts[ 4 ];
		float hSize = m_baseData.Size * 0.5f;
		verts[ 0 ].x = -hSize;
		verts[ 0 ].y = -hSize;
		verts[ 3 ].z = verts[ 2 ].z = verts[ 1 ].z = verts[ 0 ].z = m_baseParam.HeightBias;

		verts[ 1 ].x = hSize;
		verts[ 1 ].y = -hSize;

		verts[ 2 ].x = -hSize;
		verts[ 2 ].y = hSize;

		verts[ 3 ].x = hSize;
		verts[ 3 ].y = hSize;

		physx::PxRaycastBuffer hit;
		GEW_Physics::getSingleton().m_scene2->raycast( TO_PxVec3( org ), TO_PxVec3( dir ), GE_ConvertToUnit( 1000000 ), hit );
		if ( !hit.hasBlock )
			return;
		float distance = hit.block.distance;
		org += dir * distance;
		GE_Vec2 hitPos;
		hitPos.x = org.x;
		hitPos.y = org.y;

		updateBrushParameters( hitPos );
		m_brushDecal->update();
		m_brushRUO->setPosition( GE_Vec3( hitPos.x, hitPos.y, m_baseParam.HeightBias ) );
		m_brushRUO->setScale( GE_Vec3( m_baseParam.MaxHeight, m_baseParam.MaxHeight, m_baseParam.MaxHeight ) );
		m_brushRUO->update();

		if ( !i_inputData.isMouseDown( GUM_BUTTON_LEFT ) || ( m_lastHit - hitPos ).length() < m_sculptCurrentParam.ReapeatDistance )
			return;

		if ( m_sculptCurrentParam.Mode > 0 )
		{
			if ( !m_maskMapRT[ 0 ] || !m_maskMapRT[ 1 ] )
				return;
			m_undoParams.MaskMapAllowSave = true;
		}
		else
		{
			if ( !m_heightMapRT[ 0 ] || !m_heightMapRT[ 1 ] )
				return;
			m_undoParams.HeightMapAllowSave = true;
		}

		m_lastHit = hitPos;

		m_cPerFrameBrushHeight.BrushFalloff = m_sculptCurrentParam.Falloff * m_sculptCurrentParam.Size;
		m_cPerFrameBrushHeight.BrushHardness = m_sculptCurrentParam.Hardness;
		m_cPerFrameBrushHeight.Channel = m_sculptCurrentParam.Mode - 1;
		m_cPerFrameBrushHeight.BrushVolume = m_sculptCurrentParam.Volume / m_baseParam.MaxHeight;
		m_cPerFrameBrushHeight.BrushMaskRotation = m_sculptCurrentParam.Angle;
		if ( m_sculptCurrentParam.Randomize )
			m_cPerFrameBrushHeight.BrushMaskRotation = GE_ConvertToRadians( ( float )( rand() % 360 ) );
		m_cPerFrameBrushHeight.BrushSize = m_sculptCurrentParam.Size;
		m_cPerFrameBrushHeight.BrushHitXY.x = hitPos.x + hSize;
		m_cPerFrameBrushHeight.BrushHitXY.y = m_baseData.Size - ( hitPos.y + hSize );
		m_cPerFrameBrushHeight.BrushFieldSize = m_baseData.Size;
		m_cPerFrameBrushHeight.BrushMaskTile = m_sculptCurrentParam.MaskTileUV * m_baseData.Size / m_sculptCurrentParam.Size * 0.5f;
		float invSize = 1.0f / m_baseData.Size;
		m_cPerFrameBrushHeight.BrushMaskPos.x = m_cPerFrameBrushHeight.BrushHitXY.x * invSize *
			m_cPerFrameBrushHeight.BrushMaskTile.x + m_sculptCurrentParam.MaskPositionUV.x - 0.5f;
		m_cPerFrameBrushHeight.BrushMaskPos.y = m_cPerFrameBrushHeight.BrushHitXY.y * invSize *
			m_cPerFrameBrushHeight.BrushMaskTile.y + m_sculptCurrentParam.MaskPositionUV.y - 0.5f;
		GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrameBrushHeight, 0, NULL, &m_cPerFrameBrushHeight, 0, 0 );
		GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrameBrushHeight );

		GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );
		GE_Core::getSingleton().setScreenAlignQuadInputLayoutToContext();
		GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );
		GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

		ID3D11SamplerState *sam[ 3 ];
		sam[ 0 ] = GE_Core::getSingleton().getRenderStates().SamplerPointClamp;
		sam[ 1 ] = GE_Core::getSingleton().getRenderStates().SamplerLinearClamp;
		sam[ 2 ] = GE_Core::getSingleton().getRenderStates().SamplerLinearWrap;
		GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 3, sam );

		if ( m_sculptCurrentParam.Mode > 0 )
		{
			GE_Core::getSingleton().getImmediateContext()->CopyResource( m_maskMapRT[ 1 ]->getRTTexture( 0 ), m_maskMapRT[ 0 ]->getRTTexture( 0 ) );
			m_maskMapRT[ 0 ]->setToDevice();
			ID3D11ShaderResourceView *srv[ 2 ];
			srv[ 0 ] = m_maskMapRT[ 1 ]->getSRV( 0 );
			if ( m_heightMaskMap )
				srv[ 1 ] = m_heightMaskMap->getTextureSRV();
			else
				srv[ 1 ] = NULL;
			GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );

			if ( m_sculptCurrentParam.Type == SculptType::POSITIVE )
			{
				if ( srv[ 1 ] && m_sculptCurrentParam.MaskEnable )
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MaskSculptPositiveMask, NULL, 0 );
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MaskSculptPositive, NULL, 0 );
			}
			else if ( m_sculptCurrentParam.Type == SculptType::NEGATIVE )
			{
				if ( srv[ 1 ] && m_sculptCurrentParam.MaskEnable )
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MaskSculptNegativeMask, NULL, 0 );
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MaskSculptNegative, NULL, 0 );
			}
			else if ( m_sculptCurrentParam.Type == SculptType::SMOOTH )
			{
				if ( srv[ 1 ] && m_sculptCurrentParam.MaskEnable )
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MaskSculptSmoothMask, NULL, 0 );
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MaskSculptSmooth, NULL, 0 );
			}
			else
			{
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MaskSculptFlat, NULL, 0 );
			}

			GE_Core::getSingleton().renderScreenAlignQuad();

			srv[ 0 ] = NULL;
			srv[ 1 ] = NULL;
			GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );

			GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );
			GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
			//GE_Core::getSingleton().getImmediateContext()->GenerateMips(m_maskMapRT[0]->getSRV(0));
		}
		else
		{
			GE_Core::getSingleton().getImmediateContext()->CopyResource( m_heightMapRT[ 1 ]->getRTTexture( 0 ), m_heightMapRT[ 0 ]->getRTTexture( 0 ) );
			m_heightMapRT[ 0 ]->setToDevice();
			ID3D11ShaderResourceView *srv[ 2 ];
			srv[ 0 ] = m_heightMapRT[ 1 ]->getSRV( 0 );
			if ( m_heightMaskMap )
				srv[ 1 ] = m_heightMaskMap->getTextureSRV();
			else
				srv[ 1 ] = NULL;
			GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );

			if ( m_sculptCurrentParam.Type == SculptType::POSITIVE )
			{
				if ( srv[ 1 ] && m_sculptCurrentParam.MaskEnable )
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_HeightSculptPositiveMask, NULL, 0 );
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_HeightSculptPositive, NULL, 0 );
			}
			else if ( m_sculptCurrentParam.Type == SculptType::NEGATIVE )
			{
				if ( srv[ 1 ] && m_sculptCurrentParam.MaskEnable )
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_HeightSculptNegativeMask, NULL, 0 );
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_HeightSculptNegative, NULL, 0 );
			}
			else if ( m_sculptCurrentParam.Type == SculptType::SMOOTH )
			{
				if ( srv[ 1 ] && m_sculptCurrentParam.MaskEnable )
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_HeightSculptSmoothMask, NULL, 0 );
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_HeightSculptSmooth, NULL, 0 );
			}
			else
			{
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_HeightSculptFlat, NULL, 0 );
			}

			GE_Core::getSingleton().renderScreenAlignQuad();

			srv[ 0 ] = NULL;
			srv[ 1 ] = NULL;
			GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );

			GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );
			GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
			//GE_Core::getSingleton().getImmediateContext()->GenerateMips(m_heightMapRT[0]->getSRV(0));
		}

	}
}

void GEW_Terrain::getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList )
{
	if ( m_horizonRUO->isVisible() && m_baseData.Enable )
		i_outList.push_back( m_horizonRUO );

	if ( m_baseData.Enable && m_sculptData.Enable )
		i_outList.push_back( m_brushRUO );
}

void GEW_Terrain::getDecalObjects( std::list<class GE_Decal*> &i_outList )
{
	if ( m_baseData.Enable && m_sculptData.Enable )
		i_outList.push_back( m_brushDecal );
}

void GEW_Terrain::updateBrushParameters( const GE_Vec2 &i_position )
{
	GE_Mat4x4 matTrans;
	GE_Quat rot;
	GE_Vec3 pos;
	GE_Vec3 scale;
	scale.x = scale.y = m_sculptCurrentParam.Size * 2.0f;
	scale.z = m_baseParam.MaxHeight * 2.0f;
	pos.x = i_position.x;
	pos.y = i_position.y;
	pos.z = m_baseParam.MaxHeight + m_baseParam.HeightBias;

	rot.rotationAxis( &GE_Vec3( 1.0f, 0.0f, 0.0f ), GE_ConvertToRadians( 0.0f ) );
	matTrans.RotationQuaternion( &rot );
	GE_Vec3	Dir = GE_Vec3( 0.0f, 0.0f, -1.0f );
	GE_Vec3	Up = GE_Vec3( 0.0f, 1.0f, 0.0f );
	Dir.transform( &matTrans );
	Up.transform( &matTrans );
	Dir.normalize();
	Dir += pos;
	Up.normalize();

	vector<GE_Decal::Parameter > *p = m_brushDecal->beginEditParameters();
	p->data()[ 0 ].Transform.srp( pos, rot, scale );
	GE_Mat4x4 matV;
	matV.lookAtRH( &pos, &Dir, &Up );
	GE_Vec3 size = scale;
	if ( size.x <= 0.2f )
		size.x = 0.2f;
	if ( size.y <= 0.2f )
		size.y = 0.2f;
	if ( size.z <= 0.2f )
		size.z = 0.2f;

	p->data()[ 0 ].Projection.orthoRH(
		size.x + 1.0f,
		size.y + 1.0f,
		1.0f,
		size.z + 2.0f );
	p->data()[ 0 ].Projection = matV * p->data()[ 0 ].Projection;
	m_brushDecal->endEditParameters();
}

GEW_Terrain GEW_Terrain::m_singleton;

GEW_Terrain::BaseData::BaseData()
{
	Enable = false;
	Size = GE_ConvertToUnit( 20000.0f );
	NumberOfChunks = 16;
	QuadDensity = 32;
}

void GEW_Terrain::BaseData::set()
{
	GE_Terrain::getSingleton().create( Size, QuadDensity, NumberOfChunks );
}

GEW_Terrain::BaseParam::BaseParam()
{
	HeightBias = 0.0f;
	MaxHeight = GE_ConvertToUnit( 600.0f );
	NormalAspect = 500.0f;
	LodMultiplier = 4.0f;
	NormalNoiseUVTile.x = 1000.0f;
	NormalNoiseUVTile.y = 1000.0f;
}

void GEW_Terrain::BaseParam::set()
{
	GE_Terrain::getSingleton().setLodMultiplier( LodMultiplier );
	GE_Terrain::getSingleton().setHeightBias( HeightBias );
	GE_Terrain::getSingleton().setMaxHeight( MaxHeight );
	GE_Terrain::getSingleton().setNormalAspect( NormalAspect );
	GE_Terrain::getSingleton().m_cPerFrame.NormalNoiseUVTile = NormalNoiseUVTile;
}

GEW_Terrain::SculptParam::SculptParam()
{
	memset( this, 0, sizeof( SculptParam ) );
	Size = GE_ConvertToUnit( 400.0f );
	Volume = GE_ConvertToUnit( 5.0f );
	ReapeatDistance = GE_ConvertToUnit( 200.0f );
	Falloff = 0.7f;
	Hardness = 0.0f;
	GE_Vec2 MaskPositionUV;
	MaskTileUV.x = 1.0f;
	MaskTileUV.y = 1.0f;
}


void GEW_Terrain::saveBrushPresets( QString i_filename )
{
	QFile file( i_filename );
	QString resourcePath = i_filename.left( i_filename.length() - 4 ) + "_PresetResources/";
	QDir dir;
	dir.mkdir( resourcePath );
	PresetsSaveData bsd;
	bsd.NumberOfPresets = ( uint32_t )m_sculptPresets.size();
	if ( file.open( QIODevice::WriteOnly ) )
	{
		file.write( ( char * )&bsd, sizeof( PresetsSaveData ) );
		SculptParam param;
		for ( uint32_t i = 0; i < bsd.NumberOfPresets; ++i )
		{
			param = m_sculptPresets[ i ];
			if ( param.MaskFileName[ 0 ] )
				strcpy_s( param.MaskFileName, GEW_Utility::copyFile( m_sculptPresets[ i ].MaskFileName, resourcePath ).c_str() );
			file.write( ( char * )&param, sizeof( SculptParam ) );
		}
	}
	else
	{
		QMessageBox::critical(
			NULL,
			"Saving brush presets",
			"Unable to create file or directory." );
	}

}

bool GEW_Terrain::loadBrushPresets( QString i_filename )
{
	QFile file( i_filename );
	QString resourcePath = i_filename.left( i_filename.length() - 4 ) + "_PresetResources/";
	QDir dir;
	dir.mkdir( resourcePath );
	PresetsSaveData bsd;
	if ( file.open( QIODevice::ReadOnly ) )
	{
		file.read( ( char * )&bsd, sizeof( PresetsSaveData ) );
		if ( bsd.GEB[ 0 ] != 'G' || bsd.GEB[ 1 ] != 'E' || bsd.GEB[ 2 ] != 'B' || bsd.NumberOfPresets == 0 )
			return false;
		m_sculptPresets.clear();
		SculptParam param;
		for ( uint32_t i = 0; i < bsd.NumberOfPresets; ++i )
		{
			file.read( ( char * )&param, sizeof( SculptParam ) );
			if ( param.MaskFileName[ 0 ] )
				strcpy_s( param.MaskFileName, GEW_Utility::absolutePath( param.MaskFileName, resourcePath ).c_str() );
			m_sculptPresets.push_back( param );
		}
	}
	else
	{
		QMessageBox::critical(
			NULL,
			"Loading brush presets",
			"Unable to load file." );
		return false;
	}
	return true;
}

GEW_Terrain::SculptData::SculptData()
{
	Enable = false;
}

void GEW_Terrain::initHeightMapUndoMemory()
{
	if ( !m_baseData.Enable || !m_sculptData.Enable || !m_heightMapRT[ 0 ] )
	{
		m_undoParams.HeightMapMemory.clear();
		return;
	}
	if ( m_undoParams.HeightMapMemory.size() )
		return;
	m_undoParams.HeightMapMemory.clear();
	m_undoParams.HeightMapMemory.resize( m_undoParams.HeightMapMaxUndoSize );
	m_undoParams.HeightMapSaveIndex = -1;
	m_undoParams.HeightMapUndoCounter = 0;
	m_undoParams.HeightMapCanRedo = false;
	m_undoParams.HeightMapAllowSave = true;
}

void GEW_Terrain::undoHeightMap()
{
	if ( !m_undoParams.HeightMapMemory.size() || m_undoParams.HeightMapUndoCounter <= 0 )
		return;

	int index = m_undoParams.HeightMapSaveIndex - 1;
	if ( index < 0 )
		index = m_undoParams.HeightMapMaxUndoSize - 1;
	if ( !m_undoParams.HeightMapMemory[ index ].isValid() )
		return;
	m_undoParams.HeightMapSaveIndex = index;

	ID3D11ShaderResourceView *srv = NULL;
	m_undoParams.HeightMapMemory[ m_undoParams.HeightMapSaveIndex ].load( &srv );
	ID3D11Resource *resource = NULL;
	srv->GetResource( &resource );
	GE_Core::getSingleton().getImmediateContext()->CopyResource( m_heightMapRT[ 0 ]->getRTTexture( 0 ), resource );
	SAFE_RELEASE( resource );
	SAFE_RELEASE( srv );

	m_undoParams.HeightMapUndoCounter--;
	m_undoParams.HeightMapCanRedo = true;
	m_physxData.set( );
}

void GEW_Terrain::redoHeightMap()
{
	if ( !m_undoParams.HeightMapMemory.size() ||
		m_undoParams.HeightMapUndoCounter >= m_undoParams.HeightMapMaxUndoSize - 1 || !m_undoParams.HeightMapCanRedo )
		return;

	int index = m_undoParams.HeightMapSaveIndex + 1;
	if ( index >= m_undoParams.HeightMapMaxUndoSize )
		index = 0;
	if ( !m_undoParams.HeightMapMemory[ index ].isValid() )
		return;
	m_undoParams.HeightMapSaveIndex = index;

	ID3D11ShaderResourceView *srv = NULL;
	m_undoParams.HeightMapMemory[ m_undoParams.HeightMapSaveIndex ].load( &srv );
	ID3D11Resource *resource = NULL;
	srv->GetResource( &resource );
	GE_Core::getSingleton().getImmediateContext()->CopyResource( m_heightMapRT[ 0 ]->getRTTexture( 0 ), resource );
	SAFE_RELEASE( resource );
	SAFE_RELEASE( srv );
	m_undoParams.HeightMapUndoCounter++;
	if ( m_undoParams.HeightMapUndoCounter > m_undoParams.HeightMapMaxUndoSize - 1 )
		m_undoParams.HeightMapUndoCounter = m_undoParams.HeightMapMaxUndoSize - 1;
	m_physxData.set();
}

void GEW_Terrain::undoHeightMapSave()
{
	if ( !m_undoParams.HeightMapMemory.size() || !m_undoParams.HeightMapAllowSave )
		return;

	m_physxData.set( );
	m_undoParams.HeightMapAllowSave = false;

	if ( m_undoParams.HeightMapCanRedo )
	{
		m_undoParams.HeightMapUndoCounter = 0;
		m_undoParams.HeightMapCanRedo = false;
	}

	if ( m_undoParams.HeightMapSaveIndex >= m_undoParams.HeightMapMaxUndoSize - 1 )
		m_undoParams.HeightMapSaveIndex = -1;
	m_undoParams.HeightMapSaveIndex++;

	m_undoParams.HeightMapMemory[ m_undoParams.HeightMapSaveIndex ].save( m_heightMapRT[ 0 ]->getRTTexture( 0 ) );

	m_undoParams.HeightMapUndoCounter++;
	if ( m_undoParams.HeightMapUndoCounter > m_undoParams.HeightMapMaxUndoSize - 1 )
		m_undoParams.HeightMapUndoCounter = m_undoParams.HeightMapMaxUndoSize - 1;
}

void GEW_Terrain::initMaskMapUndoMemory()
{
	if ( !m_baseData.Enable || !m_sculptData.Enable || !m_maskMapRT[ 0 ] )
	{
		m_undoParams.MaskMapMemory.clear();
		return;
	}
	if ( m_undoParams.MaskMapMemory.size() )
		return;
	m_undoParams.MaskMapMemory.clear();
	m_undoParams.MaskMapMemory.resize( m_undoParams.MaskMapMaxUndoSize );
	m_undoParams.MaskMapSaveIndex = -1;
	m_undoParams.MaskMapUndoCounter = 0;
	m_undoParams.MaskMapCanRedo = false;
	m_undoParams.MaskMapAllowSave = true;
}

void GEW_Terrain::undoMaskMap()
{
	if ( !m_undoParams.MaskMapMemory.size() || m_undoParams.MaskMapUndoCounter <= 0 )
		return;

	int index = m_undoParams.MaskMapSaveIndex - 1;
	if ( index < 0 )
		index = m_undoParams.MaskMapMaxUndoSize - 1;
	if ( !m_undoParams.MaskMapMemory[ index ].isValid() )
		return;
	m_undoParams.MaskMapSaveIndex = index;

	ID3D11ShaderResourceView *srv = NULL;
	m_undoParams.MaskMapMemory[ m_undoParams.MaskMapSaveIndex ].load( &srv );
	ID3D11Resource *resource = NULL;
	srv->GetResource( &resource );
	GE_Core::getSingleton().getImmediateContext()->CopyResource( m_maskMapRT[ 0 ]->getRTTexture( 0 ), resource );
	SAFE_RELEASE( resource );
	SAFE_RELEASE( srv );

	m_undoParams.MaskMapUndoCounter--;
	m_undoParams.MaskMapCanRedo = true;
}

void GEW_Terrain::redoMaskMap()
{
	if ( !m_undoParams.MaskMapMemory.size() ||
		m_undoParams.MaskMapUndoCounter >= m_undoParams.MaskMapMaxUndoSize - 1 || !m_undoParams.MaskMapCanRedo )
		return;

	int index = m_undoParams.MaskMapSaveIndex + 1;
	if ( index >= m_undoParams.MaskMapMaxUndoSize )
		index = 0;
	if ( !m_undoParams.MaskMapMemory[ index ].isValid() )
		return;
	m_undoParams.MaskMapSaveIndex = index;

	ID3D11ShaderResourceView *srv = NULL;
	m_undoParams.MaskMapMemory[ m_undoParams.MaskMapSaveIndex ].load( &srv );
	ID3D11Resource *resource = NULL;
	srv->GetResource( &resource );
	GE_Core::getSingleton().getImmediateContext()->CopyResource( m_maskMapRT[ 0 ]->getRTTexture( 0 ), resource );
	SAFE_RELEASE( resource );
	SAFE_RELEASE( srv );
	m_undoParams.MaskMapUndoCounter++;
	if ( m_undoParams.MaskMapUndoCounter > m_undoParams.MaskMapMaxUndoSize - 1 )
		m_undoParams.MaskMapUndoCounter = m_undoParams.MaskMapMaxUndoSize - 1;
}

void GEW_Terrain::undoMaskMapSave()
{
	if ( !m_undoParams.MaskMapMemory.size() || !m_undoParams.MaskMapAllowSave )
		return;

	m_undoParams.MaskMapAllowSave = false;

	if ( m_undoParams.MaskMapCanRedo )
	{
		m_undoParams.MaskMapUndoCounter = 0;
		m_undoParams.MaskMapCanRedo = false;
	}

	if ( m_undoParams.MaskMapSaveIndex >= m_undoParams.MaskMapMaxUndoSize - 1 )
		m_undoParams.MaskMapSaveIndex = -1;
	m_undoParams.MaskMapSaveIndex++;

	m_undoParams.MaskMapMemory[ m_undoParams.MaskMapSaveIndex ].save( m_maskMapRT[ 0 ]->getRTTexture( 0 ) );

	m_undoParams.MaskMapUndoCounter++;
	if ( m_undoParams.MaskMapUndoCounter > m_undoParams.MaskMapMaxUndoSize - 1 )
		m_undoParams.MaskMapUndoCounter = m_undoParams.MaskMapMaxUndoSize - 1;
}

void GEW_Terrain::resetAll()
{
	m_sculptPresets.clear();
	m_sculptCurrentParam = SculptParam();
	m_physxData = PhysxData();
	if ( m_heightFieldActor )
	{
		GEW_Physics::getSingleton().m_scene2->removeActor( *m_heightFieldActor );
		m_heightFieldActor->release();
		m_heightFieldActor = NULL;
	}
	m_baseData = BaseData();
	m_baseParam = BaseParam();
	m_undoParams = UndoParams();
	m_sculptData = SculptData();
	m_splattingListNames.clear();
	GE_Terrain::getSingleton().m_materialParameters = GE_Terrain::MaterialParameters();
	GE_Terrain::getSingleton().m_maps = GE_Terrain::Maps();
	setSculptEnableChange();
	setHeightMap();
	setMaskMap();
	for ( uint32_t i = 0; i < GE_MAX_TERRAIN_PROCEDURAL_LAYER; ++i )
		m_splattingListNames.push_back( QObject::tr( "Procedural layer %1" ).arg( i + 1 ) );
	for ( uint32_t i = 0; i < GE_MAX_TERRAIN_PAINT_LAYER; ++i )
		m_splattingListNames.push_back( QObject::tr( "Mask layer %1" ).arg( i + 1 ) );
}

void GEW_Terrain::reCreateHeightField()
{
	if ( m_heightFieldActor )
	{
		GEW_Physics::getSingleton( ).m_scene2->removeActor( *m_heightFieldActor );
		m_heightFieldActor->release();
		m_heightFieldActor = NULL;
	}

	GE_Vec2 HeightMinMax;
	vector<GE_Vec3> vertices;
	GE_Terrain::getSingleton().getVerticesData( &vertices, NULL, HeightMinMax, m_physxData.QuadDensity );

	m_heightFieldActor = GEW_Physics::getSingleton().m_physics->createRigidStatic( physx::PxTransform::createIdentity() );
	m_heightFieldActor->setActorFlag( physx::PxActorFlag::eVISUALIZATION, false );
	m_heightFieldActor->setActorFlag( physx::PxActorFlag::eDISABLE_GRAVITY, false );
	m_heightFieldActor->setActorFlag( physx::PxActorFlag::eSEND_SLEEP_NOTIFIES, false );
	m_heightFieldActor->setActorFlag( physx::PxActorFlag::eDISABLE_SIMULATION, false );


	PxReal deltaHeight = HeightMinMax.y - HeightMinMax.x;

	PxReal quantization = ( PxReal )0x7fff;

	PxReal heightScale = PxMax( deltaHeight / quantization, PX_MIN_HEIGHTFIELD_Y_SCALE );
	uint32_t verts = m_physxData.QuadDensity + 1;

	PxU32* hfSamples = new PxU32[ verts * verts ];

	PxU32 index = 0;
	for ( PxU32 col = 0; col < verts; col++ )
	{
		for ( PxU32 row = 0; row < verts; row++ )
		{
			PxHeightFieldSample& smp = ( PxHeightFieldSample& )( hfSamples[ ( row * verts ) + col ] );
			smp.height = PxI16( quantization * ( vertices.data( )[ ( ( ( verts - 1 ) - col ) * verts ) + row ].z - HeightMinMax.x ) / deltaHeight );
			smp.materialIndex0 = 0;
			smp.materialIndex1 = 0;
			smp.clearTessFlag();
		}
	}

	PxHeightFieldDesc terrainDesc;
	terrainDesc.format = PxHeightFieldFormat::eS16_TM;
	terrainDesc.nbColumns = verts;
	terrainDesc.nbRows = verts;
	terrainDesc.samples.data = hfSamples;
	terrainDesc.samples.stride = sizeof( PxU32 );
	terrainDesc.thickness = -GE_ConvertToUnit( 2.0f );
	terrainDesc.flags = PxHeightFieldFlags();

	m_heightFieldGeometry.rowScale = m_heightFieldGeometry.columnScale = GE_Terrain::getSingleton( ).getWidth( ) / m_physxData.QuadDensity;
	m_heightFieldGeometry.heightScale = ( deltaHeight != 0 ? heightScale : 1.0f );
	m_heightFieldGeometry.heightField = GEW_Physics::getSingleton().m_physics->createHeightField( terrainDesc );

	delete[] hfSamples;

	PxTransform localPose;
	localPose.p = PxVec3( -( GE_Terrain::getSingleton().getWidth() * 0.5f ), ( GE_Terrain::getSingleton().getWidth() * 0.5f ), 0.0f );
	localPose.q = *( PxQuat * )&GE_Quat().rotationAxis( &GE_Vec3( 1.0f, 0.0f, 0.0f ), GE_ConvertToRadians( 90.0f ) );
	PxMaterial *material = GEW_Physics::getSingleton().getPxMaterial( 0 );
	PxShape* shape = m_heightFieldActor->createShape( m_heightFieldGeometry, *material, localPose );
	GEW_Physics::getSingleton().m_scene2->addActor( *m_heightFieldActor );
}

physx::PxHeightFieldGeometry & GEW_Terrain::getHeightFieldGeometry()
{
	return m_heightFieldGeometry;
}


GEW_Terrain::UndoParams::UndoParams()
{
	HeightMapMaxUndoSize = 10;
	HeightMapSaveIndex = -1;
	HeightMapUndoCounter = 0;
	HeightMapCanRedo = false;
	HeightMapAllowSave = false;

	MaskMapMaxUndoSize = 10;
	MaskMapSaveIndex = -1;
	MaskMapUndoCounter = 0;
	MaskMapCanRedo = false;
	MaskMapAllowSave = false;
}

GEW_Terrain::UndoParams::~UndoParams()
{

}

GEW_Terrain::PresetsSaveData::PresetsSaveData()
{
	GEB[ 0 ] = 'G';
	GEB[ 1 ] = 'E';
	GEB[ 2 ] = 'B';
	NumberOfPresets = 0;
}


GEW_Terrain::PresetsSaveData::~PresetsSaveData()
{

}

GEW_Terrain::PhysxData::PhysxData()
{
	VisualizeHeightfield = false;
	QuadDensity = 400;
}

void GEW_Terrain::PhysxData::set()
{
	GEW_Terrain::getSingleton()->reCreateHeightField();
}
