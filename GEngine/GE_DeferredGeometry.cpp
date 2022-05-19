#include "GE_DeferredGeometry.h"
#include "GE_Model.h"
#include "GE_Core.h"
#include "GE_View.h"
#include "GE_BufferResource.h"
#include "GE_Math.h"
#include "GE_Texture.h"
#include "GE_RenderTarget.h"
#include <boost\foreach.hpp>
#include "GE_Utility.h"
#include "GE_Material.h"
#include "GE_Light.h"
#include "GE_Terrain.h"

using namespace std;

GE_DeferredGeometry GE_DeferredGeometry::m_singleton;

void GE_DeferredGeometry::init()
{
	if ( m_pS )
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
	fc.load( string( GE_SHADER_PATH ) + "GE_DG_VS.cso" );
	GE_Core::getSingleton().getDevice()->CreateInputLayout( _vdl, 4, fc.Data, fc.Len, &m_vertexDeclaration );
	GE_Core::getSingleton().getDevice()->CreateVertexShader( fc.Data, fc.Len, NULL, &m_vS );
	fc.load( string( GE_SHADER_PATH ) + "GE_DG_GS_StreamOut.cso" );
	GE_Core::getSingleton().getDevice()->CreateGeometryShaderWithStreamOutput( fc.Data, fc.Len, _vdlSO, 4, NULL, 0, 0, NULL, &m_gS_StreamOut );

	m_vS_Morph = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DG_VS_Morph.cso" );
	m_vS_Skin = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DG_VS_Skin.cso" );
	m_vS_MorphSkin = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DG_VS_Morph_Skin.cso" );
	m_vS_Instance = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DG_VS_Instance.cso" );
	m_vS_MorphStreamOut = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DG_VS_Morph_StreamOut.cso" );
	m_vS_SkinStreamOut = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DG_VS_Skin_StreamOut.cso" );
	m_vS_MorphSkinStreamOut = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DG_VS_Morph_Skin_StreamOut.cso" );
	m_pS = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DG_PS.cso" );
	m_pS_Reflect = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DG_PS_Reflect.cso" );
	m_pS_MultiLayer = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DG_PS_Multilayer.cso" );
	m_pS_LightMap = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DG_PS_LightMap.cso" );
	m_pS_LightMap_Reflect = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DG_PS_LightMap_Reflect.cso" );
	m_pS_MultiLayer_LightMap = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DG_PS_Multilayer_LightMap.cso" );
	m_pS_MultiLayer_LightMap_Reflect = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DG_PS_Multilayer_LightMap_Reflect.cso" );
	m_pS_MultiLayer_Reflect = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DG_PS_Multilayer_Reflect.cso" );
	m_bfnTexture = GE_TextureManager::getSingleton().createTexture( ( string( GE_TEXTURE_PATH ) + "BFN.dds" ).c_str() );
	m_perFrame = GE_Core::getSingleton().createConstBuffer( sizeof( PerFrame ) );
	m_perObject = GE_Core::getSingleton().createConstBuffer( sizeof( PerObject ) );

	fc.load( string( GE_SHADER_PATH ) + "GE_DGT_GS_StreamOut.cso" );
	GE_Core::getSingleton().getDevice()->CreateGeometryShaderWithStreamOutput( fc.Data, fc.Len, _vdlSO, 4, NULL, 0, 0, NULL, &m_gS_StreamOut_T );
	m_vS_T = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DGT_VS.cso" );
	m_vS_Morph_T = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DGT_VS_Morph.cso" );
	m_vS_Skin_T = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DGT_VS_Skin.cso" );
	m_vS_MorphSkin_T = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DGT_VS_Morph_Skin.cso" );
	m_vS_Instance_T = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DGT_VS_Instance.cso" );
	m_vS_MorphStreamOut_T = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DGT_VS_Morph_StreamOut.cso" );
	m_vS_SkinStreamOut_T = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DGT_VS_Skin_StreamOut.cso" );
	m_vS_MorphSkinStreamOut_T = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_DGT_VS_Morph_Skin_StreamOut.cso" );
	m_pS_T = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DGT_PS.cso" );
	m_pS_Reflect_T = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DGT_PS_Reflect.cso" );
	m_pS_MultiLayer_T = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DGT_PS_Multilayer.cso" );
	m_pS_LightMap_T = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DGT_PS_LightMap.cso" );
	m_pS_LightMap_Reflect_T = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DGT_PS_LightMap_Reflect.cso" );
	m_pS_MultiLayer_LightMap_T = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DGT_PS_Multilayer_LightMap.cso" );
	m_pS_MultiLayer_LightMap_Reflect_T = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DGT_PS_Multilayer_LightMap_Reflect.cso" );
	m_pS_MultiLayer_Reflect_T = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_DGT_PS_Multilayer_Reflect.cso" );
	m_perFrame_T = GE_Core::getSingleton().createConstBuffer( sizeof( PerFrameT ) );
	m_perObject_T = GE_Core::getSingleton().createConstBuffer( sizeof( PerObjectT ) );

	m_materialDataBuffer.createBuffer( 256, sizeof( MaterialBufferData ), NULL, false, DXGI_FORMAT_R32G32B32A32_FLOAT, 4 );
	m_materialDataBuffer.m_data = new MaterialBufferData[ 256 ];

}

void GE_DeferredGeometry::copyRenderTarget()
{
	GE_Core::getSingleton().getImmediateContext()->CopyResource( m_renderTargetCopy->getDepthTexture(), m_renderTarget->getDepthTexture() );
	GE_Core::getSingleton().getImmediateContext()->CopyResource( m_renderTargetCopy->getRTTexture( 0 ), m_renderTarget->getRTTexture( 0 ) );
	GE_Core::getSingleton().getImmediateContext()->CopyResource( m_renderTargetCopy->getRTTexture( 1 ), m_renderTarget->getRTTexture( 1 ) );
	GE_Core::getSingleton().getImmediateContext()->CopyResource( m_renderTargetCopy->getRTTexture( 2 ), m_renderTarget->getRTTexture( 2 ) );
}

void GE_DeferredGeometry::resize()
{
	SAFE_DELETE( m_renderTarget );
	SAFE_DELETE( m_renderTargetCopy );
	SAFE_DELETE( m_hdrTarget );
	GE_RenderTarget::Parameters rp( 3 );
	rp.Width[ 0 ] = rp.Width[ 1 ] = rp.Width[ 2 ] = ( uint32_t )GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Width;
	rp.Height[ 0 ] = rp.Height[ 1 ] = rp.Height[ 2 ] = ( uint32_t )GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Height;
	rp.Format[ 0 ] = rp.Format[ 1 ] = rp.Format[ 2 ] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_renderTarget = new GE_RenderTarget( rp );
	m_renderTargetCopy = new GE_RenderTarget( rp );
	rp.Count = 1;
	rp.Format[ 0 ] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rp.HasDepth = false;
	m_hdrTarget = new GE_RenderTarget( rp );
}

ID3D11ShaderResourceView *GE_DeferredGeometry::getTargetSRV( TargetType i_targetType )
{
	if ( i_targetType == Depth )
		return m_renderTarget->getDepthSRV();
	return m_renderTarget->getSRV( ( uint32_t )i_targetType );
}

ID3D11ShaderResourceView * GE_DeferredGeometry::getTargetSRVCopy( TargetType i_targetType )
{
	if ( i_targetType == Depth )
		return m_renderTargetCopy->getDepthSRV();
	return m_renderTargetCopy->getSRV( ( uint32_t )i_targetType );
}

void GE_DeferredGeometry::render( std::list<GE_Model *> *i_modelList, std::list<GE_Model *> *i_twoSideModelList, class GE_Terrain *i_terrain, GE_View *i_view )
{
	if ( m_wireframe )
		GE_Core::getSingleton().getImmediateContext()->RSSetState( GE_Core::getSingleton().getRenderStates().RasterStateWireframe );
	else
		GE_Core::getSingleton().getImmediateContext()->RSSetState( GE_Core::getSingleton().getRenderStates().RasterStateDefault );
	if ( i_terrain )
		GE_Terrain::getSingleton().render( i_view );
	ID3D11ShaderResourceView *srv[ 1 ];
	srv[ 0 ] = m_bfnTexture->getTextureSRV();
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	_render( i_modelList, i_view );
	if ( m_wireframe )
		GE_Core::getSingleton().getImmediateContext()->RSSetState( GE_Core::getSingleton().getRenderStates().RasterStateWireframeDisableCull );
	else
		GE_Core::getSingleton().getImmediateContext()->RSSetState( GE_Core::getSingleton().getRenderStates().RasterStateDisableCull );
	_render( i_twoSideModelList, i_view );	
}

void GE_DeferredGeometry::renderTransparent( std::list<GE_Model *> *i_modelList, std::list<GE_Model *> *i_twoSideModelList,
	GE_View *i_view, GE_Light *i_light, ID3D11ShaderResourceView *i_backBufferMAP, bool i_hdr )
{
	ID3D11ShaderResourceView *srv[ 1 ];
	srv[ 0 ] = i_backBufferMAP;
	ID3D11RenderTargetView *renderTarget = NULL;
	ID3D11DepthStencilView *depthTarget = NULL;

	GE_Core::getSingleton().getImmediateContext()->OMGetRenderTargets( 1, &renderTarget, &depthTarget );
	GE_Core::getSingleton().getImmediateContext()->OMSetRenderTargets( 1, &renderTarget, m_renderTarget->getDepthView() );
	GE_Core::getSingleton().getImmediateContext()->OMSetBlendState( GE_Core::getSingleton().getRenderStates().BlendStateAlpha, 0, 0xffffffff );

	if ( i_hdr )
	{
		ID3D11Resource *targetResource = NULL;
		renderTarget->GetResource(&targetResource);
		GE_Core::getSingleton().getImmediateContext()->CopyResource( m_hdrTarget->getRTTexture( 0 ), targetResource );
		srv[ 0 ] = m_hdrTarget->getSRV( 0 );
		SAFE_RELEASE( targetResource );

	}
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );

	if ( m_wireframe )
		GE_Core::getSingleton().getImmediateContext()->RSSetState( GE_Core::getSingleton().getRenderStates().RasterStateWireframeDisableCull );
	else
		GE_Core::getSingleton().getImmediateContext()->RSSetState( GE_Core::getSingleton().getRenderStates().RasterStateDisableCull );
	_renderTransparent( i_twoSideModelList, i_view, i_light );
	if ( m_wireframe )
		GE_Core::getSingleton().getImmediateContext()->RSSetState( GE_Core::getSingleton().getRenderStates().RasterStateWireframe );
	else
		GE_Core::getSingleton().getImmediateContext()->RSSetState( GE_Core::getSingleton().getRenderStates().RasterStateDefault );
	_renderTransparent( i_modelList, i_view, i_light );
	GE_Core::getSingleton().getImmediateContext()->OMSetBlendState( GE_Core::getSingleton().getRenderStates().BlendStateDisableAlpha, 0, 0xffffffff );
	GE_Core::getSingleton().getImmediateContext()->OMSetRenderTargets( 1, &renderTarget, depthTarget );
	srv[ 0 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	SAFE_RELEASE( renderTarget );
	SAFE_RELEASE( depthTarget );
	GE_Core::getSingleton().getImmediateContext()->RSSetState( GE_Core::getSingleton().getRenderStates().RasterStateDefault );
}

void GE_DeferredGeometry::begin()
{
	m_renderTarget->setToDevice();
	m_renderTarget->clearRenderTarget();
	m_renderTarget->clearDepth();
}

void GE_DeferredGeometry::end()
{
	if ( m_numberOfMaterials )
	{
		m_materialDataBuffer.fill( m_numberOfMaterials );
		m_numberOfMaterials = 0;
	}
	ID3D11ShaderResourceView *srv[ 1 ] = { NULL };
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );
	GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
	if ( m_wireframe )
		GE_Core::getSingleton().getImmediateContext()->RSSetState( GE_Core::getSingleton().getRenderStates().RasterStateDefault );
}

void GE_DeferredGeometry::addMaterial( const GE_Material *i_material )
{
	if ( m_numberOfMaterials > 255 )
		return;
	MaterialBufferData *ptr = ( MaterialBufferData * )m_materialDataBuffer.m_data;
	memcpy( &ptr[ m_numberOfMaterials ], i_material, sizeof( MaterialBufferData ) );
	m_numberOfMaterials++;
}

GE_DeferredGeometry::GE_DeferredGeometry()
{
	m_hdrTarget = NULL;
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
	m_pS = NULL;
	m_pS_Reflect = NULL;
	m_pS_MultiLayer = NULL;
	m_pS_LightMap = NULL;
	m_pS_LightMap_Reflect = NULL;
	m_pS_MultiLayer_LightMap = NULL;
	m_pS_MultiLayer_LightMap_Reflect = NULL;
	m_pS_MultiLayer_Reflect = NULL;
	m_perFrame = NULL;
	m_perObject = NULL;
	m_vS_T = NULL;
	m_vS_Morph_T = NULL;
	m_vS_Skin_T = NULL;
	m_vS_MorphSkin_T = NULL;
	m_vS_Instance_T = NULL;
	m_vS_MorphStreamOut_T = NULL;
	m_vS_SkinStreamOut_T = NULL;
	m_vS_MorphSkinStreamOut_T = NULL;
	m_gS_StreamOut_T = NULL;
	m_pS_T = NULL;
	m_pS_Reflect_T = NULL;
	m_pS_MultiLayer_T = NULL;
	m_pS_LightMap_T = NULL;
	m_pS_LightMap_Reflect_T = NULL;
	m_pS_MultiLayer_LightMap_T = NULL;
	m_pS_MultiLayer_LightMap_Reflect_T = NULL;
	m_pS_MultiLayer_Reflect_T = NULL;
	m_perFrame_T = NULL;
	m_perObject_T = NULL;
	m_renderTarget = NULL;
	m_renderTargetCopy = NULL;
	m_bfnTexture = NULL;
	m_numberOfMaterials = 0;
	m_wireframe = false;
}

GE_DeferredGeometry::~GE_DeferredGeometry()
{
	SAFE_DELETE( m_hdrTarget );
	GE_TextureManager::getSingleton().remove( &m_bfnTexture );
	SAFE_DELETE_ARRAY( m_materialDataBuffer.m_data );
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
	SAFE_RELEASE( m_pS );
	SAFE_RELEASE( m_pS_Reflect );
	SAFE_RELEASE( m_pS_MultiLayer );
	SAFE_RELEASE( m_pS_LightMap );
	SAFE_RELEASE( m_pS_LightMap_Reflect );
	SAFE_RELEASE( m_pS_MultiLayer_LightMap );
	SAFE_RELEASE( m_pS_MultiLayer_LightMap_Reflect );
	SAFE_RELEASE( m_pS_MultiLayer_Reflect );
	SAFE_RELEASE( m_perFrame );
	SAFE_RELEASE( m_perObject );
	SAFE_RELEASE( m_vS_T );
	SAFE_RELEASE( m_vS_Morph_T );
	SAFE_RELEASE( m_vS_Skin_T );
	SAFE_RELEASE( m_vS_MorphSkin_T );
	SAFE_RELEASE( m_vS_Instance_T );
	SAFE_RELEASE( m_vS_MorphStreamOut_T );
	SAFE_RELEASE( m_vS_SkinStreamOut_T );
	SAFE_RELEASE( m_vS_MorphSkinStreamOut_T );
	SAFE_RELEASE( m_gS_StreamOut_T );
	SAFE_RELEASE( m_pS_T );
	SAFE_RELEASE( m_pS_Reflect_T );
	SAFE_RELEASE( m_pS_MultiLayer_T );
	SAFE_RELEASE( m_pS_LightMap_T );
	SAFE_RELEASE( m_pS_LightMap_Reflect_T );
	SAFE_RELEASE( m_pS_MultiLayer_LightMap_T );
	SAFE_RELEASE( m_pS_MultiLayer_LightMap_Reflect_T );
	SAFE_RELEASE( m_pS_MultiLayer_Reflect_T );
	SAFE_RELEASE( m_perFrame_T );
	SAFE_RELEASE( m_perObject_T );
	SAFE_DELETE( m_renderTarget );
	SAFE_DELETE( m_renderTargetCopy );
}

ID3D11ShaderResourceView * GE_DeferredGeometry::getBFNSRV()
{
	return m_bfnTexture->getTextureSRV();
}

void GE_DeferredGeometry::_render( std::list<GE_Model *> *i_modelList, GE_View *i_view )
{
	ID3D11ShaderResourceView *srv[ 17 ];

	if ( !i_modelList->size() )
		return;

	GE_Core::getSingleton().getImmediateContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	GE_Core::getSingleton().getImmediateContext()->IASetInputLayout( m_vertexDeclaration );


	m_cPerFrame.ViewProjection.transposeBy( &i_view->m_matViewProjection );
	m_cPerFrame.ViewPosition.x = i_view->m_position.x;
	m_cPerFrame.ViewPosition.y = i_view->m_position.y;
	m_cPerFrame.ViewPosition.z = i_view->m_position.z;

	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers( 0, 1, &m_perFrame );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );

	ID3D11SamplerState *samplers[2];
	samplers[0] = GE_Core::getSingleton().getRenderStates().SamplerLinearWrap;
	samplers[1] = GE_Core::getSingleton().getRenderStates().SamplerLinearClamp;
	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 2, samplers );

	uint32_t NOM, NOB, NOI;
	bool	 DoReflect, DoLightMap, DoMultiLayer;
	BOOST_FOREACH( GE_Model *model, *i_modelList )
	{
		srv[ 6 ] = model->m_morphValuesBuffer.m_sRV;
		NOM = model->m_lod == 0 ? model->m_morphValuesBuffer.m_lenght : 0;
		NOI = ( uint32_t )model->m_instanceData.size();
		srv[ 7 ] = model->m_instanceBuffer.m_sRV;

		m_cPerObject.World.transposeBy( model->getWorld() );
		m_cPerObject.WorldViewProjection.transposeBy( &( *model->getWorld() * i_view->m_matViewProjection ) );

		uint32_t subsetCount = ( uint32_t )model->getSubsets().size();

		for ( uint32_t i = 0; i < subsetCount; ++i )
		{
			GE_Model::Subset *Subset = model->getSubset( i );
			GE_Material *material = Subset->Material;
			GE_Texture *lightMap = Subset->LightMap;

			if ( !material || !Subset->Lods[ model->getLod() ] )
				continue;

			srv[ 0 ] = Subset->Lods[ model->getLod() ]->TexcoordBuffer.m_sRV;
			srv[ 1 ] = Subset->Lods[ model->getLod() ]->ColorBuffer.m_sRV;
			srv[ 2 ] = Subset->Lods[ model->getLod() ]->BoneIndexBuffer.m_sRV;
			srv[ 3 ] = Subset->Lods[ model->getLod() ]->BoneWeightBuffer.m_sRV;
			srv[ 4 ] = model->m_bonesBuffer.m_sRV;
			srv[ 5 ] = Subset->MorphTargetsBuffer.m_sRV;

			if ( material->DiffuseAlphaMap[ 0 ] )
				srv[ 8 ] = material->DiffuseAlphaMap[ 0 ]->getTextureSRV();
			else
				srv[ 8 ] = NULL;
			if ( material->DiffuseAlphaMap[ 1 ] )
				srv[ 9 ] = material->DiffuseAlphaMap[ 1 ]->getTextureSRV();
			else
				srv[ 9 ] = NULL;

			if ( material->NormalHeightMap[ 0 ] )
				srv[ 10 ] = material->NormalHeightMap[ 0 ]->getTextureSRV();
			else
				srv[ 10 ] = NULL;
			if ( material->NormalHeightMap[ 1 ] )
				srv[ 11 ] = material->NormalHeightMap[ 1 ]->getTextureSRV();
			else
				srv[ 11 ] = NULL;

			if ( material->GlowSpecularReflectMaskMap[ 0 ] )
				srv[ 12 ] = material->GlowSpecularReflectMaskMap[ 0 ]->getTextureSRV();
			else
				srv[ 12 ] = NULL;
			if ( material->GlowSpecularReflectMaskMap[ 1 ] )
				srv[ 13 ] = material->GlowSpecularReflectMaskMap[ 1 ]->getTextureSRV();
			else
				srv[ 13 ] = NULL;

			if ( lightMap )
				srv[ 14 ] = lightMap->getTextureSRV();
			else
				srv[ 14 ] = NULL;

			uint32_t reflectMips = 0;
			if ( material->ReflectMap )
			{
				srv[ 15 ] = material->ReflectMap->getTextureSRV();
				reflectMips = material->ReflectMap->getNumberOfMips();
			}
			else
				srv[ 15 ] = NULL;

			if (material->TintMap)
				srv[16] = material->TintMap->getTextureSRV();
			else
				srv[16] = NULL;

			DoReflect = ( srv[ 15 ] != NULL );
			DoMultiLayer = ( srv[ 8 ] != NULL && srv[ 9 ] != NULL && srv[ 10 ] != NULL
				&& srv[ 11 ] != NULL );
			DoLightMap = ( srv[ 14 ] != NULL && Subset->Lods[ model->getLod() ]->TexcoordBuffer.m_lenght );
			NOB = model->m_bonesBuffer.m_lenght;

			m_cPerObject.AlphaTest = material->AlphaTest;
			m_cPerObject.Bumpiness = material->Bumpiness;
			m_cPerObject.Glowness = ( material->getGlowIntensity() > 1.0f ? 1.0f : material->getGlowIntensity() );
			m_cPerObject.Roughness = material->Roughness * reflectMips;
			m_cPerObject.Reflectmask = material->Reflectmask;
			m_cPerObject.Falloff = material->Tint;
			m_cPerObject.FalloffSpread = material->TintSpread;
			m_cPerObject.MaterialID = ( float )m_numberOfMaterials / 255.0f;
			m_cPerObject.LayerMultiply = Subset->LayerMultiply;
			m_cPerObject.LayerOffset = Subset->LayerOffset;
			m_cPerObject.NumberOfMorphTargets = NOM;
			m_cPerObject.NumberOfMorphTargetVertices = Subset->Lods[ 0 ]->NumberOfVertices * 3;
			m_cPerObject.UseVertexColor = ( Subset->UseVertexColor ? 0.0f : 1.0f );
			GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perObject, 0, NULL, &m_cPerObject, 0, 0 );
			GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers( 1, 1, &m_perObject );
			GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 1, 1, &m_perObject );

			addMaterial( material );
			GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources( 1, 8, srv );
			GE_Core::getSingleton().getImmediateContext()->GSSetShaderResources( 1, 8, srv );
			GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 1, 17, srv );

			if ( !NOB && !NOM && !NOI )
			{
				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

				_setPs( DoLightMap, DoReflect, DoMultiLayer );

				model->render( i, model->getLod() );
			}
			else if ( NOB && !NOM && !NOI )
			{

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Skin, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

				_setPs( DoLightMap, DoReflect, DoMultiLayer );

				model->render( i, model->getLod() );

			}
			else if ( !NOB && NOM && !NOI )
			{

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Morph, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

				_setPs( DoLightMap, DoReflect, DoMultiLayer );

				model->render( i, model->getLod() );

			}
			else if ( !NOB && !NOM && NOI )
			{

				if ( !model->getNumberOfRenderInstances() )
					continue;
				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Instance, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

				_setPs( DoLightMap, DoReflect, DoMultiLayer );

				model->renderInstance( i, model->getLod() );

			}
			else if ( NOB && NOM && !NOI )
			{
				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_MorphSkin, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

				_setPs( DoLightMap, DoReflect, DoMultiLayer );

				model->render( i, model->getLod() );


			}
			else if ( NOB && !NOM && NOI )
			{

				if ( !model->getNumberOfRenderInstances() )
					continue;

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_SkinStreamOut, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( m_gS_StreamOut, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( NULL, NULL, 0 );
				model->renderInstanceToStreamOut( i, model->getLod() );

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Instance, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

				_setPs( DoLightMap, DoReflect, DoMultiLayer );

				model->renderInstanceByStreamOut( i, model->getLod() );

			}
			else if ( !NOB && NOM && NOI )
			{

				if ( !model->getNumberOfRenderInstances() )
					continue;

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_MorphStreamOut, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( m_gS_StreamOut, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( NULL, NULL, 0 );
				model->renderInstanceToStreamOut( i, model->getLod() );

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Instance, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

				_setPs( DoLightMap, DoReflect, DoMultiLayer );

				model->renderInstanceByStreamOut( i, model->getLod() );

			}
			else if ( NOB && NOM && NOI )
			{

				if ( !model->getNumberOfRenderInstances() )
					continue;

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_MorphSkinStreamOut, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( m_gS_StreamOut, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( NULL, NULL, 0 );
				model->renderInstanceToStreamOut( i, model->getLod() );

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Instance, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );
				_setPs( DoLightMap, DoReflect, DoMultiLayer );

				model->renderInstanceByStreamOut( i, model->getLod() );

			}
		}
	}

	for ( uint32_t i = 0; i < 17; i++ )
		srv[ i ] = NULL;

	GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources( 1, 8, srv );
	GE_Core::getSingleton().getImmediateContext()->GSSetShaderResources( 1, 8, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 1, 17, srv );
}

void GE_DeferredGeometry::_renderTransparent( std::list<GE_Model *> *i_modelList, GE_View *i_view, class GE_Light *i_light )
{
	ID3D11ShaderResourceView *srv[ 17 ];

	if ( !i_modelList->size() )
		return;

	GE_Core::getSingleton().getImmediateContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	GE_Core::getSingleton().getImmediateContext()->IASetInputLayout( m_vertexDeclaration );

	memcpy_s( &m_cPerFrameT.LightAmbient, sizeof( GE_Vec3 ), i_light->getAmbient(), sizeof( GE_Vec3 ) );
	memcpy_s( &m_cPerFrameT.LightDiffuse, sizeof( GE_Vec3 ), i_light->getDiffuse(), sizeof( GE_Vec3 ) );
	memcpy_s( &m_cPerFrameT.LightSpecular, sizeof( GE_Vec3 ), i_light->getSpecular(), sizeof( GE_Vec3 ) );
	memcpy_s( &m_cPerFrameT.LightDirection, sizeof( GE_Vec3 ), &( i_light->getParameters()->Direction * -1.0f ), sizeof( GE_Vec3 ) );
	for ( uint32_t i = 0; i < 4; i++ )
	{
		m_cPerFrameT.LightViewProjection[ i ].transposeBy( i_light->getViewProjection( i ) );
		m_cPerFrameT.ShadowOffsetBias[ i ].x = i_light->getParameters()->ShadowOffsetBias[ i ].x;
		m_cPerFrameT.ShadowOffsetBias[ i ].y = i_light->getParameters()->ShadowOffsetBias[ i ].y;
	}
	m_cPerFrameT.ViewProjection.transposeBy( &i_view->m_matViewProjection );
	m_cPerFrameT.ViewPosition.x = i_view->m_position.x;
	m_cPerFrameT.ViewPosition.y = i_view->m_position.y;
	m_cPerFrameT.ViewPosition.z = i_view->m_position.z;

	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame_T, 0, NULL, &m_cPerFrameT, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers( 0, 1, &m_perFrame_T );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame_T );

	ID3D11SamplerState *samplers[2];
	samplers[0] = GE_Core::getSingleton().getRenderStates().SamplerLinearWrap;
	samplers[1] = GE_Core::getSingleton().getRenderStates().SamplerLinearClamp;
	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers(0, 2, samplers);

	uint32_t NOM, NOB, NOI;
	bool	 DoReflect, DoLightMap, DoMultiLayer;
	BOOST_FOREACH( GE_Model *model, *i_modelList )
	{
		srv[ 6 ] = model->m_morphValuesBuffer.m_sRV;
		NOM = model->m_lod == 0 ? model->m_morphValuesBuffer.m_lenght : 0;
		NOI = ( uint32_t )model->m_instanceData.size();
		srv[ 7 ] = model->m_instanceBuffer.m_sRV;

		m_cPerObjectT.World.transposeBy( model->getWorld() );
		m_cPerObjectT.WorldViewProjection.transposeBy( &( *model->getWorld() * i_view->m_matViewProjection ) );

		uint32_t subsetCount = ( uint32_t )model->getSubsets().size();

		for ( uint32_t i = 0; i < subsetCount; ++i )
		{
			GE_Model::Subset *Subset = model->getSubset( i );
			GE_Material *material = Subset->Material;
			GE_Texture *lightMap = Subset->LightMap;

			if ( !material || !Subset->Lods[ model->getLod() ] )
				continue;

			srv[ 0 ] = Subset->Lods[ model->getLod() ]->TexcoordBuffer.m_sRV;
			srv[ 1 ] = Subset->Lods[ model->getLod() ]->ColorBuffer.m_sRV;
			srv[ 2 ] = Subset->Lods[ model->getLod() ]->BoneIndexBuffer.m_sRV;
			srv[ 3 ] = Subset->Lods[ model->getLod() ]->BoneWeightBuffer.m_sRV;
			srv[ 4 ] = model->m_bonesBuffer.m_sRV;
			srv[ 5 ] = Subset->MorphTargetsBuffer.m_sRV;

			if ( material->DiffuseAlphaMap[ 0 ] )
				srv[ 8 ] = material->DiffuseAlphaMap[ 0 ]->getTextureSRV();
			else
				srv[ 8 ] = NULL;
			if ( material->DiffuseAlphaMap[ 1 ] )
				srv[ 9 ] = material->DiffuseAlphaMap[ 1 ]->getTextureSRV();
			else
				srv[ 9 ] = NULL;

			if ( material->NormalHeightMap[ 0 ] )
				srv[ 10 ] = material->NormalHeightMap[ 0 ]->getTextureSRV();
			else
				srv[ 10 ] = NULL;
			if ( material->NormalHeightMap[ 1 ] )
				srv[ 11 ] = material->NormalHeightMap[ 1 ]->getTextureSRV();
			else
				srv[ 11 ] = NULL;

			if ( material->GlowSpecularReflectMaskMap[ 0 ] )
				srv[ 12 ] = material->GlowSpecularReflectMaskMap[ 0 ]->getTextureSRV();
			else
				srv[ 12 ] = NULL;
			if ( material->GlowSpecularReflectMaskMap[ 1 ] )
				srv[ 13 ] = material->GlowSpecularReflectMaskMap[ 1 ]->getTextureSRV();
			else
				srv[ 13 ] = NULL;

			if ( lightMap )
				srv[ 14 ] = lightMap->getTextureSRV();
			else
				srv[ 14 ] = NULL;

			uint32_t reflectMips = 0;
			if ( material->ReflectMap )
			{
				srv[ 15 ] = material->ReflectMap->getTextureSRV();
				reflectMips = material->ReflectMap->getNumberOfMips();
			}
			else
				srv[ 15 ] = NULL;

			if (material->TintMap)
				srv[16] = material->TintMap->getTextureSRV();
			else
				srv[16] = NULL;

			DoReflect = ( srv[ 15 ] != NULL );
			DoMultiLayer = ( srv[ 8 ] != NULL && srv[ 9 ] != NULL && srv[ 10 ] != NULL
				&& srv[ 11 ] != NULL );
			DoLightMap = ( srv[ 14 ] != NULL && Subset->Lods[ model->getLod() ]->TexcoordBuffer.m_lenght );
			NOB = model->m_bonesBuffer.m_lenght;

			m_cPerObjectT.Alpha = material->Alpha;
			m_cPerObjectT.SpecularPower = material->SpecularPower;
			m_cPerObjectT.Diffuse = material->Diffuse;
			m_cPerObjectT.Emissive = material->Emissive;
			m_cPerObjectT.Ambient = material->Ambient;
			m_cPerObjectT.Glow = material->Glow;
			m_cPerObjectT.Specular = material->Specular;
			m_cPerObjectT.Refract = material->Refract;
			m_cPerObjectT.Refractness = material->Refractness;
			m_cPerObjectT.Falloff = material->Tint;
			m_cPerObjectT.FalloffSpread = material->TintSpread;
			m_cPerObjectT.Bumpiness = material->Bumpiness;
			m_cPerObjectT.Glowness = ( material->getGlowIntensity() > 1.0f ? 1.0f : material->getGlowIntensity() );
			m_cPerObjectT.Roughness = reflectMips * material->Roughness;
			m_cPerObjectT.Reflectmask = material->Reflectmask;
			m_cPerObjectT.LayerMultiply = Subset->LayerMultiply;
			m_cPerObjectT.LayerOffset = Subset->LayerOffset;
			m_cPerObjectT.NumberOfMorphTargets = NOM;
			m_cPerObjectT.NumberOfMorphTargetVertices = Subset->Lods[ 0 ]->NumberOfVertices * 3;
			m_cPerObjectT.UseVertexColor = ( Subset->UseVertexColor ? 0.0f : 1.0f );
			GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perObject_T, 0, NULL, &m_cPerObjectT, 0, 0 );
			GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers( 1, 1, &m_perObject_T );
			GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 1, 1, &m_perObject_T );

			addMaterial( material );
			GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources( 1, 8, srv );
			GE_Core::getSingleton().getImmediateContext()->GSSetShaderResources( 1, 8, srv );
			GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 1, 17, srv );

			if ( !NOB && !NOM && !NOI )
			{
				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_T, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

				_setPsTransparent( DoLightMap, DoReflect, DoMultiLayer );

				model->render( i, model->getLod() );
			}
			else if ( NOB && !NOM && !NOI )
			{

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Skin_T, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

				_setPs( DoLightMap, DoReflect, DoMultiLayer );

				model->render( i, model->getLod() );

			}
			else if ( !NOB && NOM && !NOI )
			{

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Morph_T, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

				_setPsTransparent( DoLightMap, DoReflect, DoMultiLayer );

				model->render( i, model->getLod() );

			}
			else if ( !NOB && !NOM && NOI )
			{

				if ( !model->getNumberOfRenderInstances() )
					continue;
				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Instance_T, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

				_setPsTransparent( DoLightMap, DoReflect, DoMultiLayer );

				model->renderInstance( i, model->getLod() );

			}
			else if ( NOB && NOM && !NOI )
			{
				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_MorphSkin_T, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

				_setPsTransparent( DoLightMap, DoReflect, DoMultiLayer );

				model->render( i, model->getLod() );


			}
			else if ( NOB && !NOM && NOI )
			{

				if ( !model->getNumberOfRenderInstances() )
					continue;

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_SkinStreamOut_T, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( m_gS_StreamOut_T, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( NULL, NULL, 0 );
				model->renderInstanceToStreamOut( i, model->getLod() );

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Instance_T, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

				_setPsTransparent( DoLightMap, DoReflect, DoMultiLayer );

				model->renderInstanceByStreamOut( i, model->getLod() );

			}
			else if ( !NOB && NOM && NOI )
			{

				if ( !model->getNumberOfRenderInstances() )
					continue;

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_MorphStreamOut_T, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( m_gS_StreamOut_T, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( NULL, NULL, 0 );
				model->renderInstanceToStreamOut( i, model->getLod() );

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Instance_T, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );

				_setPsTransparent( DoLightMap, DoReflect, DoMultiLayer );

				model->renderInstanceByStreamOut( i, model->getLod() );

			}
			else if ( NOB && NOM && NOI )
			{

				if ( !model->getNumberOfRenderInstances() )
					continue;

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_MorphSkinStreamOut_T, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( m_gS_StreamOut_T, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->PSSetShader( NULL, NULL, 0 );
				model->renderInstanceToStreamOut( i, model->getLod() );

				GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );

				GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Instance_T, NULL, 0 );
				GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );
				_setPsTransparent( DoLightMap, DoReflect, DoMultiLayer );

				model->renderInstanceByStreamOut( i, model->getLod() );

			}
		}
	}

	for ( uint32_t i = 0; i < 17; i++ )
		srv[ i ] = NULL;

	GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources( 1, 8, srv );
	GE_Core::getSingleton().getImmediateContext()->GSSetShaderResources( 1, 8, srv );
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 1, 17, srv );
}

void GE_DeferredGeometry::_setPs( bool i_lightMap, bool i_reflect, bool i_multiLayer )
{

	if ( !i_lightMap && !i_reflect && !i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS, NULL, 0 );
	else if ( !i_lightMap && !i_reflect && i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MultiLayer, NULL, 0 );
	else if ( !i_lightMap && i_reflect && !i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Reflect, NULL, 0 );
	else if ( i_lightMap && !i_reflect && !i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_LightMap, NULL, 0 );
	else if ( i_lightMap && i_reflect && !i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_LightMap_Reflect, NULL, 0 );
	else if ( i_lightMap && !i_reflect && i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MultiLayer_LightMap, NULL, 0 );
	else if ( !i_lightMap && i_reflect && i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MultiLayer_Reflect, NULL, 0 );
	else if ( i_lightMap && i_reflect && i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MultiLayer_LightMap_Reflect, NULL, 0 );
}

void GE_DeferredGeometry::_setPsTransparent( bool i_lightMap, bool i_reflect, bool i_multiLayer )
{
	if ( !i_lightMap && !i_reflect && !i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_T, NULL, 0 );
	else if ( !i_lightMap && !i_reflect && i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MultiLayer_T, NULL, 0 );
	else if ( !i_lightMap && i_reflect && !i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Reflect_T, NULL, 0 );
	else if ( i_lightMap && !i_reflect && !i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_LightMap_T, NULL, 0 );
	else if ( i_lightMap && i_reflect && !i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_LightMap_Reflect_T, NULL, 0 );
	else if ( i_lightMap && !i_reflect && i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MultiLayer_LightMap_T, NULL, 0 );
	else if ( !i_lightMap && i_reflect && i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MultiLayer_Reflect_T, NULL, 0 );
	else if ( i_lightMap && i_reflect && i_multiLayer )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_MultiLayer_LightMap_Reflect_T, NULL, 0 );
}
