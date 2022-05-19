struct VS_Input
{
	float3 Position : POSITION;
};

struct VS_Output
{
	float4 Position	:	SV_POSITION;
};

VS_Output main(VS_Input Input)
{
	VS_Output Output;
	Output.Position.xyz=Input.Position.xyz;
	Output.Position.w=1;
	return Output;
}