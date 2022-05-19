#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{	
	float4 c = m_texture.Sample( m_samLinearClamp, Input.Texcoord );
    float4 c1 = m_texture1.Sample( m_samLinearClamp, Input.Texcoord );
    return c * c1.w * c.w;
}