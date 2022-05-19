#include "GE_Decal.h"
#include "GE_Texture.h"
#include "GE_Core.h"
#include "GE_Math.h"
#include "GE_View.h"
#include <boost\foreach.hpp>
#include "GE_Utility.h"
#include "GE_BufferResource.h"
#include "GE_DeferredGeometry.h"

GE_Decal::GE_Decal()
{
	m_normalTexture = NULL;
	m_diffuseTexture = NULL;
	m_glowSpecularTexture = NULL;
	m_reflectTexture = NULL;
	m_instanceData = new _GE_BufferResource();
	m_needFill = false;
}
GE_Decal::~GE_Decal()
{
	deleteDiffuseTexture();
	deleteNormalTexture();
	deleteGlowSpecularTexture();
	deleteReflectTexture();
	delete m_instanceData;
}

void GE_Decal::setDiffuseTexture( const char *i_fileName )
{
	if( m_diffuseTexture )
		GE_TextureManager::getSingleton().remove( &m_diffuseTexture );
	m_diffuseTexture = GE_TextureManager::getSingleton().createTexture( i_fileName );
}

GE_Texture *GE_Decal::getDiffuseTexture()
{
	return m_diffuseTexture;
}

void GE_Decal::deleteDiffuseTexture()
{
	if( m_diffuseTexture )
		GE_TextureManager::getSingleton().remove( &m_diffuseTexture );
}

void GE_Decal::setNormalTexture( const char *i_fileName )
{
	if( m_normalTexture )
		GE_TextureManager::getSingleton().remove( &m_normalTexture );
	m_normalTexture = GE_TextureManager::getSingleton().createTexture( i_fileName );
}

GE_Texture *GE_Decal::getNormalTexture()
{
	return m_normalTexture;
}

void GE_Decal::deleteNormalTexture()
{
	if( m_normalTexture )
		GE_TextureManager::getSingleton().remove( &m_normalTexture );
}

void GE_Decal::setGlowSpecularTexture( const char *i_fileName )
{
	if( m_glowSpecularTexture )
		GE_TextureManager::getSingleton().remove( &m_glowSpecularTexture );
	m_glowSpecularTexture = GE_TextureManager::getSingleton().createTexture( i_fileName );
}

GE_Texture *GE_Decal::getGlowSpecularTexture()
{
	return m_glowSpecularTexture;
}

void GE_Decal::deleteGlowSpecularTexture()
{
	if( m_glowSpecularTexture )
		GE_TextureManager::getSingleton().remove( &m_glowSpecularTexture );
}

void GE_Decal::setReflectTexture( const char *i_fileName )
{
	if( m_reflectTexture )
		GE_TextureManager::getSingleton().remove( &m_reflectTexture );
	m_reflectTexture = GE_TextureManager::getSingleton().createTexture( i_fileName );
}

GE_Texture* GE_Decal::getReflectTexture()
{
	return m_reflectTexture;
}

void GE_Decal::deleteReflectTexture()
{
	if( m_reflectTexture )
		GE_TextureManager::getSingleton().remove( &m_reflectTexture );
}

void GE_Decal::update()
{
	if( m_needFill && m_instanceData->m_buffer )
	{
		m_instanceData->fill();
		m_needFill = false;
	}
}

void GE_Decal::addDecal()
{
	setNumberOfDecal( ( uint32_t ) m_parameters.size() + 1 );
}

void GE_Decal::removeDecal( uint32_t i_index )
{
	if( i_index >= m_parameters.size() )
		return;
	auto it = m_parameters.begin();
	for( uint32_t i = 0; i < i_index; ++i )
		it++;
	auto ite = it;
	ite++;
	m_parameters.erase( it, ite );
	m_instanceData->createBuffer( ( uint32_t ) m_parameters.size(), sizeof( Parameter ), m_parameters.data(), false, DXGI_FORMAT_R32G32B32A32_FLOAT, 12 );
	m_needFill = true;
}

void GE_Decal::setNumberOfDecal( uint32_t i_numberOfDecal )
{
	if ( !i_numberOfDecal )
		return;
	m_parameters.resize( i_numberOfDecal );
	m_instanceData->createBuffer( ( uint32_t ) m_parameters.size(), sizeof( Parameter ), m_parameters.data(), false, DXGI_FORMAT_R32G32B32A32_FLOAT, 12 );
	m_needFill = true;
}

vector<GE_Decal::Parameter> *GE_Decal::beginEditParameters()
{
	return &m_parameters;
}

void GE_Decal::endEditParameters()
{
	m_needFill = true;
}

//---------------------------------------------------------------------------------------------------------------------------------
GE_DecalRenderer GE_DecalRenderer::m_singleton;

void GE_DecalRenderer::init()
{
	if( m_vS )
		return;

	GE_BoxVertex bv;
	float meter = GE_ConvertToUnit( 1.0f );
	bv.createVertices( GE_Vec3( meter, meter, meter ), GE_Vec3( 0.0f, 0.0f, -meter * 0.5f ) );
	m_indicesCount = ( uint32_t ) bv.Indices.size();
	m_boxVB = GE_Core::getSingleton().createVertexBuffer( &bv.Vertices.at( 0 ), sizeof( float ) * 3, ( uint32_t ) bv.Vertices.size() );
	m_boxIB = GE_Core::getSingleton().createIndexBuffer( &bv.Indices.at( 0 ), sizeof( uint32_t ), m_indicesCount );

	m_pS_D = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_Decal_PS_D.cso" );
	m_pS_DN = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_Decal_PS_DN.cso" );
	m_pS_DM = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_Decal_PS_DM.cso" );
	m_pS_DR = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_Decal_PS_DR.cso" );
	m_pS_DNM = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_Decal_PS_DNM.cso" );
	m_pS_DNR = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_Decal_PS_DNR.cso" );
	m_pS_DMR = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_Decal_PS_DMR.cso" );
	m_pS_DNMR = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_Decal_PS_DNMR.cso" );

	m_vS = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_Decal_VS.cso" );
	m_gS = GE_Core::getSingleton().createGS( string( GE_SHADER_PATH ) + "GE_Decal_GS.cso" );

	m_perFrame = GE_Core::getSingleton().createConstBuffer( sizeof( PerFrame ) );
	m_perObject = GE_Core::getSingleton().createConstBuffer( sizeof( PerObject ) );

}
void GE_DecalRenderer::render( std::list<GE_Decal *> *i_decalList, GE_View *i_view )
{
	if( !i_decalList->size() )
		return;

	GE_DeferredGeometry::getSingleton().getRenderTarget()->setToDevice( 2 );

	ID3D11ShaderResourceView *srv[ 8 ];

	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisable, 0 );

	m_cPerFrame.ViewProjection.transposeBy( &i_view->m_matViewProjection );
	m_cPerFrame.ViewProjectionInv.transposeBy( &i_view->m_matViewProjectionInv );
	m_cPerFrame.ViewPosition = i_view->m_position;
	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );
	GE_Core::getSingleton().getImmediateContext()->GSSetConstantBuffers( 0, 1, &m_perFrame );
	srv[ 0 ] = GE_DeferredGeometry::getSingleton().getTargetSRVCopy( GE_DeferredGeometry::Depth );
	srv[ 1 ] = GE_DeferredGeometry::getSingleton().getTargetSRVCopy( GE_DeferredGeometry::NormalSpecular );
	srv[ 2 ] = GE_DeferredGeometry::getSingleton().getTargetSRVCopy( GE_DeferredGeometry::DiffuseGlow );
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 3, srv );
	GE_Core::getSingleton().setScreenAlignQuadVertexDeclarationToContext();
	GE_Core::getSingleton().getImmediateContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 1, &GE_Core::getSingleton().getRenderStates().SamplerLinearWrap );
	GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->GSSetShader( m_gS, NULL, 0 );

	BOOST_FOREACH( GE_Decal *dec, *i_decalList )
	{
		srv[ 0 ] = dec->m_instanceData->m_sRV;
		GE_Core::getSingleton().getImmediateContext()->GSSetShaderResources( 3, 1, srv );
		srv[ 0 ] = ( dec->m_diffuseTexture ? dec->m_diffuseTexture->getTextureSRV() : NULL );
		srv[ 1 ] = ( dec->m_normalTexture ? dec->m_normalTexture->getTextureSRV() : NULL );
		srv[ 2 ] = ( dec->m_glowSpecularTexture ? dec->m_glowSpecularTexture->getTextureSRV() : NULL );
		if( dec->m_reflectTexture )
		{
			srv[ 3 ] = dec->m_reflectTexture->getTextureSRV();
			m_cPerObject.NumberOfMips.x = ( float ) dec->m_reflectTexture->getNumberOfMips();
		} else
		{
			srv[ 3 ] = NULL;
			m_cPerObject.NumberOfMips.x = 0.0f;
		}

		m_cPerObject.EnvironmentPosition = GE_Vec3( 0, 0, 100.0f );
		m_cPerObject.EnvironmentRadiusInv = 1.0f / 100.0f;

		GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perObject, 0, NULL, &m_cPerObject, 0, 0 );

		GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 1, 1, &m_perObject );
		GE_Core::getSingleton().getImmediateContext()->GSSetConstantBuffers( 1, 1, &m_perObject );

		GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 4, 4, srv );
		if( !srv[ 1 ] && !srv[ 2 ] && !srv[ 3 ] )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_D, NULL, 0 );
		else if( srv[ 1 ] && !srv[ 2 ] && !srv[ 3 ] )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_DN, NULL, 0 );
		else if( !srv[ 1 ] && srv[ 2 ] && !srv[ 3 ] )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_DM, NULL, 0 );
		else if( !srv[ 1 ] && !srv[ 2 ] && srv[ 3 ] )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_DR, NULL, 0 );
		else if( srv[ 1 ] && srv[ 2 ] && !srv[ 3 ] )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_DNM, NULL, 0 );
		else if( srv[ 1 ] && !srv[ 2 ] && srv[ 3 ] )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_DNR, NULL, 0 );
		else if( !srv[ 1 ] && srv[ 2 ] && srv[ 3 ] )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_DMR, NULL, 0 );
		else if( srv[ 1 ] && srv[ 2 ] && srv[ 3 ] )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_DNMR, NULL, 0 );

		uint32_t ins = ( uint32_t ) dec->m_parameters.size();
		GE_Core::getSingleton().renderTriangleList( m_boxVB, sizeof( float ) * 3, m_boxIB, m_indicesCount, ins );
	}
	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	srv[ 2 ] = NULL;
	srv[ 3 ] = NULL;
	srv[ 4 ] = NULL;
	srv[ 5 ] = NULL;
	srv[ 6 ] = NULL;
	srv[ 7 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 8, srv );
	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );
}
void GE_DecalRenderer::destroy()
{
	SAFE_RELEASE( m_boxVB );
	SAFE_RELEASE( m_boxIB );
	SAFE_RELEASE( m_perFrame );
	SAFE_RELEASE( m_perObject );
	SAFE_RELEASE( m_vS );
	SAFE_RELEASE( m_gS );
	SAFE_RELEASE( m_pS_D );
	SAFE_RELEASE( m_pS_DN );
	SAFE_RELEASE( m_pS_DM );
	SAFE_RELEASE( m_pS_DR );
	SAFE_RELEASE( m_pS_DNM );
	SAFE_RELEASE( m_pS_DNR );
	SAFE_RELEASE( m_pS_DNMR );
	SAFE_RELEASE( m_pS_DMR );
}

GE_DecalRenderer::GE_DecalRenderer()
{
	memset( this, 0, sizeof( GE_DecalRenderer ) );
}

GE_DecalRenderer::~GE_DecalRenderer()
{
	destroy();
}

GE_Decal::Parameter::Parameter()
{
	memset( this, 0, sizeof( GE_Decal::Parameter ) );
	Color = GE_Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	Visible = 1.0f;
	MaxAngle = GE_ConvertToRadians( 180.0f );
	Bumpiness = 1.0f;
	Glowness = 0.0f;
	Reflectmask = 1.0f;
	UVTile = GE_Vec2( 1.0f, 1.0f );
	Transform.identity();
	Projection.identity();
}
