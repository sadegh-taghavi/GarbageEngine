#include "GE_Core.h"
#include "GE_Prerequisites.h"
#include "GE_Math.h"
#include "GE_Utility.h"
#include "GE_RenderTarget.h"

using namespace DirectX;
using namespace std;

void GE_SwapChain::copyBackBufferToSRV()
{
	GE_Core::getSingleton().getImmediateContext()->CopyResource( BackBufferMedium, BackBuffer );
}

void GE_SwapChain::copyDepthToSRV()
{
	GE_Core::getSingleton().getImmediateContext()->CopyResource( DepthStencilCopy, DepthStencil );
}

void GE_SwapChain::resize()
{
	release();
	GE_Core::getSingleton().createSwapChain( *this );
}

GE_Core GE_Core::m_singleton;

GE_Core::GE_Core()
{
	m_currentSwapChain = NULL;
	m_pDXGIDevice = NULL;
	m_pDXGIAdapter = NULL;
	m_pIDXGIFactory = NULL;

	for(UINT32 i=0; i<GE_MAX_RENDERTARGET_VIEWS; i++)
		m_NULLRT[ i ] = NULL;

	LARGE_INTEGER qf;
	QueryPerformanceFrequency( &qf );
	m_queryFrec = qf.QuadPart;

	m_isInit = false;
	m_drawCallsCount	= 0;
	m_polyCount			= 0;
	m_testPolyCount = 0;
	m_testDrawCallsCount = 0;
	uint32_t createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	uint32_t numFeatureLevels = ARRAYSIZE( featureLevels );
	D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, numFeatureLevels,D3D11_SDK_VERSION, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext );
	createQuad();
	
	m_pd3dDevice->QueryInterface( __uuidof( IDXGIDevice ), (void **)&m_pDXGIDevice );
	m_pDXGIDevice->GetParent( __uuidof( IDXGIAdapter ), (void **)&m_pDXGIAdapter );	
	m_pDXGIAdapter->GetParent( __uuidof( IDXGIFactory ), (void **)&m_pIDXGIFactory );

/*	ID3D11Debug *d3dDebug = nullptr;
	if( SUCCEEDED( m_pd3dDevice->QueryInterface( __uuidof(ID3D11Debug), (void**)&d3dDebug ) ) )
	{
		ID3D11InfoQueue *d3dInfoQueue = nullptr;
		if( SUCCEEDED( d3dDebug->QueryInterface( __uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue ) ) )
		{
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity( D3D11_MESSAGE_SEVERITY_CORRUPTION, true );
			d3dInfoQueue->SetBreakOnSeverity( D3D11_MESSAGE_SEVERITY_ERROR, true );
#endif

			D3D11_MESSAGE_ID hide [] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				D3D11_MESSAGE_ID_LIVE_OBJECT_SUMMARY,
				D3D11_MESSAGE_ID_LIVE_BLENDSTATE,
				D3D11_MESSAGE_ID_LIVE_BUFFER,
				D3D11_MESSAGE_ID_LIVE_CLASSINSTANCE,
				D3D11_MESSAGE_ID_LIVE_CONTEXT,
				D3D11_MESSAGE_ID_LIVE_COUNTER,
				D3D11_MESSAGE_ID_LIVE_DEPTHSTENCILSTATE,
				D3D11_MESSAGE_ID_LIVE_RASTERIZERSTATE,
				D3D11_MESSAGE_ID_LIVE_QUERY,
				D3D11_MESSAGE_ID_LIVE_TEXTURE2D,
				// Add more message IDs here as needed
			};

			D3D11_INFO_QUEUE_FILTER filter;
			memset( &filter, 0, sizeof( filter ) );
			filter.DenyList.NumIDs = _countof( hide );
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries( &filter );
			d3dInfoQueue->Release();
		}
		d3dDebug->ReportLiveDeviceObjects( D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL );
		d3dDebug->Release();
	}*/
	
}

void GE_Core::createRenderStates( uint32_t i_anisotropicLevel )
{
	D3D11_SAMPLER_DESC sampDesc;
	RtlZeroMemory( &sampDesc, sizeof( D3D11_SAMPLER_DESC ) );
	if ( i_anisotropicLevel )
	{
		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampDesc.MaxAnisotropy = i_anisotropicLevel;
	}
	else
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	SAFE_RELEASE( m_renderStates.SamplerLinearWrap );
	m_pd3dDevice->CreateSamplerState( &sampDesc, &m_renderStates.SamplerLinearWrap );
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	SAFE_RELEASE( m_renderStates.SamplerLinearMirror );
	m_pd3dDevice->CreateSamplerState( &sampDesc, &m_renderStates.SamplerLinearMirror );
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SAFE_RELEASE( m_renderStates.SamplerLinearClamp );
	m_pd3dDevice->CreateSamplerState( &sampDesc, &m_renderStates.SamplerLinearClamp );
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	SAFE_RELEASE( m_renderStates.SamplerLinearBorder );
	m_pd3dDevice->CreateSamplerState( &sampDesc, &m_renderStates.SamplerLinearBorder );
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.BorderColor[ 0 ] = 1.0f;
	sampDesc.BorderColor[ 1 ] = 1.0f;
	sampDesc.BorderColor[ 2 ] = 1.0f;
	sampDesc.BorderColor[ 3 ] = 1.0f;
	SAFE_RELEASE( m_renderStates.SamplerLinearUWrapVBorderWhite );
	m_pd3dDevice->CreateSamplerState( &sampDesc, &m_renderStates.SamplerLinearUWrapVBorderWhite );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SAFE_RELEASE( m_renderStates.SamplerPointClamp );
	m_pd3dDevice->CreateSamplerState( &sampDesc, &m_renderStates.SamplerPointClamp );

	if ( m_renderStates.DepthStencilStateDisable )
		return;
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	RtlZeroMemory( &dsDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	dsDesc.DepthEnable = FALSE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	m_pd3dDevice->CreateDepthStencilState( &dsDesc, &m_renderStates.DepthStencilStateDisable );
	dsDesc.DepthEnable = TRUE;
	m_pd3dDevice->CreateDepthStencilState( &dsDesc, &m_renderStates.DepthStencilStateDisableZWrite );
	
	D3D11_RASTERIZER_DESC rasDesc;
	RtlZeroMemory( &rasDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	rasDesc.FillMode = D3D11_FILL_SOLID;
	rasDesc.AntialiasedLineEnable = false;
	rasDesc.CullMode = D3D11_CULL_BACK;
	rasDesc.FrontCounterClockwise = TRUE;
	rasDesc.FillMode = D3D11_FILL_WIREFRAME;
	m_pd3dDevice->CreateRasterizerState( &rasDesc, &m_renderStates.RasterStateWireframe );
	rasDesc.FillMode = D3D11_FILL_SOLID;
	m_pd3dDevice->CreateRasterizerState( &rasDesc, &m_renderStates.RasterStateDefault );
	m_pImmediateContext->RSSetState( m_renderStates.RasterStateDefault );
	rasDesc.CullMode = D3D11_CULL_NONE;
	m_pd3dDevice->CreateRasterizerState( &rasDesc, &m_renderStates.RasterStateDisableCull );
	rasDesc.FillMode = D3D11_FILL_WIREFRAME;
	m_pd3dDevice->CreateRasterizerState(&rasDesc, &m_renderStates.RasterStateWireframeDisableCull);

	D3D11_BLEND_DESC BlendStateDesc;
	RtlZeroMemory( &BlendStateDesc, sizeof( D3D11_BLEND_DESC ) );

	BlendStateDesc.AlphaToCoverageEnable = FALSE;
	BlendStateDesc.RenderTarget[ 0 ].BlendEnable = FALSE;
	BlendStateDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendStateDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendStateDesc.RenderTarget[ 0 ].BlendOp =  D3D11_BLEND_OP_ADD;
    BlendStateDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
    BlendStateDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ONE;
    BlendStateDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendStateDesc.RenderTarget[ 0 ].RenderTargetWriteMask = 0;
	
	m_pd3dDevice->CreateBlendState( &BlendStateDesc, &m_renderStates.BlendStateDisableColorWrite );
	BlendStateDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_pd3dDevice->CreateBlendState( &BlendStateDesc, &m_renderStates.BlendStateDisableAlpha );
	BlendStateDesc.RenderTarget[ 0 ].BlendEnable = TRUE;
	m_pd3dDevice->CreateBlendState( &BlendStateDesc, &m_renderStates.BlendStateAlpha );
	BlendStateDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_ONE;
	m_pd3dDevice->CreateBlendState( &BlendStateDesc, &m_renderStates.BlendStateAlphaOne );
	BlendStateDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_ZERO;
	BlendStateDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_SRC_COLOR;
	m_pd3dDevice->CreateBlendState( &BlendStateDesc, &m_renderStates.BlendStateColorMultiply );
}

void GE_Core::createQuad()
{
	m_quadVertices[ 0 ].Position = GE_Vec3( -1.0f, 1.0f, 1.0f );
	m_quadVertices[ 1 ].Position = GE_Vec3( -1.0f, -1.0f, 1.0f );
	m_quadVertices[ 2 ].Position = GE_Vec3( 1.0f, 1.0f, 1.0f );
	m_quadVertices[ 3 ].Position = GE_Vec3( 1.0f, -1.0f, 1.0f );

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = sizeof( quadVertexBuffer ) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER ;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = m_quadVertices;
	m_pd3dDevice->CreateBuffer( &bd, &InitData, &m_quadVB );

	m_quadStrid = sizeof( quadVertexBuffer );

	const D3D11_INPUT_ELEMENT_DESC _QVD[] =
	{
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	_GE_FileChunk fc;
	fc.load( string( GE_SHADER_PATH ) + "GE_QuadVertexLayout.cso" );

	m_quadVertexDeclaration = NULL;
	uint32_t emsize = sizeof( _QVD ) / sizeof( _QVD[ 0 ] );
	m_pd3dDevice->CreateInputLayout( _QVD, emsize, fc.Data, fc.Len, &m_quadVertexDeclaration );
	m_quadOffset = 0;
}

GE_Core::~GE_Core()
{
	SAFE_RELEASE( m_pDXGIDevice );
	SAFE_RELEASE( m_pDXGIAdapter );
	SAFE_RELEASE( m_pIDXGIFactory );

	if( m_pImmediateContext ) m_pImmediateContext->ClearState();
	SAFE_RELEASE( m_quadVertexDeclaration );
	SAFE_RELEASE( m_quadVB );
	SAFE_RELEASE( m_pImmediateContext );
	SAFE_RELEASE( m_pd3dDevice );
}

void GE_Core::setRTToNull()
{
	m_pImmediateContext->OMSetRenderTargets( 8, m_NULLRT, NULL );
}

bool GE_Core::createSwapChain( GE_SwapChain& i_swapChain )
{	
	HRESULT hr;
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferDesc.Width = (uint32_t)i_swapChain.ViewPort.Width;
	sd.BufferDesc.Height = (uint32_t)i_swapChain.ViewPort.Height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = i_swapChain.RefreshRateNumerator;
	sd.BufferDesc.RefreshRate.Denominator = i_swapChain.RefreshRateDenominator;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = i_swapChain.Hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = (BOOL) i_swapChain.Windowed;
	
	 hr = m_pIDXGIFactory->CreateSwapChain( m_pd3dDevice, &sd, &i_swapChain.SwapChain );
	
	if( FAILED( hr ) )
		return false;
	
	hr = i_swapChain.SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&i_swapChain.BackBuffer );
	if( FAILED( hr ) )
		return false;

	hr = m_pd3dDevice->CreateRenderTargetView( i_swapChain.BackBuffer, NULL, &i_swapChain.RenderTargetView );
	if( FAILED( hr ) )
		return false;

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory( &descDepth, sizeof( descDepth ) );
	descDepth.Width = (uint32_t) i_swapChain.ViewPort.Width;
	descDepth.Height = (uint32_t) i_swapChain.ViewPort.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = m_pd3dDevice->CreateTexture2D( &descDepth, NULL, &i_swapChain.DepthStencil );
	if( FAILED( hr ) )
		return false;
	descDepth.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	hr = m_pd3dDevice->CreateTexture2D( &descDepth, NULL, &i_swapChain.DepthStencilCopy );
	if( FAILED( hr ) )
		return false;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory( &descDSV, sizeof(descDSV) );
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pd3dDevice->CreateDepthStencilView( i_swapChain.DepthStencil, &descDSV, &i_swapChain.DepthStencilView );

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = sd.BufferDesc.Width ;
	desc.Height = sd.BufferDesc.Height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags =  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
	
	m_pd3dDevice->CreateTexture2D( &desc, NULL, &i_swapChain.BackBufferMedium );
	
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	RtlZeroMemory( &SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
    SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D ;
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_pd3dDevice->CreateShaderResourceView( (ID3D11Resource *)i_swapChain.BackBufferMedium, &SRVDesc, &i_swapChain.RenderTargetViewSRV );

	SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	m_pd3dDevice->CreateShaderResourceView( (ID3D11Resource *)i_swapChain.DepthStencil, &SRVDesc, &i_swapChain.DepthStencilViewSRV );
	m_pd3dDevice->CreateShaderResourceView( (ID3D11Resource *)i_swapChain.DepthStencilCopy, &SRVDesc, &i_swapChain.DepthStencilViewSRVCopy );


	if( FAILED( hr ) )
		return false;
	m_currentSwapChain = &i_swapChain;
	return true;
}

void GE_Core::setScreenAlignQuadVertexDeclarationToContext()
{
	m_pImmediateContext->IASetInputLayout( m_quadVertexDeclaration );
}

void GE_Core::setScreenAlignQuadInputLayoutToContext()
{
	m_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	m_pImmediateContext->IASetInputLayout( m_quadVertexDeclaration );
}

void GE_Core::renderScreenAlignQuad()
{
	++m_drawCallsCount;
	m_polyCount += 2;
	m_pImmediateContext->IASetVertexBuffers( 0, 1, &m_quadVB, &m_quadStrid, &m_quadOffset );
	m_pImmediateContext->Draw( 4, 0);
}

void GE_Core::renderScreenAlignQuad( uint32_t i_numberOfInstance )
{
	++m_drawCallsCount;
	m_polyCount += 2;
	m_pImmediateContext->IASetVertexBuffers( 0, 1, &m_quadVB, &m_quadStrid, &m_quadOffset );
	m_pImmediateContext->DrawInstanced( 4, i_numberOfInstance, 0, 0 );
}

ID3D11Buffer * GE_Core::createVertexBuffer( const void* i_vertices,uint32_t i_sizeofVertex,uint32_t i_verticesCount,bool i_isConst /*= true */ )
{
	ID3D11Buffer *	g_pVertexBuffer;
	D3D11_SUBRESOURCE_DATA InitData;
	D3D11_BUFFER_DESC bd;
	if(i_isConst)
	{
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0;
	}
	else
	{
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	bd.ByteWidth = i_sizeofVertex * i_verticesCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER ;
	bd.MiscFlags = 0;
	InitData.pSysMem = i_vertices;
	m_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );
	return g_pVertexBuffer;
}

ID3D11Buffer * GE_Core::createIndexBuffer( const void* i_indices,uint32_t i_sizeofIndex,uint32_t i_indicesCount,bool i_isConst /*= true */ )
{
	ID3D11Buffer *	g_pIndexBuffer;
	D3D11_SUBRESOURCE_DATA InitData;
	D3D11_BUFFER_DESC bd;

	if(i_isConst)
	{
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0;
	}
	else
	{
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	bd.ByteWidth = i_sizeofIndex * i_indicesCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.MiscFlags = 0;
	InitData.pSysMem = i_indices;
	m_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pIndexBuffer );
	return g_pIndexBuffer;
}

void GE_Core::renderTriangleListTest( ID3D11Buffer *i_vertices, uint32_t i_sizeofVertex, ID3D11Buffer *i_indices, uint32_t &i_indicesCount )
{
	++m_testDrawCallsCount;
	m_testPolyCount += i_indicesCount / 3;
	uint32_t offset = 0;
	m_pImmediateContext->IASetVertexBuffers( 0, 1, &i_vertices, &i_sizeofVertex, &offset );
	m_pImmediateContext->IASetIndexBuffer( i_indices, DXGI_FORMAT_R32_UINT, 0 );
	m_pImmediateContext->DrawIndexed( i_indicesCount, 0, 0 );
}

void GE_Core::renderTriangleList( ID3D11Buffer *i_vertices, uint32_t i_sizeofVertex, ID3D11Buffer *i_indices, uint32_t &i_indicesCount )
{
	++m_drawCallsCount;
	m_polyCount += i_indicesCount / 3;
	uint32_t offset = 0;
	m_pImmediateContext->IASetVertexBuffers( 0, 1, &i_vertices, &i_sizeofVertex, &offset );
	m_pImmediateContext->IASetIndexBuffer( i_indices, DXGI_FORMAT_R32_UINT, 0 );
	m_pImmediateContext->DrawIndexed( i_indicesCount, 0, 0 );
}

void GE_Core::renderTriangleList(ID3D11Buffer *i_indices, uint32_t &i_indicesCount, uint32_t &i_numberOfInstance)
{
	++m_drawCallsCount;
	m_polyCount += i_indicesCount / 3;
	m_pImmediateContext->IASetIndexBuffer(i_indices, DXGI_FORMAT_R32_UINT, 0);
	m_pImmediateContext->DrawIndexedInstanced(i_indicesCount, i_numberOfInstance, 0, 0, 0);
}

void GE_Core::renderTriangleList( ID3D11Buffer *i_vertices, uint32_t i_sizeofVertex, ID3D11Buffer *i_indices, uint32_t &i_indicesCount, uint32_t &i_numberOfInstance )
{
	++m_drawCallsCount;
	m_polyCount += i_indicesCount / 3;
	uint32_t offset = 0;
	m_pImmediateContext->IASetVertexBuffers( 0, 1, &i_vertices, &i_sizeofVertex, &offset );
	m_pImmediateContext->IASetIndexBuffer( i_indices, DXGI_FORMAT_R32_UINT, 0 );
	m_pImmediateContext->DrawIndexedInstanced( i_indicesCount, i_numberOfInstance, 0, 0, 0 );
}

void GE_Core::renderTriangle( ID3D11Buffer *i_vertices, uint32_t i_sizeofVertex, uint32_t &i_verticesCount )
{
	++m_drawCallsCount;
	m_polyCount += i_verticesCount / 3;
	uint32_t offset = 0;
	m_pImmediateContext->IASetVertexBuffers( 0, 1, &i_vertices, &i_sizeofVertex, &offset );
	m_pImmediateContext->Draw( i_verticesCount, 0 );
}

void GE_Core::renderTriangle( ID3D11Buffer *i_vertices, uint32_t i_sizeofVertex, uint32_t &i_verticesCount, uint32_t &i_numberOfInstance )
{
	++m_drawCallsCount;
	m_polyCount += i_verticesCount / 3;
	uint32_t offset = 0;
	m_pImmediateContext->IASetVertexBuffers( 0, 1, &i_vertices, &i_sizeofVertex, &offset );
	m_pImmediateContext->DrawInstanced( i_verticesCount, i_numberOfInstance, 0, 0 );
}

ID3D11Buffer * GE_Core::createConstBuffer(uint32_t i_sizeofConst)
{
	ID3D11Buffer *	g_pConstBuffer = NULL;
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = i_sizeofConst;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER ;
	bd.MiscFlags = 0;
	bd.StructureByteStride=0;
	m_pd3dDevice->CreateBuffer( &bd, NULL, &g_pConstBuffer );
	return g_pConstBuffer;
}

void GE_Core::setClearSwapChain( GE_SwapChain &i_swapChain,bool i_clearRT ,bool i_clearDepth ,bool i_clearStencil,float i_depthValue,uint8_t i_stencilValue )
{
	m_currentSwapChain = &i_swapChain;
	GE_Core::getSingleton().getImmediateContext()->OMSetRenderTargets( 1,&i_swapChain.RenderTargetView, i_swapChain.DepthStencilView );
	GE_Core::getSingleton().getImmediateContext()->RSSetViewports( 1, &i_swapChain.ViewPort);
	if( i_clearRT )
		GE_Core::getSingleton().getImmediateContext()->ClearRenderTargetView( i_swapChain.RenderTargetView, (float*)&i_swapChain.ClearColor );
	if( i_clearDepth && i_clearStencil )
		GE_Core::getSingleton().getImmediateContext()->ClearDepthStencilView( i_swapChain.DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, i_depthValue, i_stencilValue );
	else if( i_clearDepth )
		GE_Core::getSingleton().getImmediateContext()->ClearDepthStencilView( i_swapChain.DepthStencilView, D3D11_CLEAR_DEPTH, i_depthValue, i_stencilValue );
	else if( i_clearStencil )
		GE_Core::getSingleton().getImmediateContext()->ClearDepthStencilView( i_swapChain.DepthStencilView, D3D11_CLEAR_STENCIL, i_depthValue, i_stencilValue );
}

void GE_Core::updateTime()
{
	m_frameRate = 1.0f / m_elapsedTime;

	LARGE_INTEGER qt;
	QueryPerformanceCounter( &qt );
	m_elapsedTime = (float)((double)(qt.QuadPart-m_lastQueryCounter) / (double)m_queryFrec );
	if( m_elapsedTime < 0.0f || m_elapsedTime > 10.0f)
		m_elapsedTime = 0;
	m_lastQueryCounter = qt.QuadPart;
}

ID3D11VertexShader * GE_Core::createVS( const std::string &i_fileName )
{
	ID3D11VertexShader *vs;
	_GE_FileChunk fc;
	fc.load( i_fileName );
	m_pd3dDevice->CreateVertexShader( fc.Data, fc.Len, NULL, &vs );
	return vs;
}

ID3D11PixelShader * GE_Core::createPS( const std::string &i_fileName )
{
	ID3D11PixelShader *ps;
	_GE_FileChunk fc;
	fc.load( i_fileName );
	m_pd3dDevice->CreatePixelShader( fc.Data, fc.Len, NULL, &ps );
	return ps;
}

ID3D11GeometryShader * GE_Core::createGS( const std::string &i_fileName )
{
	ID3D11GeometryShader *gs;
	_GE_FileChunk fc;
	fc.load( i_fileName );
	m_pd3dDevice->CreateGeometryShader( fc.Data, fc.Len, NULL, &gs );
	return gs;
}

GE_SwapChain * GE_Core::getCurrentSwapchain()
{
	return m_currentSwapChain;
}

GE_Core::RenderState::RenderState()
{
	memset( this, 0, sizeof( RenderState ) );
}

GE_Core::RenderState::~RenderState()
{
	SAFE_RELEASE( SamplerLinearUWrapVBorderWhite );
	SAFE_RELEASE( SamplerPointClamp );
	SAFE_RELEASE( SamplerLinearMirror );
	SAFE_RELEASE( SamplerLinearClamp );
	SAFE_RELEASE( SamplerLinearWrap );
	SAFE_RELEASE( SamplerLinearBorder );
	SAFE_RELEASE( RasterStateDefault );
	SAFE_RELEASE( DepthStencilStateDisableZWrite );
	SAFE_RELEASE( DepthStencilStateDisable );
	SAFE_RELEASE( BlendStateDisableAlpha );
	SAFE_RELEASE( BlendStateAlpha );
	SAFE_RELEASE( BlendStateAlphaOne );
	SAFE_RELEASE( BlendStateColorMultiply );
	SAFE_RELEASE( BlendStateDisableColorWrite );
	SAFE_RELEASE( RasterStateDisableCull );
	SAFE_RELEASE( RasterStateWireframe );
	SAFE_RELEASE(RasterStateWireframeDisableCull );
}
