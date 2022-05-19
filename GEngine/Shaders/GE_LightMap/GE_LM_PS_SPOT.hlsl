#include "GE_LM.hlsli"

float4 main( PS_Input Input ) :SV_Target0
{
	float ShadowFactor;
	float4 shadowCoord = mul( float4( Input.WorldPos.xyz + Input.Normal.xyz * m_shadowOffsetBias.x, 1.0f ), m_lightViewProjection[ 0 ] );
		shadowCoord.xyz /= shadowCoord.w;
	if ( shadowCoord.x < -0.99f || shadowCoord.x > 0.99f || shadowCoord.y < -0.99f
		|| shadowCoord.y > 0.99f || shadowCoord.z < -1.0f || shadowCoord.z > 1.0f )
		ShadowFactor = 1.0f;
	else
		CalculateShadowFactor( 0, shadowCoord.xyz, ShadowFactor );

	float3 LightVec = m_lightPosition.xyz - Input.WorldPos.xyz;
		float3 LightDir = normalize( LightVec );
		float ldd = max( 0.0f, dot( LightDir, m_lightDirection.xyz ) );
	float lo = saturate( m_lightAttenuation * ( m_lightRange - length( LightVec ) )  * smoothstep( m_lightSpotSpread, m_lightSpotSharpness, ldd ) );

	float4 lm = m_preLightMap.Sample( m_samLinear, Input.Texcoord.zw );
		float3 c = m_lightDiffuse.xyz * ShadowFactor * lo;
		float  o = ( ( c.x + c.y + c.z ) / 3.0f ) + lm.x;

	return float4( o.xxx, 1.0f );
}