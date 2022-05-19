#include "GE_PostProcess.hlsli"


//Alpha value don't pass
float4 main(VS_Output Input) : SV_Target0
{
	float3 c=m_texture.Sample( m_samLinearClamp, Input.Texcoord , int2( 0, -1) ).xyz * 2.0f - 1.0f;
	float s01 = dot( c, m_lumVector );
	c=m_texture.Sample( m_samLinearClamp, Input.Texcoord , int2( -1, 0) ).xyz * 2.0f - 1.0f;
	float s10 = dot( c, m_lumVector );
	c=m_texture.Sample( m_samLinearClamp, Input.Texcoord , int2( 1, 0) ).xyz * 2.0f - 1.0f;
	float s12 = dot( c, m_lumVector );
	c=m_texture.Sample( m_samLinearClamp, Input.Texcoord , int2( 0, 1) ).xyz * 2.0f - 1.0f;
	float s21 = dot( c, m_lumVector );
    
	float sobelX =  s10 - s12 ;
	float sobelY =  s01 - s21 ;

	return float4( m_float4Val0.xyz * ( saturate( length(float2( sobelX , sobelY ) ) * 5.0f ) > m_float4Val0.w ) ,1.0f ); // use as color use as threshold
}