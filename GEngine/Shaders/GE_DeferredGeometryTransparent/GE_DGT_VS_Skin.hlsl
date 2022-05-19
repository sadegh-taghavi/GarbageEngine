#include "GE_DGT.hlsli"

void main( VS_Input Input, out PS_Input Output )
{
	CalculateOutput skin;
	CalculateSkin( Input, skin );
	Output.Texcoord = m_vertexDataTexcoord[ Input.VertexID ];
	Output.VertexColor = m_vertexDataColor[ Input.VertexID ];
	Output.PositionOffset = mul( float4( skin.Position, 1.0f ), m_world ).xyz;
	Output.ViewVec = Output.PositionOffset - m_viewPosition.xyz;
	Output.PositionOffset -= m_environmentPosition;
	Output.PositionOffset *= m_environmentRadiusInv;
	Output.Position = mul( float4( skin.Position, 1.0f ), m_worldViewProjection );
	Output.Normal = normalize( mul( skin.Normal, ( float3x3 )m_world ) );
	Output.Tangent.xyz = normalize( mul( skin.Tangent, ( float3x3 )m_world ) );
	Output.Tangent.w = Input.Tangent.w;
	Output.ScreenTexcoord.xyz = Output.Position.xyw;
}