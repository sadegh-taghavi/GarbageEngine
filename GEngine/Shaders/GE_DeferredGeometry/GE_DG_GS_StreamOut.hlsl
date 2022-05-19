#include "GE_DG.hlsli"

[ maxvertexcount( 3 ) ]
void main( triangle VS_StreamOut Input[ 3 ], inout TriangleStream<VS_StreamOut> Output )
{
	Output.Append( Input[ 0 ] );
	Output.Append( Input[ 1 ] );
	Output.Append( Input[ 2 ] );
	Output.RestartStrip();
}


