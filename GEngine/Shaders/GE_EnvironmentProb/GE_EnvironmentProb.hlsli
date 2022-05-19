struct VS_Input
{
	float3 Position				:	POSITION;
};

struct VS_Output
{
	float3 Normal				:	NORMAL0;
	float3 ViewVec				:	TEXCOORD0;
	float4 ScreenTexcoord_Depth	:	TEXCOORD1;
	float4 Position				:	SV_POSITION;
};

struct VS_Output_Cube
{
	float4 Position				:	POSITION;
	float2 Texcoord				:	TEXCOORD0;
};

struct GS_Output_Cube
{
	float4 Position				:	SV_POSITION;
	float2 Texcoord				:	TEXCOORD0;
	uint   TargetIndex			:	SV_RenderTargetArrayIndex;
};

SamplerState	 m_sampler		: register( s0 );
TextureCube		 m_probMap		: register( t0 );
Texture2D<float> m_depthMap		: register( t1 );
Texture2D		 m_sideMaps[ 6 ]: register( t2 );

cbuffer PerFrame : register( c0 )
{
	float4		m_viewPosition;
	float4x4	m_inverseProjection;
};

cbuffer PerObject : register( c1 )
{
	float4x4	m_world;
	float4x4	m_worldViewProjection;
};
