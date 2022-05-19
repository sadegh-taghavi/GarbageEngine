#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{	
   float2 dir = m_float4Val0.xy - Input.Texcoord;
   float dist = distance( m_float4Val0.xy, Input.Texcoord ); // use as radial position
   dir = normalize( dir );
   float4 c = m_texture.Sample( m_samLinearClamp, Input.Texcoord );
   float3 sum = c.xyz;
   [unroll]
   for (int i = -8; i < 0; i++)
	   sum += m_texture.Sample( m_samLinearClamp, Input.Texcoord + dir  *  m_float4Val0.zw * i ).xyz;  // use as invert Width Height  
   [unroll]
   for (i = 1; i <= 8; i++)
	   sum += m_texture.Sample( m_samLinearClamp, Input.Texcoord + dir *  m_float4Val0.zw * i ).xyz;  // use as invert Width Height  
   sum *= 0.1111111111111111f;
   return float4( lerp( c.xyz, sum, saturate( dist * m_float4Val1.x ) ), c.w ); // use as radial focus
}