struct VS_Input
{
	float3 Position	:	POSITION;
};

struct GS_Input
{
	float3 Position		: POSITION;
	uint   InstanceID	: SV_InstanceID;
};

struct GS_Output
{
	float4 Position									:	SV_POSITION;
	float3 Texcoord									:	TEXCOORD0;
	float4 ProjectionR0								:	TEXCOORD1;
	float4 ProjectionR1								:	TEXCOORD2;
	float4 ProjectionR2								:	TEXCOORD3;
	float4 ProjectionR3								:	TEXCOORD4;
	float4 Bumpiness_Glowness_Roughness_Reflectmask	:	TEXCOORD5;
	float4 TileUV_Layer_Rotation					:	TEXCOORD6;
	float3 MoveUV_MaxAngle							:	TEXCOORD7;
	float4 Color									:	TEXCOORD8;
	float3 Normal									:	TEXCOORD9;
};

struct PS_Output
{
	float4 Normal_Specular		: SV_Target0;
	float4 Diffuse_Glow			: SV_Target1;
};

SamplerState m_samLinear				: register( s0 );

Texture2D<float>	m_depthMap				: register( t0 );
Texture2D			m_normalSpecularRTMap	: register( t1 );
Texture2D			m_diffuseGlowRTMap		: register( t2 );
Buffer<float4>		m_instances				: register( t3 );
Texture2DArray		m_diffuseMap			: register( t4 );
Texture2DArray		m_normalMap				: register( t5 );
Texture2DArray		m_glowSpecularMap		: register( t6 );
TextureCube			m_reflectMap			: register( t7 );

cbuffer PerFrame	: register( c0 )
{
	float4x4	m_viewProjection;
	float4x4	m_viewProjectionInv;
	float3		m_viewPosition;
	float		m_padding;
};

cbuffer PerObject : register( c1 )
{
	float  m_environmentRadiusInv;
	float3 m_environmentPosition;
	float4 m_numberOfMips;
};

void unpackNormal( float3 i_packedNormal, out float3 i_normal )
{
	i_normal = 2.0f * i_packedNormal - 1.0f;
	i_normal = normalize( i_normal );
}