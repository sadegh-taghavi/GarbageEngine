#include "GE_Basic.hlsli"

void main( VS_Input Input, out PS_Input Output )
{
	CalculateOutput outp;
	CalculateMorph( Input, outp );

	VS_Input vi = Input;
	vi.Position = outp.Position;
	CalculateSkin( vi, outp );

	Output.Position = mul( float4( outp.Position, 1.0f ), m_worldViewProjection );
	Output.Texcoord = m_vertexDataTexcoord[ Input.VertexID ].xy;
}