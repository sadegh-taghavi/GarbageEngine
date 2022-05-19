#include "GE_Basic.hlsli"

void main( VS_Input Input, out VS_StreamOut Output )
{
	CalculateOutput morph;
	CalculateMorph( Input, morph );

	Output.Position = morph.Position;
	Output.Normal = 0;
	Output.Tangent = 0;
	Output.VertexID = Input.VertexID;
}
