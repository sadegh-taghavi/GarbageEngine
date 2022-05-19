#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"
#include "GE_RenderTarget.h"
#include "GE_BufferResource.h"
#include "GE_Model.h"
#include <list>

class GE_DeferredGeometry
{
	struct MaterialBufferData
	{
		float Material[ 16 ];
	};
	static GE_DeferredGeometry	m_singleton;
	class GE_Texture			*m_bfnTexture;
	ID3D11InputLayout			*m_vertexDeclaration;
	ID3D11VertexShader			*m_vS;
	ID3D11VertexShader			*m_vS_Morph;
	ID3D11VertexShader			*m_vS_Skin;
	ID3D11VertexShader			*m_vS_MorphSkin;
	ID3D11VertexShader			*m_vS_Instance;
	ID3D11VertexShader			*m_vS_MorphStreamOut;
	ID3D11VertexShader			*m_vS_SkinStreamOut;
	ID3D11VertexShader			*m_vS_MorphSkinStreamOut;
	ID3D11GeometryShader		*m_gS_StreamOut;
	ID3D11PixelShader			*m_pS;
	ID3D11PixelShader			*m_pS_Reflect;
	ID3D11PixelShader			*m_pS_MultiLayer;
	ID3D11PixelShader			*m_pS_LightMap;
	ID3D11PixelShader			*m_pS_LightMap_Reflect;
	ID3D11PixelShader			*m_pS_MultiLayer_LightMap;
	ID3D11PixelShader			*m_pS_MultiLayer_LightMap_Reflect;
	ID3D11PixelShader			*m_pS_MultiLayer_Reflect;
	ID3D11Buffer				*m_perFrame;
	ID3D11Buffer				*m_perObject;

	ID3D11VertexShader			*m_vS_T;
	ID3D11VertexShader			*m_vS_Morph_T;
	ID3D11VertexShader			*m_vS_Skin_T;
	ID3D11VertexShader			*m_vS_MorphSkin_T;
	ID3D11VertexShader			*m_vS_Instance_T;
	ID3D11VertexShader			*m_vS_MorphStreamOut_T;
	ID3D11VertexShader			*m_vS_SkinStreamOut_T;
	ID3D11VertexShader			*m_vS_MorphSkinStreamOut_T;
	ID3D11GeometryShader		*m_gS_StreamOut_T;
	ID3D11PixelShader			*m_pS_T;
	ID3D11PixelShader			*m_pS_Reflect_T;
	ID3D11PixelShader			*m_pS_MultiLayer_T;
	ID3D11PixelShader			*m_pS_LightMap_T;
	ID3D11PixelShader			*m_pS_LightMap_Reflect_T;
	ID3D11PixelShader			*m_pS_MultiLayer_LightMap_T;
	ID3D11PixelShader			*m_pS_MultiLayer_LightMap_Reflect_T;
	ID3D11PixelShader			*m_pS_MultiLayer_Reflect_T;
	ID3D11Buffer				*m_perFrame_T;
	ID3D11Buffer				*m_perObject_T;


	uint32_t					m_numberOfMaterials;

	GE_RenderTarget				*m_renderTarget;
	GE_RenderTarget				*m_renderTargetCopy;

	_GE_BufferResource			m_materialDataBuffer;
	GE_RenderTarget				*m_hdrTarget;

	struct PerFrame
	{
		GE_Mat4x4	ViewProjection;
		GE_Vec3		ViewPosition;
		float		Padding;
	}m_cPerFrame;

	struct PerObject
	{
		GE_Mat4x4	WorldViewProjection;
		GE_Mat4x4	World;
		GE_Vec3		EnvironmentPosition;
		float		EnvironmentRadiusInv;
		uint32_t	NumberOfMorphTargets;
		uint32_t	NumberOfMorphTargetVertices;
		float		UseVertexColor;
		float		AlphaTest;
		float		Bumpiness;
		float		Roughness;
		float		Reflectmask;
		float		MaterialID;
		float		LayerMultiply;
		float		LayerOffset;
		float		Glowness;
		float		Falloff;
		float		FalloffSpread;
		float		Padding0;
		float		Padding1;
		float		Padding2;
		
	}m_cPerObject;

	struct PerFrameT
	{
		GE_Vec4		LightAmbient;
		GE_Vec4		LightDiffuse;
		GE_Vec4		LightSpecular;
		GE_Vec4		LightPosition;
		GE_Vec4		LightDirection;
		GE_Vec4		ViewPosition;
		GE_Vec4		ShadowOffsetBias[ 6 ];
		GE_Mat4x4	LightViewProjection[ 6 ];
		GE_Mat4x4	ViewProjection;
		GE_Mat4x4	ViewProjectionInv;
	}m_cPerFrameT;

	struct PerObjectT
	{
		GE_Mat4x4	WorldViewProjection;
		GE_Mat4x4	World;
		GE_Vec3		EenvironmentPosition;
		float		EnvironmentRadiusInv;
		GE_Vec3		Emissive;
		float		UseVertexColor;
		GE_Vec3		Ambient;
		float		Refract;
		GE_Vec3		Specular;
		float		Bumpiness;
		GE_Vec3		Diffuse;
		float		Roughness;
		GE_Vec3		Glow;
		float		Reflectmask;
		float		SpecularPower;
		float		Alpha;
		float		LayerMultiply;
		float		LayerOffset;
		float		Glowness;
		float		Refractness;
		float		Falloff;
		float		FalloffSpread;
		float		Padding0;
		float		Padding1;
		uint32_t	NumberOfMorphTargets;
		uint32_t	NumberOfMorphTargetVertices;

	}m_cPerObjectT;

	GE_DeferredGeometry();
	~GE_DeferredGeometry();
	bool m_wireframe;
	void _render(std::list<GE_Model *> *i_modelList, GE_View *i_view);
	void _setPs( bool i_lightMap, bool i_reflect, bool i_multiLayer );
	void _renderTransparent( std::list<GE_Model *> *i_modelList, GE_View *i_view, class GE_Light *i_light );
	void _setPsTransparent( bool i_lightMap, bool i_reflect, bool i_multiLayer );
public:

	enum  TargetType
	{
		Depth = -1,
		NormalSpecular,
		DiffuseGlow,
		MaterialID,
	};
	uint32_t getNumberOfMaterials(){ return m_numberOfMaterials; }
	static GE_DeferredGeometry& getSingleton(){ return m_singleton; }
	void resize();
	void init();
	void begin();
	void end();
	bool isWireframeMode(){ return m_wireframe; }
	void setWireframeMode( bool i_wireframe ) { m_wireframe = i_wireframe; }
	void addMaterial( const GE_Material *i_material );
	void render(std::list<GE_Model *> *i_modelList, std::list<GE_Model *> *i_twoSideModelList, class GE_Terrain *i_terrain, GE_View *i_view);
	void renderTransparent(std::list<GE_Model *> *i_modelList, std::list<GE_Model *> *i_twoSideModelList, GE_View *i_view, class GE_Light *i_light, ID3D11ShaderResourceView *i_backBufferMAP, bool i_hdr = false);
	GE_RenderTarget  *getRenderTargetCopy(){ return m_renderTargetCopy; }
	GE_RenderTarget  *getRenderTarget(){ return m_renderTarget; }
	void setRenderTarget( GE_RenderTarget *i_renderTarget ){ m_renderTarget = i_renderTarget; }
	void copyRenderTarget();
	ID3D11ShaderResourceView *getBFNSRV();
	ID3D11ShaderResourceView *getTargetSRV( TargetType i_targetType );
	ID3D11ShaderResourceView *getTargetSRVCopy( TargetType i_targetType );
	ID3D11ShaderResourceView *getMaterialBufferSRV(){ return m_materialDataBuffer.m_sRV; }
};