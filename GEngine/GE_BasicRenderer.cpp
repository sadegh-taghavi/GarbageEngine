#include "GE_BasicRenderer.h"
#include "GE_Model.h"
#include "GE_Core.h"
#include "GE_View.h"
#include "GE_BufferResource.h"
#include "GE_Texture.h"
#include "GE_RenderTarget.h"
#include "GE_Light.h"
#include <boost\foreach.hpp>
#include "GE_Utility.h"
#include "GE_Material.h"

using namespace std;

GE_BasicRenderer GE_BasicRenderer::m_singleton;

void GE_BasicRenderer::init()
{
	if ( m_pixelShader )
		return;

	const D3D11_INPUT_ELEMENT_DESC _vdl[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32_UINT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const D3D11_SO_DECLARATION_ENTRY _vdlSO[] =
	{
		{ 0, "POSITION", 0, 0, 3, 0 },
		{ 0, "NORMAL", 0, 0, 3, 0 },
		{ 0, "TANGENT", 0, 0, 4, 0 },
		{ 0, "BLENDINDICES", 0, 0, 1, 0 },
	};

	_GE_FileChunk fc;
	fc.load( string( GE_SHADER_PATH ) + "GE_Basic_VS.cso" );
	GE_Core::getSingleton().getDevice()->CreateInputLayout( _vdl, 4, fc.Data, fc.Len, &m_vertexDeclaration );
	GE_Core::getSingleton().getDevice()->CreateVertexShader( fc.Data, fc.Len, NULL, &m_vS );
	fc.load( string( GE_SHADER_PATH ) + "GE_Basic_GS_StreamOut.cso" );
	GE_Core::getSingleton().getDevice()->CreateGeometryShaderWithStreamOutput( fc.Data, fc.Len, _vdlSO, 4, NULL, 0, 0, NULL, &m_gS_StreamOut );

	m_vS_Morph = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_Basic_VS_Morph.cso" );
	m_vS_Skin = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_Basic_VS_Skin.cso" );
	m_vS_MorphSkin = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_Basic_VS_Morph_Skin.cso" );
	m_vS_Instance = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_Basic_VS_Instance.cso" );
	m_vS_MorphStreamOut = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_Basic_VS_Morph_StreamOut.cso" );
	m_vS_SkinStreamOut = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_Basic_VS_Skin_StreamOut.cso" );
	m_vS_MorphSkinStreamOut = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_Basic_VS_Morph_Skin_StreamOut.cso" );
	m_pixelShader = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_Basic_PS.cso" );;
	m_pixelShaderTexture = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_Basic_PS_Texture.cso" );;

	m_perFrame = GE_Core::getSingleton().createConstBuffer( sizeof( PerFrame ) );
	m_perObject = GE_Core::getSingleton().createConstBuffer( sizeof( PerObject ) );

}

void GE_BasicRenderer::render(std::list<GE_Model *> *i_modelList, std::list<GE_Model *> *i_twoSideModelList, const GE_Mat4x4 *i_viewProjection, bool i_renderMaterial /*= true*/)
{
	GE_Core::getSingleton().getImmediateContext()->RSSetState(GE_Core::getSingleton().getRenderStates().RasterStateDisableCull);
	_render(i_twoSideModelList, i_viewProjection, i_renderMaterial);
	GE_Core::getSingleton().getImmediateContext()->RSSetState(GE_Core::getSingleton().getRenderStates().RasterStateDefault);
	_render(i_modelList, i_viewProjection, i_renderMaterial);
}

void GE_BasicRenderer::renderToDepth(GE_RenderTarget &i_renderTarget, std::list<GE_Model *> *i_modelList, std::list<GE_Model *> *i_twoSideModelList, const GE_Mat4x4 *i_viewProjection)
{
	GE_Core::getSingleton().getImmediateContext()->OMSetBlendState(GE_Core::getSingleton().getRenderStates().BlendStateDisableColorWrite, 0, 0xffffffff);
	i_renderTarget.setToDevice();
	i_renderTarget.clearRenderTargetWithDepth(0);
	render(i_modelList, i_twoSideModelList, i_viewProjection, true);
	GE_Core::getSingleton().getImmediateContext()->OMSetBlendState(GE_Core::getSingleton().getRenderStates().BlendStateDisableAlpha, 0, 0xffffffff);
	GE_Core::getSingleton().setClearSwapChain(*GE_Core::getSingleton().getCurrentSwapchain(), false, false, false);
}

void GE_BasicRenderer::renderToDepth(GE_Light *i_light, std::list< std::list<GE_Model *> *> *i_modelList, std::list< std::list<GE_Model *> *> *i_twoSideModelList)
{
	if (!i_light->hasShadow())
		return;
	GE_Core::getSingleton().getImmediateContext()->OMSetBlendState(GE_Core::getSingleton().getRenderStates().BlendStateDisableColorWrite, 0, 0xffffffff);
	uint32_t n = i_light->getNumberOfCascade();
	if (i_light->getType() == GE_Light::Spot)
		n = 1;
	else if (i_light->getType() == GE_Light::Point)
		n = LIGHT_PARAM_SIZE;
	auto it = i_modelList->begin();
	auto itTwo = i_twoSideModelList->begin();
	for (uint32_t i = 0; i < n; ++i)
	{
		i_light->getShadowMap(i)->setToDevice();
		i_light->getShadowMap(i)->clearRenderTargetWithDepth(0);
		render((*it),(*itTwo), i_light->getViewProjection(i), true);
		it++;
		itTwo++;
	}
	GE_Core::getSingleton().getImmediateContext()->OMSetBlendState(GE_Core::getSingleton().getRenderStates().BlendStateDisableAlpha, 0, 0xffffffff);
	GE_Core::getSingleton().setClearSwapChain(*GE_Core::getSingleton().getCurrentSwapchain(), false, false, false);
}

void GE_BasicRenderer::_render(std::list<GE_Model *> *i_modelList, const GE_Mat4x4 *i_viewProjection, bool i_renderMaterial /*= true*/)
{
	ID3D11ShaderResourceView *srv[8];

	GE_Mat4x4 transpose;

	if (!i_modelList->size())
		return;

	GE_Core::getSingleton().getImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	GE_Core::getSingleton().getImmediateContext()->IASetInputLayout(m_vertexDeclaration);

	transpose.transposeBy(i_viewProjection);

	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource(m_perFrame, 0, NULL, &transpose, 0, 0);
	GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers(0, 1, &m_perFrame);

	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers(0, 1, &GE_Core::getSingleton().getRenderStates().SamplerLinearWrap);

	uint32_t NOM;
	uint32_t NOB;
	uint32_t NOI;

	BOOST_FOREACH(GE_Model *model, *i_modelList)
	{
		NOM = model->m_lod == 0 ? model->m_morphValuesBuffer.m_lenght : 0;
		NOI = (uint32_t)model->m_instanceData.size();

		m_cPerObject.World.transposeBy(model->getWorld());
		m_cPerObject.WorldViewProjection.transposeBy(&(*model->getWorld() * (*i_viewProjection)));

		srv[4] = model->m_morphValuesBuffer.m_sRV;
		srv[5] = model->m_instanceBuffer.m_sRV;

		uint32_t subsetCount = (uint32_t)model->getSubsets().size();

		for (uint32_t subIndex = 0; subIndex < subsetCount; ++subIndex)
		{

			GE_Model::Subset *Subset = model->getSubset(subIndex);
			GE_Material *material = Subset->Material;
			NOB = model->m_bonesBuffer.m_lenght;

			m_cPerObject.NumberOfMorphTargets = NOM;
			m_cPerObject.NumberOfMorphTargetVertices = Subset->Lods[0]->NumberOfVertices * 3;

			bool has_mat = i_renderMaterial;

			srv[0] = Subset->Lods[model->getLod()]->BoneIndexBuffer.m_sRV;
			srv[1] = Subset->Lods[model->getLod()]->BoneWeightBuffer.m_sRV;
			srv[2] = model->m_bonesBuffer.m_sRV;
			srv[3] = Subset->MorphTargetsBuffer.m_sRV;
			srv[6] = Subset->Lods[model->getLod()]->TexcoordBuffer.m_sRV;

			if (!srv[6])
				has_mat = false;

			if (material)
			{
				m_cPerObject.Color = material->Diffuse;
				m_cPerObject.AlphaTest = material->AlphaTest;
				if (material->DiffuseAlphaMap[0])
					srv[7] = material->DiffuseAlphaMap[0]->getTextureSRV();
				else
				{
					srv[7] = NULL;
					has_mat = false;
				}
			}
			else
				has_mat = false;

			GE_Core::getSingleton().getImmediateContext()->UpdateSubresource(m_perObject, 0, NULL, &m_cPerObject, 0, 0);
			GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers(1, 1, &m_perObject);
			GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers(1, 1, &m_perObject);

			GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources(0, 8, srv);
			GE_Core::getSingleton().getImmediateContext()->GSSetShaderResources(0, 8, srv);
			GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources(0, 8, srv);

			if (!NOB && !NOM && !NOI)
			{
				GE_Core::getSingleton().getImmediateContext()->VSSetShader(m_vS, NULL, 0);
				GE_Core::getSingleton().getImmediateContext()->GSSetShader(NULL, NULL, 0);
				if (has_mat)
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShaderTexture, NULL, 0);
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShader, NULL, 0);
				model->render(subIndex, model->getLod());
			}
			else if (NOB && !NOM && !NOI)
			{

				GE_Core::getSingleton().getImmediateContext()->VSSetShader(m_vS_Skin, NULL, 0);
				GE_Core::getSingleton().getImmediateContext()->GSSetShader(NULL, NULL, 0);
				if (has_mat)
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShaderTexture, NULL, 0);
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShader, NULL, 0);
				model->render(subIndex, model->getLod());

			}
			else if (!NOB && NOM && !NOI)
			{

				GE_Core::getSingleton().getImmediateContext()->VSSetShader(m_vS_Morph, NULL, 0);
				GE_Core::getSingleton().getImmediateContext()->GSSetShader(NULL, NULL, 0);
				if (has_mat)
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShaderTexture, NULL, 0);
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShader, NULL, 0);
				model->render(subIndex, model->getLod());

			}
			else if (!NOB && !NOM && NOI)
			{

				if (!model->getNumberOfRenderInstances())
					continue;

				GE_Core::getSingleton().getImmediateContext()->VSSetShader(m_vS_Instance, NULL, 0);
				GE_Core::getSingleton().getImmediateContext()->GSSetShader(NULL, NULL, 0);
				if (has_mat)
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShaderTexture, NULL, 0);
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShader, NULL, 0);
				model->renderInstance(subIndex, model->getLod());

			}
			else if (NOB && NOM && !NOI)
			{
				GE_Core::getSingleton().getImmediateContext()->VSSetShader(m_vS_MorphSkin, NULL, 0);
				GE_Core::getSingleton().getImmediateContext()->GSSetShader(NULL, NULL, 0);
				if (has_mat)
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShaderTexture, NULL, 0);
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShader, NULL, 0);
				model->render(subIndex, model->getLod());

			}
			else if (NOB && !NOM && NOI)
			{

				if (!model->getNumberOfRenderInstances())
					continue;

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState(GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0);

				GE_Core::getSingleton().getImmediateContext()->VSSetShader(m_vS_SkinStreamOut, NULL, 0);
				GE_Core::getSingleton().getImmediateContext()->GSSetShader(m_gS_StreamOut, NULL, 0);
				GE_Core::getSingleton().getImmediateContext()->PSSetShader(NULL, NULL, 0);
				model->renderInstanceToStreamOut(subIndex, model->getLod());

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState(NULL, 0);

				GE_Core::getSingleton().getImmediateContext()->VSSetShader(m_vS_Instance, NULL, 0);
				GE_Core::getSingleton().getImmediateContext()->GSSetShader(NULL, NULL, 0);
				if (has_mat)
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShaderTexture, NULL, 0);
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShader, NULL, 0);
				model->renderInstanceByStreamOut(subIndex, model->getLod());

			}
			else if (!NOB && NOM && NOI)
			{

				if (!model->getNumberOfRenderInstances())
					continue;

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState(GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0);

				GE_Core::getSingleton().getImmediateContext()->VSSetShader(m_vS_MorphStreamOut, NULL, 0);
				GE_Core::getSingleton().getImmediateContext()->GSSetShader(m_gS_StreamOut, NULL, 0);
				GE_Core::getSingleton().getImmediateContext()->PSSetShader(NULL, NULL, 0);
				model->renderInstanceToStreamOut(subIndex, model->getLod());

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState(NULL, 0);

				GE_Core::getSingleton().getImmediateContext()->VSSetShader(m_vS_Instance, NULL, 0);
				GE_Core::getSingleton().getImmediateContext()->GSSetShader(NULL, NULL, 0);
				if (has_mat)
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShaderTexture, NULL, 0);
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShader, NULL, 0);
				model->renderInstanceByStreamOut(subIndex, model->getLod());

			}
			else if (NOB && NOM && NOI)
			{

				if (!model->getNumberOfRenderInstances())
					continue;

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState(GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0);

				GE_Core::getSingleton().getImmediateContext()->VSSetShader(m_vS_MorphSkinStreamOut, NULL, 0);
				GE_Core::getSingleton().getImmediateContext()->GSSetShader(m_gS_StreamOut, NULL, 0);
				GE_Core::getSingleton().getImmediateContext()->PSSetShader(NULL, NULL, 0);
				model->renderInstanceToStreamOut(subIndex, model->getLod());

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState(NULL, 0);

				GE_Core::getSingleton().getImmediateContext()->VSSetShader(m_vS_Instance, NULL, 0);
				GE_Core::getSingleton().getImmediateContext()->GSSetShader(NULL, NULL, 0);
				if (has_mat)
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShaderTexture, NULL, 0);
				else
					GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pixelShader, NULL, 0);
				model->renderInstanceByStreamOut(subIndex, model->getLod());
			}
		}
	}
	GE_Core::getSingleton().getImmediateContext()->RSSetState(GE_Core::getSingleton().getRenderStates().RasterStateDefault);
}

GE_BasicRenderer::GE_BasicRenderer()
{
	m_vertexDeclaration = NULL;
	m_vS = NULL;
	m_vS_Morph = NULL;
	m_vS_Skin = NULL;
	m_vS_MorphSkin = NULL;
	m_vS_Instance = NULL;
	m_vS_MorphStreamOut = NULL;
	m_vS_SkinStreamOut = NULL;
	m_vS_MorphSkinStreamOut = NULL;
	m_gS_StreamOut = NULL;
	m_pixelShader = NULL;
	m_pixelShaderTexture = NULL;
	m_perFrame = NULL;
	m_perObject = NULL;
}

GE_BasicRenderer::~GE_BasicRenderer()
{
	SAFE_RELEASE( m_vertexDeclaration );
	SAFE_RELEASE( m_vS );
	SAFE_RELEASE( m_vS_Morph );
	SAFE_RELEASE( m_vS_Skin );
	SAFE_RELEASE( m_vS_MorphSkin );
	SAFE_RELEASE( m_vS_Instance );
	SAFE_RELEASE( m_vS_MorphStreamOut );
	SAFE_RELEASE( m_vS_SkinStreamOut );
	SAFE_RELEASE( m_vS_MorphSkinStreamOut );
	SAFE_RELEASE( m_gS_StreamOut );
	SAFE_RELEASE( m_pixelShader );
	SAFE_RELEASE( m_pixelShaderTexture );
	SAFE_RELEASE( m_perFrame );
	SAFE_RELEASE( m_perObject );
}
