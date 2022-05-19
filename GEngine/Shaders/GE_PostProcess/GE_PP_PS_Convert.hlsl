#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{	
	return m_texture.Sample( m_samLinearClamp, Input.Texcoord );
}