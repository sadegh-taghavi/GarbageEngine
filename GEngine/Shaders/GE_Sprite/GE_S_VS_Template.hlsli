#include "GE_Sprite.hlsli"

void main(VS_Input Input,out VS_Output Output)
{
	Input.Position.z=0;
	Input.Position.xy*=m_size_Shimmerness.xy * 0.5f;

#ifdef INSTANCE
	uint index=Input.InstanceID * 10;
	Output.Diffuse=m_instances[index];
	Output.Emissive_Layer = m_instances[index+1];
	float4x4 uv=float4x4(m_instances[index+2],m_instances[index+3],m_instances[index+4],m_instances[index+5]);
	float4x4 world=float4x4(m_instances[index+6],m_instances[index+7],m_instances[index+8],m_instances[index+9]);
	Output.Position=mul(float4(Input.Position,1.0f),mul(world,m_projection));
	Output.Texcoord.xy=uv[Input.VertexID].xy;
#else
	Output.Position=mul(float4(Input.Position,1.0f),mul(m_world,m_projection));
	Output.Texcoord.xy=m_uVRectangle[Input.VertexID].xy;
	Output.Diffuse = m_diffuse;
	Output.Emissive_Layer = m_emissive_Layer;
#endif
	Output.Shimmerness = m_size_Shimmerness.zw;
	Output.Texcoord.zw=((Output.Position.xy / Output.Position.w) + float2(1.0f,1.0f)) * 0.5f;
	Output.Texcoord.w=1.0f - Output.Texcoord.w;
}