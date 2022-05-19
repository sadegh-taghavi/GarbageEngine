#include "GE_RenderTarget.h"
#include "GE_Core.h"
#include <DirectXTex.h>

ID3D11ShaderResourceView	*GE_RenderTarget::getSRV(uint32_t i_index)
{
	return  m_sRV[i_index];
}
ID3D11RenderTargetView	*GE_RenderTarget::operator [](uint32_t i_index)
{
	return  m_rtw[i_index];
}

void GE_RenderTarget::setDepthToDevice()
{
	GE_Core::getSingleton().getImmediateContext()->RSSetViewports(1, &m_viewPort);
	ID3D11RenderTargetView * nullView[] = { NULL };
	GE_Core::getSingleton().getImmediateContext()->OMSetRenderTargets(1, nullView, m_depth);
}

GE_RenderTarget::GE_RenderTarget(const Parameters &i_parameters)
{

	m_depthSRV = NULL;
	m_depth = NULL;
	m_depthTexture = NULL;
	for (uint32_t i = 0; i > GE_MAX_RENDERTARGET_VIEWS; i++)
	{
		m_sRV[i] = NULL;
		m_rtwTexture[i] = NULL;
		m_rtw[i] = NULL;
	}

	m_count = i_parameters.Count;
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_RENDER_TARGET_VIEW_DESC RtwDesc;
	RtlZeroMemory(&texDesc, sizeof(texDesc));
	RtlZeroMemory(&srvDesc, sizeof(srvDesc));

	for (uint32_t i = 0; i < i_parameters.Count; i++)
	{
		texDesc.Width = i_parameters.Width[i];
		texDesc.Height = i_parameters.Height[i];
		if (i_parameters.MipLevel[i])
		{
			texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			texDesc.MipLevels = 0;
			srvDesc.TextureCube.MipLevels = srvDesc.Texture2D.MipLevels = srvDesc.Texture2DArray.MipLevels = i_parameters.MipLevel[i];
		}
		else
		{
			texDesc.MiscFlags = 0;
			texDesc.MipLevels = 1;
			srvDesc.TextureCube.MipLevels = srvDesc.Texture2D.MipLevels = srvDesc.Texture2DArray.MipLevels = 1;
		}

		if (i_parameters.Type == Parameters::CUBE)
		{
			texDesc.ArraySize = 6;
			texDesc.MiscFlags |= D3D10_RESOURCE_MISC_TEXTURECUBE;

			RtwDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		}
		else if (i_parameters.Type == Parameters::ARRAY)
		{
			texDesc.ArraySize = i_parameters.ArraySize[i];
			RtwDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		}
		else
		{
			texDesc.ArraySize = 1;
			RtwDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		}
		texDesc.Format = i_parameters.Format[i];
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		RtwDesc.Format = texDesc.Format;
		RtwDesc.Texture2D.MipSlice = 0;
		RtwDesc.Texture2DArray.MipSlice = 0;
		RtwDesc.Texture2DArray.ArraySize = 6;
		RtwDesc.Texture2DArray.FirstArraySlice = 0;

		srvDesc.Format = texDesc.Format;

		GE_Core::getSingleton().getDevice()->CreateTexture2D(&texDesc, NULL, &m_rtwTexture[i]);
		GE_Core::getSingleton().getDevice()->CreateRenderTargetView(m_rtwTexture[i], &RtwDesc, &m_rtw[i]);
		GE_Core::getSingleton().getDevice()->CreateShaderResourceView((ID3D11Resource*)m_rtwTexture[i], &srvDesc, &m_sRV[i]);
	}
	setBackColor();

	m_viewPort.Height = (float)texDesc.Height;
	m_viewPort.Width = (float)texDesc.Width;
	m_viewPort.TopLeftX = m_viewPort.TopLeftY = 0;
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	if (!i_parameters.HasDepth)
		return;
	
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.Flags = 0;

	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;

	texDesc.Width = (uint32_t)m_viewPort.Width;
	texDesc.Height = (uint32_t)m_viewPort.Height;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;

	if (i_parameters.Type == Parameters::CUBE)
	{
		texDesc.ArraySize = 6;
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Texture2DArray.ArraySize = 6;
		dsvDesc.Texture2DArray.FirstArraySlice = 0;
		dsvDesc.Texture2DArray.MipSlice = 0;
		dsvDesc.Flags = 0;

		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = 1;
	}
	else
	{
		texDesc.ArraySize = 1;
		texDesc.MiscFlags = 0;

		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
	}

	GE_Core::getSingleton().getDevice()->CreateTexture2D(&texDesc, NULL, &m_depthTexture);
	GE_Core::getSingleton().getDevice()->CreateShaderResourceView((ID3D11Resource*)m_depthTexture, &srvDesc, &m_depthSRV);
	GE_Core::getSingleton().getDevice()->CreateDepthStencilView(m_depthTexture, &dsvDesc, &m_depth);
}


ID3D11Texture2D *GE_RenderTarget::getRTTexture(uint32_t i_index)
{
	return m_rtwTexture[i_index];
}
void GE_RenderTarget::setViewPort(uint32_t i_width, uint32_t i_height, int i_topLeftX, int i_topLeftY, float i_maxDepth, float i_minDepth)
{
	m_viewPort.Width = (float)i_width;
	m_viewPort.Height = (float)i_height;
	m_viewPort.TopLeftX = (float)i_topLeftX;
	m_viewPort.TopLeftY = (float)i_topLeftY;
	m_viewPort.MaxDepth = i_maxDepth;
	m_viewPort.MinDepth = i_minDepth;
}

void GE_RenderTarget::setBackColor(float i_r, float i_g, float i_b, float i_a)
{
	m_backColor[0] = i_r;
	m_backColor[1] = i_g;
	m_backColor[2] = i_b;
	m_backColor[3] = i_a;
}

void GE_RenderTarget::setBackColor(const float *i_color)
{
	m_backColor[0] = i_color[0];
	m_backColor[1] = i_color[1];
	m_backColor[2] = i_color[2];
	m_backColor[3] = i_color[3];
}
void GE_RenderTarget::getRenderTargetTextureDescriptor(uint32_t i_index, D3D11_TEXTURE2D_DESC &i_desc)
{
	m_rtwTexture[i_index]->GetDesc(&i_desc);
}
void GE_RenderTarget::getDepthTextureDescriptor(D3D11_TEXTURE2D_DESC &i_desc)
{
	if (m_depth)
		m_depthTexture->GetDesc(&i_desc);
}


void GE_RenderTarget::setToDevice()
{
	GE_Core::getSingleton().getImmediateContext()->OMSetRenderTargets(m_count, (ID3D11RenderTargetView *const*)&m_rtw, m_depth);
	GE_Core::getSingleton().getImmediateContext()->RSSetViewports(1, &m_viewPort);
}

void GE_RenderTarget::setToDevice(uint32_t i_count)
{
	GE_Core::getSingleton().getImmediateContext()->OMSetRenderTargets(i_count, (ID3D11RenderTargetView *const*)&m_rtw, m_depth);
	GE_Core::getSingleton().getImmediateContext()->RSSetViewports(1, &m_viewPort);
}

void GE_RenderTarget::clearRenderTarget()
{
	for (unsigned int i = 0; i < m_count; i++)
		GE_Core::getSingleton().getImmediateContext()->ClearRenderTargetView(m_rtw[i], m_backColor);
}

void GE_RenderTarget::clearRenderTarget(uint32_t i_index)
{
	GE_Core::getSingleton().getImmediateContext()->ClearRenderTargetView(m_rtw[i_index], m_backColor);
}
void GE_RenderTarget::clearRenderTargetWithDepth(uint32_t i_index, float i_depthVal)
{
	GE_Core::getSingleton().getImmediateContext()->ClearRenderTargetView(m_rtw[i_index], m_backColor);
	if (m_depth)
		GE_Core::getSingleton().getImmediateContext()->ClearDepthStencilView(m_depth, D3D11_CLEAR_DEPTH, i_depthVal, 0);
}

void GE_RenderTarget::clearDepth(float i_depthVal)
{
	if (m_depth)
		GE_Core::getSingleton().getImmediateContext()->ClearDepthStencilView(m_depth, D3D11_CLEAR_DEPTH, i_depthVal, 0);
}

void GE_RenderTarget::clearStencil(uint8_t stencilVal)
{
	if (m_depth)
		GE_Core::getSingleton().getImmediateContext()->ClearDepthStencilView(m_depth, D3D11_CLEAR_STENCIL, 1.0f, stencilVal);
}

void GE_RenderTarget::saveRTToFile(const char *i_path, uint32_t i_index, bool i_generateMips)
{
	std::string postFix(i_path + (strlen(i_path) - 3));
	std::transform(postFix.begin(), postFix.end(), postFix.begin(), ::tolower);
	CString str = i_path;
	DirectX::ScratchImage img;
	DirectX::CaptureTexture(GE_Core::getSingleton().getDevice(), GE_Core::getSingleton().getImmediateContext(), m_rtwTexture[i_index], img);

	if (postFix == "dds")
	{
		if (i_generateMips && img.GetMetadata().width > 1 && img.GetMetadata().height > 1)
		{
			DirectX::ScratchImage scrachMips;
			DirectX::GenerateMipMaps(img.GetImages(), img.GetImageCount(), img.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, scrachMips);
			DirectX::SaveToDDSFile(scrachMips.GetImages(), scrachMips.GetImageCount(), scrachMips.GetMetadata(), 0, str.GetString());
		}
		else
			DirectX::SaveToDDSFile(img.GetImages(), img.GetImageCount(), img.GetMetadata(), 0, str.GetString());

	}
	else if (postFix == "tga")
		DirectX::SaveToTGAFile(*img.GetImage(0, 0, 0), str.GetString());
}

void GE_RenderTarget::saveDepthToFile(const char *i_path)
{
	if (!m_depth)
		return;
	std::string postFix(i_path + (strlen(i_path) - 3));
	std::transform(postFix.begin(), postFix.end(), postFix.begin(), ::tolower);
	CString str = i_path;
	DirectX::ScratchImage img;
	DirectX::CaptureTexture(GE_Core::getSingleton().getDevice(), GE_Core::getSingleton().getImmediateContext(), m_depthTexture, img);
	if (postFix == "dds")
		DirectX::SaveToDDSFile(img.GetImages(), 1, img.GetMetadata(), 0, str.GetString());
	else if (postFix == "tga")
		DirectX::SaveToTGAFile(*img.GetImage(0, 0, 0), str.GetString());
}

GE_RenderTarget::~GE_RenderTarget()
{
	SAFE_RELEASE(m_depthSRV);
	SAFE_RELEASE(m_depth);
	SAFE_RELEASE(m_depthTexture);
	for (uint32_t i = 0; i < m_count; i++)
	{
		SAFE_RELEASE(m_sRV[i]);
		SAFE_RELEASE(m_rtw[i]);
		SAFE_RELEASE(m_rtwTexture[i]);

	}
}

ID3D11DepthStencilView	* GE_RenderTarget::getDepthView()
{
	return m_depth;
}

GE_RenderTarget::Parameters::Parameters(uint32_t i_count/*=GE_MAX_RENDERTARGET_VIEWS*/)
{
	HasDepth = true;
	Type = BASE;
	Count = i_count;
	for (uint32_t i = 0; i < GE_MAX_RENDERTARGET_VIEWS; i++)
	{
		ArraySize[i] = 1;
		MipLevel[i] = 0;
		Width[i] = 0;
		Height[i] = 0;
		Format[i] = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
}
