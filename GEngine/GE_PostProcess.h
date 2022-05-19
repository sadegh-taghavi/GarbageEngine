#pragma once

#include "GE_Prerequisites.h"
#include "GE_RenderTarget.h"
#include "GE_Math.h"

class GE_PostProcess
{
	struct PerFrame
	{
		GE_Vec4		Float4Val0;
		GE_Vec4		Float4Val1;
		GE_Vec4		Float4Val2;
		GE_Mat4x4	View;
		GE_Mat4x4	ViewProjectionInv;
		GE_Mat4x4	PreviousViewProjection;
	}m_cPerFrame;

	class GE_Texture			*m_rgbGradientTexture;

	ID3D11Buffer		*m_perFrame;
	ID3D11VertexShader	*m_vS;
	ID3D11PixelShader	*m_pS_Convert;
	ID3D11PixelShader	*m_pS_Grayscale;
	ID3D11PixelShader	*m_pS_Down;
	ID3D11PixelShader	*m_pS_DownMulAlpha;
	ID3D11PixelShader	*m_pS_BlurHorizontal;
	ID3D11PixelShader	*m_pS_BlurVertical;
	ID3D11PixelShader	*m_pS_BrightCalculateBoth;
	ID3D11PixelShader	*m_pS_BrightCalculate;
	ID3D11PixelShader	*m_pS_BrightCalculateAlpha;
	ID3D11PixelShader	*m_pS_Combine;
	ID3D11PixelShader	*m_pS_Lerp;
	ID3D11PixelShader	*m_pS_Multiple;
	ID3D11PixelShader	*m_pS_Subtract;
	ID3D11PixelShader	*m_pS_MaskWithAlpha;
	ID3D11PixelShader	*m_pS_Border;
	ID3D11PixelShader	*m_pS_Edge;
	ID3D11PixelShader	*m_pS_CartoonEdge;
	ID3D11PixelShader	*m_pS_RadialBlur;
	ID3D11PixelShader	*m_pS_RadialBlurDouble;
	ID3D11PixelShader	*m_pS_MotionBlur;
	ID3D11PixelShader	*m_pS_AtmosphereFog;
	ID3D11PixelShader	*m_pS_DOF;
	ID3D11PixelShader	*m_pS_DOFAuto;
	ID3D11PixelShader	*m_pS_LightScatter;
	ID3D11PixelShader	*m_pS_Down2X2;
	ID3D11PixelShader	*m_pS_Down3X3;
	ID3D11PixelShader	*m_pS_ToneMap;
	ID3D11PixelShader	*m_pS_Adaptation;
	ID3D11PixelShader	*m_pS_LensDirt;
	ID3D11PixelShader	*m_pS_FXAA[ 6 ];
	ID3D11PixelShader	*m_pS_GrowByAlpha;
	ID3D11PixelShader	*m_pS_To1x1;
	ID3D11PixelShader	*m_pS_FlipUV;
	ID3D11PixelShader	*m_pS_SSAO;
	GE_RenderTarget		*m_downRT[ 3 ];
	GE_RenderTarget		*m_2downRT[ 2 ];
	GE_RenderTarget		*m_upRT[ 2 ];
	GE_RenderTarget		*m_hDRRT[ 5 ];
	bool				m_currentAdaptationTarget;
	bool				m_enableHDR;
	float				m_adaptationRate;
	float				m_whiteCutoff;
	float				m_luminanceMax;
	float				m_luminanceMin;
	float				m_gamma;

	GE_PostProcess();
	~GE_PostProcess();
	static GE_PostProcess m_singleton;
public:
	static GE_PostProcess &getSingleton(){ return m_singleton; }
	void resize();
	void loadHDRGradientMap( const char *i_mapFileName );
	void deleteHDRGradientMap();
	class GE_Texture* getHDRGradientMap();
	void setEnableHDR( bool i_enable ){ m_enableHDR = i_enable; }
	bool &isEnableHDR(){ return m_enableHDR; }
	void setHDRAdaptationRate( float i_adaptationRate ){ m_adaptationRate = i_adaptationRate; }
	float &getHDRAdaptationRate(){ return m_adaptationRate; }
	void  setHDRWhiteCutoff( float i_whiteCutoff ){ m_whiteCutoff = i_whiteCutoff; }
	float &getHDRWhiteCutoff(){ return m_whiteCutoff; }
	void  setHDRLuminanceMax( float i_luminanceMax ){ m_luminanceMax = i_luminanceMax; }
	float &getHDRLuminanceMax(){ return m_luminanceMax; }
	void  setHDRLuminanceMin( float i_luminanceMin ){ m_luminanceMin = i_luminanceMin; }
	float &getHDRLuminanceMin(){ return m_luminanceMin; }
	void  setHDRGamma( float i_gamma ){ m_gamma = i_gamma; }
	float &getHDRGamma(){ return m_gamma; }

	GE_RenderTarget *getHDRTarget(){ return m_hDRRT[ 0 ]; }
	void beginHDR();
	void endHDR( GE_RenderTarget *i_outRenderTarget = NULL, bool i_useRate = true );
	ID3D11ShaderResourceView *getHDRLuminanceSRV();
	ID3D11ShaderResourceView *getHDRLuminanceAdaptationSRV();

	void init();
	void begin();
	void end();

	void renderFlipUV( ID3D11ShaderResourceView *i_texture );

	void renderGrayscale( ID3D11ShaderResourceView *i_texture, float i_grayscaleValue = 1.0f );

	void renderGrowByAlpha( ID3D11ShaderResourceView *i_texture );

	void renderDown( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height );

	void renderDownWithAlpha( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height );

	void renderBlurHorizontal( ID3D11ShaderResourceView *i_texture, float i_bloomScale = 1.0f );

	void renderBlurVertical( ID3D11ShaderResourceView *i_texture, float i_bloomScale = 1.0f );

	void renderBrightCalculate( ID3D11ShaderResourceView *i_texture, float i_luminance = 2.5f, float i_whiteCutoff = 1.5f );

	void renderBrightCalculateAlpha( ID3D11ShaderResourceView *i_texture, float i_luminance = 2.5f, float i_whiteCutoff = 1.5f );

	void renderBrightCalculateBoth( ID3D11ShaderResourceView *i_texture, float i_luminance = 2.5f, float i_whiteCutoff = 1.5f );

	void renderCombine( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_texture1 );

	void renderLerp( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_texture1, float i_lerp = 0.5f );

	void renderMultiple( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_texture1 );

	void renderSubtract( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_texture1 );

	void renderBorder( ID3D11ShaderResourceView *i_texture, float i_sharpness = -0.1f, float i_shape = -0.5f, float i_limit = -3.0f );

	void renderEdge( ID3D11ShaderResourceView *i_texture, GE_Vec3 &i_edgeColor = GE_Vec3( 1.0f, 1.0f, 1.0f ), float i_threshold = 0.8f );

	void renderCartoonEdge( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_edgeTexture, GE_Vec3 &i_edgeColor = GE_Vec3( 0.0f, 0.0f, 0.0f ),
		float i_threshold = 0.8f );

	void renderEdgeGlow( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_edgeTexture, uint32_t i_width, uint32_t i_height,
		GE_Vec3 &i_edgeColor = GE_Vec3( 1.0f, 1.0f, 1.0f ), float i_horizontalGlowScale = 1.5f, float i_verticalGlowScale = 1.5f, float i_threshold = 0.8f,
		GE_RenderTarget *i_outRenderTarget = NULL );

	void renderMaskWithAlpha( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_texture1 );

	void renderRadialBlur( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height, GE_Vec2 &i_radialPosition = GE_Vec2( 0.5f, 0.5f )
		, float i_radialFocus = 5.0f );

	void renderRadialBlurDouble( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height, GE_Vec2 &i_radialPosition = GE_Vec2( 0.5f, 0.5f )
		, float i_radialFocus = 5.0f );

	void renderMotionBlur( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_depth
		, GE_Mat4x4 &i_viewProjectionInv, GE_Mat4x4 &i_previousViewProjection, float i_damp = 100.0f, float i_max = 0.03f );

	void renderBlur( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height, float i_verticalGlowScale = 1.0f, float i_horizontalGlowScale = 1.0f,
		float i_lerp = 1.0f, GE_RenderTarget *i_outRenderTarget = NULL );

	void renderGlow( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height, float i_horizontalGlowScale = 1.0f
		, float i_verticalGlowScale = 1.0f, float i_luminance = 2.5f, float i_whiteCutoff = 1.5f, GE_RenderTarget *i_outRenderTarget = NULL );

	void renderGlowMask( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_mask, uint32_t i_width, uint32_t i_height, float i_horizontalGlowScale = 1.0f
		, float i_verticalGlowScale = 1.0f, float i_luminance = 2.5f, float i_whiteCutoff = 1.5f, GE_RenderTarget *i_outRenderTarget = NULL );

	void renderDepthOfField( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_depth, uint32_t i_width, uint32_t i_height,
		GE_Mat4x4 &i_view, GE_Mat4x4 &i_viewProjectionInv, bool i_auto = false, GE_Vec2  &i_rangeFocus = GE_Vec2( 100.0f, 100.0f ), GE_RenderTarget *i_outRenderTarget = NULL );

	void renderAtmosphereFog(ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_depth,
		GE_Mat4x4 &i_view, GE_Mat4x4 &i_viewProjectionInv, GE_Vec4 i_depth_PositionFieldFalloffHardness = 
		GE_Vec4( GE_ConvertToUnit(4000.0f),GE_ConvertToUnit(2000.0f), 0.0f, 0.0f),
		GE_Vec4 i_height_PositionFieldFalloffHardness =
		GE_Vec4(GE_ConvertToUnit(0.0f), GE_ConvertToUnit(2000.0f), 0.0f, 0.0f),
		GE_Vec3 &i_color = GE_Vec3(0.9f, 0.9f, 1.0f), float i_intensity = 1.0f);

	void renderLightScatter( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height, GE_Vec4  &i_2DLightPosition,
		float i_density = 1.0f, float i_weight = 0.1f, float i_decay = 1.0f, float i_exposure = 1.0f,
		float i_luminance = 2.5f, float i_whiteCutoff = 1.5f, GE_RenderTarget *i_outRenderTarget = NULL );

	void renderLensDirt( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_dirt, uint32_t i_width, uint32_t i_height
		, float i_luminance = 2.5f, float i_whiteCutoff = 1.5f, float i_exp = 1.0f, float i_coreExp = 1.0f, float i_dirtExp = 1.0f, GE_RenderTarget *i_outRenderTarget = NULL );

	void renderSSAO( ID3D11ShaderResourceView *i_texture, ID3D11ShaderResourceView *i_depth, 
		ID3D11ShaderResourceView *i_normal, GE_Mat4x4 &i_view, GE_Mat4x4 &i_viewProjectionInv, 
		uint32_t i_width, uint32_t i_height, float i_radius, float i_depthValue, float i_normalValue,
		float i_intensity, float i_start, float i_end );

	void renderConvert( ID3D11ShaderResourceView *i_texture );

	void renderFXAA( ID3D11ShaderResourceView *i_texture, uint32_t i_width, uint32_t i_height, uint32_t i_quality = 3 );

};