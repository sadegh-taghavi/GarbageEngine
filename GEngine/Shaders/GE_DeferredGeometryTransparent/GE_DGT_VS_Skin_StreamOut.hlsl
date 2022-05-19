#include "GE_DGT.hlsli"

void main( VS_Input Input, out VS_StreamOut Output )
{
	CalculateOutput skin;
	CalculateSkin( Input, skin );

	Output.Position = skin.Position;
	Output.Normal = skin.Normal;
	Output.Tangent.xyz = skin.Tangent;
	Output.Tangent.w = Input.Tangent.w;
	Output.VertexID = Input.VertexID;
}
