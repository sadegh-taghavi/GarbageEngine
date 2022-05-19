#include "GE_DGT.hlsli"

void main( VS_Input Input, out PS_Input Output )
{
	Output.Texcoord = m_vertexDataTexcoord[ Input.VertexID ];
	Output.VertexColor = m_vertexDataColor[ Input.VertexID ];
	float4x4 newWorld = ExtractInstanceMatrix( Input.InstanceID );
	Output.PositionOffset = mul( float4( Input.Position, 1.0f ), newWorld ).xyz;
	Output.ViewVec = Output.PositionOffset - m_viewPosition.xyz;
	Output.PositionOffset -= m_environmentPosition;
	Output.PositionOffset *= m_environmentRadiusInv;
	Output.Position = mul( float4( Input.Position, 1.0f ), mul( newWorld, m_viewProjection ) );
	Output.Normal = normalize( mul( Input.Normal, ( float3x3 )newWorld ) );
	Output.Tangent.xyz = normalize( mul( Input.Tangent.xyz, ( float3x3 )newWorld ) );
	Output.Tangent.w = Input.Tangent.w;
	Output.ScreenTexcoord.xyz = Output.Position.xyw;
}
