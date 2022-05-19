#include "GE_EnvironmentProb.hlsli"

void main( VS_Input Input, out VS_Output Output )
{
	Output.ScreenTexcoord_Depth = Output.Position = mul( float4( -Input.Position, 1.0f ), m_worldViewProjection );
	Output.ViewVec = mul( float4( -Input.Position, 1.0f ), m_world ).xyz - m_viewPosition.xyz;
	Output.Normal = normalize( -Input.Position.xyz );
}