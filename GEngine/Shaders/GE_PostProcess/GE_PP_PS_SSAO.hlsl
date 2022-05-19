#include "GE_PostProcess.hlsli"

void getPosition( float2 i_uv, out float3 i_pos )
{
	float d = m_texture1.SampleLevel( m_samPointClamp, i_uv, 0 ).x;
	float4 pos = float4( i_uv.x * 2.0f - 1.0f, ( 1.0f - i_uv.y ) * 2.0f - 1.0f, d, 1.0f );
	pos = mul( pos, m_viewProjectionInv );
	pos /= pos.w;
	i_pos = mul( pos, m_view ).xyz;
	i_pos.z *= -1.0f;
}
void unpackNormal( float3 i_packedNormal, out float3 i_normal )
{
	i_normal = 2.0f * i_packedNormal - 1.0f;
	i_normal = normalize( i_normal );
}
float doAO( float2 i_texcoord, float3 i_vPos, float3 i_normal )
{
	float3 diff;
	getPosition( i_texcoord, diff );
	diff -= i_vPos;
	float3 v = normalize( diff );
	float d = length( diff );
	return max( 0.0f, dot( i_normal, v ) - m_float4Val0.z ) *
		( 1.0f / ( 1.0f + d * m_float4Val0.y ) ) * m_float4Val0.w  *
		( 1.0f - smoothstep( m_float4Val1.x, m_float4Val1.y, d ) );
}

float4 main( VS_Output Input ) : SV_Target0
{
	const float2 vec[ 4 ] = { float2( 1.0f, 0.0f ), float2( -1.0f, 0.0f ),
	float2( 0.0f, 1.0f ), float2( 0.0f, -1.0f ) };

	float depth = m_texture1.SampleLevel( m_samPointClamp, Input.Texcoord, 0 ).x;

	clip( 0.9999f - depth );

	float4 pos = float4( Input.Texcoord.x * 2.0f - 1.0f, ( 1.0f - Input.Texcoord.y ) * 2.0f - 1.0f, depth, 1.0f );
	pos = mul( pos, m_viewProjectionInv );
	pos /= pos.w;
	float3 vpos = mul( pos, m_view ).xyz;
	vpos.z *= -1.0f;

	float4 pd = m_texture2.SampleLevel( m_samPointClamp, Input.Texcoord, 0 );
	float3 normal;
	unpackNormal( pd.xyz, normal );
	normal = mul( normal, ( float3x3 )m_view ).xyz;
	normal.z *= -1.0f;

	float ao = 0.0f;
	float rad = m_float4Val0.x / vpos.z;
	if ( rad < 5.0f )
		rad = 5.0f;
	
	for ( int j = 0; j < 4; ++j )
	{
		float2 coord1 = vec[ j ] * rad;
		float2 coord2 = float2( coord1.x * 0.707f - coord1.y * 0.707f, coord1.x * 0.707f + coord1.y * 0.707f );

		ao += doAO( Input.Texcoord + coord1 * 0.25f * m_float4Val1.zw, vpos, normal );
		ao += doAO( Input.Texcoord + coord2 * 0.5f * m_float4Val1.zw, vpos, normal );
		ao += doAO( Input.Texcoord + coord1 * 0.75f * m_float4Val1.zw, vpos, normal );
		ao += doAO( Input.Texcoord + coord2 * m_float4Val1.zw, vpos, normal );
	}
	ao *= 0.0625f;
	return (1.0f - ao ) * m_texture.SampleLevel( m_samPointClamp, Input.Texcoord, 0 );
}