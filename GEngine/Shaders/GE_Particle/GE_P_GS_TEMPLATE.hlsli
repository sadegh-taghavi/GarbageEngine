#include "GE_Particle.hlsli"

[maxvertexcount(4)]
void main(point VS_Output Input[1],inout TriangleStream<GS_Output> StreamOut)
{
	GS_Output Output;

	float4 position;

#ifdef INSTANCE
		float4x4 NewWorld=mul(m_world,ExtractInstanceMatrix(Input[0].Size_InstanceID.z));
		position = mul( float4(Input[0].Position_Layer.xyz,1.0f), NewWorld );
#else
		position = mul( float4(Input[0].Position_Layer.xyz,1.0f), m_world );
#endif

	float3 dir=normalize(position.xyz - m_viewPosition.xyz);
	float3 right = normalize(cross(dir,float3(0,0,1)));
	float3 up = cross(right,dir);
	float3 pos[4];
	
	pos[0] = -(	up		* Input[0].Size_InstanceID.y + right	* Input[0].Size_InstanceID.x );
	pos[1] =	right	* Input[0].Size_InstanceID.x - up		* Input[0].Size_InstanceID.y;
	pos[2] =  -	right	* Input[0].Size_InstanceID.x + up		* Input[0].Size_InstanceID.y;
	pos[3] =	up		* Input[0].Size_InstanceID.y + right	* Input[0].Size_InstanceID.x;
		
	[unroll] for(uint i=0; i<4; i++)
    {
		Output.Diffuse=Input[0].Diffuse;
		Output.Emissive_SceneW.xyz=Input[0].Emissive_Rotation.xyz;
		Output.Position = mul( float4(position.xyz + pos[i],1.0f), m_viewProjection );
        Output.Texcoord_Rotation_Layer = float4(m_texcoords[i],Input[0].Emissive_Rotation.w,Input[0].Position_Layer.w);
		Output.Depth_ScreenTexcoord.xy=Output.Position.zw;
        Output.Depth_ScreenTexcoord.zw=Output.Position.xy;
		Output.Emissive_SceneW.w=Output.Position.w;

	    StreamOut.Append(Output);
    }
    StreamOut.RestartStrip();
}