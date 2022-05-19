#include "RenderUtility.hlsli"

void main(VS_INPUT Input,out PS_INPUT Output)
{
	Output.Position=mul(m_worldViewProjection,float4(mul((float3x3)m_inverseView,Input.Position),1.0f));
	Output.Color=m_vertexDataColor[Input.VertexID];
	Output.Texcoord=m_vertexDataTexcoord[Input.VertexID];
	Output.ScreenTexcoord_Depth=Output.Position;
}
