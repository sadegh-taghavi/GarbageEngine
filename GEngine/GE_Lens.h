#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"

class GE_Sprite;
class GE_SpriteInstance;
struct GE_Occluder;
class GE_Texture;

class GE_LensFlare
{
public:
	struct Parameter
	{
		GE_Vec4		Diffuse;
		GE_Vec3		Emissive;
		float		EmissiveIntensity;
		float		DiffuseIntensity;
		float		Layer;
		float		Rotation;
		float		PositionMultiplier;
		bool		AutoRotation;
		bool		AutoScale;
		GE_Vec2		ScaleMultiplier;
		Parameter();
	};
private:
	GE_Sprite			*m_sprite;
	vector<Parameter>	m_parameters;
	GE_Vec3				m_lightPosition;
	GE_Vec3				m_lightPositionOffset;
	uint32_t			m_visiblePixelOffset;
	float				m_visibleAngleOffset;
	bool				m_visible;
	list<GE_SpriteInstance *>::iterator m_spriteInstances;
	list<GE_SpriteInstance *>::iterator m_tempSpriteInstances;
	GE_Occluder *m_occluder;
public:
	GE_LensFlare();
	void setBlendState( ID3D11BlendState *i_blendState );
	ID3D11BlendState *getBlendState();
	GE_Texture *getTexture();
	void addFlare();
	void removeFlare( uint32_t i_index );
	void setNumberOfFlare( uint32_t i_numberOfFlare );
	void setTexture( const char *i_filename );
	void deleteTexture();
	void setSRV( ID3D11ShaderResourceView *i_srv );
	void setLightPosition( const GE_Vec3 &i_lightPosition ){ m_lightPosition = i_lightPosition; }
	GE_Vec3* getLightPosition(){ return &m_lightPosition; }
	void update( const class GE_View *i_view );
	GE_Occluder *getOccluder();
	~GE_LensFlare( void );
	void setOccluderPositionOffset( const GE_Vec3 &i_offset );
	GE_Vec3 *getOccluderPositionOffset();
	void setOccluderSize( const GE_Vec3 &i_size );
	GE_Vec3 *getOccluderSize();
	void setOccluderRotation( const GE_Quat &i_rotation );
	GE_Quat *getOccluderRotation();
	void setVisible( bool i_visible );
	bool &isVisible(){ return m_visible; }
	void setVisiblePixelOffset( uint32_t i_visiblePixelOffset ){ m_visiblePixelOffset = i_visiblePixelOffset; }
	uint32_t &getVisiblePixelOffset( ){ return m_visiblePixelOffset; }
	void setVisibleAngleOffset( float i_visibleAngleOffset ){ m_visibleAngleOffset = i_visibleAngleOffset; }
	float &getVisibleAngleOffset(){ return m_visibleAngleOffset; }
	GE_Sprite *getSprite(){ return m_sprite; }
	std::vector<Parameter> *getParameter(){ return &m_parameters; }
};

