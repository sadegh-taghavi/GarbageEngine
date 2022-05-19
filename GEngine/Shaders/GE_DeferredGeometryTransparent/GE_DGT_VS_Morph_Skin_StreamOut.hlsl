#include "GE_DGT.hlsli"

void main( VS_Input Input, out VS_StreamOut Output )
{
	CalculateOutput outp;
	CalculateMorph( Input, outp );

	VS_Input vi = Input;
	vi.Position = outp.Position;
	vi.Normal = outp.Normal;
	vi.Tangent.xyz = outp.Tangent;
	CalculateSkin( vi, outp );

	Output.Position = outp.Position;
	Output.Normal = outp.Normal;
	Output.Tangent.xyz = outp.Tangent;
	Output.Tangent.w = Input.Tangent.w;
	Output.VertexID = Input.VertexID;
}