#include "GE_PostProcess.hlsli"

float4 main( VS_Output Input ) : SV_Target0
{
	float l = m_texture.Load( int3( 0, 0, 0 ) ).x;
	float la = m_texture1.Load( int3( 0, 0, 0 ) ).x;
	//return l.xxxx;
	//float a = la + ( l - la ) * ( 1.0f - exp( -m_float4Val0.x * 0.5f ) );
	float a = la + ( l - la ) * ( 1.0f - pow( 0.98f, m_float4Val0.x ) ); // adaptation rate
	a = max( a, m_float4Val0.y );
	a = min( a, m_float4Val0.z );
	return float4( a.xxx, 1.0f );
}