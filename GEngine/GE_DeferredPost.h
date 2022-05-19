#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"

class GE_DeferredPost
{
	ID3D11Buffer			*m_sphereVB;
	ID3D11Buffer			*m_sphereIB;
	uint32_t				m_indicesCount;
	ID3D11VertexShader		*m_vS;
	ID3D11VertexShader		*m_vS_Dir;
	ID3D11PixelShader		*m_pS_Dir[ 5 ];
	ID3D11PixelShader		*m_pS_Dir_Visualize[ 4 ];
	ID3D11PixelShader		*m_pS_Spot_NoShadow_NoFilter;
	ID3D11PixelShader		*m_pS_Spot_Shadow_NoFilter;
	ID3D11PixelShader		*m_pS_Spot_NoShadow_Filter;
	ID3D11PixelShader		*m_pS_Spot_Shadow_Filter;
	ID3D11PixelShader		*m_pS_Point_NoFilter_NoShadow;
	ID3D11PixelShader		*m_pS_Point_Filter_Shadow;
	ID3D11PixelShader		*m_pS_Point_Filter_NoShadow;
	ID3D11PixelShader		*m_pS_Point_NoFilter_Shadow;
	struct PerFrame
	{
		float				LightRange;
		float				LightAttenuation;
		float				LightSpotSpread;
		float				LightSpotSharpness;
		GE_Vec4				LightAmbient;
		GE_Vec4				LightDiffuse;
		GE_Vec4				LightSpecular;
		GE_Vec4				LightPosition;
		GE_Vec4				LightDirection;
		GE_Vec4				ViewPosition;
		GE_Vec4				ShadowOffsetBias[ 6 ];
		GE_Mat4x4			LightViewProjection[ 6 ];
		GE_Mat4x4			ViewProjection;
		GE_Mat4x4			WorldViewProjection;
		GE_Mat4x4			ViewProjectionInv;
		GE_Mat4x4			CubeFilterRotate;
	}m_cPerFrame;
	ID3D11Buffer			*m_perFrame;
	bool					m_useOne;
	static GE_DeferredPost	m_singleton;
	GE_DeferredPost();
	~GE_DeferredPost();
public:
	static GE_DeferredPost &getSingleton(){ return m_singleton; }
	void init();
	void begin();
	void end();
	void render( class GE_View *i_view, class GE_Light *i_light );
};