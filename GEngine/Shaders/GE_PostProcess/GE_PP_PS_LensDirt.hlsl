#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{
	float4 dest = m_texture.Sample( m_samLinearClamp, Input.Texcoord );
	float3 lum = m_texture1.Sample( m_samLinearClamp, Input.Texcoord ).xyz ;
	float3 dirt = m_texture2.Sample( m_samLinearClamp, Input.Texcoord ).xyz * m_float4Val0.y;
	float  l = dot( lum.xyz, m_lumVector );
	l = ( 1.0f - saturate( l * m_float4Val0.x ) ) * l;
	float3 dirtLum = lerp( float3( 1.0f, 1.0f, 1.0f ) , dirt , saturate( l ) );
	return float4( dirtLum * dest.xyz, dest.w );
}