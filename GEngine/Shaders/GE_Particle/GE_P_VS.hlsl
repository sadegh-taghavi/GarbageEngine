#include "GE_Particle.hlsli"

void main(VS_Input Input,out VS_Output Output)
{
	Output.Position_Layer=Input.Position_Layer;
	Output.Emissive_Rotation=Input.Emissive_Rotation;
	Output.Diffuse=Input.Diffuse;
	Output.Size_InstanceID.xy=Input.Size;
	Output.Size_InstanceID.z=Input.InstanceID;
}