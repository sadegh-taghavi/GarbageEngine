#include "GE_Terrain.hlsli"

[ maxvertexcount( 1 ) ]
void main( point VS_StreamOut Input[1], inout PointStream<VS_StreamOut> Output )
{
	Output.Append( Input[ 0 ] );
	Output.RestartStrip();
}


