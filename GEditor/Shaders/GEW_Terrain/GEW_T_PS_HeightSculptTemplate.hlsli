#include "GEW_Terrain.hlsli"

float main(VS_Output Input) : SV_Target0
{	
	float2 xy = Input.Texcoord.xy * m_brushFieldSize;
	float len = distance(xy, m_brushHitXY);
	float sampleValue = m_texture.Sample(m_samPointClamp, Input.Texcoord).x;
	float volume = m_brushVolume;
	[branch]
	if (len > m_brushSize)
		return sampleValue;
#ifdef MASK
	float2 Tcoord = Input.Texcoord;
	Tcoord.xy *= m_brushMaskTile;
	Tcoord.xy -= m_brushMaskPos;
	float2 ltc = Tcoord.xy;
	ltc.x -= 0.5f;
	ltc.y -= 0.5f;
	float2 tc = Tcoord.xy;
	float ca;
	float sa;
	sincos(-m_brushMaskRotation, sa, ca);
	tc.x = ca * ltc.x - sa * ltc.y;
	tc.y = sa * ltc.x + ca * ltc.y;
	tc.x += 0.5f;
	tc.y += 0.5f;
	volume *= m_textureMask.Sample(m_samLinearWrap, tc);
#endif
	float fill = saturate(1.0f - smoothstep(m_brushFalloff, lerp(m_brushSize, m_brushFalloff, m_brushHardness), len)) * volume;
#ifdef POSITIVE
	sampleValue += fill;
#endif
#ifdef NEGATIVE
	sampleValue -= fill;
#endif
#ifdef FLAT
	sampleValue = 0.0f;
#endif
#ifdef SMOOTH
	float c = 0;
	[unroll]
	for (int y = -1; y <= 1; y++)
	{
		[unroll]
		for (int x = -1; x <= 1; x++)
		{
			c += m_texture.Sample(m_samPointClamp, Input.Texcoord, int2(x, y)).x;
		}
	}
	sampleValue = lerp(sampleValue, c * 0.11111111111111111111111111111111f, fill);
#endif
	return saturate(sampleValue);
}