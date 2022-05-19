#include "GE_DGT.hlsli"

void main(PS_Input_FaceDir Input, out PS_Output Output)
{
	float2 InputScreenTex = Input.ScreenTexcoord.xy / Input.ScreenTexcoord.z;
	float2 Texcoord = 0.5f * (InputScreenTex + float2(1.0f, 1.0f));
	Texcoord.y = 1.0f - Texcoord.y;

	float4 diffuse = m_diffuseAlphaMap[ 0 ].Sample( m_samLinear, Input.Texcoord.xy );
	Input.Normal = ( Input.FrontFace ? Input.Normal : Input.Normal * -1.0f );

	float4 glow_spec_refMask = m_glowSpecularMap[ 0 ].Sample( m_samLinear, Input.Texcoord.xy );
	float4 bump = m_normalHeightMap[ 0 ].Sample( m_samLinear, float3( Input.Texcoord.xy,0.0f ) );

#ifdef MULTI_LAYER
	float lerpVal = saturate( Input.VertexColor.w * m_layerMultiply + m_layerOffset );
	diffuse = lerp(diffuse, m_diffuseAlphaMap[1].Sample(m_samLinear, Input.Texcoord.xy ), lerpVal);
	glow_spec_refMask = lerp( glow_spec_refMask, m_glowSpecularMap[ 1 ].Sample( m_samLinear, Input.Texcoord.xy ), lerpVal );
	bump = lerp(bump, m_normalHeightMap[1].Sample(m_samLinear, float3(Input.Texcoord.xy, 0.0f) ), lerpVal);
#endif

	diffuse.xyz *= saturate( Input.VertexColor.xyz + m_useVertexColor.xxx );
	float glowness = glow_spec_refMask.x * m_glowness;

	bump.xyz = lerp( float3( 0.5f, 0.5f, 1.0f ), bump.xyz, m_bumpiness ) * 2.0f - 1.0f;

	float3 BiNorm = normalize( cross( Input.Tangent.xyz, Input.Normal ) ) * Input.Tangent.w;

	float3 Normal = normalize( bump.x * Input.Tangent.xyz + bump.y * BiNorm + bump.z * Input.Normal );
		
#ifdef REFLECT
	float3 rf = reflect( normalize( Input.ViewVec ), Normal );
	float3 ref = m_reflectMap.SampleLevel( m_samLinear, rf.xzy, m_roughness * glow_spec_refMask.w ).xyz;
	diffuse.xyz *= lerp( float3( 1.0f, 1.0, 1.0 ), ref, glow_spec_refMask.z * m_reflectmask );
#endif

#ifdef LIGHT_MAP
	float lmV = m_lightMap.Sample( m_samLinear, Input.Texcoord.zw ).x * 0.5f + 0.5f;
	diffuse.xyz *= lmV;
#endif

	float ShadowFactor = 1.0f;
	float NdotL = dot( Normal, m_lightDirection.xyz );
	float SNdotL = max( 0.0f, NdotL );
	float3 diffuseColor = lerp( m_ambient, m_diffuse, ShadowFactor );
	float3 lightDiffuse = lerp( m_lightAmbient.xyz, m_lightDiffuse.xyz, ShadowFactor );

	float3 ViewDir = -normalize( Input.ViewVec );

	float3 H = normalize( m_lightDirection.xyz + ViewDir );
	float HdotN = dot( Normal, H );
	HdotN = pow( max( 0.0f, HdotN ), m_specularPower ) * ( ( m_specularPower + 2.0f ) * 0.125f );

	float3 SColor = m_lightSpecular.xyz * m_specular;
	SColor = SColor + ( 1.0f - SColor ) * pow( 1.0f - dot( H, m_lightDirection.xyz ), 5.0f );

	float alpha = 1.0f / ( sqrt( 0.78539816339744830961566084581988f * m_specularPower + 1.5707963267948966192313216916398f ) );
	SColor *= 1.0f / ( SNdotL * ( 1.0f - alpha ) + alpha ) * ( max( 0.0f, dot( Normal, ViewDir ) ) * ( 1.0f - alpha ) + alpha );

	SColor *= ShadowFactor * SNdotL * glow_spec_refMask.y;

	Output.Diffuse =  float4( ( SNdotL * lightDiffuse * diffuseColor.xyz + HdotN * SColor + m_ambient * m_lightAmbient.xyz  *
		( 1.0f - abs( min( 0.0f, NdotL ) ) ) ) * diffuse.xyz + diffuse.xyz * m_glow * glowness + m_emissive, diffuse.w * m_alpha );

	[branch]
	if (m_refract > 0.1f)
	{
		float2 refract = m_normalHeightMap[0].Sample(m_samLinear, float3(Input.Texcoord.xy, 1.0f)).xy;
		refract = lerp(float2(0.5f, 0.5f), refract, m_refractness) * 2.0f - 1.0f;
		Output.Diffuse = lerp(Output.Diffuse, float4(lerp(m_backBufferMap.Sample(m_samLinear, Texcoord.xy + refract).xyz,
			Output.Diffuse.xyz, Output.Diffuse.w), 1.0f), m_refract);
	}

	[branch]
	if (m_falloff)
		Output.Diffuse.xyz += m_falloffMap.Sample( m_samLinearClamp, float2(
		pow( max( 0.0f, dot( ViewDir, Normal ) ), m_falloffSpread )
		, 0.0f ) ).xyz * m_falloff;
}