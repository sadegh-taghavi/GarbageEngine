#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"

class GE_Texture;

struct GE_Material
{
	float		SpecularPower;
	GE_Vec3		Emissive;
	GE_Vec3		Ambient;
	GE_Vec3		Specular;
	GE_Vec3		Diffuse;
	GE_Vec3		Glow;
	float		Roughness;
	float		Reflectmask;
	float		Bumpiness;
	float		Alpha;
	float		AlphaTest;
	float		Refract;
	float		Refractness;
	float		Tint;
	float		TintSpread;
	GE_Texture*	DiffuseAlphaMap[2];
	GE_Texture*	GlowSpecularReflectMaskMap[2];
	GE_Texture*	NormalHeightMap[2];
	GE_Texture*	ReflectMap;
	GE_Texture*	TintMap;
	std::string Name;
	GE_Material();
	~GE_Material();

	void		copyFrom( GE_Material &i_other );

	void		reloadMaps();

	float		&getDiffuseIntensity();
	void		setDiffuseIntensity(float i_intensity);
	GE_Vec3		&getBaseDiffuse();
	void		setBaseDiffuse(const GE_Vec3 &i_color);

	float		&getGlowIntensity();
	void		setGlowIntensity(float i_intensity);
	GE_Vec3		&getBaseGlow();
	void		setBaseGlow(const GE_Vec3 &i_color);

	float		&getEmissiveIntensity();
	void		setEmissiveIntensity(float i_intensity);
	GE_Vec3		&getBaseEmissive();
	void		setBaseEmissive(const GE_Vec3 &i_color);
	
	float		&getAmbientIntensity();
	void		setAmbientIntensity(float i_intensity);
	GE_Vec3		&getBaseAmbient();
	void		setBaseAmbient(const GE_Vec3 &i_color);

	float		&getSpecularIntensity();
	void		setSpecularIntensity(float i_intensity);
	GE_Vec3		&getBaseSpecular();
	void		setBaseSpecular(const GE_Vec3 &i_color);

private:
	float		EmissiveIntensity;
	float		AmbientIntensity;
	float		SpecularIntensity;
	float		DiffuseIntensity;
	float		GlowIntensity;
	GE_Vec3		BaseEmissive;
	GE_Vec3		BaseAmbient;
	GE_Vec3		BaseSpecular;
	GE_Vec3		BaseDiffuse;
	GE_Vec3		BaseGlow;
};

#include "GE_Material.inl"