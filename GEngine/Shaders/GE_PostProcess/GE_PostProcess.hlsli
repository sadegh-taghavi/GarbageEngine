struct VS_Input
{
	float3 Position		:	POSITION;
};
struct VS_Output
{
	float2 Texcoord		:	TEXCOORD0;
	float4 Position		:	SV_POSITION;
};

static const int2 m_pixelCoordsAA[ 7 ] =
{
	{ 0, 0 },
	{ -1, -1 },
	{ 1, 1 },
	{ 1, -1 },
	{ -1, 1 },
	{ -1, 0 },
	{ 0, -1 },
};
static const float2 m_pixelCoordsDownFilter[ 16 ] =
{
	{ 1.5, -1.5 },
	{ 1.5, -0.5 },
	{ 1.5, 0.5 },
	{ 1.5, 1.5 },

	{ 0.5, -1.5 },
	{ 0.5, -0.5 },
	{ 0.5, 0.5 },
	{ 0.5, 1.5 },

	{ -0.5, -1.5 },
	{ -0.5, -0.5 },
	{ -0.5, 0.5 },
	{ -0.5, 1.5 },

	{ -1.5, -1.5 },
	{ -1.5, -0.5 },
	{ -1.5, 0.5 },
	{ -1.5, 1.5 },
};
static const int2 m_horizontalPixelKernel[ 13 ] =
{
	{ -6, 0 },
	{ -5, 0 },
	{ -4, 0 },
	{ -3, 0 },
	{ -2, 0 },
	{ -1, 0 },
	{ 0, 0 },
	{ 1, 0 },
	{ 2, 0 },
	{ 3, 0 },
	{ 4, 0 },
	{ 5, 0 },
	{ 6, 0 },
};
static const int2 m_verticalPixelKernel[ 13 ] =
{
	{ 0, -6 },
	{ 0, -5 },
	{ 0, -4 },
	{ 0, -3 },
	{ 0, -2 },
	{ 0, -1 },
	{ 0, 0 },
	{ 0, 1 },
	{ 0, 2 },
	{ 0, 3 },
	{ 0, 4 },
	{ 0, 5 },
	{ 0, 6 },
};
static const float m_blurWeights[ 13 ] =
{
	0.002216,
	0.008764,
	0.026995,
	0.064759,
	0.120985,
	0.176033,
	0.199471,
	0.176033,
	0.120985,
	0.064759,
	0.026995,
	0.008764,
	0.002216,
};
static const float3 m_lumVector = float3( 0.2125f, 0.7154f, 0.0721f );

SamplerState m_samPointClamp			: register( s0 );
SamplerState m_samLinearClamp			: register( s1 );
SamplerState m_samLinearBorder			: register( s2 );

#define MAX_ARRAY		6

cbuffer PerFrame						: register( c0 )
{
	float4		m_float4Val0;
	float4		m_float4Val1;
	float4		m_float4Val2;
	float4x4	m_view;
	float4x4	m_viewProjectionInv;
	float4x4	m_previousViewProjection;
};

Texture2D m_texture		: register( t0 );
Texture2D m_texture1	: register( t1 );
Texture2D m_texture2	: register( t2 );
Texture3D m_texture3	: register( t3 );