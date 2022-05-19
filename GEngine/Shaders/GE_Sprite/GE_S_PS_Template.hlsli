#include "GE_Sprite.hlsli"

float4 main( VS_Output Input ) : SV_Target
{
	float4 color = m_diffuseMap.SampleLevel( m_samLinear, float3( Input.Texcoord.xy, Input.Emissive_Layer.w ), 0 ) * Input.Diffuse;
	color.xyz += Input.Emissive_Layer.xyz;

#ifdef SHIMMER
	uint2 WH;
	m_backBufferMap.GetDimensions( WH.x, WH.y );
	float2 shimmer = ( color.xy * 2.0f - 1.0f ) * Input.Shimmerness.x;
	float3 TexCoord = float3( ( Input.Texcoord.zw + shimmer ) * WH, 0 );
	float3 shimmerSample = m_backBufferMap.Load( TexCoord ).xyz;
	return float4( shimmerSample, color.a );
#else
	return color;
#endif
}