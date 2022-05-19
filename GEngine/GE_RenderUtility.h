#pragma once
#include "GE_Prerequisites.h"
#include "GE_BufferResource.h"
#include "GE_Bound.h"
#include "GE_Texture.h"
#include "GE_Math.h"

class GE_View;

//GE_RenderUtilityObject----------------------------------------------------------------------------------------------------------------------------------------------
class GE_RenderUtilityObject 
{
protected:
	friend class GE_RenderUtility;
	ID3D11Buffer				*m_vB;
	ID3D11Buffer				*m_iB;
	_GE_BufferResource			m_colorBuffer;
	_GE_BufferResource			m_texcoordBuffer;
	std::vector<GE_Vec3>		m_verticesData;
	std::vector<GE_Vec3>		m_colorsData;
	std::vector<GE_Vec2>		m_texcoordData;
	std::vector<int>			m_indicesData;
	bool						m_visible;
	bool						m_depthDisable;
	bool						m_billboard;
	bool						m_verticesChange;
	bool						m_colorChange;
	bool						m_texcoordChange;
	bool						m_indicesChange;
	bool						m_transformChange;
	GE_Mat4x4					m_world;
	GE_Mat4x4					*m_transformBy;
	GE_Mat4x4					*m_transformForce;
	GE_Vec3						m_position;
	GE_Vec3						m_scale;
	GE_Vec3						m_color;
	GE_Quat						m_rotation;
	GE_Bound					m_bound;
	float						m_alphaTest;
	UINT						m_indicesLen;
	UINT						m_verticesLen;
	UINT						m_colorsLen;
	UINT						m_texcoordLen;
	GE_Texture					*m_texture;
	ID3D11ShaderResourceView	*m_sRV;
	D3D11_PRIMITIVE_TOPOLOGY	m_topology;

	virtual void				calculateBound();
	virtual void				render();
public:

	GE_RenderUtilityObject();
	virtual ~GE_RenderUtilityObject();

	bool						&isVisible();
	bool						&isBillboard();
	bool						&isDepthDisable();
	ID3D11ShaderResourceView	*getSRV();
	D3D11_PRIMITIVE_TOPOLOGY	&getRenderTopology();
	GE_Bound					&getBound();
	float						&getAlphaTest();
	GE_Vec3						&getColor();

	GE_Vec3						&getPosition();
	GE_Quat						&getRotation();
	GE_Vec3						&getScale();
	GE_Mat4x4					*getWorld();
	GE_Mat4x4					*getTransformBy();
	GE_Mat4x4					*getTransformForce();
	
	void						setBillboard(bool i_billboard);
	void						setTexture(const char *i_fileName);
	void						setSRV(ID3D11ShaderResourceView *i_sRV = NULL);
	void						setRenderTopology(D3D11_PRIMITIVE_TOPOLOGY i_topology);
	void						setBoundRadiusMultiply(float i_multiply);
	void						setAlphaTest(float i_alphaTest);
	void						setVisible(bool i_visible);
	void						setDepthDisable(bool i_depthDisable);
	void						setTransformBy( const GE_Mat4x4 *i_transform );
	void						setTransformForce( const GE_Mat4x4 *i_transform );
	void						setPosition(GE_Vec3 &i_position);
	void						setRotation(GE_Quat &i_rotation);
	void						setScale(GE_Vec3 &i_scale);
	void						setColor(const GE_Vec3 &i_color);
	
	virtual void				update();
	std::vector<GE_Vec2>		*texcoordDataBeginEdit();
	void						texcoordDataEndEdit();
	std::vector<GE_Vec3>		*verticesDataBeginEdit();
	void						verticesDataEndEdit();
	std::vector<GE_Vec3>		*colorsDataBeginEdit();
	void						colorsDataEndEdit();
	std::vector<int>			*indicesDataBeginEdit();
	void						indicesDataEndEdit();
	void						fillColorBuffer(GE_Vec3 &i_color);
	void						createPlane( GE_Vec3 &i_offsetPos, GE_Quat &i_offsetRot, GE_Vec3 &i_offsetScale = GE_Vec3(1.0f, 1.0f, 1.0f) );
	void						createBox( GE_Vec3 &i_size, GE_Vec3 &i_offset );
	void						createBoxBB( GE_Vec3 &i_bbMin, GE_Vec3 &i_bbMax);
	void						createSphere( float i_radius, GE_Vec3 &i_offset, uint32_t i_vSlice, uint32_t i_hSlice);
	void						createCylinder( float i_radius, float i_height, GE_Vec3 &i_offset, uint32_t i_slice);
	void						createCylinderNoCap( float i_radius, float i_height, GE_Vec3 &i_offset, uint32_t i_slice);
	void						createCapsule( float i_radius, float i_halfHeight, GE_Vec3 &i_offset,uint32_t i_capSlice, uint32_t i_slice);
	void						createTriangleMesh( GE_Vec3 *i_vertices, uint32_t i_nbVertices, uint32_t *i_indices, uint32_t i_nbIndices );
};

//GE_RenderUtility----------------------------------------------------------------------------------------------------------------------------------------------
class GE_RenderUtility
{
	ID3D11VertexShader	*m_vS;
	ID3D11PixelShader	*m_pS;
	ID3D11PixelShader	*m_pS_Color;
	ID3D11PixelShader	*m_pS_Texture;
	ID3D11PixelShader	*m_pS_Depth;
	ID3D11PixelShader	*m_pS_Color_Texture;
	ID3D11PixelShader	*m_pS_Color_Depth;
	ID3D11PixelShader	*m_pS_Texture_Depth;
	ID3D11PixelShader	*m_pS_Color_Texture_Depth;
	ID3D11Buffer		*m_perObject;
	struct PerObject
	{
		GE_Mat4x4	Matrix;
		GE_Mat4x4	InvView;
		GE_Mat4x4	InvProjection;
		GE_Vec4		Color;
	}m_temp;
	static GE_RenderUtility m_singleton;

	GE_RenderUtility(){}
	~GE_RenderUtility();
public:
	static GE_RenderUtility &getSingleton(){return m_singleton;}
	void init();
	void render(std::list<GE_RenderUtilityObject *> *i_objects,GE_View *i_view, ID3D11ShaderResourceView *i_depth);
};

#include "GE_RenderUtility.inl"