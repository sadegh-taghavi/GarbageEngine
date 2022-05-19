#include "GE_PostProcess.hlsli"

float4 main( VS_Output Input ) : SV_Target0
{
	uint2 WH;
	m_texture1.GetDimensions( WH.x, WH.y );
	float3 TexCoord = float3( Input.Texcoord * WH, 0 );
		float depth = m_texture1.Load( TexCoord ).x;
	float4 CurrentPos = float4( Input.Texcoord.x * 2.0f - 1.0f, ( 1.0f - Input.Texcoord.y ) * 2.0f - 1.0f, depth, 1.0f );
	float4 WorldPos = mul( CurrentPos, m_viewProjectionInv );
	WorldPos /= WorldPos.w;

	float4 PreviousPos = mul( WorldPos, m_previousViewProjection );
		PreviousPos /= PreviousPos.w;

	float2 velocity = ( CurrentPos.xy - PreviousPos.xy ) * m_float4Val0.x; // use as damp
		velocity.y = -velocity.y;
	float len = length( velocity );
	if ( len > m_float4Val0.y ) // use as max
		len = m_float4Val0.y;
	velocity = normalize( velocity );
	float velStep = len * 0.1f;

	float4 c = m_texture.Sample( m_samLinearClamp, Input.Texcoord );

		[ unroll ]
	for ( int i = 1; i < 10; i++ )
		c.xyz += m_texture.Sample( m_samLinearClamp, Input.Texcoord + velocity * velStep * i ).xyz;
	c.xyz *= 0.1f;
	return c;
}