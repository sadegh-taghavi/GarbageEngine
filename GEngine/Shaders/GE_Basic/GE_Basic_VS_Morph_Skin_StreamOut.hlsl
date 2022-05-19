#include "GE_Basic.hlsli"

void main( VS_Input Input, out VS_StreamOut Output )
{
	CalculateOutput outp;
	CalculateMorph( Input, outp );

	VS_Input vi = Input;
	vi.Position = outp.Position;
	CalculateSkin( vi, outp );

	Output.Position = outp.Position;
	Output.Normal = 0;
	Output.Tangent = 0;
	Output.VertexID = Input.VertexID;
}