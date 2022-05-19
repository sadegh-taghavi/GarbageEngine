#include "GE_DP.hlsli"

float4 main( VS_Output Input ) : SV_Target0
{
	float2 ScreenTexcoord = Input.Texcoord.xy / Input.Texcoord.z;
	float2 Texcoord = 0.5f * ( ScreenTexcoord + float2( 1.0f, 1.0f ) );
	Texcoord.y = 1.0f - Texcoord.y;

	uint2 WH;
	m_depthMap.GetDimensions( WH.x, WH.y );
	float3 TexCoord = float3( Texcoord * WH, 0 );

	float DM = m_depthMap.Load( TexCoord );
	if (DM == 1.0f)
		discard;

	float4 WorldPos = mul( float4( ScreenTexcoord, DM, 1.0f ), m_viewProjectionInv );
		WorldPos /= WorldPos.w;

	float3 LightVec = m_lightPosition.xyz - WorldPos.xyz;
		float3 LightDir = normalize( LightVec );
		float ldd = max( 0.0f, dot( LightDir, m_lightDirection.xyz ) );

	float lo = saturate( m_lightAttenuation * ( m_lightRange - length( LightVec ) )  * smoothstep( m_lightSpotSpread, m_lightSpotSharpness, ldd ) );

	float3 Normal;
	float4 NormalSpecular = m_normal_SpecularMap.Load( TexCoord );
	unpackNormal( NormalSpecular.xyz, Normal );
	float4 MID = m_materialIDMap.Load( TexCoord );

	Material_Data mat;
	LoadMaterial( round( MID.x * 255.0f ), mat );

	float ShadowFactor = 1.0f;

#ifdef SHADOW
	float4 shadowCoord = mul( float4( WorldPos.xyz + Normal * m_shadowOffsetBias[ 0 ].x, 1.0f ), m_lightViewProjection[ 0 ] );
		shadowCoord.xyz /= shadowCoord.w;
	if ( shadowCoord.x < -0.99f || shadowCoord.x > 0.99f || shadowCoord.y < -0.99f
		|| shadowCoord.y > 0.99f || shadowCoord.z < -1.0f || shadowCoord.z > 1.0f )
		ShadowFactor = 1.0f;
	else
		CalculateShadowFactor( 0, shadowCoord.xyz, ShadowFactor );
#endif
	
	float NdotL = dot( Normal, LightDir );
	float SNdotL = max( 0.0f, NdotL );
	mat.Diffuse.xyz = lerp( mat.Ambient, mat.Diffuse.xyz, ShadowFactor );
	float3 lightDiffuse = lerp( m_lightAmbient.xyz, m_lightDiffuse.xyz, ShadowFactor );

	float3 ViewDir = normalize( m_viewPosition.xyz - WorldPos.xyz );

	float3 H = normalize( LightDir + ViewDir );
	float HdotN = dot( Normal, H );
	HdotN = pow( max( 0.0f, HdotN ), mat.SpecularPower ) * ( ( mat.SpecularPower + 2.0f ) * 0.125f );

	float3 SColor = m_lightSpecular.xyz * mat.Specular;
		SColor = SColor + ( 1.0f - SColor ) * pow( 1.0f - dot( H, m_lightDirection.xyz ), 5.0f );

	float alpha = 1.0f / ( sqrt( 0.78539816339744830961566084581988f * mat.SpecularPower + 1.5707963267948966192313216916398f ) );
	SColor *= 1.0f / ( SNdotL * ( 1.0f - alpha ) + alpha ) * ( max( 0.0f, dot( Normal, ViewDir ) ) * ( 1.0f - alpha ) + alpha );

	SColor *= ShadowFactor * SNdotL * NormalSpecular.w;

	float4 color = m_diffuse_GlowMap.Load( TexCoord );

	float3 Composite = ( SNdotL * lightDiffuse * mat.Diffuse.xyz + HdotN * SColor + mat.Ambient * m_lightAmbient.xyz  * ( 1.0f - abs( min( 0.0f, NdotL ) ) ) ) * color.xyz;

#ifdef FILTER
	float4 Tcoord = mul( float4( WorldPos.xyz, 1.0f ), m_lightViewProjection[ 0 ] );
		Tcoord.xyz /= Tcoord.w;

	if ( Tcoord.x < -0.99f || Tcoord.x > 0.99f || Tcoord.y < -0.99f || Tcoord.y > 0.99f || Tcoord.z < -1.0f || Tcoord.z > 1.0f )
		Composite = Composite;
	else
	{
		Tcoord.x = 0.5f * Tcoord.x + 0.5f;
		Tcoord.y = -0.5f * Tcoord.y + 0.5f;
		Composite *= m_lightFilterMap.Sample( m_samLinearB, Tcoord.xy ).xyz;
	}
#endif
	return float4( Composite * lo, 1.0f );
}