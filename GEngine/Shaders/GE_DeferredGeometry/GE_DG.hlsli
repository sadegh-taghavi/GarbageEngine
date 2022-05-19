cbuffer m_perFrame : register( b0 )
{
	float4x4	m_viewProjection;
	float3		m_viewPosition;
	float		m_padding;
};


cbuffer m_perObject : register( b1 )
{
	float4x4	m_worldViewProjection;
	float4x4	m_world;
	float3		m_environmentPosition;
	float		m_environmentRadiusInv;
	uint		m_numberOfMorphTargets;
	uint		m_numberOfMorphTargetVertices;
	float		m_useVertexColor;
	float		m_alphaTest;
	float		m_bumpiness;
	float		m_roughness;
	float		m_reflectmask;
	float		m_materialID;
	float		m_layerMultiply;
	float		m_layerOffset;
	float		m_glowness;
	float		m_falloff;
	float		m_falloffSpread;
	float		__Padding0;
	float		__Padding1;
	float		__Padding2;
};


Texture2D		m_bfnMap				: register( t0 );
Buffer<float4>	m_vertexDataTexcoord	: register( t1 );
Buffer<float4>	m_vertexDataColor		: register( t2 );

Buffer<int>		m_vertexDataBoneIndex	: register( t3 );
Buffer<float>	m_vertexDataBoneWeight	: register( t4 );
Buffer<float4>	m_bones					: register( t5 );

Buffer<float3>	m_morphVertices			: register( t6 );
Buffer<float>	m_morphValue			: register( t7 );

Buffer<float4>	m_instances				: register( t8 );

Texture2D		m_diffuseAlphaMap[ 2 ]	: register( t9 );
Texture2D		m_normalHeightMap[ 2 ]	: register( t11 );
Texture2D		m_glowSpecularMap[ 2 ]	: register( t13 );
Texture2D		m_lightMap				: register( t15 );
TextureCube		m_reflectMap			: register( t16 );
Texture2D		m_falloffMap			: register( t17 );

SamplerState	m_samLinear				: register( s0 );
SamplerState	m_samLinearClamp		: register( s1 );

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
	uint   FrontFace		:	SV_IsFrontFace;
};

struct PS_Output
{
	float4 Normal_Specular	: SV_Target0;
	float4 Diffuse_Glow		: SV_Target1;
	float4 MaterialID		: SV_Target2;
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

void packNormal( float3 i_normal, out float3 i_packedNormal )
{
	float3 vNormalUns = abs( i_normal );
	float maxNAbs = max( vNormalUns.z, max( vNormalUns.x, vNormalUns.y ) );
	float2 vTexCoord = vNormalUns.z < maxNAbs ? ( vNormalUns.y < maxNAbs ? vNormalUns.yz : vNormalUns.xz ) : vNormalUns.xy;
	vTexCoord = vTexCoord.x < vTexCoord.y ? vTexCoord.yx : vTexCoord.xy;
	vTexCoord.y /= vTexCoord.x;
	i_packedNormal = i_normal / maxNAbs;
	i_packedNormal *= m_bfnMap.Sample( m_samLinear, vTexCoord ).w;
	i_packedNormal = i_packedNormal * 0.5f + 0.5f;
}