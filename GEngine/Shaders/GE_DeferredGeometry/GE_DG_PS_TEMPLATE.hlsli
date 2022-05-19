#include "GE_DG.hlsli"

void main(PS_Input_FaceDir Input, out PS_Output Output)
{
	Input.Normal = (Input.FrontFace  ? Input.Normal : Input.Normal * -1.0f );
	
	Output.Diffuse_Glow = m_diffuseAlphaMap[ 0 ].Sample( m_samLinear, Input.Texcoord.xy );

	clip( Output.Diffuse_Glow.w - m_alphaTest );

	float4 glow_spec_refMask = m_glowSpecularMap[ 0 ].Sample( m_samLinear, Input.Texcoord.xy );
	float4 bump = m_normalHeightMap[ 0 ].Sample( m_samLinear, Input.Texcoord.xy );

#ifdef MULTI_LAYER
	float lerpVal = saturate( Input.VertexColor.w * m_layerMultiply + m_layerOffset );
	Output.Diffuse_Glow = lerp( Output.Diffuse_Glow, m_diffuseAlphaMap[ 1 ].Sample( m_samLinear, Input.Texcoord.xy ), lerpVal );
	glow_spec_refMask = lerp( glow_spec_refMask, m_glowSpecularMap[ 1 ].Sample( m_samLinear, Input.Texcoord.xy ), lerpVal );
	bump = lerp( bump, m_normalHeightMap[ 1 ].Sample( m_samLinear, Input.Texcoord.xy ), lerpVal );
#endif

	Output.Diffuse_Glow.xyz *= saturate( Input.VertexColor.xyz + m_useVertexColor.xxx );
	Output.Diffuse_Glow.w = glow_spec_refMask.x * m_glowness;

	bump.xyz = lerp( float3( 0.5f, 0.5f, 1.0f ), bump.xyz, m_bumpiness ) * 2.0f - 1.0f;
	
	float3 BiNorm = normalize( cross( Input.Tangent.xyz, Input.Normal ) ) * Input.Tangent.w;
		//float3x3 BTNMatrix = float3x3( Input.Tangent, BiNorm, Input.Normal );

		/*float3 dp1 = ddx( -Input.ViewVec );
		float3 dp2 = ddy( -Input.ViewVec );
		float2 duv1 = ddx( Input.Texcoord.xy );
		float2 duv2 = ddy( Input.Texcoord.xy );

		// solve the linear system
		float3 dp2perp = cross( dp2, Input.Normal );
		float3 dp1perp = cross( Input.Normal, dp1 );
		float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
		float3 B = dp2perp * duv1.y + dp1perp * duv2.y;

		// construct a scale-invariant frame
		float invmax = rsqrt( max( dot(T,T), dot(B,B) ) );
		float3x3 BTNMatrix = float3x3( T * invmax, B * invmax, Input.Normal );

		float3 bN = normalize( mul(bump.xyz, BTNMatrix) );*/

	float3 bN = normalize( bump.x * Input.Tangent.xyz + bump.y * BiNorm + bump.z * Input.Normal );

	packNormal( bN, Output.Normal_Specular.xyz );
	Output.MaterialID = m_materialID;
	Output.Normal_Specular.w = glow_spec_refMask.y;
	
#ifdef REFLECT
	float3 rf = reflect( normalize( Input.ViewVec ), bN );
	float3 ref = m_reflectMap.SampleLevel( m_samLinear, rf.xzy, m_roughness * glow_spec_refMask.w ).xyz;
	Output.Diffuse_Glow.xyz *= lerp( float3( 1.0f, 1.0, 1.0 ), ref, glow_spec_refMask.z * m_reflectmask );
#endif

#ifdef LIGHT_MAP
	float lmV = m_lightMap.Sample( m_samLinear, Input.Texcoord.zw ).x * 0.5f + 0.5f;
	Output.Diffuse_Glow.xyz *= lmV;
#endif

	[branch]
	if (m_falloff)
		Output.Diffuse_Glow.xyz += m_falloffMap.Sample( m_samLinearClamp, float2(
		pow( max( 0.0f, dot( normalize( -Input.ViewVec ), bN ) ) , m_falloffSpread )
		, 0.0f ) ).xyz * m_falloff;
}