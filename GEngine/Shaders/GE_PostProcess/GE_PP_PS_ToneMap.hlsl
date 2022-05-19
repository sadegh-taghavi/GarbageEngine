#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{
	float4 c = m_texture.Sample( m_samLinearClamp, Input.Texcoord );
	//return c;
	float l = m_texture1.Load( int3( 0, 0, 0 ) ).x;
	c.xyz /= l * m_float4Val0.x ; // use as white cut off
	c.xyz = pow( abs( c.xyz ), m_float4Val0.y ); // use as gamma
	c.xyz = m_texture3.Sample( m_samLinearClamp, c.xyz ).xyz;
	return c;
}