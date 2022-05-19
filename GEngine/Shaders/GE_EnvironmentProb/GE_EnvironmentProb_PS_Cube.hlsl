#include "GE_EnvironmentProb.hlsli"

float4 main( GS_Output_Cube Input ) : SV_TARGET0
{
	float4 color = (float4) 0;
	[ branch ]
	if ( Input.TargetIndex == 0 )
		color = m_sideMaps[ 0 ].SampleLevel( m_sampler, Input.Texcoord, 0.0f );
	else if ( Input.TargetIndex == 1 )
		color = m_sideMaps[ 1 ].SampleLevel( m_sampler, Input.Texcoord, 0.0f );
	else if ( Input.TargetIndex == 2 )
		color = m_sideMaps[ 2 ].SampleLevel( m_sampler, Input.Texcoord, 0.0f );
	else if ( Input.TargetIndex == 3 )
		color = m_sideMaps[ 3 ].SampleLevel( m_sampler, Input.Texcoord, 0.0f );
	else if ( Input.TargetIndex == 4 )
		color = m_sideMaps[ 4 ].SampleLevel( m_sampler, Input.Texcoord, 0.0f );
	else if ( Input.TargetIndex == 5 )
		color = m_sideMaps[ 5 ].SampleLevel( m_sampler, Input.Texcoord, 0.0f );
	return color;
}