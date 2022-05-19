#include "GE_Particle.hlsli"

float4 main(GS_Output Input) : SV_Target
{
	float2 InputScreenTex=Input.Depth_ScreenTexcoord.zw / Input.Emissive_SceneW.w;
	float2 screenTex = 0.5f * ( InputScreenTex + float2(1.0f,1.0f));
    screenTex.y = 1.0f - screenTex.y;

	float2 ltc=Input.Texcoord_Rotation_Layer.xy;
	ltc.x-=0.5f;
	ltc.y-=0.5f;
	float3 tc=Input.Texcoord_Rotation_Layer.xyw;
	float ca=cos(-Input.Texcoord_Rotation_Layer.z);
	float sa=sin(-Input.Texcoord_Rotation_Layer.z);
	tc.x=ca * ltc.x - sa * ltc.y;
	tc.y=sa * ltc.x + ca * ltc.y;
	tc.x+=0.5f;
	tc.y+=0.5f;

    float4 particleSample = m_diffuseMap.SampleLevel( m_samLinear,tc,0 ) * Input.Diffuse;
	particleSample.xyz+=Input.Emissive_SceneW.xyz;
	float depthFade = 1.0f;
	float particleDepth = Input.Depth_ScreenTexcoord.x / Input.Depth_ScreenTexcoord.y;

	uint2 WH;
	m_depthMap.GetDimensions(WH.x,WH.y);
	float3 TexCoord=float3(screenTex * WH,0);

    float depthSample=m_depthMap.Load(TexCoord ).x;
    float4 depthViewSample = mul( float4( InputScreenTex, depthSample, 1.0f ), m_inverseProjection );
    float4 depthViewParticle = mul( float4( InputScreenTex, particleDepth, 1.0f ), m_inverseProjection );
    float depthDiff =  depthViewParticle.z/depthViewParticle.w - depthViewSample.z/depthViewSample.w;
	clip(depthDiff);
    depthFade = saturate( depthDiff * m_softness );
	particleSample.a *= depthFade;

#ifdef SHIMMER
	float2 shimmer=(particleSample.xy * 2.0f - 1.0f) * m_shimmerness;
	TexCoord=float3((screenTex + shimmer) * WH,0);
	float3 shimmerSample=m_backBufferMap.Load(TexCoord ).xyz;
    return float4(shimmerSample,particleSample.a);
#else
	return particleSample;
#endif
}

