#pragma once
#include "GE_Prerequisites.h"
#include "GE_BufferResource.h"
#include "GE_Math.h"

class GE_Texture;
class GE_View;

class GE_Particle
{
public:
	struct Data
	{
		GE_Vec4 Position_Layer;
		GE_Vec4 Emissive_Rotation;
		GE_Vec4 Diffuse;
		GE_Vec2 Size;
		Data();
	};

private:
	float						m_softness,m_shimmerness;
	bool						m_instanceChange;
	bool						m_particleChange;
	bool						m_translateChange;
	bool						m_visible;
	_GE_BufferResource			m_instanceBuffer;
	ID3D11ShaderResourceView	*m_diffuseSRV;
	GE_Texture					*m_diffuseTexture;
	Data						*m_particleData;
	float						m_boundingRadius;
	GE_Quat						m_rotation;
	GE_Vec3						m_position;
	GE_Vec3						m_scale;
	uint32_t					m_numberOfParticle;
	uint32_t					m_userData;
	GE_Mat4x4					m_world;
	GE_Vec3						m_axisMask;
	ID3D11Buffer				*m_particleBuffer;
	ID3D11BlendState			*m_blendState;

	void render();
	void renderInstance();
public:
	friend class GE_ParticelRenderer;

	GE_Particle(uint32_t i_numberOfParticle);
	~GE_Particle();
	void setBlendState(ID3D11BlendState *i_blendState){m_blendState=i_blendState;}
	ID3D11BlendState *getBlendState(){return m_blendState;}
	float &getShimmerness(){return m_shimmerness;}
	void setShimmerness(float i_shimmerness=0.05f){m_shimmerness = i_shimmerness;}
	float &getSoftness(){return m_softness;}
	float &getBoundingRadius(){return m_boundingRadius;}
	GE_Vec3 *getBoundingCenter(){return &m_position;}
	void setSoftness(float i_softness=0.08f){m_softness = i_softness;}
	void setUserData(uint32_t i_userData){m_userData=i_userData;}
	uint32_t &getUserData(){return m_userData;}
	void setAxisMask(const GE_Vec3 &i_axisMask){m_axisMask=i_axisMask;}
	void setXBaseMask(){m_axisMask.x=0;m_axisMask.y=1;m_axisMask.z=1;}
	void setYBaseMask(){m_axisMask.x=1;m_axisMask.y=0;m_axisMask.z=1;}
	void setZBaseMask(){m_axisMask.x=1;m_axisMask.y=1;m_axisMask.z=0;}
	void resetBaseMask(){m_axisMask.x=1;m_axisMask.y=1;m_axisMask.z=1;}
	GE_Vec3 &getAxisMask(){return m_axisMask;}
	bool &isVisible(){return m_visible;}
	void setVisible(bool i_visible){m_visible=i_visible;}
	const GE_Mat4x4 *getWorld(){return &m_world;}
	void setPosition(const GE_Vec3 &i_position){m_position=i_position;m_translateChange=true;}
	GE_Vec3 *getPosition(){return &m_position;}
	void setRotation(const GE_Quat &i_rotation){m_rotation=i_rotation;m_translateChange=true;}
	GE_Quat &getRotation(){return m_rotation;}
	void setScale(const GE_Vec3 &i_scale){m_scale=i_scale;m_translateChange=true;}
	GE_Vec3 &getScale(){return m_scale;}
	uint32_t &getNumberOfParticle(){return m_numberOfParticle;}
	uint32_t &getInstanceCount();
	void setDiffuseTexture(const char *i_fileName);
	void setDiffuseSRV(ID3D11ShaderResourceView *i_diffuse=NULL){m_diffuseSRV=i_diffuse;}
	void createInstance(uint32_t i_numberOfInstance);
	void setParticleData(uint32_t i_index,const Data &i_particleData);
	Data *getParticleData(uint32_t i_index);
	void setVertexDeclarationToDevice();
	GE_Mat4x4 *getInstanceData(uint32_t i_index);
	void setInstanceData(uint32_t i_index,const GE_Mat4x4 &i_matrix);
	void setInstanceData(uint32_t i_index,const GE_Vec3 &i_position,const GE_Quat &i_rotation,const GE_Vec3 &i_scale);
	ID3D11ShaderResourceView *getInstanceSRV();
	ID3D11ShaderResourceView *getDiffuseSRV();
	void update();		
};

//----------------------------------------------------------------------------------------------------------------------------------
class GE_ParticelRenderer
{
	ID3D11InputLayout		*m_vertexDeclaration;
	ID3D11VertexShader		*m_vS;
	ID3D11GeometryShader	*m_gS;
	ID3D11GeometryShader	*m_gS_Instance;
	ID3D11PixelShader		*m_pS;
	ID3D11PixelShader		*m_pS_Shimmer;
	ID3D11Buffer			*m_perFrame;
	ID3D11Buffer			*m_perObject;

	struct PerFrame
	{
		GE_Mat4x4  ViewProjection;
		GE_Mat4x4  InverseProjection;
	}m_cPerFrame;

	struct PerObject
	{
		float		Softness;
		float		Shimmerness;
		GE_Vec2		Padding;
		GE_Vec4		ViewPosition;
		GE_Mat4x4	World;
		GE_Mat4x4	WorldViewProjection;
	}m_cPerObject;

	GE_ParticelRenderer();
	~GE_ParticelRenderer();
	static GE_ParticelRenderer m_singleton;
public:
	static GE_ParticelRenderer &getSingleton(){return m_singleton;}
	void init();
	void render(std::list<GE_Particle *> *i_particleList,GE_View *i_view,ID3D11ShaderResourceView *i_depthMap,ID3D11ShaderResourceView *i_backBufferMAP=NULL);
	void destroy();
};