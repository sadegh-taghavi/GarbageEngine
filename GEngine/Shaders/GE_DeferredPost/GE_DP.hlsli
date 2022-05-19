struct VS_Input
{
	float3 Position		:	POSITION;
};
struct VS_Dir_Output
{
	float2 Texcoord		:	TEXCOORD0;
	float4 Position		:	SV_POSITION;
};
struct VS_Output
{
	float3 Texcoord		:	TEXCOORD0;
	float4 Position		:	SV_POSITION;
};
struct Material_Data
{
	float	SpecularPower;
	float3	Emissive;
	float3	Ambient;
	float3	Specular;
	float3	Diffuse;
	float3	Glow;
};
SamplerState m_samLinearB : register( s0 );

Texture2D		m_normal_SpecularMap	: register( t0 );
Texture2D		m_diffuse_GlowMap		: register( t1 );
Texture2D		m_materialIDMap			: register( t2 );
Texture2D<float>m_depthMap				: register( t3 );
Buffer<float4>	m_materialBuffer		: register( t4 );
Texture2D<float>m_shadowMap[ 6 ]		: register( t5 );
TextureCube		m_lightCubeFilterMap	: register( t11 );
Texture2D		m_lightFilterMap		: register( t12 );

cbuffer PerFrame : register( c0 )
{
	float		m_lightRange;
	float		m_lightAttenuation;
	float		m_lightSpotSpread;
	float		m_lightSpotSharpness;
	float4		m_lightAmbient;
	float4		m_lightDiffuse;
	float4		m_lightSpecular;
	float4		m_lightPosition;
	float4		m_lightDirection;
	float4		m_viewPosition;
	float4		m_shadowOffsetBias[ 6 ];
	float4x4	m_lightViewProjection[ 6 ];
	float4x4	m_viewProjection;
	float4x4	m_worldViewProjection;
	float4x4	m_viewProjectionInv;
	float4x4	m_cubeFilterRotate;
};

void LoadMaterial( uint i_materialID, out Material_Data Output )
{
	i_materialID *= 4;
	float4 temp = m_materialBuffer[ i_materialID ];
		Output.SpecularPower = temp.x;
	Output.Emissive = temp.yzw;
	temp = m_materialBuffer[ i_materialID + 1 ];
	Output.Ambient = temp.xyz;
	Output.Specular.x = temp.w;
	temp = m_materialBuffer[ i_materialID + 2 ];
	Output.Specular.yz = temp.xy;
	Output.Diffuse.xy = temp.zw;
	temp = m_materialBuffer[ i_materialID + 3 ];
	Output.Diffuse.z = temp.x;
	Output.Glow = temp.yzw;
}

void CalculateShadowFactor( int shadowIndex, float3 i_shadowCoord, out float i_shadowFactor )
{
	i_shadowCoord.x = 0.5f * i_shadowCoord.x + 0.5f;
	i_shadowCoord.y = -0.5f * i_shadowCoord.y + 0.5f;

	float depth = i_shadowCoord.z - m_shadowOffsetBias[ shadowIndex ].y;

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

void unpackNormal( float3 i_packedNormal, out float3 i_normal )
{
	i_normal = 2.0f * i_packedNormal - 1.0f;
	i_normal = normalize( i_normal );
}