#include "GE_Decal.hlsli"

void main( GS_Output Input, out PS_Output Output )
{
	float2 InputScreenTex = Input.Texcoord.xy / Input.Texcoord.z;
	float2 Texcoord = 0.5f * ( InputScreenTex + float2( 1.0f, 1.0f ) );
	Texcoord.y = 1.0f - Texcoord.y;

	uint2 WH;
	m_depthMap.GetDimensions( WH.x, WH.y );
	float3 TexCoord = float3( Texcoord * WH, 0 );

	Output.Normal_Specular = m_normalSpecularRTMap.Load( TexCoord );
	float3 norm;
	unpackNormal( Output.Normal_Specular.xyz, norm );

	[ branch ]
	if ( acos( dot( norm, Input.Normal ) ) > Input.MoveUV_MaxAngle.z )
		discard;

	float DM = m_depthMap.Load( TexCoord ).x;
	float4 WorldPos = mul( float4( InputScreenTex, DM, 1.0f ), m_viewProjectionInv );
	WorldPos /= WorldPos.w;

	float4x4 projection = float4x4( Input.ProjectionR0, Input.ProjectionR1, Input.ProjectionR2, Input.ProjectionR3 );

	float4 Tcoord = mul( float4( WorldPos.xyz, 1.0f ), projection );
	Tcoord.xyz /= Tcoord.w;
	
	[ branch ]
	if ( Tcoord.x < -1.0f || Tcoord.x > 1.0f || Tcoord.y < -1.0f || Tcoord.y > 1.0f || Tcoord.z < -1.0f || Tcoord.z > 1.0f )
		discard;
	
	Tcoord.x = 0.5f * Tcoord.x + 0.5f;
	Tcoord.y = -0.5f * Tcoord.y + 0.5f;
	Tcoord.xy *= Input.TileUV_Layer_Rotation.xy;
	Tcoord.xy += Input.MoveUV_MaxAngle.xy;
	float2 ltc = Tcoord.xy;
	ltc.x -= 0.5f;
	ltc.y -= 0.5f;
	float2 tc = Tcoord.xy;
	float ca;
	float sa;
	sincos( -Input.TileUV_Layer_Rotation.w, sa, ca );
	tc.x = ca * ltc.x - sa * ltc.y;
	tc.y = sa * ltc.x + ca * ltc.y;
	tc.x += 0.5f;
	tc.y += 0.5f;
	float3 uv = float3( tc, Input.TileUV_Layer_Rotation.z );

	float4 diffuse = m_diffuseMap.Sample( m_samLinear, uv ) * Input.Color;
			
	Output.Diffuse_Glow = m_diffuseGlowRTMap.Load( TexCoord );
	
	[branch]
	if ( any(diffuse.xyz) )
		Output.Diffuse_Glow.xyz = lerp( Output.Diffuse_Glow.xyz, diffuse.xyz, diffuse.w );

	float4 gsr = (float4)1.0f;

#ifdef N
	Output.Normal_Specular.xyz = lerp( Output.Normal_Specular.xyz,
	lerp( float3( 0.5f, 0.5f, 1.0f ), m_normalMap.Sample( m_samLinear, uv ).xyz, Input.Bumpiness_Glowness_Roughness_Reflectmask.x )
	, diffuse.w  );
#endif

#ifdef M
	gsr = m_glowSpecularMap.Sample( m_samLinear, uv );
	Output.Diffuse_Glow.w = lerp( Output.Diffuse_Glow.w, gsr.x * Input.Bumpiness_Glowness_Roughness_Reflectmask.y, diffuse.w );
	Output.Normal_Specular.w = lerp( Output.Normal_Specular.w, gsr.y, diffuse.w );
#endif

#ifdef R
	float3 ViewVec = WorldPos.xyz - m_viewPosition.xyz;
	float3 rf = reflect(normalize(ViewVec), Output.Normal_Specular.xyz);
	float3 ref = m_reflectMap.SampleLevel(m_samLinear, rf, Input.Bumpiness_Glowness_Roughness_Reflectmask.z * gsr.w).xyz;
	Output.Diffuse_Glow.xyz *= lerp( float3( 1.0f, 1.0, 1.0 ), ref, gsr.z * Input.Bumpiness_Glowness_Roughness_Reflectmask.w * diffuse.w );
#endif

}