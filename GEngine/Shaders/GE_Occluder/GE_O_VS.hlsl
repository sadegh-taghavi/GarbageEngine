#include "GE_Occluder.hlsli"

void main( VS_Input Input, out VS_Output Output )
{
	Output.Position = mul( float4( Input.Position, 1.0f ), m_worldViewProjection );
}