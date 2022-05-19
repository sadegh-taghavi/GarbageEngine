#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{	
	float3 c = 0;
	float4 cl;
	[unroll]
    for (int i = 0; i < 16; i++)
	{
		cl = m_texture.Sample(m_samLinearClamp, Input.Texcoord + m_pixelCoordsDownFilter[i].xy * m_float4Val0.xy); // use as invert Width Height
        c += cl.xyz * cl.w;
	}
    return float4(c.xyz * 0.0625f, cl.w );
}