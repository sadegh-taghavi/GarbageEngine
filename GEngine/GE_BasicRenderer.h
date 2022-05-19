#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"
#include <list>

class GE_Model;
class GE_RenderTarget;
class GE_Light;

class GE_BasicRenderer
{
	static GE_BasicRenderer m_singleton;
	ID3D11InputLayout *m_vertexDeclaration;
	ID3D11VertexShader *m_vS, *m_vS_Morph, *m_vS_Skin, *m_vS_MorphSkin, *m_vS_Instance, *m_vS_MorphStreamOut, *m_vS_SkinStreamOut, *m_vS_MorphSkinStreamOut;
	ID3D11GeometryShader *m_gS_StreamOut;
	ID3D11PixelShader *m_pixelShader, *m_pixelShaderTexture;
	ID3D11Buffer		 *m_perFrame, *m_perObject;

	struct PerFrame
	{
		GE_Mat4x4 ViewProjection;
	};

	struct PerObject
	{
		GE_Mat4x4	WorldViewProjection;
		GE_Mat4x4	World;
		GE_Vec3		Color;
		float	    AlphaTest;
		uint32_t	NumberOfMorphTargets;
		uint32_t	NumberOfMorphTargetVertices;
		uint32_t	Padding[ 2 ];
	}m_cPerObject;

	GE_BasicRenderer();
	~GE_BasicRenderer();
	void _render(std::list<GE_Model *> *i_modelList, const GE_Mat4x4 *i_viewProjection, bool i_renderMaterial = true);
public:
	static GE_BasicRenderer& getSingleton(){ return m_singleton; }
	void init();
	void render(std::list<GE_Model *> *i_modelList, std::list<GE_Model *> *i_twoSideModelList, const GE_Mat4x4 *i_viewProjection, bool i_renderMaterial = true);
	void renderToDepth(GE_RenderTarget &i_renderTarget, std::list<GE_Model *> *i_modelList, std::list<GE_Model *> *i_twoSideModelList, const GE_Mat4x4 *i_viewProjection);
	void renderToDepth(GE_Light *i_light, std::list< std::list<GE_Model *> *> *i_modelList, std::list< std::list<GE_Model *> *> *i_twoSideModelList);
	ID3D11InputLayout *getInputLayout(){ return m_vertexDeclaration; }
};