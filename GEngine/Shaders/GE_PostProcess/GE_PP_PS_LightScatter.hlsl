#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{	
	float2 tc = Input.Texcoord;
	float2 deltaTexCoord = ( tc - m_float4Val0.xy ); //use as light position
	deltaTexCoord *= m_float4Val0.z; // use as Density
	float3 color = m_texture.Sample( m_samLinearClamp, tc ).xyz;
	float illuminationDecay = 1.0f;
	[unroll]
	for (int i = 0; i < 32; i++)
	{
		tc -= deltaTexCoord;
		float3 sample =  m_texture.Sample( m_samLinearBorder, tc ).xyz;
		sample *= illuminationDecay * m_float4Val0.w;// use as Weight
		color += sample;
		illuminationDecay *= m_float4Val1.x;// use as Decay;
	}
	float4 c = m_texture1.Sample( m_samLinearClamp, Input.Texcoord );
	return float4( c.xyz + color * m_float4Val1.y, c.w); // use as Exposure
} 
