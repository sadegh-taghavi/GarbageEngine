#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{	
	uint2 WH;
	m_texture1.GetDimensions(WH.x, WH.y);
	float3 TexCoord = float3(Input.Texcoord * WH, 0);
	float depth = m_texture1.Load(TexCoord).x;

	float4 CurrentPos = float4(Input.Texcoord.x * 2.0f - 1.0f, (1.0f - Input.Texcoord.y) * 2.0f - 1.0f, depth, 1.0f);
	float4 WorldPos = mul(CurrentPos, m_viewProjectionInv);
	WorldPos /= WorldPos.w;
	float3 pos = mul(WorldPos, m_view).xyz;
	pos.z *= -1;
	
	//m_float4Val0.x use as depthStart
	//m_float4Val0.y use as depthField
	//m_float4Val0.z use as depthFalloff
	//m_float4Val0.w use as depthHardness

	//m_float4Val1.x use as heightStart
	//m_float4Val1.y use as heightField
	//m_float4Val1.z use as heightFalloff
	//m_float4Val1.w use as heightHardness

	//m_float4Val2.xyz use as color
	//m_float4Val2.w use as intensity
	
	float alpha = m_float4Val2.w;

	alpha *= saturate(1.0f - smoothstep(m_float4Val0.z, lerp(m_float4Val0.y,
		m_float4Val0.z, m_float4Val0.w), abs(pos.z - m_float4Val0.x)));

	alpha *= saturate(1.0f - smoothstep(m_float4Val1.z, lerp(m_float4Val1.y,
		m_float4Val1.z, m_float4Val1.w), abs(WorldPos.z - m_float4Val1.x)));

	float4 c = m_texture.Sample(m_samLinearClamp, Input.Texcoord);
	return float4(lerp(c.xyz, m_float4Val2.xyz, alpha), c.w);
}