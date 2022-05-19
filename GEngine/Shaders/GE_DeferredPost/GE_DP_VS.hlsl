#include "GE_DP.hlsli"

void main( VS_Input Input, out VS_Output Output )
{
	Output.Position = mul( float4( Input.Position * m_lightRange, 1.0f ), m_worldViewProjection );
	Output.Texcoord = Output.Position.xyw;
}