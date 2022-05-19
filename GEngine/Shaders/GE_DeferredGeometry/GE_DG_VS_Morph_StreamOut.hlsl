#include "GE_DG.hlsli"

void main( VS_Input Input, out VS_StreamOut Output )
{
	CalculateOutput morph;
	CalculateMorph( Input, morph );

	Output.Position = morph.Position;
	Output.Normal = morph.Normal;
	Output.Tangent.xyz = morph.Tangent;
	Output.Tangent.w = Input.Tangent.w;
	Output.VertexID = Input.VertexID;
}
