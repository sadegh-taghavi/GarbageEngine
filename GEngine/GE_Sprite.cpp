#include "GE_Sprite.h"
#include "GE_BufferResource.h"
#include "GE_Texture.h"
#include "GE_Core.h"
#include "GE_Math.h"
#include "GE_View.h"
#include <boost\foreach.hpp>

void GE_SpriteInstance::update()
{
	if( !m_transformChange )
		return;
	m_instanceData.World.transformation2D( &m_scaleCenter, m_scalingRotation, &m_scale, &m_rotationCenter, m_rotation, &m_position );
	m_sprite->m_instanceData[ m_index ].World = m_instanceData.World;
	m_transformChange = false;
	m_sprite->m_instanceNeedFill = true;
}

void GE_SpriteInstance::setLayer( float i_layer )
{
	m_sprite->m_instanceData[ m_index ].Layer = m_instanceData.Layer = i_layer;
	m_sprite->m_instanceNeedFill = true;
}

void GE_SpriteInstance::setDiffuse( const GE_Vec4 &i_diffuse )
{
	m_sprite->m_instanceData[ m_index ].Diffuse = m_instanceData.Diffuse = i_diffuse;
	m_sprite->m_instanceNeedFill = true;
}

void GE_SpriteInstance::setEmissive( const GE_Vec3 &i_emissive )
{
	m_sprite->m_instanceData[ m_index ].Emissive = m_instanceData.Emissive = i_emissive;
	m_sprite->m_instanceNeedFill = true;
}

void GE_SpriteInstance::setUvRectangle( const GE_Vec4 &i_uvRectangle )
{
	m_uvRectangle = i_uvRectangle;

	m_instanceData.UV[ 0 ].x = m_uvRectangle.x / m_sprite->m_wH.x;
	m_instanceData.UV[ 0 ].y = m_uvRectangle.y / m_sprite->m_wH.y;

	m_instanceData.UV[ 1 ].x = m_uvRectangle.x / m_sprite->m_wH.x;
	m_instanceData.UV[ 1 ].y = m_uvRectangle.w / m_sprite->m_wH.y;

	m_instanceData.UV[ 2 ].x = m_uvRectangle.z / m_sprite->m_wH.x;
	m_instanceData.UV[ 2 ].y = m_uvRectangle.y / m_sprite->m_wH.y;

	m_instanceData.UV[ 3 ].x = m_uvRectangle.z / m_sprite->m_wH.x;
	m_instanceData.UV[ 3 ].y = m_uvRectangle.w / m_sprite->m_wH.y;

	memcpy( m_sprite->m_instanceData[ m_index ].UV, m_instanceData.UV, sizeof( GE_Vec4 ) * 4 );
	m_sprite->m_instanceNeedFill = true;
}

//-----------------------------------------------------------------------------------------------------------------------------------
GE_Sprite::GE_Sprite()
{
	m_diffuseSRV = NULL;
	m_blendState = GE_Core::getSingleton().getRenderStates().BlendStateAlpha;
	m_scale = GE_Vec2( 1.0f, 1.0f );
	m_baseDiffuse = m_diffuse = GE_Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_baseEmissive = m_emissive = GE_Vec3( 0, 0, 0 );
	m_diffuseIntensity = m_emissiveIntensity = 1.0f;
	m_scaleCenter = m_rotationCenter = m_position = GE_Vec2( 0, 0 );
	m_rotation = m_layer = m_scalingRotation = m_shimmerness = 0;
	m_visible = true;
	m_instanceNeedFill = false;
	m_transformChange = true;
	m_diffuseTexture = NULL;
	m_numberOfInstances = m_numberOfRenderInstances = 0;
	m_order = 0;
	update();
}
void GE_Sprite::update()
{
	fillInstances();

	if( !m_transformChange )
		return;
	m_world.transformation2D( &m_scaleCenter, m_scalingRotation, &m_scale, &m_rotationCenter, m_rotation, &m_position );
	m_transformChange = false;
}

std::list<GE_SpriteInstance *>::iterator GE_Sprite::createInstance( uint32_t i_count )
{
	removeAllInstances();
	for( uint32_t i = 0; i < i_count; ++i )
	{
		GE_SpriteInstance *instance = new GE_SpriteInstance();
		instance->m_sprite = this;
		instance->m_instanceData.Diffuse = m_diffuse;
		instance->m_instanceData.Emissive = m_emissive;
		instance->m_instanceData.Layer = m_layer;
		instance->m_position = m_position;
		instance->m_rotation = m_rotation;
		instance->m_scale = m_scale;
		instance->m_rotationCenter = m_rotationCenter;
		instance->m_scaleCenter = m_scaleCenter;
		instance->m_scalingRotation = m_scalingRotation;
		instance->m_instanceData.World = m_world;
		instance->m_transformChange = true;
		instance->m_index = ( uint32_t ) m_instancesList.size();
		m_instancesList.push_back( instance );
		m_instanceData.push_back( instance->m_instanceData );
		instance->update();
		instance->setUvRectangle( m_uvRectangle );
	}
	list<GE_SpriteInstance *>::iterator it = m_instancesList.end();
	for( ; i_count; i_count-- )
		it--;

	m_numberOfRenderInstances = m_numberOfInstances = ( uint32_t ) m_instancesList.size();
	m_instanceBuffer.createBuffer( m_numberOfInstances, sizeof( GE_SpriteInstance::Data ), m_instanceData.data(), false, DXGI_FORMAT_R32G32B32A32_FLOAT, 10 );
	m_instanceNeedFill = true;

	return it;
}
void GE_Sprite::removeInstance( GE_SpriteInstance **i_instance )
{
	int i = 0;
	BOOST_FOREACH( GE_SpriteInstance *instance, m_instancesList )
	{
		if( ( *i_instance ) == instance )
		{
			m_instancesList.remove( instance );
			SAFE_DELETE( instance );
			break;
		}
		i++;
	}

	m_instanceData.clear();
	i = 0;
	BOOST_FOREACH( GE_SpriteInstance *instance, m_instancesList )
	{
		instance->m_index = i;
		m_instanceData.push_back( instance->m_instanceData );
		i++;
	}

	m_numberOfRenderInstances = m_numberOfInstances = ( uint32_t ) m_instancesList.size();
	if( m_numberOfInstances )
	{
		m_instanceBuffer.createBuffer( m_numberOfInstances, sizeof( GE_SpriteInstance::Data ), m_instanceData.data(), false, DXGI_FORMAT_R32G32B32A32_FLOAT, 10 );
		m_instanceNeedFill = false;
	} else
		m_instanceBuffer.release();
}
void GE_Sprite::removeAllInstances()
{
	BOOST_FOREACH( GE_SpriteInstance *instance, m_instancesList )
		SAFE_DELETE( instance );
	m_instanceData.clear();
	m_instancesList.clear();
	m_instanceBuffer.release();
}
void GE_Sprite::fillInstances()
{
	if( !m_instanceNeedFill || !m_instanceBuffer.m_lenght )
		return;
	BOOST_FOREACH( GE_SpriteInstance *instance, m_instancesList )
		instance->update();
	m_instanceBuffer.fill();
	m_instanceNeedFill = false;
}

GE_Sprite::~GE_Sprite()
{
	if( m_diffuseTexture )
		GE_TextureManager::getSingleton().remove( &m_diffuseTexture );
	removeAllInstances();
}

void GE_Sprite::setNumberOfRenderInstances( uint32_t i_numberOfRenderInstances )
{
	if( i_numberOfRenderInstances > m_numberOfInstances )
		return;
	m_numberOfRenderInstances = i_numberOfRenderInstances;
}

void GE_Sprite::setDiffuseSRV( ID3D11ShaderResourceView *i_diffuseSRV )
{
	m_diffuseSRV = i_diffuseSRV;
	if( !m_diffuseSRV )
		return;
	ID3D11Resource *res;
	m_diffuseSRV->GetResource( &res );
	D3D11_TEXTURE2D_DESC desc;
	( ( ID3D11Texture2D * ) res )->GetDesc( &desc );
	m_size.x = ( float ) desc.Width;
	m_size.y = ( float ) desc.Height;
	m_wH = m_size;
	m_uvRectangle = GE_Vec4( 0, 0, m_size.x, m_size.y );
	SAFE_RELEASE( res );
}

ID3D11ShaderResourceView * GE_Sprite::getDiffuseSRV()
{
	if( m_diffuseSRV )
		return m_diffuseSRV;
	if( m_diffuseTexture )
		return m_diffuseTexture->getTextureSRV();
	return NULL;
}

void GE_Sprite::setDiffuseTexture( const char *i_fileName )
{
	if( m_diffuseTexture )
		GE_TextureManager::getSingleton().remove( &m_diffuseTexture );
	m_diffuseTexture = GE_TextureManager::getSingleton().createTexture( i_fileName );
	if( !m_diffuseSRV )
	{
		ID3D11Resource *res;
		m_diffuseTexture->getTextureSRV()->GetResource( &res );
		D3D11_TEXTURE2D_DESC desc;
		( ( ID3D11Texture2D * ) res )->GetDesc( &desc );
		m_size.x = ( float ) desc.Width;
		m_size.y = ( float ) desc.Height;
		m_wH = m_size;
		m_uvRectangle = GE_Vec4( 0, 0, m_size.x, m_size.y );
		SAFE_RELEASE( res );
	}
}

void GE_Sprite::deleteDiffuseTexture()
{
	if( m_diffuseTexture )
		GE_TextureManager::getSingleton().remove( &m_diffuseTexture );
}

//----------------------------------------------------------------------------------------------------------------------------------
GE_SpriteRenderer GE_SpriteRenderer::m_singleton;

void GE_SpriteRenderer::init()
{
	if( m_vS )
		return;

	m_vS = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_S_VS.cso" );
	m_vS_Instance = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_S_VS_Instance.cso" );
	m_pS = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_S_PS.cso" );
	m_pS_Shimmer = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_S_PS_Shimmer.cso" );

	m_perObject = GE_Core::getSingleton().createConstBuffer( sizeof( PerObject ) );

}
void GE_SpriteRenderer::render( std::list<GE_Sprite *> *i_spriteList, ID3D11ShaderResourceView *i_backBufferMAP )
{
	ID3D11ShaderResourceView *srv[ 3 ];

	if( !i_spriteList->size() )
		return;

	srv[ 0 ] = i_backBufferMAP;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 1, 1, srv );

	BOOST_FOREACH( GE_Sprite *spr, *i_spriteList )
	{

		GE_Core::getSingleton().getImmediateContext()->OMSetBlendState( spr->getBlendState(), 0, 0xffffffff );

		m_cPerObject.World.transposeBy( &spr->m_world );
		m_cPerObject.Projection.transposeBy( &m_projection );
		m_cPerObject.Diffuse = *spr->getDiffuse();
		m_cPerObject.Emissive_Layer = GE_Vec4( spr->getEmissive()->x, spr->getEmissive()->y, spr->getEmissive()->z, spr->getLayer() );
		m_cPerObject.Size_Shimmerness.x = spr->getSize()->x;
		m_cPerObject.Size_Shimmerness.y = spr->getSize()->y;
		m_cPerObject.Size_Shimmerness.z = spr->getShimmerness();
		GE_Vec4 *tRect = spr->getUvRectangle();

		m_cPerObject.UVRectangle[ 0 ].x = tRect->x / spr->m_wH.x;
		m_cPerObject.UVRectangle[ 0 ].y = tRect->y / spr->m_wH.y;

		m_cPerObject.UVRectangle[ 1 ].x = tRect->x / spr->m_wH.x;
		m_cPerObject.UVRectangle[ 1 ].y = tRect->w / spr->m_wH.y;

		m_cPerObject.UVRectangle[ 2 ].x = tRect->z / spr->m_wH.x;
		m_cPerObject.UVRectangle[ 2 ].y = tRect->y / spr->m_wH.y;

		m_cPerObject.UVRectangle[ 3 ].x = tRect->z / spr->m_wH.x;
		m_cPerObject.UVRectangle[ 3 ].y = tRect->w / spr->m_wH.y;

		GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perObject, 0, NULL, &m_cPerObject, 0, 0 );
		//GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers(0,1,&m_perObject);
		GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers( 0, 1, &m_perObject );

		srv[ 0 ] = spr->getDiffuseSRV();
		GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 1, srv );

		if( m_cPerObject.Size_Shimmerness.z )
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Shimmer, NULL, 0 );
		else
			GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS, NULL, 0 );

		if( spr->getNumberOfInstances() )
		{
			srv[ 0 ] = spr->getInstanceBuffer()->m_sRV;
			GE_Core::getSingleton().getImmediateContext()->VSSetShaderResources( 2, 1, srv );
			GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS_Instance, NULL, 0 );
			GE_Core::getSingleton().renderScreenAlignQuad( spr->getNumberOfRenderInstances() );
		} else
		{
			GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );
			GE_Core::getSingleton().renderScreenAlignQuad();
		}
	}

	srv[ 0 ] = NULL;
	srv[ 1 ] = NULL;
	srv[ 2 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 3, srv );
}
void GE_SpriteRenderer::destroy()
{
	SAFE_RELEASE( m_perObject );
	SAFE_RELEASE( m_vS );
	SAFE_RELEASE( m_vS_Instance );
	SAFE_RELEASE( m_pS );
	SAFE_RELEASE( m_pS_Shimmer );
}

GE_SpriteRenderer::GE_SpriteRenderer()
{
	memset( this, 0, sizeof( GE_SpriteRenderer ) );
}

GE_SpriteRenderer::~GE_SpriteRenderer()
{
	destroy();
}

void GE_SpriteRenderer::begin()
{
	m_projection.orthoRH( GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Width, GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Height, 0, 5.0f );
	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisableZWrite, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 1, &GE_Core::getSingleton().getRenderStates().SamplerLinearClamp );
	GE_Core::getSingleton().setScreenAlignQuadInputLayoutToContext();

}

void GE_SpriteRenderer::end()
{
	GE_Core::getSingleton().getImmediateContext()->OMSetBlendState( GE_Core::getSingleton().getRenderStates().BlendStateDisableAlpha, 0, 0xffffffff );
	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );
}
