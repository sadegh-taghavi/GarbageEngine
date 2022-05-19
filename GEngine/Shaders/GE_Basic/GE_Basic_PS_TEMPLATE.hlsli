#include "GE_Basic.hlsli"

float4 main( PS_Input Input ) : SV_Target
{
#ifdef TEXTURE
	float4 color = m_texture.Sample( m_samLinear, Input.Texcoord );
	color.xyz *= m_color;
	clip( color.w - m_alphaTest );
	return color;
#else
	return float4( 8.0f, 0.3f, 0.3f, 1.0f );
#endif

}