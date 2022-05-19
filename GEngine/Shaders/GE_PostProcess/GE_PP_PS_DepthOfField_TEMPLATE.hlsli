#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{	
	uint2 WH;
	m_texture1.GetDimensions( WH.x, WH.y );
	float3 TexCoord = float3( Input.Texcoord * WH, 0 );
	float depth = m_texture1.Load( TexCoord ).x;
	
	float4 CurrentPos = float4( Input.Texcoord.x * 2.0f - 1.0f, (1.0f - Input.Texcoord.y) * 2.0f - 1.0f, depth, 1.0f ); 
	float4 WorldPos = mul( CurrentPos, m_viewProjectionInv );
	WorldPos /=WorldPos.w;
	float3 pos = mul( WorldPos,m_view ).xyz;
	pos.z *= -1;

	//m_float4Val0.x use as 1.0f / range
	//m_float4Val0.y use as focus

#ifdef DOF_AUTO

	float centerDepth = m_texture1.Load( float3( WH * 0.5f, 0 ) ).x;
	float4 CenterPos = float4( Input.Texcoord.x * 2.0f - 1.0f, (1.0f - Input.Texcoord.y) * 2.0f - 1.0f, centerDepth, 1.0f ); 
	float4 WorldCenterPos = mul( CenterPos, m_viewProjectionInv );
	WorldCenterPos /= WorldCenterPos.w;
	float3 center = mul( WorldCenterPos, m_view ).xyz;
	center.z *= -1;
	float focus = m_float4Val0.x * abs( center.z - pos.z );

#else
	float focus = m_float4Val0.x * abs( m_float4Val0.y - pos.z );

#endif

	float3 cb = m_texture2.Sample( m_samLinearClamp,Input.Texcoord ).xyz;
	float4 c = m_texture.Sample( m_samLinearClamp, Input.Texcoord );
	return float4( lerp( c.xyz, cb, clamp( focus, 0.1f, 0.9f ) ), c.w);
} 
