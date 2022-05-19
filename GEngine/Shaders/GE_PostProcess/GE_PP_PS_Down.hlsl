#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_TARGET
{
	float4 c = m_texture.Sample( m_samLinearClamp, Input.Texcoord ); 
	[unroll]
    for ( int i = 0; i < 16; i++ )
		c.xyz += m_texture.Sample( m_samLinearClamp, Input.Texcoord + m_pixelCoordsDownFilter[i].xy * m_float4Val0.xy ).xyz; // use as invert Width Height
    return float4( c.xyz * (1.0f / 17.0f), c.w );
}