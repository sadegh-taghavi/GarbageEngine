#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"

struct GE_SwapChain
{
	IDXGISwapChain*				SwapChain;
	ID3D11Texture2D*			BackBuffer;
	ID3D11Texture2D*			BackBufferMedium;
	ID3D11RenderTargetView*		RenderTargetView;
	ID3D11ShaderResourceView *	RenderTargetViewSRV;
	ID3D11Texture2D*			DepthStencil;
	ID3D11Texture2D*			DepthStencilCopy;
	ID3D11DepthStencilView*		DepthStencilView;
	ID3D11ShaderResourceView*	DepthStencilViewSRV;
	ID3D11ShaderResourceView*	DepthStencilViewSRVCopy;
	D3D11_VIEWPORT				ViewPort;
	GE_Vec4						ClearColor;
	HWND						Hwnd;
	bool						Windowed;
	uint32_t					RefreshRateNumerator;
	uint32_t					RefreshRateDenominator;

	GE_SwapChain()
	{
		SwapChain = NULL;
		BackBuffer = NULL;
		RenderTargetView = NULL;
		DepthStencilView = NULL;
		DepthStencil = NULL;
		BackBufferMedium = NULL;
		RenderTargetViewSRV = NULL;
		DepthStencilViewSRV = NULL;
		DepthStencilCopy = NULL;
		DepthStencilViewSRVCopy = NULL;
		ViewPort.MaxDepth = 1.0f;
		ViewPort.MinDepth = 0.0f;
		ViewPort.TopLeftX = 0.0f;
		ViewPort.TopLeftY = 0.0f;
		ClearColor = GE_Vec4( 0.0f, 1.0f, 1.0f, 1.0f );
		Windowed = true;
		RefreshRateNumerator = 60;
		RefreshRateDenominator = 1;
	}
	void copyBackBufferToSRV();
	void copyDepthToSRV();
	void resize();
	void release()
	{
		SAFE_RELEASE( BackBufferMedium );
		SAFE_RELEASE( RenderTargetViewSRV );
		SAFE_RELEASE( DepthStencilViewSRV );
		SAFE_RELEASE( DepthStencil );
		SAFE_RELEASE( DepthStencilView );
		SAFE_RELEASE( BackBuffer );
		SAFE_RELEASE( RenderTargetView );
		SAFE_RELEASE( DepthStencilViewSRVCopy );
		SAFE_RELEASE( DepthStencilCopy );
		SAFE_RELEASE( SwapChain );
	}

	~GE_SwapChain()
	{
		release();
	}
};

class GE_Core
{
	struct RenderState
	{
		ID3D11BlendState		*BlendStateDisableAlpha;
		ID3D11BlendState		*BlendStateDisableColorWrite;
		ID3D11BlendState		*BlendStateAlpha;
		ID3D11BlendState		*BlendStateAlphaOne;
		ID3D11BlendState		*BlendStateColorMultiply;
		ID3D11SamplerState		*SamplerLinearMirror;
		ID3D11SamplerState		*SamplerLinearClamp;
		ID3D11SamplerState		*SamplerLinearUWrapVBorderWhite;
		ID3D11SamplerState		*SamplerLinearWrap;
		ID3D11SamplerState		*SamplerLinearBorder;
		ID3D11SamplerState		*SamplerPointClamp;
		ID3D11DepthStencilState *DepthStencilStateDisableZWrite;
		ID3D11DepthStencilState *DepthStencilStateDisable;
		ID3D11RasterizerState	*RasterStateDefault;
		ID3D11RasterizerState	*RasterStateWireframe;
		ID3D11RasterizerState	*RasterStateDisableCull;
		ID3D11RasterizerState	*RasterStateWireframeDisableCull;
		RenderState();
		~RenderState();
	}m_renderStates;

	static GE_Core			m_singleton;
	bool m_isInit;

	IDXGIDevice	 *			m_pDXGIDevice;
	IDXGIAdapter *			m_pDXGIAdapter;
	IDXGIFactory *			m_pIDXGIFactory;

	D3D_FEATURE_LEVEL		m_featureLevel;
	ID3D11Device*			m_pd3dDevice;
	ID3D11DeviceContext*	m_pImmediateContext;
	ID3D11RenderTargetView*	m_NULLRT[ 8 ];

	//Quad values
	ID3D11InputLayout*		m_quadVertexDeclaration;
	ID3D11Buffer*			m_quadVB;
	uint32_t				m_quadStrid;
	uint32_t				m_quadOffset;
	struct quadVertexBuffer
	{
		GE_Vec3 Position;
	}m_quadVertices[ 4 ];

	//draw values
	uint32_t				m_drawCallsCount;
	uint32_t				m_testDrawCallsCount;
	uint32_t				m_testPolyCount;
	uint32_t				m_polyCount;

	float					m_frameRate;

	long long				m_lastQueryCounter, m_queryFrec;
	float					m_elapsedTime;

	void createQuad();
	GE_Core();
	~GE_Core();

	GE_SwapChain			*m_currentSwapChain;
public:

	RenderState &getRenderStates(){ return m_renderStates; }
	void createRenderStates( uint32_t i_anisotropicLevel = 0 );
	float &getFrameRate(){ return m_frameRate; }

	float &getElapsedTime(){ return m_elapsedTime; }
	void updateTime();
	bool createSwapChain( GE_SwapChain& i_swapChain );
	static inline GE_Core& getSingleton(){ return m_singleton; }
	D3D_FEATURE_LEVEL getFeautureLevel(){ return m_featureLevel; }
	ID3D11Device* getDevice(){ return m_pd3dDevice; }
	ID3D11DeviceContext* getImmediateContext(){ return m_pImmediateContext; }

	void	setRTToNull();
	GE_SwapChain *getCurrentSwapchain();
	void setClearSwapChain( GE_SwapChain &i_swapChain, bool i_clearRT = true, bool i_clearDepth = true, bool i_clearStencil = true, float i_depthValue = 1.0f, uint8_t i_stencilValue = 0 );

	ID3D11Buffer *createConstBuffer( uint32_t i_sizeofConst );
	ID3D11Buffer *createVertexBuffer( const void* i_vertices, uint32_t i_sizeofVertex, uint32_t i_verticesCount, bool i_isConst = true );
	ID3D11Buffer *createIndexBuffer( const void* i_indices, uint32_t i_sizeofIndex, uint32_t i_indicesCount, bool i_isConst = true );

	void renderTriangleListTest( ID3D11Buffer *i_vertices, uint32_t i_sizeofVertex, ID3D11Buffer *i_indices, uint32_t &i_indicesCount );
	void renderTriangleList( ID3D11Buffer *i_vertices, uint32_t i_sizeofVertex, ID3D11Buffer *i_indices, uint32_t &i_indicesCount );
	void renderTriangleList( ID3D11Buffer *i_vertices, uint32_t i_sizeofVertex, ID3D11Buffer *i_indices, uint32_t &i_indicesCount, uint32_t &i_numberOfInstance );
	void renderTriangleList( ID3D11Buffer *i_indices, uint32_t &i_indicesCount, uint32_t &i_numberOfInstance);
	void renderTriangle( ID3D11Buffer *i_vertices, uint32_t i_sizeofVertex, uint32_t &i_verticesCount );
	void renderTriangle( ID3D11Buffer *i_vertices, uint32_t i_sizeofVertex, uint32_t &i_verticesCount, uint32_t &i_numberOfInstance );

	//draw functions
	uint32_t getDrawCallsCount(){ return m_drawCallsCount; }
	uint32_t getPolygonCount(){ return m_polyCount; }
	void clearDrawCallAndPolygonCount() { m_drawCallsCount = 0; m_polyCount = 0; m_testPolyCount = 0; m_testDrawCallsCount = 0; }

	//quad functions
	void setScreenAlignQuadVertexDeclarationToContext();
	void setScreenAlignQuadInputLayoutToContext();
	void renderScreenAlignQuad();
	void renderScreenAlignQuad( uint32_t i_numberOfInstance );

	ID3D11VertexShader *createVS( const std::string &i_fileName );
	ID3D11PixelShader *createPS( const std::string &i_fileName );
	ID3D11GeometryShader *createGS( const std::string &i_fileName );
};