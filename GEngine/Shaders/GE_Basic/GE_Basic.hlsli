cbuffer m_perFrame : register( b0 )
{
	float4x4 m_viewProjection;
};
cbuffer m_perObject : register( b1 )
{
	float4x4 m_worldViewProjection;
	float4x4 m_world;
	float3	 m_color;
	float    m_alphaTest;
	uint     m_numberOfMorphTargets;
	uint	 m_numberOfMorphTargetVertices;
	uint2	 m_padding;
};

sampler m_samLinear						: register( s0 );

Buffer<int>   m_vertexDataBoneIndex		: register( t0 );
Buffer<float> m_vertexDataBoneWeight	: register( t1 );
Buffer<float4> m_bones					: register( t2 );

Buffer<float3> m_morphVertices			: register( t3 );
Buffer<float>  m_morphValue				: register( t4 );

Buffer<float4> m_instances				: register( t5 );

Buffer<float4> m_vertexDataTexcoord		: register( t6 );

texture2D	   m_texture				: register( t7 );

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
	float4 Position		:	SV_POSITION;
	float2 Texcoord		:   TEXCOORD0;
};

struct CalculateOutput
{
	float3 Position;
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
		++index;
	}
	lastWeight = 1.0f - lastWeight;
	Out.Position += lastWeight * tPos.xyz;

}

void CalculateMorph( VS_Input Input, out CalculateOutput Out )
{
	uint ind = ( Input.VertexID * 3 );
	Out.Position = Input.Position;

	[ unroll ]
	for ( uint i = 0; i < 50 && i < m_numberOfMorphTargets; i++ )
		Out.Position += m_morphVertices[ ( i * m_numberOfMorphTargetVertices ) + ind ] * m_morphValue[ i ];
}
