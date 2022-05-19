#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{
	float3 c;
    float a = 0.0f;
	[unroll]
    for( int y = -1; y <= 1; y++ )
    {
		[unroll]
        for( int x = -1; x <= 1; x++ )
        {
			c = m_texture.Sample( m_samLinearClamp, Input.Texcoord, int2( x, y ) ).xyz;
			a+=c.r;
		}
	}
	a*=0.11111111111111111111111111111111f;
	return float4( a.xxx, 1.0f );
}