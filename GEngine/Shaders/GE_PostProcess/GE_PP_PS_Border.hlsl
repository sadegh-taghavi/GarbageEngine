#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{	
   float4 c = m_texture.Sample( m_samLinearClamp, Input.Texcoord );
   float f = ( ( 1.0f - Input.Texcoord.x ) * Input.Texcoord.y ) * ( ( 1.0f - Input.Texcoord.y ) * Input.Texcoord.x );
   return float4( c.xyz * saturate( m_float4Val0.x * pow( abs( f ), m_float4Val0.y ) - m_float4Val0.z ), c.w ); 
   //use as frame sharpness
   //use as frame shape
   //use as frame limit
}