#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{	
	float4 c = m_texture.Sample( m_samLinearClamp, Input.Texcoord );
	//float4 color = c;
    c.xyz *= m_float4Val0.x; // use as luminance
    c.xyz *= ( 1.0f + c.xyz /  m_float4Val0.y ); // use as m_whiteCutoff
    c.xyz -= 5.0f;
    c.xyz = max( c.xyz, 0.0f);
    c.xyz /= ( 1.0f + c.xyz );
	return float4( c.xyz /*color.xyz * saturate( dot( c.xyz, m_lumVector ) )*/ * ( 1.0f - c.w ), c.w );
}