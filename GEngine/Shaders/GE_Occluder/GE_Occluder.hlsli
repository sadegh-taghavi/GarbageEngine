struct VS_Input
{
	float3 Position				:	POSITION;
};
struct VS_Output
{
	float4 Position				:	SV_POSITION;
};

cbuffer PerObject : register( b0 )
{
	float4x4	m_worldViewProjection;
};