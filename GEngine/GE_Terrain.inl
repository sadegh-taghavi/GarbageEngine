#include "GE_Terrain.h"

inline float & GE_Terrain::MaterialParameters::getEmissiveIntensity()
{
	return EmissiveIntensity;
}

inline void GE_Terrain::MaterialParameters::setEmissiveIntensity(float i_intensity)
{
	EmissiveIntensity = i_intensity;
	Emissive = BaseEmissive * EmissiveIntensity;
}

inline GE_Vec3 & GE_Terrain::MaterialParameters::getBaseEmissive()
{
	return BaseEmissive;
}

inline void GE_Terrain::MaterialParameters::setBaseEmissive(const GE_Vec3 &i_color)
{
	BaseEmissive = i_color;
	Emissive = BaseEmissive * EmissiveIntensity;
}

inline float & GE_Terrain::MaterialParameters::getDiffuseIntensity()
{
	return DiffuseIntensity;
}

inline void GE_Terrain::MaterialParameters::setDiffuseIntensity(float i_intensity)
{
	DiffuseIntensity = i_intensity;
	Diffuse = BaseDiffuse * DiffuseIntensity;
}

inline GE_Vec3 & GE_Terrain::MaterialParameters::getBaseDiffuse()
{
	return BaseDiffuse;
}

inline void GE_Terrain::MaterialParameters::setBaseDiffuse(const GE_Vec3 &i_color)
{
	BaseDiffuse = i_color;
	Diffuse = BaseDiffuse * DiffuseIntensity;
}

inline float & GE_Terrain::MaterialParameters::getGlowIntensity()
{
	return GlowIntensity;
}

inline void GE_Terrain::MaterialParameters::setGlowIntensity(float i_intensity)
{
	GlowIntensity = i_intensity;
	Glow = BaseGlow * GlowIntensity;
}

inline GE_Vec3	& GE_Terrain::MaterialParameters::getBaseGlow()
{
	return BaseGlow;
}

inline void GE_Terrain::MaterialParameters::setBaseGlow(const GE_Vec3 &i_color)
{
	BaseGlow = i_color;
	Glow = BaseGlow * GlowIntensity;
}

inline float & GE_Terrain::MaterialParameters::getAmbientIntensity()
{
	return AmbientIntensity;
}

inline void GE_Terrain::MaterialParameters::setAmbientIntensity(float i_intensity)
{
	AmbientIntensity = i_intensity;
	Ambient = BaseAmbient * AmbientIntensity;
}

inline GE_Vec3 & GE_Terrain::MaterialParameters::getBaseAmbient()
{
	return BaseAmbient;
}

inline void GE_Terrain::MaterialParameters::setBaseAmbient(const GE_Vec3 &i_color)
{
	BaseAmbient = i_color;
	Ambient = BaseAmbient * AmbientIntensity;
}

inline float & GE_Terrain::MaterialParameters::getSpecularIntensity()
{
	return SpecularIntensity;
}

inline void GE_Terrain::MaterialParameters::setSpecularIntensity(float i_intensity)
{
	SpecularIntensity = i_intensity;
	Specular = BaseSpecular * SpecularIntensity;
}

inline GE_Vec3 & GE_Terrain::MaterialParameters::getBaseSpecular()
{
	return BaseSpecular;
}

inline void GE_Terrain::MaterialParameters::setBaseSpecular(const GE_Vec3 &i_color)
{
	BaseSpecular = i_color;
	Specular = BaseSpecular * SpecularIntensity;
}
inline float GE_Terrain::getWidth( )
{
	return m_width;
}
inline uint32_t GE_Terrain::getQuadDensity()
{
	return m_quadDensity;
}