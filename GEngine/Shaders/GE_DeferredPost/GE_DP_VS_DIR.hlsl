#include "GE_DP.hlsli"

void main( VS_Input Input, out VS_Dir_Output Output )
{
	Output.Position = float4( sign( Input.Position.xy ), 0.0f, 1.0f );
	Output.Texcoord = ( float2( Output.Position.x, -Output.Position.y ) + 1.0f ) * 0.5f;
}