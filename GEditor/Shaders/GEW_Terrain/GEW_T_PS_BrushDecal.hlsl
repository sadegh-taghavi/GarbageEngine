#include "GEW_Terrain.hlsli"

float4 main(VS_Output Input) : SV_Target0
{
	float2 center = float2(0.5f, 0.5f);
	float len = length(center - Input.Texcoord);
	[branch]
	if ( len > 0.5f)
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	len *= 2.0f;
	float4 color = ( float4 )0.0f;
	/*float lmf = 1.0f - m_decalFalloff;
	color.w = 1.0f - smoothstep(m_decalFalloff, 1.0f - lmf * m_decalHardness, len)*/;
	
	if ( len > m_decalFalloff - 0.01f && len < m_decalFalloff + 0.01f )
		color = float4( 0.0f, 1.0f, 0.0f, 1.0f );

	float hn = ( 1.0f - m_decalHardness );
	if ( len > hn - 0.01f && len < hn + 0.01f )
		color += float4( 1.0f, 0.0f, 0.0f, 1.0f );
		
	return color;
}