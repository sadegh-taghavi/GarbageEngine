cbuffer m_perObject : register( b0 )
{
	float		m_lightRange;
	float		m_lightAttenuation;
	float		m_lightSpotSpread;
	float		m_lightSpotSharpness;
	float4		m_lightPosition;
	float4		m_lightAmbient;
	float4		m_lightDirection;
	float4		m_viewPosition;
	float4		m_lightDiffuse;
	float4		m_shadowOffsetBias;
	float4x4	m_lightViewProjection[ 6 ];
	float4x4    m_world;
};

Buffer<float4>	 m_vertexDataTexcoord		: register( t0 );
Texture2D		 m_preLightMap				: register( t1 );
Texture2D<float> m_shadowMap[ 6 ]				: register( t2 );

SamplerState m_samLinear					: register( s0 );

struct VS_Input
{
	float3 Position		:	POSITION;
	float3 Normal		:	NORMAL0;
	float4 Tangent		:	TANGENT0;
	uint   VertexID	 	:   BLENDINDICES0;
	uint   InstanceID	:	SV_InstanceID;

};

struct PS_Input
{
	float4 Position		:	SV_POSITION;
	float4 Texcoord		:	TEXCOORD0;
	float3 Normal		:	NORMAL0;
	float4 Tangent		:	TANGENT0;
	float3 WorldPos		:	TEXCOORD1;
};

void CalculateShadowFactor( int shadowIndex, float3 i_shadowCoord, out float i_shadowFactor )
{
	i_shadowCoord.x = 0.5f * i_shadowCoord.x + 0.5f;
	i_shadowCoord.y = -0.5f * i_shadowCoord.y + 0.5f;

	float depth = i_shadowCoord.z - m_shadowOffsetBias.y;

	uint2 WH;
	m_shadowMap[ shadowIndex ].GetDimensions( WH.x, WH.y );
	float3 TexCoord = float3( i_shadowCoord.xy * WH, 0 );

		float s0 = m_shadowMap[ shadowIndex ].Load( TexCoord ).r;
	float s1 = m_shadowMap[ shadowIndex ].Load( TexCoord, int2( 1, 0 ) ).r;
	float s2 = m_shadowMap[ shadowIndex ].Load( TexCoord, int2( 0, 1 ) ).r;
	float s3 = m_shadowMap[ shadowIndex ].Load( TexCoord, int2( 1, 1 ) ).r;

	float2 t = frac( TexCoord.xy );
		i_shadowFactor = lerp( lerp( step( depth, s0 ), step( depth, s1 ), t.x ), lerp( step( depth, s2 ), step( depth, s3 ), t.x ), t.y );
}
