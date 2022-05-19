#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{	
	float4 c = m_texture.Sample( m_samLinearClamp, Input.Texcoord , m_verticalPixelKernel[0].xy );
	c.xyz *= m_blurWeights[0];

	[unroll]
    for (int i = 1; i < 13; i++)
        c.xyz += m_texture.Sample( m_samLinearClamp, Input.Texcoord , m_verticalPixelKernel[i].xy ).xyz * m_blurWeights[i];
    return float4(c.xyz * m_float4Val0.x ,c.w); // use as bloomScale 
}