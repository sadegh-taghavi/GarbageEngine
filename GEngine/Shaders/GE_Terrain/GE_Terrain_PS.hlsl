#include "GE_Terrain.hlsli"

void main( PS_Input Input, out PS_Output Output )
{
	Output.Diffuse_Glow = m_heightMap.Sample( m_samLinear, float3( Input.Texcoord.xy, 0.0f ) ) * 0.8f;
	Output.Normal_Specular = float4( 0.5f, 0.5, 1.0f , 1.0f );
	Output.MaterialID = 0.0f;
}











