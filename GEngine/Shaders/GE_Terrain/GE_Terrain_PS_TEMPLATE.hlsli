
#include "GE_Terrain.hlsli"

void main(PS_Input Input, out PS_Output Output)
{
	float3 Normal /*= normalize(Input.Normal)*/;
	float4 heightVal;
	heightVal.x = m_heightMap.Sample(m_samLinear, Input.Texcoord.xy, int2(0, -1)).x;
	heightVal.y = m_heightMap.Sample(m_samLinear, Input.Texcoord.xy, int2(-1, 0)).x;
	heightVal.z = m_heightMap.Sample(m_samLinear, Input.Texcoord.xy, int2(1, 0)).x;
	heightVal.w = m_heightMap.Sample(m_samLinear, Input.Texcoord.xy, int2(0, 1)).x;
	Normal.y = (heightVal.w - heightVal.x);
	Normal.x = (heightVal.y - heightVal.z);
	Normal.z = 2.0f;
	Normal.xy *= m_normalAspect;
	Normal = normalize(Normal);

	float3 Tangent = float3( 1.0f, 0.0f, 0.0f );
	Tangent = normalize( Tangent - Normal * dot( Normal, Tangent ) );
	float3 BiNorm = normalize( cross( normalize( Tangent ), Normal ) );
	float3 vn = float3( 0.0f, 0.0f, 1.0f );
	[branch]
	if ( m_normalNoiseEnable.x )
		vn = m_normalNoiseMap.Sample( m_samLinear, Input.Texcoord.xy * m_normalNoiseUVTile ).xyz * 2.0f - 1.0f;
	Normal = normalize( vn.x * Tangent + vn.y * BiNorm + vn.z * Normal );

	float3 DiffuseVal = (float3)0.0f;
	float3 NormalVal = float3(0.5f, 0.5f, 1.0f);
	float4 GSRMVal = (float4)0.0f;
	uint i;
	float2 uv,uvTile;
	float4 DA;
	float alpha;

#if NUM_PROC > 0
	float2 xy = Input.Texcoord.xy * m_fieldSize;
	float angle = abs(acos(Normal.z));
	float len;

	[unroll]
	for (i = 0; i < NUM_PROC; ++i)
	{
		len = distance(xy, m_perProceduralParams[i].MaskPosition);

		alpha = saturate(1.0f - smoothstep(m_perProceduralParams[i].MaskFalloff, lerp(m_perProceduralParams[i].MaskField,
			m_perProceduralParams[i].MaskFalloff, m_perProceduralParams[i].MaskHardness), len));

		uvTile = Input.Texcoord.xy * m_perProceduralParams[ i ].MaskUVTile - m_perProceduralParams[ i ].MaskUVPosition;
		uv = Input.Texcoord.xy * m_perProceduralParams[i].UVTile + m_perProceduralParams[i].UVPosition;
		DA = m_diffuseMapP[i].Sample(m_samLinear, uv);
		alpha *= m_perProceduralParams[ i ].Opacity * DA.w * m_maskMapP[ i ].Sample( m_samLinear, uvTile ).x;

		float len = abs( angle - m_perProceduralParams[ i ].Angle );
		
		alpha *= saturate(1.0f - smoothstep(m_perProceduralParams[i].AngleFalloff, 
			lerp(m_perProceduralParams[i].AngleField, m_perProceduralParams[i].AngleFalloff, m_perProceduralParams[i].AngleHardness), len)) ;

		len = abs(Input.ViewVec_Height.w - m_perProceduralParams[i].Height);
		alpha *= saturate(1.0f - smoothstep(m_perProceduralParams[i].HeightFalloff,
			lerp(m_perProceduralParams[i].HeightField, m_perProceduralParams[i].HeightFalloff, m_perProceduralParams[i].HeightHardness), len));
		if (alpha)
		{
			DiffuseVal = lerp(DiffuseVal, DA.xyz, alpha);
			NormalVal = lerp(NormalVal, m_normalMapP[i].Sample(m_samLinear, uv).xyz, alpha);
			GSRMVal = lerp(GSRMVal, m_gSRMapP[i].Sample(m_samLinear, uv), alpha);
		}
	}
#endif

#if NUM_MASK > 0
	float4 mask = m_maskMap.Sample( m_samLinear, Input.Texcoord.xy );

		[ unroll ]
	for ( i = 0; i < NUM_MASK; ++i )
	{
		uv = Input.Texcoord.xy * m_perMaskParams[ i ].UVTile + m_perMaskParams[ i ].UVPosition;
		DA = m_diffuseMapM[ i ].Sample( m_samLinear, uv );
		alpha = mask[ i ] * m_perMaskParams[ i ].Opacity * DA.w;

		if ( alpha )
		{
			DiffuseVal = lerp( DiffuseVal, DA.xyz, alpha );
			NormalVal = lerp( NormalVal, m_normalMapM[ i ].Sample( m_samLinear, uv ).xyz, alpha );
			GSRMVal = lerp( GSRMVal, m_gSRMapM[ i ].Sample( m_samLinear, uv ), alpha );
		}
	}
#endif

	Output.Diffuse_Glow.xyz = DiffuseVal;
	Output.Normal_Specular.xyz = lerp(float3(0.5f, 0.5f, 1.0f), NormalVal, m_bumpiness) * 2.0f - 1.0f;
	Output.Diffuse_Glow.w = GSRMVal.x;
	Output.Normal_Specular.w = GSRMVal.y;

	/*Tangent = float3( 1.0f, 0.0f, 0.0f );
	Tangent = normalize( Tangent - Normal * dot( Normal, Tangent ) );
	BiNorm = normalize( cross( normalize( Tangent ), Normal ) );*/

	float3 bN = normalize(Output.Normal_Specular.x * Tangent + Output.Normal_Specular.y
	* BiNorm + Output.Normal_Specular.z * Normal);


//#ifdef REFLECT
[branch]
if (m_reflectmask)
{
	float3 rf = reflect(normalize(Input.ViewVec_Height.xyz), bN);
	float3 ref = m_reflectMap.SampleLevel(m_samLinear, rf.xzy, m_roughness * GSRMVal.w).xyz;
	Output.Diffuse_Glow.xyz *= lerp(float3(1.0f, 1.0, 1.0), ref, GSRMVal.z * m_reflectmask);
}
	
//#endif

	packNormal(bN, Output.Normal_Specular.xyz);
	Output.MaterialID = m_materialID;
}



















