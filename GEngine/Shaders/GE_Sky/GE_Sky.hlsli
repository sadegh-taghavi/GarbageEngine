struct VS_Input
{
	float3 Position : POSITION0;
};
struct VS_Output
{
	float4 Position 	: SV_POSITION;
	float3 Normal		: TEXCOORD0;
	float3 CubeTexcoord1: TEXCOORD1;
	float3 CubeTexcoord2: TEXCOORD2;
	float3 Color0		: TEXCOORD3;
	float3 Color1		: TEXCOORD4;
	float3 Direction	: TEXCOORD5;
};

cbuffer PerFrame : register(b0)
{
		float		m_cameraHeight;
		float		m_cameraHeight2;
		float		m_outerRadius;
		float		m_outerRadius2;
		float		m_innerRadius;
		float		m_InnerRadius2;
		float		m_krESun;
		float		m_kmESun;
		float		m_kr4PI;
		float		m_km4PI;
		float		m_invScale;
		float		m_scaleDepth;
		float		m_scaleOverScaleDepth;
		float		m_map1ScaleZ;
		float		m_map1PositionZ;
		float		m_map2ScaleZ;
		float		m_map2PositionZ;
		float		m_g;
		float		m_g2;
		float		m_scale;
		uint		m_numberOfSamples;
		uint3		m_padding;
		float4		m_cameraPos;
		float4		m_lightDir;
		float4		m_position;
		float4		m_invWavelength_Intensity;
		float4x4	m_viewProjection;
		float4x4	m_map1Rotation;
		float4x4	m_map2Rotation;
};

TextureCube	 m_skyMap1	: register( t0 );
TextureCube	 m_skyMap2	: register( t1 );

SamplerState m_samLinear: register(s0);

