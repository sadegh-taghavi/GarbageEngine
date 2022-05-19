#include "GE_Terrain.hlsli"

void main( VS_Input Input, out VS_StreamOut Output )
{
	float4 TexcoordProjection = mul( float4( Input.Position.xy, 0.0f, 1.0f ), m_texcoordViewProjection );
	float2 InputScreenTex = TexcoordProjection.xy / TexcoordProjection.w;
	float2 Texcoord = 0.5f * ( InputScreenTex + float2( 1.0f, 1.0f ) );
	Texcoord.y = 1.0f - Texcoord.y;
	Output.Position.z = m_heightMap.SampleLevel( m_samPoint, Texcoord.xy, 0 ).x * m_maxHeight;
	//Output.Position.z += m_heightBias;
	Output.Position.xy = Input.Position.xy;
}