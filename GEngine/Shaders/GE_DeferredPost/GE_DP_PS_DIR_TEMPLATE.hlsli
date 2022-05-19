#include "GE_DP.hlsli"

float4 main( VS_Dir_Output Input ) : SV_Target0
{
	uint2 WH;
	m_depthMap.GetDimensions( WH.x, WH.y );
	float3 TexCoord = float3( Input.Texcoord * WH, 0 );

	float DM = m_depthMap.Load( TexCoord );

	if (DM == 1.0f)
		discard;

	float3 Normal;
	float4 NormalSpecular = m_normal_SpecularMap.Load( TexCoord );
	unpackNormal( NormalSpecular.xyz, Normal );
	float4 MID = m_materialIDMap.Load( TexCoord );

	Material_Data mat;
	LoadMaterial( round( MID.x * 255.0f ), mat );

	float4 CurrentPos = float4( Input.Texcoord.x * 2.0f - 1.0f, ( 1.0f - Input.Texcoord.y ) * 2.0f - 1.0f, DM, 1.0f );
	float4 WorldPos = mul( CurrentPos, m_viewProjectionInv );
	WorldPos /= WorldPos.w;

	float ShadowFactor = 1.0f;
	float4 shadowCoord;

	[ unroll ]
	for ( int i = NUMBER_OF_SHADOW - 1; i >= 0; --i )
	{
		shadowCoord = mul( float4( WorldPos.xyz + Normal * m_shadowOffsetBias[ i ].x, 1.0f ), m_lightViewProjection[ i ] );
		shadowCoord.xyz /= shadowCoord.w;
		if ( shadowCoord.x >= -0.99f && shadowCoord.x <= 0.99f && shadowCoord.y >= -0.99f
			&& shadowCoord.y < 0.99f && shadowCoord.z >= -1.0f && shadowCoord.z <= 1.0f )
			CalculateShadowFactor( i, shadowCoord.xyz, ShadowFactor );
	}

	float NdotL = dot( Normal, m_lightDirection.xyz );
	float SNdotL = max( 0.0f, NdotL );
	mat.Diffuse.xyz = lerp( mat.Ambient, mat.Diffuse.xyz, ShadowFactor );
	float3 lightDiffuse = lerp( m_lightAmbient.xyz, m_lightDiffuse.xyz, ShadowFactor );

	float3 ViewDir = normalize( m_viewPosition.xyz - WorldPos.xyz );

	float3 H = normalize( m_lightDirection.xyz + ViewDir );
	float HdotN = dot( Normal, H );
	HdotN = pow( max( 0.0f, HdotN ), mat.SpecularPower ) * ( ( mat.SpecularPower + 2.0f ) * 0.125f );

	float3 SColor = m_lightSpecular.xyz * mat.Specular;
	SColor = SColor + ( 1.0f - SColor ) * pow( 1.0f - dot( H, m_lightDirection.xyz ), 5.0f );

	float alpha = 1.0f / ( sqrt( 0.78539816339744830961566084581988f * mat.SpecularPower + 1.5707963267948966192313216916398f ) );
	SColor *= 1.0f / ( SNdotL * ( 1.0f - alpha ) + alpha ) * ( max( 0.0f, dot( Normal, ViewDir ) ) * ( 1.0f - alpha ) + alpha );

	SColor *= ShadowFactor * SNdotL * NormalSpecular.w;

	float4 color = m_diffuse_GlowMap.Load( TexCoord );

		float4 composite = float4( ( SNdotL * lightDiffuse * mat.Diffuse.xyz + HdotN * SColor + mat.Ambient *
		m_lightAmbient.xyz  * ( 1.0f - abs( min( 0.0f, NdotL ) ) ) ) * color.xyz + color.xyz * mat.Glow * color.w + mat.Emissive, 1.0f );

#ifdef VISUALIZE_CASCADE

	[ unroll ]
	for ( i = 0; i < NUMBER_OF_SHADOW; ++i )
	{
		shadowCoord = mul( float4( WorldPos.xyz + Normal * m_shadowOffsetBias[ i ].x, 1.0f ),
			m_lightViewProjection[ i ] );
		shadowCoord.xyz /= shadowCoord.w;

		if ( shadowCoord.x >= -0.99f && shadowCoord.x <= 0.99f && shadowCoord.y >= -0.99f
			&& shadowCoord.y < 0.99f && shadowCoord.z >= -1.0f && shadowCoord.z <= 1.0f )
		{
			if ( i == 0 )
				composite += float4( 0.0f, 0.0f, 0.5f, 1.0f );
			if ( i == 1 )
				composite += float4( 0.5f, 0.0f, 0.0f, 1.0f );
			if ( i == 2 )
				composite += float4( 0.0f, 0.5f, 0.0f, 1.0f );
			if ( i == 3 )
				composite += float4( 0.5f, 0.5f, 0.0f, 1.0f );
		}
	}
	composite *= float4( 1.0f, 1.0f, 1.0f, 1.0f );

#endif
	return composite;
}