#include "GE_Basic.hlsli"

void main( VS_Input Input, out PS_Input Output )
{
	Output.Position = mul( float4( Input.Position, 1.0f ), m_worldViewProjection );
	Output.Texcoord = m_vertexDataTexcoord[ Input.VertexID ].xy;
}