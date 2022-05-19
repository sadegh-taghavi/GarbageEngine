struct VS_Input
{
	float3 Position				:	POSITION;
	uint   VertexID				:	SV_VertexID;
	uint   InstanceID			:	SV_InstanceID;
};
struct VS_Output
{
	float4 Position				:	SV_POSITION;
	float4 Texcoord				:	TEXCOORD0;
	float4 Diffuse				:	TEXCOORD1;
	float4 Emissive_Layer		:	TEXCOORD2;
	float2 Shimmerness			:	TEXCOORD3;
};

cbuffer PerObject : register( b0 )
{
	float4		m_diffuse;
	float4		m_emissive_Layer;
	float4		m_size_Shimmerness;
	float4		m_uVRectangle[ 4 ];
	float4x4	m_world;
	float4x4	m_projection;
};

Texture2DArray		m_diffuseMap	: register( t0 );
texture2D			m_backBufferMap : register( t1 );
Buffer<float4>		m_instances		: register( t2 );

SamplerState		m_samLinear		: register( s0 );
