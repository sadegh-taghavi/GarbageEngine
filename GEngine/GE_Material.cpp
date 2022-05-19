#include "GE_Material.h"
#include "GE_Texture.h"

GE_Material::~GE_Material()
{
	GE_TextureManager::getSingleton().remove( &DiffuseAlphaMap[ 0 ] );
	GE_TextureManager::getSingleton().remove( &DiffuseAlphaMap[ 1 ] );
	GE_TextureManager::getSingleton().remove( &GlowSpecularReflectMaskMap[ 0 ] );
	GE_TextureManager::getSingleton().remove( &GlowSpecularReflectMaskMap[ 1 ] );
	GE_TextureManager::getSingleton().remove( &NormalHeightMap[ 0 ] );
	GE_TextureManager::getSingleton().remove( &NormalHeightMap[ 1 ] );
	GE_TextureManager::getSingleton().remove( &ReflectMap );
	GE_TextureManager::getSingleton().remove(&TintMap);
}

GE_Material::GE_Material()
{
	BaseEmissive = Emissive = GE_Vec3( 0.0f, 0.0f, 0.0f );
	BaseAmbient = Ambient = GE_Vec3( 0.3f, 0.3f, 0.3f );
	BaseDiffuse = Diffuse = GE_Vec3( 0.8f, 0.8f, 0.8f );
	BaseGlow = Glow = GE_Vec3( 1.0f, 1.0f, 1.0f );
	BaseSpecular = Specular = GE_Vec3( 0.8f, 0.8f, 0.8f );
	EmissiveIntensity = 1.0f;
	AmbientIntensity = 1.0f;
	DiffuseIntensity = 1.0f;
	setGlowIntensity(0.0f);
	SpecularIntensity = 1.0f;
	
	SpecularPower = 10.0f;
	Roughness = 0.0f;
	Reflectmask = 1.0f;
	Bumpiness = 1.0f;
	AlphaTest = 0.0f;
	Alpha = 1.0f;
	Refractness = 1.0f;
	Refract = 0.0f;

	Tint = 0.0f;
	TintSpread = 0.5f;

	DiffuseAlphaMap[ 0 ] = DiffuseAlphaMap[ 1 ] = NULL;
	GlowSpecularReflectMaskMap[ 0 ] = GlowSpecularReflectMaskMap[ 1 ] = NULL;
	NormalHeightMap[ 0 ] = NormalHeightMap[ 1 ] = NULL;
	ReflectMap = NULL;
	TintMap = NULL;
}

void GE_Material::copyFrom( GE_Material &i_other )
{
	SpecularPower = i_other.SpecularPower;
	Roughness = i_other.Roughness;
	Reflectmask = i_other.Reflectmask;
	Bumpiness = i_other.Bumpiness;
	AlphaTest = i_other.AlphaTest;
	Alpha = i_other.Alpha;
	Refract = i_other.Refract;
	Refractness = i_other.Refractness;
	Tint = i_other.Tint;
	TintSpread = i_other.TintSpread;

	setBaseEmissive( i_other.getBaseEmissive() );
	setEmissiveIntensity( i_other.getEmissiveIntensity() );
	setBaseAmbient( i_other.getBaseAmbient() );
	setAmbientIntensity( i_other.getAmbientIntensity() );
	setBaseSpecular( i_other.getBaseSpecular() );
	setSpecularIntensity( i_other.getSpecularIntensity() );
	setBaseDiffuse( i_other.getBaseDiffuse() );
	setDiffuseIntensity( i_other.getDiffuseIntensity() );
	setBaseGlow( i_other.getBaseGlow() );
	setGlowIntensity( i_other.getGlowIntensity() );

	if( DiffuseAlphaMap[ 0 ] )
		GE_TextureManager::getSingleton().remove( &DiffuseAlphaMap[ 0 ] );
	if( DiffuseAlphaMap[ 1 ] )
		GE_TextureManager::getSingleton().remove( &DiffuseAlphaMap[ 1 ] );
	if( GlowSpecularReflectMaskMap[ 0 ] )
		GE_TextureManager::getSingleton().remove( &GlowSpecularReflectMaskMap[ 0 ] );
	if( GlowSpecularReflectMaskMap[ 1 ] )
		GE_TextureManager::getSingleton().remove( &GlowSpecularReflectMaskMap[ 1 ] );
	if( NormalHeightMap[ 0 ] )
		GE_TextureManager::getSingleton().remove( &NormalHeightMap[ 0 ] );
	if( NormalHeightMap[ 1 ] )
		GE_TextureManager::getSingleton().remove( &NormalHeightMap[ 1 ] );
	if( ReflectMap )
		GE_TextureManager::getSingleton().remove( &ReflectMap );
	if (TintMap)
		GE_TextureManager::getSingleton().remove(&TintMap);

	if( i_other.DiffuseAlphaMap[ 0 ] )
		DiffuseAlphaMap[ 0 ] = GE_TextureManager::getSingleton().createTexture( 
		i_other.DiffuseAlphaMap[ 0 ]->getTextureName() );
	if( i_other.DiffuseAlphaMap[ 1 ] )
		DiffuseAlphaMap[ 1 ] = GE_TextureManager::getSingleton().createTexture(
		i_other.DiffuseAlphaMap[ 1 ]->getTextureName() );
	if( i_other.GlowSpecularReflectMaskMap[ 0 ] )
		GlowSpecularReflectMaskMap[ 0 ] = GE_TextureManager::getSingleton().createTexture(
		i_other.GlowSpecularReflectMaskMap[ 0 ]->getTextureName() );
	if( i_other.GlowSpecularReflectMaskMap[ 1 ] )
		GlowSpecularReflectMaskMap[ 1 ] = GE_TextureManager::getSingleton().createTexture(
		i_other.GlowSpecularReflectMaskMap[ 1 ]->getTextureName() );
	if( i_other.NormalHeightMap[ 0 ] )
		NormalHeightMap[ 0 ] = GE_TextureManager::getSingleton().createTexture(
		i_other.NormalHeightMap[ 0 ]->getTextureName() );
	if( i_other.NormalHeightMap[ 1 ] )
		NormalHeightMap[ 1 ] = GE_TextureManager::getSingleton().createTexture(
		i_other.NormalHeightMap[ 1 ]->getTextureName() );
	if( i_other.ReflectMap )
		ReflectMap = GE_TextureManager::getSingleton().createTexture(
		i_other.ReflectMap->getTextureName() );
	if (i_other.TintMap)
		TintMap = GE_TextureManager::getSingleton().createTexture(
		i_other.TintMap->getTextureName());
}

void GE_Material::reloadMaps()
{

	if ( DiffuseAlphaMap[ 0 ] )
		DiffuseAlphaMap[ 0 ]->reload();
	if ( DiffuseAlphaMap[ 1 ] )
		DiffuseAlphaMap[ 1 ]->reload();

	if ( GlowSpecularReflectMaskMap[ 0 ] )
		GlowSpecularReflectMaskMap[ 0 ]->reload();
	if ( GlowSpecularReflectMaskMap[ 1 ] )
		GlowSpecularReflectMaskMap[ 1 ]->reload();

	if ( NormalHeightMap[ 0 ] )
		NormalHeightMap[ 0 ]->reload();
	if ( NormalHeightMap[ 1 ] )
		NormalHeightMap[ 1 ]->reload();

	if ( ReflectMap )
		ReflectMap->reload();

	if (TintMap)
		TintMap->reload();
}
