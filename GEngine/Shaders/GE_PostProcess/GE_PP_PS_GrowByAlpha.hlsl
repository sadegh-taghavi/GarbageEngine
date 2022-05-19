#include "GE_PostProcess.hlsli"

float4 main(VS_Output Input) : SV_Target0
{	
	float4 c = m_texture.Sample( m_samPointClamp, Input.Texcoord );
	if(c.w == 1.0f)
		return c;

	float4 cE[8];
	int div = 8;
	cE[0]=m_texture.Sample( m_samPointClamp, Input.Texcoord, int2( 1, 0 ) );
	if( cE[0].w == 0.0f )
		div--;
	cE[1]=m_texture.Sample( m_samPointClamp, Input.Texcoord, int2(-1, 0 ) );
	if( cE[1].w == 0.0f )
		div--;
	cE[2]=m_texture.Sample( m_samPointClamp, Input.Texcoord, int2( 0, 1 ) );
	if( cE[2].w == 0.0f )
		div--;
	cE[3]=m_texture.Sample( m_samPointClamp, Input.Texcoord, int2( 0,-1 ) );
	if( cE[3].w == 0.0f )
		div--;
	cE[4]=m_texture.Sample( m_samPointClamp, Input.Texcoord, int2( 1, 1 ) );
	if( cE[4].w == 0.0f )
		div--;
	cE[5]=m_texture.Sample( m_samPointClamp, Input.Texcoord, int2(-1, 1 ) );
	if( cE[5].w == 0.0f )
		div--;
	cE[6]=m_texture.Sample( m_samPointClamp, Input.Texcoord, int2( 1,-1 ) );
	if( cE[6].w == 0.0f )
		div--;
	cE[7]=m_texture.Sample( m_samPointClamp, Input.Texcoord, int2(-1,-1 ) );
	if( cE[7].w == 0.0f )
		div--;

	float4 o=float4(0.0f, 0.0f, 0.0f, 0.0f);
	[unroll]
	for(uint i=0; i < 8; i++)
		o += cE[i];
	o /= div;
	return o;
}