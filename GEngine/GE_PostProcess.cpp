#include "GE_PostProcess.h"
#include "GE_Core.h"
#include "GE_RenderTarget.h"
#include "GE_Texture.h"
#include "GE_Math.h"

GE_PostProcess GE_PostProcess::m_singleton;

void GE_PostProcess::init()
{
	if ( m_vS )
		return;

	m_rgbGradientTexture = GE_TextureManager::getSingleton().createTexture( ( string( GE_TEXTURE_PATH ) + "GE_Data/GE_Texture/RGBTable.dds" ).c_str() );

	m_vS = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_PP_VS.cso" );

	m_pS_FlipUV = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_FlipUV.cso" );

	m_pS_Grayscale = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_Grayscale.cso" );

	m_pS_GrowByAlpha = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_GrowByAlpha.cso" );

	m_pS_Down = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_Down.cso" );

	m_pS_DownMulAlpha = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_DownMulAlpha.cso" );

	m_pS_BlurHorizontal = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_BlurHorizontal.cso" );

	m_pS_BlurVertical = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_BlurVertical.cso" );

	m_pS_BrightCalculateBoth = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_BrightCalculateBoth.cso" );

	m_pS_BrightCalculate = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_BrightCalculate.cso" );

	m_pS_BrightCalculateAlpha = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_BrightCalculateAlpha.cso" );

	m_pS_Combine = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_Combine.cso" );

	m_pS_Lerp = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_Lerp.cso" );

	m_pS_Multiple = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_Multiple.cso" );

	m_pS_Subtract = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_Subtract.cso" );

	m_pS_MaskWithAlpha = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_MaskWithAlpha.cso" );

	m_pS_Border = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_Border.cso" );

	m_pS_Edge = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_Edge.cso" );

	m_pS_CartoonEdge = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_CartoonEdge.cso" );

	m_pS_RadialBlur = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_RadialBlur.cso" );

	m_pS_RadialBlurDouble = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_RadialBlurDouble.cso" );

	m_pS_MotionBlur = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_MotionBlur.cso" );

	m_pS_DOF = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_DOF.cso" );
	m_pS_DOFAuto = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_DOFAuto.cso" );

	m_pS_AtmosphereFog = GE_Core::getSingleton().createPS(string(GE_SHADER_PATH) + "GE_PP_PS_AtmosphereFog.cso");

	m_pS_LightScatter = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_LightScatter.cso" );

	m_pS_Down2X2 = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_Down2X2.cso" );

	m_pS_Down3X3 = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_Down3X3.cso" );

	m_pS_ToneMap = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_ToneMap.cso" );

	m_pS_Adaptation = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_Adaptation.cso" );

	m_pS_LensDirt = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_LensDirt.cso" );

	m_pS_To1x1 = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_To1x1.cso" );

	m_pS_SSAO = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_SSAO.cso" );

	m_pS_Convert = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_Convert.cso" );

	m_pS_FXAA[ 0 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_FXAA.cso" );
	m_pS_FXAA[ 1 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_FXAA_1.cso" );
	m_pS_FXAA[ 2 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_FXAA_2.cso" );
	m_pS_FXAA[ 3 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_FXAA_3.cso" );
	m_pS_FXAA[ 4 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_FXAA_4.cso" );
	m_pS_FXAA[ 5 ] = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_PP_PS_FXAA_5.cso" );

	m_perFrame = GE_Core::getSingleton().createConstBuffer( sizeof( PerFrame ) );

	resize();

	m_adaptationRate = 60.0f;
	m_enableHDR = false;
	m_currentAdaptationTarget = false;
	m_whiteCutoff = 4.0f;
	m_luminanceMax = 0.2f;
	m_luminanceMin = 2.2f;
	m_gamma = 2.2f;
}

void GE_PostProcess::resize()
{
	for ( uint32_t i = 0; i < 2; i++ )
	{
		SAFE_DELETE( m_downRT[ i ] );
		SAFE_DELETE( m_2downRT[ i ] );
		SAFE_DELETE( m_upRT[ i ] );
	}
	SAFE_DELETE( m_downRT[ 2 ] );

	for ( uint32_t i = 0; i < 5; i++ )
		SAFE_DELETE( m_hDRRT[ i ] );

	GE_RenderTarget::Parameters rp( 1 );
	rp.HasDepth = false;
	rp.Width[ 0 ] = ( uint32_t )GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Width / 4;
	rp.Height[ 0 ] = ( uint32_t )GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Height / 4;

	m_downRT[ 0 ] = new GE_RenderTarget( rp );
	m_downRT[ 0 ]->setViewPort( rp.Width[ 0 ], rp.Height[ 0 ], 0, 0 );
	m_downRT[ 1 ] = new GE_RenderTarget( rp );
	m_downRT[ 1 ]->setViewPort( rp.Width[ 0 ], rp.Height[ 0 ], 0, 0 );
	m_downRT[ 2 ] = new GE_RenderTarget( rp );
	m_downRT[ 2 ]->setViewPort( rp.Width[ 0 ], rp.Height[ 0 ], 0, 0 );

	rp.Width[ 0 ] /= 4;
	rp.Height[ 0 ] /= 4;
	m_2downRT[ 0 ] = new GE_RenderTarget( rp );
	m_2downRT[ 0 ]->setViewPort( rp.Width[ 0 ], rp.Height[ 0 ], 0, 0 );
	m_2downRT[ 1 ] = new GE_RenderTarget( rp );
	m_2downRT[ 1 ]->setViewPort( rp.Width[ 0 ], rp.Height[ 0 ], 0, 0 );

	rp.Width[ 0 ] = ( uint32_t )GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Width;
	rp.Height[ 0 ] = ( uint32_t )GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Height;

	m_upRT[ 0 ] = new GE_RenderTarget( rp );
	m_upRT[ 0 ]->setViewPort( rp.Width[ 0 ], rp.Height[ 0 ], 0, 0 );
	m_upRT[ 1 ] = new GE_RenderTarget( rp );
	m_upRT[ 1 ]->setViewPort( rp.Width[ 0 ], rp.Height[ 0 ], 0, 0 );

	rp.Format[ 0 ] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	m_hDRRT[ 0 ] = new GE_RenderTarget( rp );
	m_hDRRT[ 0 ]->setViewPort( rp.Width[ 0 ], rp.Height[ 0 ], 0, 0 );

	rp.Format[ 0 ] = DXGI_FORMAT_R16_FLOAT;
	rp.Width[ 0 ] = rp.Height[ 0 ] = 128;
	rp.MipLevel[ 0 ] = 7;
	m_hDRRT[ 1 ] = new GE_RenderTarget( rp );
	m_hDRRT[ 1 ]->setViewPort( rp.Width[ 0 ], rp.Height[ 0 ], 0, 0 );

	rp.MipLevel[ 0 ] = 0;
	rp.Width[ 0 ] = rp.Height[ 0 ] = 1;
	m_hDRRT[ 2 ] = new GE_RenderTarget( rp );
	m_hDRRT[ 2 ]->setViewPort( rp.Width[ 0 ], rp.Height[ 0 ], 0, 0 );

	rp.Width[ 0 ] = rp.Height[ 0 ] = 1;
	m_hDRRT[ 3 ] = new GE_RenderTarget( rp );
	m_hDRRT[ 3 ]->setViewPort( rp.Width[ 0 ], rp.Height[ 0 ], 0, 0 );

	rp.Width[ 0 ] = rp.Height[ 0 ] = 1;
	m_hDRRT[ 4 ] = new GE_RenderTarget( rp );
	m_hDRRT[ 4 ]->setViewPort( rp.Width[ 0 ], rp.Height[ 0 ], 0, 0 );
}

void GE_PostProcess::beginHDR()
{
	if ( !m_enableHDR )
		return;
	m_hDRRT[ 0 ]->setToDevice();
	m_hDRRT[ 0 ]->clearRenderTarget( 0 );
}

void GE_PostProcess::endHDR( GE_RenderTarget *i_outRenderTarget, bool i_useRate )
{
	if ( !m_enableHDR )
		return;

	begin();

	ID3D11ShaderResourceView *srv[ 4 ];

	m_hDRRT[ 1 ]->setToDevice();
	m_hDRRT[ 1 ]->clearRenderTarget( 0 );
	srv[ 0 ] = m_hDRRT[ 0 ]->getSRV( 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Down2X2, NULL, NULL );
	GE_Core::getSingleton().renderScreenAlignQuad();

	m_hDRRT[ 2 ]->setToDevice();
	m_hDRRT[ 2 ]->clearRenderTarget( 0 );

	GE_Core::getSingleton().getImmediateContext()->GenerateMips( m_hDRRT[ 1 ]->getSRV( 0 ) );

	srv[ 0 ] = m_hDRRT[ 1 ]->getSRV( 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_To1x1, NULL, NULL );
	GE_Core::getSingleton().renderScreenAlignQuad();
	if ( i_useRate )
	{
		m_hDRRT[ 3 + ( uint32_t )m_currentAdaptationTarget ]->setToDevice();
		m_hDRRT[ 3 + ( uint32_t )m_currentAdaptationTarget ]->clearRenderTarget( 0 );

		srv[ 0 ] = m_hDRRT[ 2 ]->getSRV( 0 );
		srv[ 1 ] = m_hDRRT[ 3 + ( uint32_t )!m_currentAdaptationTarget ]->getSRV( 0 );
		GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
		m_cPerFrame.Float4Val0.x = GE_Core::getSingleton().getElapsedTime() * m_adaptationRate;
		m_cPerFrame.Float4Val0.y = m_luminanceMax;
		m_cPerFrame.Float4Val0.z = m_luminanceMin;
		GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
		GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );

		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Adaptation, NULL, NULL );
		GE_Core::getSingleton().renderScreenAlignQuad();

		m_currentAdaptationTarget = !m_currentAdaptationTarget;
	}

	if ( i_outRenderTarget )
		i_outRenderTarget->setToDevice();
	else
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );

	srv[ 0 ] = m_hDRRT[ 0 ]->getSRV( 0 );
	if ( i_useRate )
		srv[ 1 ] = m_hDRRT[ 3 + ( uint32_t )m_currentAdaptationTarget ]->getSRV( 0 );
	else
		srv[ 1 ] = m_hDRRT[ 2 ]->getSRV( 0 );
	srv[ 2 ] = NULL;
	srv[ 3 ] = m_rgbGradientTexture->getTextureSRV();
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 4, srv );

	m_cPerFrame.Float4Val0.x = m_whiteCutoff;
	m_cPerFrame.Float4Val0.y = 1.0f / m_gamma;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );

	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_ToneMap, NULL, NULL );
	GE_Core::getSingleton().renderScreenAlignQuad();

	end();
}

ID3D11ShaderResourceView *GE_PostProcess::getHDRLuminanceSRV()
{
	return m_hDRRT[ 2 ]->getSRV( 0 );
}
ID3D11ShaderResourceView *GE_PostProcess::getHDRLuminanceAdaptationSRV()
{
	return m_hDRRT[ 4 ]->getSRV( 0 );
}

void GE_PostProcess::renderGrayscale( ID3D11ShaderResourceView *i_texture, float i_grayscaleValue )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	m_cPerFrame.Float4Val0.x = i_grayscaleValue;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Grayscale, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );

}

void GE_PostProcess::renderGrowByAlpha( ID3D11ShaderResourceView *i_texture )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_GrowByAlpha, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );

}

void GE_PostProcess::renderDown( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	m_cPerFrame.Float4Val0.x = 1.0f / ( float )i_width;
	m_cPerFrame.Float4Val0.y = 1.0f / ( float )i_height;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Down, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
}

void GE_PostProcess::renderDownWithAlpha( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	m_cPerFrame.Float4Val0.x = 1.0f / ( float )i_width;
	m_cPerFrame.Float4Val0.y = 1.0f / ( float )i_height;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_DownMulAlpha, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
}

void GE_PostProcess::renderBlurHorizontal( ID3D11ShaderResourceView *i_texture, float i_bloomScale )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	m_cPerFrame.Float4Val0.x = i_bloomScale;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_BlurHorizontal, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
}

void GE_PostProcess::renderBlurVertical( ID3D11ShaderResourceView *i_texture, float i_bloomScale )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	m_cPerFrame.Float4Val0.x = i_bloomScale;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_BlurVertical, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
}

void GE_PostProcess::renderBrightCalculate( ID3D11ShaderResourceView *i_texture, float i_luminance, float i_whiteCutoff )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	m_cPerFrame.Float4Val0.x = i_luminance;
	m_cPerFrame.Float4Val0.y = i_whiteCutoff;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_BrightCalculate, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
}

void GE_PostProcess::renderBrightCalculateBoth( ID3D11ShaderResourceView *i_texture, float i_luminance, float i_whiteCutoff )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	m_cPerFrame.Float4Val0.x = i_luminance;
	m_cPerFrame.Float4Val0.y = i_whiteCutoff;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_BrightCalculateBoth, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
}

void GE_PostProcess::renderBrightCalculateAlpha( ID3D11ShaderResourceView *i_texture, float i_luminance, float i_whiteCutoff )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	m_cPerFrame.Float4Val0.x = i_luminance;
	m_cPerFrame.Float4Val0.y = i_whiteCutoff;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_BrightCalculateAlpha, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
}

void GE_PostProcess::renderCombine( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_texture1 )
{
	ID3D11ShaderResourceView *srv[ 2 ];
	srv[ 0 ] = i_texture;
	srv[ 1 ] = i_texture1;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Combine, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
}

void GE_PostProcess::renderLerp( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_texture1, float i_lerp )
{
	ID3D11ShaderResourceView *srv[ 2 ];
	srv[ 0 ] = i_texture;
	srv[ 1 ] = i_texture1;
	m_cPerFrame.Float4Val0.x = i_lerp;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Lerp, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
}

void GE_PostProcess::renderMultiple( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_texture1 )
{
	ID3D11ShaderResourceView *srv[ 2 ];
	srv[ 0 ] = i_texture;
	srv[ 1 ] = i_texture1;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Multiple, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
}

void GE_PostProcess::renderSubtract( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_texture1 )
{
	ID3D11ShaderResourceView *srv[ 2 ];
	srv[ 0 ] = i_texture;
	srv[ 1 ] = i_texture1;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Subtract, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
}

void GE_PostProcess::renderBorder( ID3D11ShaderResourceView *i_texture, float i_sharpness, float i_shape, float i_limit )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	m_cPerFrame.Float4Val0.x = i_sharpness;
	m_cPerFrame.Float4Val0.y = i_shape;
	m_cPerFrame.Float4Val0.z = i_limit;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Border, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
}

void GE_PostProcess::renderEdge( ID3D11ShaderResourceView *i_texture, GE_Vec3 &i_edgeColor, float i_threshold )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	m_cPerFrame.Float4Val0 = GE_Vec4( i_edgeColor.x, i_edgeColor.y, i_edgeColor.z, i_threshold );
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Edge, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
}

void GE_PostProcess::renderCartoonEdge( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_edgeTexture, GE_Vec3 &i_edgeColor, float i_threshold )
{
	ID3D11ShaderResourceView *srv[ 2 ];
	srv[ 0 ] = i_texture;
	srv[ 1 ] = i_edgeTexture;
	m_cPerFrame.Float4Val0 = GE_Vec4( i_edgeColor.x, i_edgeColor.y, i_edgeColor.z, i_threshold );
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_CartoonEdge, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
}

void GE_PostProcess::renderEdgeGlow( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_edgeTexture, uint32_t i_width, uint32_t i_height,
	GE_Vec3 &i_edgeColor, float i_horizontalGlowScale, float i_verticalGlowScale, float i_threshold,
	GE_RenderTarget *i_outRenderTarget )
{
	m_upRT[ 0 ]->setToDevice();
	renderEdge( i_edgeTexture, i_edgeColor, i_threshold );
	m_downRT[ 0 ]->setToDevice();
	renderDown( m_upRT[ 0 ]->getSRV( 0 ), ( uint32_t )m_upRT[ 0 ]->getViewPort()->Width, ( uint32_t )m_upRT[ 0 ]->getViewPort()->Height );
	m_downRT[ 1 ]->setToDevice();
	renderBlurHorizontal( m_downRT[ 0 ]->getSRV( 0 ), i_horizontalGlowScale );
	m_downRT[ 0 ]->setToDevice();
	renderBlurVertical( m_downRT[ 1 ]->getSRV( 0 ), i_verticalGlowScale );
	if ( i_outRenderTarget )
		i_outRenderTarget->setToDevice();
	else
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
	renderCombine( m_downRT[ 0 ]->getSRV( 0 ), i_texture );
	if ( i_outRenderTarget )
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
}

void GE_PostProcess::renderMaskWithAlpha( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_texture1 )
{
	ID3D11ShaderResourceView *srv[ 2 ];
	srv[ 0 ] = i_texture;
	srv[ 1 ] = i_texture1;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MaskWithAlpha, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
}

void GE_PostProcess::renderRadialBlur( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height, GE_Vec2 &i_radialPosition, float i_radialFocus )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	m_cPerFrame.Float4Val0.x = i_radialPosition.x;
	m_cPerFrame.Float4Val0.y = i_radialPosition.y;
	m_cPerFrame.Float4Val0.z = 1.0f / ( float )i_width;
	m_cPerFrame.Float4Val0.w = 1.0f / ( float )i_height;
	m_cPerFrame.Float4Val1.x = i_radialFocus;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_RadialBlur, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
}

void GE_PostProcess::renderRadialBlurDouble( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height, GE_Vec2 &i_radialPosition, float i_radialFocus )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	m_cPerFrame.Float4Val0.x = i_radialPosition.x;
	m_cPerFrame.Float4Val0.y = i_radialPosition.y;
	m_cPerFrame.Float4Val0.z = 1.0f / ( float )i_width;
	m_cPerFrame.Float4Val0.w = 1.0f / ( float )i_height;
	m_cPerFrame.Float4Val1.x = i_radialFocus;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_RadialBlurDouble, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
}

void GE_PostProcess::renderMotionBlur( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_depth
	, GE_Mat4x4 &i_viewProjectionInv, GE_Mat4x4 &i_previousViewProjection, float i_damp /*= 100.0f*/, float i_max /*= 0.1f */ )
{
	ID3D11ShaderResourceView *srv[ 2 ];
	srv[ 0 ] = i_texture;
	srv[ 1 ] = i_depth;
	m_cPerFrame.PreviousViewProjection.transposeBy( &i_previousViewProjection );
	m_cPerFrame.ViewProjectionInv.transposeBy( &i_viewProjectionInv );
	m_cPerFrame.Float4Val0.x = 1.0f / ( GE_Core::getSingleton().getElapsedTime() * i_damp );
	m_cPerFrame.Float4Val0.y = i_max;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MotionBlur, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
}

void GE_PostProcess::renderBlur( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height, float i_verticalGlowScale, float i_horizontalGlowScale,
	float i_lerp, GE_RenderTarget *i_outRenderTarget )
{
	m_downRT[ 0 ]->setToDevice();
	renderDown( i_texture, i_width, i_height );
	m_downRT[ 1 ]->setToDevice();
	renderBlurHorizontal( m_downRT[ 0 ]->getSRV( 0 ), i_horizontalGlowScale );
	m_downRT[ 0 ]->setToDevice();
	renderBlurVertical( m_downRT[ 1 ]->getSRV( 0 ), i_verticalGlowScale );
	if ( i_outRenderTarget )
		i_outRenderTarget->setToDevice();
	else
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
	renderLerp( i_texture, m_downRT[ 0 ]->getSRV( 0 ), i_lerp );
	if ( i_outRenderTarget )
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
}

void GE_PostProcess::renderGlow( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height, float i_horizontalGlowScale
	, float i_verticalGlowScale, float i_luminance, float i_whiteCutoff, GE_RenderTarget *i_outRenderTarget )
{
	m_upRT[ 0 ]->setToDevice();
	renderBrightCalculate( i_texture, i_luminance, i_whiteCutoff );
	m_downRT[ 0 ]->setToDevice();
	renderDown( m_upRT[ 0 ]->getSRV( 0 ), i_width, i_height );
	m_2downRT[ 0 ]->setToDevice();
	renderDown( m_downRT[ 0 ]->getSRV( 0 ), ( uint32_t )m_downRT[ 0 ]->getViewPort()->Width, ( uint32_t )m_downRT[ 0 ]->getViewPort()->Height );
	m_2downRT[ 1 ]->setToDevice();
	renderBlurHorizontal( m_2downRT[ 0 ]->getSRV( 0 ), i_horizontalGlowScale );
	m_2downRT[ 0 ]->setToDevice();
	renderBlurVertical( m_2downRT[ 1 ]->getSRV( 0 ), i_verticalGlowScale );
	m_2downRT[ 1 ]->setToDevice();
	renderBlurHorizontal( m_2downRT[ 0 ]->getSRV( 0 ), i_horizontalGlowScale );
	m_2downRT[ 0 ]->setToDevice();
	renderBlurVertical( m_2downRT[ 1 ]->getSRV( 0 ), i_verticalGlowScale );
	if ( i_outRenderTarget )
		i_outRenderTarget->setToDevice();
	else
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
	renderCombine( m_2downRT[ 0 ]->getSRV( 0 ), i_texture );
	if ( i_outRenderTarget )
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
}

void GE_PostProcess::renderGlowMask( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_mask, uint32_t i_width, uint32_t i_height,
	float i_horizontalGlowScale, float i_verticalGlowScale, float i_luminance, float i_whiteCutoff, GE_RenderTarget *i_outRenderTarget )
{
	m_upRT[ 0 ]->setToDevice();
	renderMaskWithAlpha( i_texture, i_mask );
	m_upRT[ 1 ]->setToDevice();
	renderBrightCalculate( m_upRT[ 0 ]->getSRV( 0 ), i_luminance, i_whiteCutoff );
	m_downRT[ 0 ]->setToDevice();
	renderDown( m_upRT[ 1 ]->getSRV( 0 ), i_width, i_height );
	m_2downRT[ 0 ]->setToDevice();
	renderDown( m_downRT[ 0 ]->getSRV( 0 ), ( uint32_t )m_downRT[ 0 ]->getViewPort()->Width, ( uint32_t )m_downRT[ 0 ]->getViewPort()->Height );
	m_2downRT[ 1 ]->setToDevice();
	renderBlurHorizontal( m_2downRT[ 0 ]->getSRV( 0 ), i_horizontalGlowScale );
	m_2downRT[ 0 ]->setToDevice();
	renderBlurVertical( m_2downRT[ 1 ]->getSRV( 0 ), i_verticalGlowScale );
	m_2downRT[ 1 ]->setToDevice();
	renderBlurHorizontal( m_2downRT[ 0 ]->getSRV( 0 ), i_horizontalGlowScale );
	m_2downRT[ 0 ]->setToDevice();
	renderBlurVertical( m_2downRT[ 1 ]->getSRV( 0 ), i_verticalGlowScale );
	if ( i_outRenderTarget )
		i_outRenderTarget->setToDevice();
	else
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
	renderCombine( m_2downRT[ 1 ]->getSRV( 0 ), i_texture );
	if ( i_outRenderTarget )
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
}

void GE_PostProcess::renderAtmosphereFog(ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_depth
	, GE_Mat4x4 &i_view, GE_Mat4x4 &i_viewProjectionInv, GE_Vec4 i_depth_PositionFieldFalloffHardness
	/*= GE_Vec4( GE_ConvertToUnit(4000.0f),GE_ConvertToUnit(3000.0f), 0.0f, 0.0f)*/, 
	GE_Vec4 i_height_PositionFieldFalloffHardness /*= GE_Vec4(GE_ConvertToUnit(0.0f), GE_ConvertToUnit(2000.0f), 0.0f, 0.0f)*/,
	GE_Vec3 &i_color /*= GE_Vec3(1.0f, 1.0f, 1.0f)*/, float i_intensity /*= 1.0f*/)
{
	ID3D11ShaderResourceView *srv[2];
	srv[0] = i_texture;
	srv[1] = i_depth;
	m_cPerFrame.Float4Val0.x = i_depth_PositionFieldFalloffHardness.x;
	m_cPerFrame.Float4Val0.y = i_depth_PositionFieldFalloffHardness.y;
	m_cPerFrame.Float4Val0.z = i_depth_PositionFieldFalloffHardness.z * i_depth_PositionFieldFalloffHardness.y;
	m_cPerFrame.Float4Val0.w = i_depth_PositionFieldFalloffHardness.w;

	m_cPerFrame.Float4Val1.x = i_height_PositionFieldFalloffHardness.x;
	m_cPerFrame.Float4Val1.y = i_height_PositionFieldFalloffHardness.y;
	m_cPerFrame.Float4Val1.z = i_height_PositionFieldFalloffHardness.z * i_height_PositionFieldFalloffHardness.y;
	m_cPerFrame.Float4Val1.w = i_height_PositionFieldFalloffHardness.w;

	memcpy_s(&m_cPerFrame.Float4Val2, sizeof(GE_Vec3), &i_color, sizeof(GE_Vec3));
	m_cPerFrame.Float4Val2.w = i_intensity;
	m_cPerFrame.View.transposeBy(&i_view);
	m_cPerFrame.ViewProjectionInv.transposeBy(&i_viewProjectionInv);
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource(m_perFrame, 0, NULL, &m_cPerFrame, 0, 0);
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers(0, 1, &m_perFrame);
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources(0, 2, srv);
	GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pS_AtmosphereFog, NULL, 0);
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[0] = NULL;
	srv[1] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources(0, 2, srv);
}

void GE_PostProcess::renderDepthOfField( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_depth, uint32_t i_width, uint32_t i_height,
	GE_Mat4x4 &i_view, GE_Mat4x4 &i_viewProjectionInv, bool i_auto, GE_Vec2  &i_rangeFocus, GE_RenderTarget *i_outRenderTarget )
{
	m_downRT[ 0 ]->setToDevice();
	renderDown( i_texture, i_width, i_height );
	m_downRT[ 1 ]->setToDevice();
	renderBlurHorizontal( m_downRT[ 0 ]->getSRV( 0 ) );
	m_downRT[ 0 ]->setToDevice();
	renderBlurVertical( m_downRT[ 1 ]->getSRV( 0 ) );
	if ( i_outRenderTarget )
		i_outRenderTarget->setToDevice();
	else
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );


	ID3D11ShaderResourceView *srv[ 3 ];
	srv[ 0 ] = i_texture;
	srv[ 1 ] = i_depth;
	srv[ 2 ] = m_downRT[ 0 ]->getSRV( 0 );
	m_cPerFrame.Float4Val0.x = 1.0f / i_rangeFocus.x;
	m_cPerFrame.Float4Val0.y = i_rangeFocus.y;

	m_cPerFrame.View.transposeBy( &i_view );
	m_cPerFrame.ViewProjectionInv.transposeBy( &i_viewProjectionInv );
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 3, srv );
	if ( i_auto )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_DOFAuto, NULL, 0 );
	else
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_DOF, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	srv[ 2 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 3, srv );

	if ( i_outRenderTarget )
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
}

void GE_PostProcess::renderLightScatter( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height, GE_Vec4  &i_2DLightPosition,
	float i_density, float i_weight, float i_decay, float i_exposure, float i_luminance, float i_whiteCutoff, GE_RenderTarget *i_outRenderTarget )
{
	const float vo = 0.99f;
	float h = ( float )i_width / ( float )i_height;
	h *= vo;
	if ( i_2DLightPosition.x < -vo || i_2DLightPosition.x > 1.0f + vo ||
		i_2DLightPosition.y < -vo || i_2DLightPosition.y > 1.0f + vo || i_2DLightPosition.w < 0.0f )
		return;

	m_upRT[ 0 ]->setToDevice();
	renderBrightCalculateAlpha( i_texture, i_luminance, i_whiteCutoff );
	/*m_downRT[ 0 ]->setToDevice();
	renderDown( m_upRT[ 0 ]->getSRV( 0 ), i_width / 4, i_height / 4 );
	m_downRT[ 1 ]->setToDevice();
	renderBlurVertical( m_downRT[ 0 ]->getSRV( 0 ) );
	m_downRT[ 0 ]->setToDevice();
	renderBlurHorizontal( m_downRT[ 1 ]->getSRV( 0 ) );*/

	/*m_downRT[ 1 ]->setToDevice();
	renderRadialBlurDouble( m_downRT[ 0 ]->getSRV( 0 ), i_width / 4, i_height / 4, GE_Vec2( i_2DLightPosition.x, i_2DLightPosition.y ), 10.0f );*/

	if ( i_outRenderTarget )
		i_outRenderTarget->setToDevice();
	else
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );


	ID3D11ShaderResourceView *srv[ 2 ];
	srv[ 0 ] = m_upRT[ 0 ]->getSRV( 0 );
	srv[ 1 ] = i_texture;

	m_cPerFrame.Float4Val0.x = i_2DLightPosition.x;
	m_cPerFrame.Float4Val0.y = i_2DLightPosition.y;
	m_cPerFrame.Float4Val0.z = i_density * 0.0625f;
	m_cPerFrame.Float4Val0.w = i_weight;
	m_cPerFrame.Float4Val1.x = i_decay;
	m_cPerFrame.Float4Val1.y = i_exposure;

	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_LightScatter, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );

	if ( i_outRenderTarget )
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
}

void GE_PostProcess::renderFlipUV( ID3D11ShaderResourceView *i_texture )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_FlipUV, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
}


void GE_PostProcess::renderLensDirt( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_dirt, uint32_t i_width,
	uint32_t i_height, float i_luminance /*= 2.5f*/, float i_whiteCutoff /*= 1.5f*/, float i_exp /*= 1.0f*/, float i_coreExp /*= 1.0f*/,
	float i_dirtExp /*= 1.0f*/, GE_RenderTarget *i_outRenderTarget /*= NULL */ )
{

	m_upRT[ 0 ]->setToDevice();
	renderBrightCalculateBoth( i_texture, i_luminance, i_whiteCutoff );
	m_downRT[ 0 ]->setToDevice();
	renderDown( m_upRT[ 0 ]->getSRV( 0 ), i_width, i_height );
	m_2downRT[ 0 ]->setToDevice();
	renderDown( m_downRT[ 0 ]->getSRV( 0 ), ( uint32_t )m_downRT[ 0 ]->getViewPort()->Width, ( uint32_t )m_downRT[ 0 ]->getViewPort()->Height );
	m_2downRT[ 1 ]->setToDevice();
	renderBlurHorizontal( m_2downRT[ 0 ]->getSRV( 0 ), i_exp );
	m_2downRT[ 0 ]->setToDevice();
	renderBlurVertical( m_2downRT[ 1 ]->getSRV( 0 ), i_exp );
	m_2downRT[ 1 ]->setToDevice();
	renderBlurHorizontal( m_2downRT[ 0 ]->getSRV( 0 ), i_exp );
	m_2downRT[ 0 ]->setToDevice();
	renderBlurVertical( m_2downRT[ 1 ]->getSRV( 0 ), i_exp );

	if ( i_outRenderTarget )
		i_outRenderTarget->setToDevice();
	else
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );


	ID3D11ShaderResourceView *srv[ 3 ];
	srv[ 0 ] = i_texture;
	srv[ 1 ] = m_2downRT[ 0 ]->getSRV( 0 );
	srv[ 2 ] = i_dirt;

	m_cPerFrame.Float4Val0.x = i_coreExp;
	m_cPerFrame.Float4Val0.y = i_dirtExp;

	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 3, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_LensDirt, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	srv[ 2 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 3, srv );


	if ( i_outRenderTarget )
		GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
}

void GE_PostProcess::renderFXAA( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height, uint32_t i_quality )
{
	if ( i_quality > 5 )
		i_quality = 5;
	ID3D11ShaderResourceView *srv[ 1 ];
	m_cPerFrame.Float4Val0.x = 1.0f / ( float )i_width;
	m_cPerFrame.Float4Val0.y = 1.0f / ( float )i_height;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_FXAA[ i_quality ], NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
}

void GE_PostProcess::begin()
{
	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );
	GE_Core::getSingleton().setScreenAlignQuadInputLayoutToContext();
	GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );
	ID3D11SamplerState *sam[ 3 ];
	sam[ 0 ] = GE_Core::getSingleton().getRenderStates().SamplerPointClamp;
	sam[ 1 ] = GE_Core::getSingleton().getRenderStates().SamplerLinearClamp;
	sam[ 2 ] = GE_Core::getSingleton().getRenderStates().SamplerLinearBorder;
	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 3, sam );
}
void GE_PostProcess::end()
{
	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );
}

GE_PostProcess::GE_PostProcess()
{
	memset( this, 0, sizeof( GE_PostProcess ) );
}

GE_PostProcess::~GE_PostProcess()
{
	GE_TextureManager::getSingleton().remove( &m_rgbGradientTexture );
	for ( uint32_t i = 0; i < 2; i++ )
	{
		SAFE_DELETE( m_downRT[ i ] );
		SAFE_DELETE( m_2downRT[ i ] );
		SAFE_DELETE( m_upRT[ i ] );
	}
	SAFE_DELETE( m_downRT[ 2 ] );

	for ( uint32_t i = 0; i < 5; i++ )
		SAFE_DELETE( m_hDRRT[ i ] );

	for ( uint32_t i = 0; i < 6; i++ )
		SAFE_RELEASE( m_pS_FXAA[ i ] );
	SAFE_RELEASE( m_pS_To1x1 );
	SAFE_RELEASE( m_perFrame );
	SAFE_RELEASE( m_vS );
	SAFE_RELEASE( m_pS_Grayscale );
	SAFE_RELEASE( m_pS_BrightCalculateBoth );
	SAFE_RELEASE( m_pS_Down );
	SAFE_RELEASE( m_pS_DownMulAlpha );
	SAFE_RELEASE( m_pS_BlurHorizontal );
	SAFE_RELEASE( m_pS_BlurVertical );
	SAFE_RELEASE( m_pS_BrightCalculate );
	SAFE_RELEASE( m_pS_BrightCalculateAlpha );
	SAFE_RELEASE( m_pS_Combine );
	SAFE_RELEASE( m_pS_Lerp );
	SAFE_RELEASE( m_pS_Multiple );
	SAFE_RELEASE( m_pS_Subtract );
	SAFE_RELEASE( m_pS_MaskWithAlpha );
	SAFE_RELEASE( m_pS_Border );
	SAFE_RELEASE( m_pS_Edge );
	SAFE_RELEASE( m_pS_CartoonEdge );
	SAFE_RELEASE( m_pS_RadialBlur );
	SAFE_RELEASE( m_pS_RadialBlurDouble );
	SAFE_RELEASE( m_pS_MotionBlur );
	SAFE_RELEASE(m_pS_AtmosphereFog);
	SAFE_RELEASE( m_pS_DOF );
	SAFE_RELEASE( m_pS_DOFAuto );
	SAFE_RELEASE( m_pS_LightScatter );
	SAFE_RELEASE( m_pS_Down2X2 );
	SAFE_RELEASE( m_pS_Down3X3 );
	SAFE_RELEASE( m_pS_ToneMap );
	SAFE_RELEASE( m_pS_Adaptation );
	SAFE_RELEASE( m_pS_LensDirt );
	SAFE_RELEASE( m_pS_GrowByAlpha );
	SAFE_RELEASE( m_pS_FlipUV );
	SAFE_RELEASE( m_pS_Convert );
	SAFE_RELEASE( m_pS_SSAO );
}

void GE_PostProcess::loadHDRGradientMap( const char *i_mapFileName )
{
	if ( m_rgbGradientTexture )
		GE_TextureManager::getSingleton().remove( &m_rgbGradientTexture );
	m_rgbGradientTexture = GE_TextureManager::getSingleton().createTexture( i_mapFileName );
}

void GE_PostProcess::deleteHDRGradientMap()
{
	if ( m_rgbGradientTexture )
		GE_TextureManager::getSingleton().remove( &m_rgbGradientTexture );
}

GE_Texture* GE_PostProcess::getHDRGradientMap()
{
	return m_rgbGradientTexture;
}

void GE_PostProcess::renderSSAO( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_depth,
	ID3D11ShaderResourceView *i_normal, GE_Mat4x4 &i_view, GE_Mat4x4 &i_viewProjectionInv, uint32_t i_width, uint32_t i_height, float i_radius, float i_depthValue,
	float i_normalValue, float i_intensity, float i_start, float i_end )
{
	ID3D11ShaderResourceView *srv[ 3 ];
	m_cPerFrame.Float4Val0.x = i_radius;
	m_cPerFrame.Float4Val0.y = i_depthValue;
	m_cPerFrame.Float4Val0.z = i_normalValue;
	m_cPerFrame.Float4Val0.w = i_intensity;
	m_cPerFrame.Float4Val1.x = i_start;
	m_cPerFrame.Float4Val1.y = i_end;
	m_cPerFrame.Float4Val1.z = 1.0f / i_width;
	m_cPerFrame.Float4Val1.w = 1.0f / i_height;
	m_cPerFrame.View.transposeBy( &i_view );
	m_cPerFrame.ViewProjectionInv.transposeBy( &i_viewProjectionInv );
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = i_texture;
	srv[ 1 ] = i_depth;
	srv[ 2 ] = i_normal;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 3, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_SSAO, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	srv[ 2 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 3, srv );
}

void GE_PostProcess::renderConvert( ID3D11ShaderResourceView *i_texture )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	srv[ 0 ] = i_texture;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Convert, NULL, 0 );
	GE_Core::getSingleton().renderScreenAlignQuad();
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
}
