#include "GE_Material.h"

inline float & GE_Material::getEmissiveIntensity()
{
	return EmissiveIntensity;
}

inline void GE_Material::setEmissiveIntensity( float i_intensity )
{
	EmissiveIntensity = i_intensity;
	Emissive = BaseEmissive * EmissiveIntensity;
}

inline GE_Vec3 & GE_Material::getBaseEmissive()
{
	return BaseEmissive;
}

inline void GE_Material::setBaseEmissive(const GE_Vec3 &i_color )
{
	BaseEmissive = i_color;
	Emissive = BaseEmissive * EmissiveIntensity;
}

inline float & GE_Material::getDiffuseIntensity()
{
	return DiffuseIntensity;
}

inline void GE_Material::setDiffuseIntensity( float i_intensity )
{
	DiffuseIntensity = i_intensity;
	Diffuse = BaseDiffuse * DiffuseIntensity;
}

inline GE_Vec3 & GE_Material::getBaseDiffuse()
{
	return BaseDiffuse;
}

inline void GE_Material::setBaseDiffuse(const GE_Vec3 &i_color )
{
	BaseDiffuse = i_color;
	Diffuse = BaseDiffuse * DiffuseIntensity;
}

inline float & GE_Material::getGlowIntensity()
{
	return GlowIntensity;
}

inline void GE_Material::setGlowIntensity( float i_intensity )
{
	GlowIntensity = i_intensity;
	Glow = BaseGlow * GlowIntensity;
}

inline GE_Vec3	& GE_Material::getBaseGlow()
{
	return BaseGlow;
}

inline void GE_Material::setBaseGlow( const GE_Vec3 &i_color )
{
	BaseGlow = i_color;
	Glow = BaseGlow * GlowIntensity;
}

inline float & GE_Material::getAmbientIntensity()
{
	return AmbientIntensity;
}

inline void GE_Material::setAmbientIntensity( float i_intensity )
{
	AmbientIntensity = i_intensity;
	Ambient = BaseAmbient * AmbientIntensity;
}

inline GE_Vec3 & GE_Material::getBaseAmbient()
{
	return BaseAmbient;
}

inline void GE_Material::setBaseAmbient(const GE_Vec3 &i_color )
{
	BaseAmbient = i_color;
	Ambient = BaseAmbient * AmbientIntensity;
}

inline float & GE_Material::getSpecularIntensity()
{
	return SpecularIntensity;
}

inline void GE_Material::setSpecularIntensity( float i_intensity )
{
	SpecularIntensity = i_intensity;
	Specular = BaseSpecular * SpecularIntensity;
}

inline GE_Vec3 & GE_Material::getBaseSpecular()
{
	return BaseSpecular;
}

inline void GE_Material::setBaseSpecular(const GE_Vec3 &i_color )
{
	BaseSpecular = i_color;
	Specular = BaseSpecular * SpecularIntensity;
}