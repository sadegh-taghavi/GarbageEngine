#include "GE_EnvironmentProb.hlsli"

float4 main( VS_Output Input ) : SV_TARGET0
{
	float3 rf = reflect( normalize( Input.ViewVec ), Input.Normal );

	float2 InputScreenTex = Input.ScreenTexcoord_Depth.xy / Input.ScreenTexcoord_Depth.w;
	float2 screenTex = 0.5f * ( InputScreenTex + float2( 1.0f, 1.0f ) );
	screenTex.y = 1.0f - screenTex.y;
	uint2 WH;
	m_depthMap.GetDimensions( WH.x, WH.y );
	float3 TexCoord = float3( screenTex * WH, 0 );

	float depthSample = m_depthMap.Load( TexCoord ).x;
	float4 depthViewSample = mul( float4( InputScreenTex, depthSample, 1.0f ), m_inverseProjection );
	float4 depthViewParticle = mul( float4( InputScreenTex, Input.ScreenTexcoord_Depth.z / Input.ScreenTexcoord_Depth.w, 1.0f ), m_inverseProjection );
	float depthDiff = depthViewParticle.z / depthViewParticle.w - depthViewSample.z / depthViewSample.w;
	clip( depthDiff );
	
	return m_probMap.Sample( m_sampler, float3( rf.x, rf.z, rf.y ) );
}