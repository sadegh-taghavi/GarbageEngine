#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"
#include "GE_BufferResource.h"

class GE_Texture;

class GE_SpriteInstance
{
public:
	struct Data
	{
		GE_Vec4		Diffuse;
		GE_Vec3		Emissive;
		float		Layer;
		GE_Vec4		UV[4];
		GE_Mat4x4	World;
	};
private:
	friend class GE_Sprite;
	GE_Vec2		m_position;
	GE_Vec2		m_rotationCenter;
	GE_Vec2		m_scale;
	GE_Vec2		m_scaleCenter;
	GE_Vec4		m_uvRectangle;
	GE_Vec4		m_baseDiffuse;
	GE_Vec3		m_baseEmissive;
	float		m_diffuseIntensity;
	float		m_emissiveIntensity;
	float		m_rotation;
	float		m_scalingRotation;
	bool		m_transformChange;
	Data		m_instanceData;
	uint32_t	m_index;
	GE_Sprite   *m_sprite;

	GE_SpriteInstance(){}
	~GE_SpriteInstance(){}
	void update();
public:
	GE_Mat4x4 &getWorld(){return m_instanceData.World;}
	
	void setUvRectangle(const GE_Vec4 &i_uvRectangle);
	GE_Vec4 *getUvRectangle(){return &m_uvRectangle;}

	void setScalingRotation(float i_scalingRotation){m_scalingRotation=i_scalingRotation; m_transformChange=true;}
	float &getScalingRotation(){return m_scalingRotation;}

	void setRotation(float i_rotation){m_rotation=i_rotation; m_transformChange=true;}
	float &getRotation(){return m_rotation;}

	void setLayer(float i_layer);
	float &getLayer(){return m_instanceData.Layer;}

	void setDiffuse(const GE_Vec4 &i_diffuse);
	GE_Vec4 *getDiffuse(){return &m_instanceData.Diffuse;}

	void setEmissive(const GE_Vec3 &i_emissive);
	GE_Vec3 *getEmissive(){return &m_instanceData.Emissive;}

	float & getEmissiveIntensity()
	{
		return m_emissiveIntensity;
	}

	void setEmissiveIntensity( float i_intensity )
	{
		m_emissiveIntensity = i_intensity;
		m_instanceData.Emissive = m_baseEmissive * m_emissiveIntensity;
	}

	GE_Vec3 & getBaseEmissive()
	{
		return m_baseEmissive;
	}

	void setBaseEmissive(const GE_Vec3 &i_color )
	{
		m_baseEmissive = i_color;
		m_instanceData.Emissive = m_baseEmissive * m_emissiveIntensity;
	}

	float & getDiffuseIntensity()
	{
		return m_diffuseIntensity;
	}

	void setDiffuseIntensity( float i_intensity )
	{
		m_diffuseIntensity = i_intensity;
		m_instanceData.Diffuse = m_baseDiffuse * m_diffuseIntensity;
		m_instanceData.Diffuse.w = m_baseDiffuse.w;
	}

	GE_Vec4 & getBaseDiffuse()
	{
		return m_baseDiffuse;
	}

	void setBaseDiffuse(const GE_Vec4 &i_color )
	{
		m_baseDiffuse = i_color;
		m_instanceData.Diffuse = m_baseDiffuse * m_diffuseIntensity;
		m_instanceData.Diffuse.w = m_baseDiffuse.w;
	}

	void setPosition(const GE_Vec2 &i_position){m_position=i_position; m_transformChange=true;}
	GE_Vec2 *getPosition(){return &m_position;}

	void setRotationCenter(const GE_Vec2 &i_rotationCenter){m_rotationCenter=i_rotationCenter; m_transformChange=true;}
	GE_Vec2 *getRotationCenter(){return &m_rotationCenter;}

	void setScale(const GE_Vec2 &i_scale){m_scale=i_scale; m_transformChange=true;}
	GE_Vec2 *getScale(){return &m_scale;}

	void setScaleCenter(const GE_Vec2 &i_scaleCenter){m_scaleCenter=i_scaleCenter; m_transformChange=true;}
	GE_Vec2 *getScaleCenter(){return &m_scaleCenter;}
	
};

class GE_Sprite
{
	friend class GE_SpriteInstance;
	ID3D11BlendState						*m_blendState;
	GE_Texture								*m_diffuseTexture;
	ID3D11ShaderResourceView				*m_diffuseSRV;
	GE_Vec2									m_wH;
	GE_Vec3									m_emissive;
	GE_Vec2									m_position;
	GE_Vec2									m_rotationCenter;
	GE_Vec2									m_scale;
	GE_Vec2									m_scaleCenter;
	GE_Vec2									m_size;
	GE_Vec4									m_uvRectangle;
	GE_Vec4									m_diffuse;
	GE_Vec4									m_baseDiffuse;
	GE_Vec3									m_baseEmissive;
	float									m_diffuseIntensity;
	float									m_emissiveIntensity;
	float									m_rotation;
	float									m_layer;
	float									m_scalingRotation;
	float									m_shimmerness;
	bool									m_visible;
	bool									m_transformChange;
	uint32_t								m_order;
	GE_Mat4x4								m_world;
	uint32_t								m_numberOfRenderInstances;
	uint32_t								m_numberOfInstances;
	bool									m_instanceNeedFill;
	_GE_BufferResource						m_instanceBuffer;
	std::list<GE_SpriteInstance *>			m_instancesList;
	std::vector<GE_SpriteInstance::Data>	m_instanceData;
public:
	GE_Sprite();
	~GE_Sprite();
	GE_Vec2 *getWidthHeight(){return &m_wH;}

	std::list<GE_SpriteInstance *>::iterator createInstance(uint32_t i_count);
	void removeInstance(GE_SpriteInstance **i_instance);
	void removeAllInstances();
	void fillInstances();
	uint32_t &getNumberOfInstances(){return m_numberOfInstances;}
	uint32_t &getNumberOfRenderInstances(){return m_numberOfRenderInstances;}
	void setNumberOfRenderInstances(uint32_t i_numberOfRenderInstances);
	_GE_BufferResource *getInstanceBuffer(){return &m_instanceBuffer;}
	std::list<GE_SpriteInstance *> *getInstanceList(){return &m_instancesList;}
	void setOrder(uint32_t i_order){ m_order = i_order;}
	uint32_t &getOrder(){return m_order;}
	void setVisible(bool i_visible){m_visible=i_visible;}
	bool &isVisible(){return m_visible;}
	void setDiffuseSRV(ID3D11ShaderResourceView *i_diffuseSRV);
	ID3D11ShaderResourceView *getDiffuseSRV();
	void setDiffuseTexture(const char *i_fileName);
	void deleteDiffuseTexture();
	void setBlendState(ID3D11BlendState *i_blendState){m_blendState=i_blendState;}
	ID3D11BlendState *getBlendState(){return m_blendState;}
	GE_Texture *getDiffuseTexture(){return m_diffuseTexture;}
	void setLayer(float i_layer){m_layer=i_layer;}
	float &getLayer(){return m_layer;}
	void setEmissive(const GE_Vec3 &i_emissive){m_emissive=i_emissive;}
	GE_Vec3 *getEmissive(){return &m_emissive;}
	void setDiffuse(const GE_Vec4 &i_diffuse){m_diffuse=i_diffuse;}
	GE_Vec4 *getDiffuse(){return &m_diffuse;}

	float & getEmissiveIntensity()
	{
		return m_emissiveIntensity;
	}

	void setEmissiveIntensity( float i_intensity )
	{
		m_emissiveIntensity = i_intensity;
		m_emissive = m_baseEmissive * m_emissiveIntensity;
	}

	GE_Vec3 & getBaseEmissive()
	{
		return m_baseEmissive;
	}

	void setBaseEmissive(const GE_Vec3 &i_color )
	{
		m_baseEmissive = i_color;
		m_emissive = m_baseEmissive * m_emissiveIntensity;
	}

	float & getDiffuseIntensity()
	{
		return m_diffuseIntensity;
	}

	void setDiffuseIntensity( float i_intensity )
	{
		m_diffuseIntensity = i_intensity;
		m_diffuse = m_baseDiffuse * m_diffuseIntensity;
		m_diffuse.w = m_baseDiffuse.w;
	}

	GE_Vec4 & getBaseDiffuse()
	{
		return m_baseDiffuse;
	}

	void setBaseDiffuse(const GE_Vec4 &i_color )
	{
		m_baseDiffuse = i_color;
		m_diffuse = m_baseDiffuse * m_diffuseIntensity;
		m_diffuse.w = m_baseDiffuse.w;
	}
	
	void setUvRectangle(const GE_Vec4 &i_uvRectangle){m_uvRectangle=i_uvRectangle;}
	GE_Vec4 *getUvRectangle(){return &m_uvRectangle;}

	void setSize(const GE_Vec2 &i_size){m_size=i_size;}
	GE_Vec2 *getSize(){return &m_size;}

	void setShimmerness(float i_shimmerness){m_shimmerness=i_shimmerness;}
	float &getShimmerness(){return m_shimmerness;}

	void setScalingRotation(float i_scalingRotation){m_scalingRotation=i_scalingRotation; m_transformChange=true;}
	float &getScalingRotation(){return m_scalingRotation;}

	void setRotation(float i_rotation){m_rotation=i_rotation; m_transformChange=true;}
	float &getRotation(){return m_rotation;}

	void setPosition(const GE_Vec2 &i_position){m_position=i_position; m_transformChange=true;}
	GE_Vec2 *getPosition(){return &m_position;}

	void setRotationCenter(const GE_Vec2 &i_rotationCenter){m_rotationCenter=i_rotationCenter; m_transformChange=true;}
	GE_Vec2 *getRotationCenter(){return &m_rotationCenter;}

	void setScale(const GE_Vec2 &i_scale){m_scale=i_scale; m_transformChange=true;}
	GE_Vec2 *getScale(){return &m_scale;}

	void setScaleCenter(const GE_Vec2 &i_scaleCenter){m_scaleCenter=i_scaleCenter; m_transformChange=true;}
	GE_Vec2 *getScaleCenter(){return &m_scaleCenter;}

	void update();
	friend class GE_SpriteRenderer;

};

//----------------------------------------------------------------------------------------------------------------------------------
class GE_SpriteRenderer
{
		ID3D11VertexShader	*m_vS,*m_vS_Instance;
		ID3D11PixelShader	*m_pS,*m_pS_Shimmer;
		ID3D11Buffer		*m_perObject;
		GE_Mat4x4			m_projection;
		ID3D11Buffer		*m_vB;

		struct PerObject
		{
			GE_Vec4 Diffuse;
			GE_Vec4	Emissive_Layer;
			GE_Vec4 Size_Shimmerness;
			GE_Vec4 UVRectangle[4];
			GE_Mat4x4	World;
			GE_Mat4x4	Projection;
		}m_cPerObject;

		GE_SpriteRenderer();
		~GE_SpriteRenderer();
		static GE_SpriteRenderer m_singleton;
public:
	GE_Mat4x4 *getProjection(){return &m_projection;}
	static GE_SpriteRenderer &getSingleton(){return m_singleton;}
	void init();
	void begin();
	void render(std::list<GE_Sprite *> *i_spriteList,ID3D11ShaderResourceView *i_backBufferMAP=NULL);
	void end();
	void destroy();
};
