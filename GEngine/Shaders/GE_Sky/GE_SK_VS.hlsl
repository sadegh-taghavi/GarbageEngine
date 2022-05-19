#include "GE_SKY.hlsli"

float scale( float i_cos )
{
	float x = 1.0 - i_cos;
	return m_scaleDepth * exp( -0.00287 + x*( 0.459 + x*( 3.83 + x*( -6.80 + x*5.25 ) ) ) );
}

void main( VS_Input Input, out VS_Output Output )
{
	Output.Normal = normalize( Input.Position.xyz );
	Input.Position.xyz *= m_scale;
	Input.Position.xy += m_position.xy;
	Output.Position = mul( float4( Input.Position, 1.0f ), m_viewProjection );

	Output.CubeTexcoord1 = mul( Output.Normal, ( float3x3 )m_map1Rotation );
	Output.CubeTexcoord1 = float3( Output.CubeTexcoord1.x * m_map1ScaleZ, Output.CubeTexcoord1.z + m_map1PositionZ, Output.CubeTexcoord1.y * m_map1ScaleZ );
	Output.CubeTexcoord2 = mul( Output.Normal, ( float3x3 )m_map2Rotation );
	Output.CubeTexcoord2 = float3( Output.CubeTexcoord2.x * m_map2ScaleZ, Output.CubeTexcoord2.z + m_map2PositionZ, Output.CubeTexcoord2.y * m_map2ScaleZ );
	
	float3 v3Pos = Output.Normal;
	v3Pos.z += m_innerRadius;
	float3 v3Ray = v3Pos - m_cameraPos.xyz;
	float fFar = length( v3Ray );
	v3Ray /= fFar;

	float3 v3Start = m_cameraPos.xyz;
	float fHeight = length( v3Start );
	float fDepth = exp( m_scaleOverScaleDepth * ( m_innerRadius - m_cameraHeight ) );
	float fStartAngle = dot( v3Ray, v3Start ) / fHeight;
	float fStartOffset = fDepth*scale( fStartAngle );

	float fSampleLength = fFar / ( float )m_numberOfSamples;
	float fScaledLength = fSampleLength * m_invScale;
	float3 v3SampleRay = v3Ray * fSampleLength;
	float3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	float3 v3FrontColor = float3( 0.0, 0.0, 0.0 );
	for ( uint i = 0; i < m_numberOfSamples; i++ )
	{
		float fHeight = length( v3SamplePoint );
		float fDepth = exp( m_scaleOverScaleDepth * ( m_innerRadius - fHeight ) );
		float fLightAngle = dot( m_lightDir.xyz, v3SamplePoint ) / fHeight;
		float fCameraAngle = dot( v3Ray, v3SamplePoint ) / fHeight;
		float fScatter = ( fStartOffset + fDepth*( scale( fLightAngle ) - scale( fCameraAngle ) ) );
		float3 v3Attenuate = exp( -fScatter * ( m_invWavelength_Intensity.xyz * m_kr4PI + m_km4PI ) );
		v3FrontColor += v3Attenuate * ( fDepth * fScaledLength );
		v3SamplePoint += v3SampleRay;
	}
	Output.Color1.rgb = v3FrontColor * m_kmESun;
	Output.Color0.rgb = v3FrontColor * ( m_invWavelength_Intensity.xyz * m_krESun );
	Output.Direction = m_cameraPos.xyz - v3Pos;
}
