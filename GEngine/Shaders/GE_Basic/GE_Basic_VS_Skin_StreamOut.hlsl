#include "GE_Basic.hlsli"

void main( VS_Input Input, out VS_StreamOut Output )
{
	CalculateOutput skin;
	CalculateSkin( Input, skin );

	Output.Position = skin.Position;
	Output.Normal = 0;
	Output.Tangent = 0;
	Output.VertexID = Input.VertexID;
}
