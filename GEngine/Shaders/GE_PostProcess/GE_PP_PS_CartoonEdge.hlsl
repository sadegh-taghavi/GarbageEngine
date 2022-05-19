#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{
	float4 cl = m_texture.Sample( m_samLinearClamp, Input.Texcoord ) ;

	float3 c = m_texture1.Sample( m_samLinearClamp, Input.Texcoord, int2( 0, -1) ).xyz * 2.0f - 1.0f;
	float s01 = dot( c, m_lumVector );
	c=m_texture1.Sample( m_samLinearClamp, Input.Texcoord , int2( -1, 0) ).xyz * 2.0f - 1.0f;
	float s10 = dot( c, m_lumVector );
	c=m_texture1.Sample( m_samLinearClamp, Input.Texcoord , int2( 1, 0) ).xyz * 2.0f - 1.0f;
	float s12 = dot( c, m_lumVector );
	c=m_texture1.Sample( m_samLinearClamp, Input.Texcoord , int2( 0, 1) ).xyz * 2.0f - 1.0f;
	float s21 = dot( c, m_lumVector );
    
	float sobelX =  s10   - s12 ;
	float sobelY =  s01  - s21 ;

	if( saturate( length( float2( sobelX , sobelY ) ) * 5.0f ) > m_float4Val0.w )  // use as threshold
		return float4( cl.xyz * m_float4Val0.xyz, cl.w ); // use as color
	return cl;
}