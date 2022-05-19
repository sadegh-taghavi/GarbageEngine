#pragma once
#include "GE_Prerequisites.h"

#define GE_MAX_RENDERTARGET_VIEWS 8

class GE_RenderTarget
{
protected:
	D3D11_VIEWPORT				m_viewPort;
	ID3D11ShaderResourceView	*m_sRV[GE_MAX_RENDERTARGET_VIEWS],*m_depthSRV;
	ID3D11Texture2D				*m_rtwTexture[GE_MAX_RENDERTARGET_VIEWS];
	ID3D11RenderTargetView		*m_rtw[GE_MAX_RENDERTARGET_VIEWS];
	ID3D11Texture2D				*m_depthTexture;
	ID3D11DepthStencilView		*m_depth;
	uint32_t					m_count;
	float						m_backColor[4];
public:	
	struct Parameters
	{
		enum TYPE
		{
			BASE,
			CUBE,
			ARRAY,
		} Type;
		uint32_t	Count;
		uint32_t	MipLevel[GE_MAX_RENDERTARGET_VIEWS];
		uint32_t	ArraySize[GE_MAX_RENDERTARGET_VIEWS];
		uint32_t	Width[GE_MAX_RENDERTARGET_VIEWS];
		uint32_t	Height[GE_MAX_RENDERTARGET_VIEWS];
		DXGI_FORMAT Format[GE_MAX_RENDERTARGET_VIEWS];
		bool		HasDepth;
		Parameters(uint32_t i_count = GE_MAX_RENDERTARGET_VIEWS);
	};

	GE_RenderTarget(const Parameters &i_parameters);
	~GE_RenderTarget();
	ID3D11Texture2D				*getRTTexture(uint32_t i_index);
	ID3D11Texture2D				*getDepthTexture(){return m_depthTexture;}
	ID3D11ShaderResourceView	*getDepthSRV(){return m_depthSRV;}
	ID3D11ShaderResourceView	*getSRV(uint32_t i_index);
	ID3D11RenderTargetView		*operator [](uint32_t i_index);
	ID3D11DepthStencilView		*getDepthView( );
	void						getRenderTargetTextureDescriptor(uint32_t i_index,D3D11_TEXTURE2D_DESC &i_desc);
	void						getDepthTextureDescriptor(D3D11_TEXTURE2D_DESC &i_desc);
	D3D11_VIEWPORT				*getViewPort(){return &m_viewPort;}
	void						setViewPort(uint32_t i_width,uint32_t i_height,int i_topLeftX,int i_topLeftY,float i_maxDepth=1.0f,float i_minDepth=0.0f);
	void						setBackColor(float i_r=0,float i_g=0,float i_b=0,float i_a=1.0f);
	void						setBackColor(const float *i_color);
	void						setToDevice();
	void						setToDevice( uint32_t i_count );
	void						setDepthToDevice();
	void						clearRenderTarget();
	void						clearRenderTarget(uint32_t i_index);
	void						clearRenderTargetWithDepth(uint32_t i_index,float i_depthVal=1.0f);
	void						clearDepth(float i_depthVal=1.0f);
	void						clearStencil(uint8_t i_stencilVal=0);
	void						saveRTToFile(const char *i_path, uint32_t i_index, bool i_generateMips=false);
	void						saveDepthToFile(const char *i_path);
};