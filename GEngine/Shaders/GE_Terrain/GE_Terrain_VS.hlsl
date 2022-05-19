#include "GE_Terrain.hlsli"

void main( VS_Input Input, out PS_Input Output )
{
	Output.PositionInstanceID = m_instances[Input.InstanceID];
	
	float ca, sa;
	sincos(Output.PositionInstanceID.w, sa, ca);
	float2 pos = Input.Position.xy;
	Input.Position.x = ca * pos.x - sa * pos.y;
	Input.Position.y = sa * pos.x + ca * pos.y;
	float4 position = float4(Input.Position.xy + Output.PositionInstanceID.xy, 0.0f, 1.0f);
	Output.Texcoord = mul(position, m_texcoordViewProjection);

	float2 InputScreenTex = Output.Texcoord.xy / Output.Texcoord.w;
	float2 Texcoord = 0.5f * (InputScreenTex + float2(1.0f, 1.0f));
	Texcoord.y = 1.0f - Texcoord.y;
	Output.Texcoord.xy = Texcoord;
	Output.ViewVec_Height.w = position.z = m_heightMap.SampleLevel( m_samPoint, Texcoord.xy, 0 ).x * m_maxHeight;

	position.z += m_heightBias;
	Output.ViewVec_Height.xyz = position.xyz - m_viewPosition.xyz;
	Output.Position = mul(position, m_viewProjection);
}