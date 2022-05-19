#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{	
	float4 c = m_texture.Sample( m_samLinearClamp, Input.Texcoord );
	float gc = dot( c.xyz, m_lumVector );
	return float4( lerp( c.xyz, gc.xxx, m_float4Val0.x ), c.w); // use as gray scale value
}