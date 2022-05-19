#include "GE_Basic.hlsli"

void main( VS_Input Input, out PS_Input Output )
{
	float4 pos = mul( float4( Input.Position, 1.0f ), ExtractInstanceMatrix( Input.InstanceID ) );
		pos.w = 1.0f;
	Output.Position = mul( pos, m_viewProjection );
	Output.Texcoord = m_vertexDataTexcoord[ Input.VertexID ].xy;
}
