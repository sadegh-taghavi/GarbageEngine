#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"

#define LIGHT_PARAM_SIZE 6

class GE_Light
{
public:
	enum  Type
	{
		Directional,
		Spot,
		Point,
	};

	struct Parameters
	{
	public:
		Type			LightType;
		bool			Enable;
		bool			TargetMode;
		bool			VisualizeCascades;
		bool			HasShadow;
		bool			DoShadow;
		bool			Dynamic;
		GE_Vec3			Position;
		GE_Vec3			DirectionalShadowPosition;
		GE_Vec3			Target;
		GE_Vec3			Ambient;
		GE_Vec3			Direction;
		GE_Quat			RotateCubeFilter;
		GE_Vec3			Diffuse;
		GE_Vec3			Specular;
		uint32_t		NumberOfCascade;
		GE_Vec2			ShadowOffsetBias[ LIGHT_PARAM_SIZE ];
		float			Intensity;
		float			Attenuation;
		float			Range;
		float			SpotSpread;
		float			SpotSharpness;
		float			Yaw;
		float			Pitch;
		float			DirectionalTop;
		GE_Vec4			ShadowFrustum[ LIGHT_PARAM_SIZE ];
		GE_Vec2			ShadowMapSize[ LIGHT_PARAM_SIZE ];
		Parameters();
	};
private:
	Parameters					m_parameters;
	GE_Mat4x4					m_world;
	GE_Mat4x4					m_cubeFilterRotate;
	GE_Mat4x4					m_view[ LIGHT_PARAM_SIZE ];
	GE_Mat4x4					m_projection[ LIGHT_PARAM_SIZE ];
	GE_Mat4x4					m_viewProjection[ LIGHT_PARAM_SIZE ];
	ID3D11ShaderResourceView	*m_filterSRV;
	class GE_Texture			*m_filterTexture;
	GE_Vec3						m_pointTargets[ LIGHT_PARAM_SIZE ];
	class GE_RenderTarget		*m_shadowMap[ LIGHT_PARAM_SIZE ];
	class GE_View				*m_linkToView;
public:
	GE_Light( const Parameters &i_parameters );
	~GE_Light();
	void			update();
	void			setLinkToView( GE_View * i_link ){ m_linkToView = i_link; }
	GE_View			*getLinkToView(){ return m_linkToView; }
	GE_Texture		*getFilterTexture(){ return m_filterTexture; }
	void			setEnable( bool i_enable ){ m_parameters.Enable = i_enable; }
	bool&			iSEnable(){ return m_parameters.Enable; }
	void			setIntensity( float i_intensity ){ m_parameters.Intensity = i_intensity; }
	float			&getIntensity(){ return m_parameters.Intensity; }
	void			setTargetMode( bool i_targetMode ){ m_parameters.TargetMode = i_targetMode; }
	bool			&iSTargetMode(){ return m_parameters.TargetMode; }
	void			setYaw( float i_yaw ){ m_parameters.Yaw = i_yaw; }
	float			&getYaw(){ return m_parameters.Yaw; }
	void			setPitch( float i_pitch ){ m_parameters.Pitch = i_pitch; }
	float			&getPitch(){ return m_parameters.Pitch; }
	void			setFilterSRV( ID3D11ShaderResourceView *i_filter = NULL ){ m_filterSRV = i_filter; }
	void			setFilterMap( const char *i_fileName );
	void			deleteFilterMap();
	ID3D11ShaderResourceView *getFilterSRV();
	void			setType( Type i_type ){ m_parameters.LightType = i_type; }
	Type&			getType(){ return m_parameters.LightType; }
	void			setShadowOffsetBias( GE_Vec2 &i_shadowOffsetBias, uint32_t i_index = 0 ){ m_parameters.ShadowOffsetBias[ i_index ] = i_shadowOffsetBias; }
	GE_Vec2*		getShadowOffsetBias( uint32_t i_index = 0 ){ return &m_parameters.ShadowOffsetBias[ i_index ]; }
	void			setSpotSpread( float i_spotSpread ){ m_parameters.SpotSpread = i_spotSpread; }
	float&			getSpotSpread(){ return m_parameters.SpotSpread; }
	void			setSpotSharpness( float i_spotSharpness ){ m_parameters.SpotSharpness = i_spotSharpness; }
	float&			getSpotSharpness(){ return m_parameters.SpotSharpness; }
	void			setRange( float i_range ){ m_parameters.Range = i_range; }
	float&			getRange(){ return m_parameters.Range; }
	void			setNumberOfCascade( uint32_t i_numberOfCascade ){ m_parameters.NumberOfCascade = i_numberOfCascade; }
	uint32_t&		getNumberOfCascade(){ return m_parameters.NumberOfCascade; }
	void			setAttenuation( float i_attenuation ){ m_parameters.Attenuation = i_attenuation; }
	float&			getAttenuation(){ return m_parameters.Attenuation; }
	void			setShadowCaster( bool i_hasShadow ){ m_parameters.HasShadow = i_hasShadow; }
	bool&			hasShadow(){ return m_parameters.HasShadow; }
	void			setDiffuse( const GE_Vec3 &i_diffuse ){ m_parameters.Diffuse = i_diffuse; }
	GE_Vec3*		getDiffuse(){ return &m_parameters.Diffuse; }
	void			setSpecular( const GE_Vec3 &i_specular ){ m_parameters.Specular = i_specular; }
	GE_Vec3*		getSpecular(){ return &m_parameters.Specular; }
	void			setAmbient( const GE_Vec3 &i_ambient ){ m_parameters.Ambient = i_ambient; }
	GE_Vec3*		getAmbient(){ return &m_parameters.Ambient; }
	void			setPosition( const GE_Vec3 &i_position ){ m_parameters.Position = i_position; }
	GE_Vec3*		getPosition(){ return &m_parameters.Position; }
	void			setDirectionalShadowPosition( const GE_Vec3 &i_position ){ m_parameters.DirectionalShadowPosition = i_position; }
	GE_Vec3*		getDirectionalShadowPosition(){ return &m_parameters.DirectionalShadowPosition; }
	GE_Vec3*		getDirection(){ return &m_parameters.Direction; }
	GE_Quat			*getRotateCubeFilter(){ return &m_parameters.RotateCubeFilter; }
	void			setRotateCubeFilter( const GE_Quat &i_rotate );
	void			setTarget( const GE_Vec3 &i_target );
	GE_Vec3*		getTarget(){ return &m_parameters.Target; }
	void			setShadowFrustum( const GE_Vec4 &i_shadowFrustum, uint32_t i_index = 0 ){ m_parameters.ShadowFrustum[ i_index ] = i_shadowFrustum; }
	GE_Vec4*		getShadowFrustum( uint32_t i_index = 0 ){ return &m_parameters.ShadowFrustum[ i_index ]; }
	void			setParameters( const Parameters &i_parameters ){ m_parameters = i_parameters; }
	void			setDirectionalTop( float i_top ){ m_parameters.DirectionalTop = i_top; }
	float			getDirectionalTop(){ return m_parameters.DirectionalTop; }
	Parameters*		getParameters(){ return &m_parameters; }
	GE_Mat4x4*		getCubeFilterRotate() { return &m_cubeFilterRotate; }
	GE_Mat4x4*		getWorld() { return &m_world; }
	GE_Mat4x4*		getView( uint32_t i_index = 0 ) { return &m_view[ i_index ]; }
	GE_Mat4x4*		getProjection( uint32_t i_index = 0 ) { return &m_projection[ i_index ]; }
	GE_Mat4x4*		getViewProjection( uint32_t i_index = 0 ) { return &m_viewProjection[ i_index ]; }

	void			createShadowMap();
	GE_RenderTarget* getShadowMap( uint32_t i_index = 0 ) { return m_shadowMap[ i_index ]; }
};
