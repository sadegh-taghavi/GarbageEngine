#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{
	float cl = m_texture.SampleLevel( m_samLinearClamp, Input.Texcoord , 10 ).x ;
	return cl.xxxx;
}