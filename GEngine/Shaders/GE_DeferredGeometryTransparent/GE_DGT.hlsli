cbuffer m_perFrame : register( b0 )
{
	float4		m_lightAmbient;
	float4		m_lightDiffuse;
	float4		m_lightSpecular;
	float4		m_lightPosition;
	float4		m_lightDirection;
	float4		m_viewPosition;
	float4		m_shadowOffsetBias[6];
	float4x4	m_lightViewProjection[6];
	float4x4	m_viewProjection;
	float4x4	m_viewProjectionInv;
};

cbuffer m_perObject : register( b1 )
{
	float4x4	m_worldViewProjection;
	float4x4	m_world;
	float3		m_environmentPosition;
	float		m_environmentRadiusInv;
	float3		m_emissive;
	float		m_useVertexColor;
	float3		m_ambient;
	float		m_refract;
	float3		m_specular;
	float		m_bumpiness;
	float3		m_diffuse;
	float		m_roughness;
	float3		m_glow;
	float		m_reflectmask;
	float		m_specularPower;
	float		m_alpha;
	float		m_layerMultiply;
	float		m_layerOffset;
	float		m_glowness;
	float		m_refractness;
	float		m_falloff;
	float		m_falloffSpread;
	float		__Padding0;
	float		__Padding1;
	uint		m_numberOfMorphTargets;
	uint		m_numberOfMorphTargetVertices;
};

Texture2D	   m_backBufferMap			: register( t0 );
Buffer<float4> m_vertexDataTexcoord		: register( t1 );
Buffer<float4> m_vertexDataColor		: register( t2 );

Buffer<int>    m_vertexDataBoneIndex	: register( t3 );
Buffer<float>  m_vertexDataBoneWeight	: register( t4 );
Buffer<float4> m_bones					: register( t5 );

Buffer<float3> m_morphVertices			: register( t6 );
Buffer<float>  m_morphValue				: register( t7 );

Buffer<float4> m_instances				: register( t8 );

Texture2D		m_diffuseAlphaMap[ 2 ]		: register( t9 );
Texture2DArray	m_normalHeightMap[ 2 ]		: register( t11 );
Texture2D		m_glowSpecularMap[ 2 ]		: register( t13 );
Texture2D		m_lightMap					: register( t15 );
TextureCube		m_reflectMap				: register( t16 );
Texture2D		m_falloffMap				: register( t17 );
Texture2D<float>m_shadowMap[6]				: register( t18 );

SamplerState	m_samLinear					: register( s0 );
SamplerState	m_samLinearClamp			: register( s1 );

struct VS_Input
{
	float3 Position		:	POSITION;
	float3 Normal		:	NORMAL0;
	float4 Tangent		:	TANGENT0;
	uint   VertexID	 	:   BLENDINDICES0;
	uint   InstanceID	:	SV_InstanceID;
};

struct VS_StreamOut
{
	float3 Position		:	POSITION;
	float3 Normal		:	NORMAL0;
	float4 Tangent		:	TANGENT0;
	uint   VertexID 	:   BLENDINDICES0;
};

struct PS_Input
{
	float4 Position			:	SV_POSITION;
	float3 Normal			:	NORMAL0;
	float4 Tangent			:	TANGENT0;
	float3 ViewVec			:	TEXCOORD0;
	float3 PositionOffset	:	TEXCOORD1;
	float4 Texcoord			:	TEXCOORD2;
	float4 VertexColor		:   TEXCOORD3;
	float3 ScreenTexcoord	:   TEXCOORD4;

};

struct PS_Input_FaceDir
{
	float4 Position			:	SV_POSITION;
	float3 Normal			:	NORMAL0;
	float4 Tangent			:	TANGENT0;
	float3 ViewVec			:	TEXCOORD0;
	float3 PositionOffset	:	TEXCOORD1;
	float4 Texcoord			:	TEXCOORD2;
	float4 VertexColor		:   TEXCOORD3;
	float3 ScreenTexcoord	:   TEXCOORD4;
	uint   FrontFace		:	SV_IsFrontFace;
};

struct PS_Output
{
	float4 Diffuse			: SV_Target0;
};

struct PS_Output_Transparent
{
	float4 Diffuse : SV_Target0;
};

struct CalculateOutput
{
	float3 Position;
	float3 Normal;
	float3 Tangent;
};

float4x4 ExtractBoneMatrix( uint i_index )
{
	i_index *= 4;
	return float4x4( m_bones[ i_index ], m_bones[ i_index + 1 ], m_bones[ i_index + 2 ], m_bones[ i_index + 3 ] );
}
float4x4 ExtractInstanceMatrix( uint i_index )
{
	i_index *= 4;
	return float4x4( m_instances[ i_index ], m_instances[ i_index + 1 ], m_instances[ i_index + 2 ], m_instances[ i_index + 3 ] );
}

#define BONE_MAX_IW 8
void CalculateSkin( VS_Input Input, out CalculateOutput Out )
{
	Out = ( CalculateOutput )0;
	uint index = Input.VertexID * BONE_MAX_IW;
	float lastWeight = 0.0f;
	float4 tPos;
	tPos.xyz = Input.Position;
	tPos.w = 1.0f;

	[ unroll ]
	for ( uint i = 0; i < BONE_MAX_IW; ++i )
	{
		int bi = m_vertexDataBoneIndex[ index ];
		if ( bi < 0 )
			break;
		float bw = m_vertexDataBoneWeight[ index ];
		lastWeight += bw;
		float4x4 m = ExtractBoneMatrix( bi );

		Out.Position += ( bw * mul( tPos, m ) ).xyz;
		Out.Normal += ( bw * mul( Input.Normal, ( float3x3 )m ) ).xyz;
		Out.Tangent += ( bw * mul( Input.Tangent.xyz, ( float3x3 )m ) ).xyz;
		++index;
	}
	lastWeight = 1.0f - lastWeight;
	Out.Position += lastWeight * tPos.xyz;
	Out.Normal += lastWeight * Input.Normal;
	Out.Tangent += lastWeight * Input.Tangent.xyz;

}

void CalculateMorph( VS_Input Input, out CalculateOutput Out )
{
	uint ind = ( Input.VertexID * 3 );

	Out.Position = Input.Position;
	Out.Normal = Input.Normal;
	Out.Tangent = Input.Tangent.xyz;

	[ unroll ]
	for ( uint i = 0; i < 50 && i < m_numberOfMorphTargets; i++ )
	{
		uint index = ( i * m_numberOfMorphTargetVertices ) + ind;
		Out.Position += m_morphVertices[ index ] * m_morphValue[ i ];
		Out.Normal += m_morphVertices[ index + 1 ] * m_morphValue[ i ];
		Out.Tangent += m_morphVertices[ index + 2 ] * m_morphValue[ i ];
	}
}

void CalculateShadowFactor(int shadowIndex, float3 i_shadowCoord, out float i_shadowFactor)
{
	i_shadowCoord.x = 0.5f * i_shadowCoord.x + 0.5f;
	i_shadowCoord.y = -0.5f * i_shadowCoord.y + 0.5f;

	float depth = i_shadowCoord.z - m_shadowOffsetBias[shadowIndex].y;

	uint2 WH;
	m_shadowMap[shadowIndex].GetDimensions(WH.x, WH.y);
	float3 TexCoord = float3(i_shadowCoord.xy * WH, 0);

	float s0 = m_shadowMap[shadowIndex].Load(TexCoord).r;
	float s1 = m_shadowMap[shadowIndex].Load(TexCoord, int2(1, 0)).r;
	float s2 = m_shadowMap[shadowIndex].Load(TexCoord, int2(0, 1)).r;
	float s3 = m_shadowMap[shadowIndex].Load(TexCoord, int2(1, 1)).r;
	float2 t = frac(TexCoord.xy);
		i_shadowFactor = lerp(lerp(step(depth, s0), step(depth, s1), t.x), lerp(step(depth, s2), step(depth, s3), t.x), t.y);
}
