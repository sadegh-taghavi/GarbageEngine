struct VS_Input
{
	float4 Position_Layer		:	POSITION;
	float4 Emissive_Rotation	:	TEXCOORD0;
	float4 Diffuse				:	TEXCOORD1;
	float2 Size					:	TEXCOORD2;	
	uint   InstanceID			:	SV_InstanceID;
};
struct VS_Output
{
	float4 Position_Layer		:	POSITION;
	float4 Emissive_Rotation	:	TEXCOORD0;
	float4 Diffuse				:	TEXCOORD1;
	float3 Size_InstanceID		:	TEXCOORD2;
};
struct GS_Output
{
	float4 Position					:	SV_POSITION;
	float4 Diffuse					:	TEXCOORD0;
	float4 Depth_ScreenTexcoord		:	TEXCOORD1;
	float4 Texcoord_Rotation_Layer	:	TEXCOORD2;	 
	float4 Emissive_SceneW			:	TEXCOORD3;
};
cbuffer Immutable
{
    static const float3 m_positions[4] =
    {
        float3(-0.5f,0.5f,0),
        float3(-0.5f,-0.5f,0),
        float3(0.5f,0.5f,0),
        float3(0.5f,-0.5f,0),
    };
    static const float2 m_texcoords[4] = 
    { 
        float2(0,1),
        float2(1,1),
		float2(0,0), 
		float2(1,0),
    };
}
cbuffer PerFrame : register(b0)
{
	float4x4 m_viewProjection;
	float4x4 m_inverseProjection;
};
cbuffer PerObject : register(b1)
{
	float		m_softness;
	float		m_shimmerness;
	float2		m_padding;
	float4		m_viewPosition;
	float4x4	m_world;
	float4x4	m_worldViewProjection;
	
};

Buffer<float4>		m_instance		: register(t0);
Texture3D			m_diffuseMap	: register(t1);
Texture2D<float>	m_depthMap		: register(t2);
Texture2D			m_backBufferMap	: register(t3);

SamplerState m_samLinear		: register(s0);

float4x4 ExtractInstanceMatrix(uint i_index)
{
	i_index*=4;
	return float4x4(m_instance[i_index],m_instance[i_index+1],m_instance[i_index+2],m_instance[i_index+3]);
}