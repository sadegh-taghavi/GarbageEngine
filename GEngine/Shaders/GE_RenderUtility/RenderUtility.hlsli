struct VS_INPUT
{
	float3 Position		:	POSITION;
	uint   VertexID		:	SV_VertexID;
};

struct PS_INPUT
{
	float4 Position				:	SV_POSITION; 
	float3 Color				:	TEXCOORD0;
	float2 Texcoord				:   TEXCOORD1;
	float4 ScreenTexcoord_Depth	:	TEXCOORD2;

};

cbuffer m_perObject : register( b0 )
{
	float4x4 m_worldViewProjection;
	float4x4 m_inverseView;
	float4x4 m_inverseProjection;
	float4	 m_color;
};


SamplerState 	m_samLinear 			: register( s0 );

Buffer<float3> 	m_vertexDataColor   	: register(t0);
Buffer<float2> 	m_vertexDataTexcoord	: register(t1);
Texture2D		m_texture				: register(t2);
Texture2D<float> m_depthMap				: register(t3);

