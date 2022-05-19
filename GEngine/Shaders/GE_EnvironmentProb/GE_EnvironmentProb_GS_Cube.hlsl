#include "GE_EnvironmentProb.hlsli"

[ maxvertexcount( 18 ) ]
void main( triangle VS_Output_Cube Input[ 3 ], inout TriangleStream< GS_Output_Cube > Output )
{
	for ( uint f = 0; f < 6; ++f )
	{
		GS_Output_Cube output;
		output.TargetIndex = f;
		for ( uint v = 0; v < 3; ++v )
		{
			output.Position = Input[ v ].Position;
			output.Texcoord = Input[ v ].Texcoord;
			Output.Append( output );
		}
		Output.RestartStrip();
	}
}