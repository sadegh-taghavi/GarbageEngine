struct VS_Input
{
	float3 Position		:	POSITION;
};
struct VS_Output
{
	float2 Texcoord		:	TEXCOORD0;
	float4 Position		:	SV_POSITION;
};

SamplerState m_samPointClamp			: register( s0 );
SamplerState m_samLinearClamp			: register( s1 );
SamplerState m_samLinearWrap			: register( s2 );

cbuffer PerFrameBrushDecal : register(c0)
{
	float		m_decalFalloff;
	float		m_decalHardness;
	float		m_decalPadding0;
	float		m_decalPadding1;
};

cbuffer PerFrameBrushHeight : register(c0)
{
	float		__Padding;
	float		__Padding1;
	float		__Padding2;
	int			m_channel;
	float		m_brushSize;
	float		m_brushFalloff;
	float		m_brushHardness;
	float		m_brushVolume;
	float		m_brushFieldSize;
	float		m_brushMaskRotation;
	float2		m_brushHitXY;
	float2		m_brushMaskPos;
	float2		m_brushMaskTile;
};

Texture2D		 m_texture		: register( t0 );
Texture2D<float> m_textureMask	: register( t1 );