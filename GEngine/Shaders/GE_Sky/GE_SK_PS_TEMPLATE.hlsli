#include "GE_SKY.hlsli"

float4 main( VS_Output Input ) : SV_TARGET0
{
	float2 uv;
	uv.y = acos( Input.Normal.z ) * 0.31830988618379067153776752674503f;

	/*uv.x = 0.5f - atan2( Input.Normal.x, Input.Normal.y ) * 0.15915494309189533576888376337251f;
	uv.x += m_mapXRotation;
	uv.y += m_mapYPosition;
	uv.y *= m_mapYScale;*/
#ifndef MAP1
#ifndef MAP2
	if ( uv.y > 0.499f )
		return float4( 0.4f, 0.4f, 0.8f, 0.0f );
#endif
#endif
	float4 colorOutput;
	float fCos = dot( m_lightDir.xyz, Input.Direction ) / length( Input.Direction );
	float fMiePhase = 1.5 * ( ( 1.0 - m_g2 ) / ( 2.0 + m_g2 ) ) * ( 1.0 + fCos * fCos ) / pow( abs( 1.0 + m_g2 - 2.0 * m_g * fCos ), 1.5 );
	colorOutput.xyz = Input.Color0 * m_invWavelength_Intensity.w + fMiePhase * Input.Color1;
	colorOutput.xyz = lerp( colorOutput.xyz, float3( 0.4f, 0.4f, 0.8f ) ,smoothstep( 0.49f, 0.499f, uv.y ) );
	if ( uv.y > 0.499f )
		colorOutput.xyz = float3( 0.4f, 0.4f, 0.8f );
	colorOutput.w = 0.0f;

#ifdef MAP1
	colorOutput.xyz *= m_skyMap1.Sample( m_samLinear, Input.CubeTexcoord1 ).xyz;
#endif
#ifdef MAP2
	colorOutput.xyz *= m_skyMap2.Sample( m_samLinear, Input.CubeTexcoord2 ).xyz;
#endif
	return colorOutput;
}