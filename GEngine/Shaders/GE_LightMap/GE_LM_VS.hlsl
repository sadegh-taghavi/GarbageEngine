#include "GE_LM.hlsli"

void main( VS_Input Input, out PS_Input Output )
{
	Output.Texcoord = m_vertexDataTexcoord[ Input.VertexID ];
	float2 tc = Output.Texcoord.zw;
		tc.y = 1.0f - tc.y;
	Output.Position = float4( tc * 2.0f - 1.0f, 0.0f, 1.0f );
	Output.WorldPos = mul( float4( Input.Position, 1.0f ), m_world ).xyz;
	Output.Normal = normalize( mul( Input.Normal, ( float3x3 )m_world ) );
	Output.Tangent.xyz = normalize( mul( Input.Tangent.xyz, ( float3x3 )m_world ) );
	Output.Tangent.w = Input.Tangent.w;
}