#include "GE_Sky.h"
#include "GE_Core.h"
#include "GE_Texture.h"
#include "GE_Utility.h"
#include "GE_View.h"

GE_Sky GE_Sky::m_singleton;

void GE_Sky::init()
{
	if( m_vS )
		return;
	m_vS = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_SK_VS.cso" );
	m_pS = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_SK_PS.cso" );
	m_pS_Map10 = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_SK_PS_Map10.cso" );
	m_pS_Map01 = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_SK_PS_Map01.cso" );
	m_pS_Map11 = GE_Core::getSingleton().createPS( string( GE_SHADER_PATH ) + "GE_SK_PS_Map11.cso" );
	m_perFrame = GE_Core::getSingleton().createConstBuffer( sizeof( PerFrame ) );
	GE_SphereVertex sv;
	sv.createVertices( 5 );
	m_vB = GE_Core::getSingleton().createVertexBuffer( &sv.Vertices[ 0 ], sizeof( GE_Vec3 ), ( uint32_t ) sv.Vertices.size() );
	m_iB = GE_Core::getSingleton().createIndexBuffer( &sv.Indices[ 0 ], sizeof( uint32_t ), ( uint32_t ) sv.Indices.size() );
	m_numberOfIndices = ( uint32_t ) sv.Indices.size();
	m_parameters.Sharpness = 4.0f;
	m_parameters.Highlight = 1.0f;
	m_parameters.Core = 1.0f;
	m_parameters.Height = 1.0f;
	m_parameters.Sun = 15.0f;
	m_parameters.InnerRadius = 10.0f;
	m_parameters.OuterRadius = 10.25f;
	m_parameters.Wavelength.x = 0.650f;
	m_parameters.Wavelength.y = 0.570f;
	m_parameters.Wavelength.z = 0.475f;
	m_parameters.Samples = 10;
	m_parameters.Intensity = 1.0f;
	m_parameters.Map1ScaleZ = 1.0f;
	m_parameters.Radius = GE_ConvertToUnit( 4000.0f );
	m_parameters.Map1Enable = false;
	m_parameters.Map1ScaleZ = 1.0f;
	m_parameters.Map1PositionZ = 0.0f;
	m_parameters.Map1Rotation.identity();
	m_parameters.Map2Enable = false;
	m_parameters.Map2ScaleZ = 1.0f;
	m_parameters.Map2PositionZ = 0.0f;
	m_parameters.LightDir = GE_Vec3( 0.0f, 0.0f, 0.0f );
	m_parameters.Map2Rotation.identity();
}

void GE_Sky::setMap1( const char *i_map1FileName )
{
	if( m_texture1 )
		GE_TextureManager::getSingleton().remove( &m_texture1 );
	if( i_map1FileName && i_map1FileName[ 0 ] )
		m_texture1 = GE_TextureManager::getSingleton().createTexture( i_map1FileName );
}

void GE_Sky::deleteMap1()
{
	if( m_texture1 )
		GE_TextureManager::getSingleton().remove( &m_texture1 );
}

void GE_Sky::setMap2( const char *i_map2FileName )
{
	if( m_texture2 )
		GE_TextureManager::getSingleton().remove( &m_texture2 );
	if( i_map2FileName && i_map2FileName[ 0 ] )
		m_texture2 = GE_TextureManager::getSingleton().createTexture( i_map2FileName );
}

void GE_Sky::deleteMap2()
{
	if( m_texture2 )
		GE_TextureManager::getSingleton().remove( &m_texture2 );
}


void GE_Sky::render( GE_View *i_view )
{
	ID3D11ShaderResourceView *srv[ 2 ];

	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisableZWrite, 0 );

	if( m_texture1 )
		srv[ 0 ] = m_texture1->getTextureSRV();
	else
		srv[ 0 ] = NULL;
	if( m_texture2 )
		srv[ 1 ] = m_texture2->getTextureSRV();
	else
		srv[ 1 ] = NULL;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );

	m_cPerFrame.NumberOfSamples = m_parameters.Samples;

	m_cPerFrame.InvWavelength_Intensity.x = 1.0f / pow( m_parameters.Wavelength.x, 4.0f );
	m_cPerFrame.InvWavelength_Intensity.y = 1.0f / pow( m_parameters.Wavelength.y, 4.0f );
	m_cPerFrame.InvWavelength_Intensity.z = 1.0f / pow( m_parameters.Wavelength.z, 4.0f );

	float kr = m_parameters.Sharpness * 0.0025f;
	float km = m_parameters.Highlight * 0.0010f;

	m_cPerFrame.SharpnessSun = kr * m_parameters.Sun;
	m_cPerFrame.Sharpness4PI = kr * 4.0f * GE_PI;
	m_cPerFrame.HighlightSun = km * m_parameters.Sun;
	m_cPerFrame.Highlight4PI = km * 4.0f * GE_PI;

	m_cPerFrame.InnerRadius = m_parameters.InnerRadius;
	m_cPerFrame.InnerRadius2 = m_cPerFrame.InnerRadius * m_cPerFrame.InnerRadius;
	m_cPerFrame.OuterRadius = m_parameters.OuterRadius;
	m_cPerFrame.OuterRadius2 = m_cPerFrame.OuterRadius * m_cPerFrame.OuterRadius;

	m_cPerFrame.InvScale = 1.0f / ( m_cPerFrame.OuterRadius - m_cPerFrame.InnerRadius );
	m_cPerFrame.ScaleDepth = ( m_cPerFrame.OuterRadius - m_cPerFrame.InnerRadius );
	m_cPerFrame.ScaleOverScaleDepth = m_cPerFrame.InvScale / m_cPerFrame.ScaleDepth;
	m_cPerFrame.Core = m_parameters.Core * -0.95f;
	m_cPerFrame.Core2 = m_cPerFrame.Core * m_cPerFrame.Core;

	m_cPerFrame.Map1ScaleZ = m_parameters.Map1ScaleZ;
	m_cPerFrame.Map1PositionZ = m_parameters.Map1PositionZ;
	m_parameters.Map1Rotation.transposeOut( &m_cPerFrame.Map1Rotation );

	m_cPerFrame.Map2ScaleZ = m_parameters.Map2ScaleZ;
	m_cPerFrame.Map2PositionZ = m_parameters.Map2PositionZ;
	m_parameters.Map2Rotation.transposeOut( &m_cPerFrame.Map2Rotation );

	m_cPerFrame.Scale = m_parameters.Radius;
	m_cPerFrame.InvWavelength_Intensity.w = m_parameters.Intensity;

	m_cPerFrame.LightDir.x = m_parameters.LightDir.x;
	m_cPerFrame.LightDir.y = m_parameters.LightDir.y;
	m_cPerFrame.LightDir.z = m_parameters.LightDir.z;

	GE_Vec3 vecCamera;
	vecCamera.x = 0;
	vecCamera.y = 0;
	vecCamera.z = m_parameters.InnerRadius * m_parameters.Height;

	memcpy_s( &m_cPerFrame.Position, sizeof( GE_Vec3 ), &i_view->m_position, sizeof( GE_Vec3 ) );

	m_cPerFrame.CameraPos.x = vecCamera.x;
	m_cPerFrame.CameraPos.y = vecCamera.y;
	m_cPerFrame.CameraPos.z = vecCamera.z;

	m_cPerFrame.CameraHeight = vecCamera.z;
	m_cPerFrame.CameraHeight2 = m_cPerFrame.CameraHeight * m_cPerFrame.CameraHeight;

	m_cPerFrame.ViewProjection.transposeBy( &i_view->m_matViewProjection );

	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perFrame, 0, NULL, &m_cPerFrame, 0, 0 );
	GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers( 0, 1, &m_perFrame );
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers( 0, 1, &m_perFrame );

	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers( 0, 1, &GE_Core::getSingleton().getRenderStates().SamplerLinearUWrapVBorderWhite );
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources( 0, 2, srv );

	GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );
	if( m_parameters.Map1Enable && !m_parameters.Map2Enable )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Map10, NULL, 0 );
	else if( !m_parameters.Map1Enable && m_parameters.Map2Enable )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Map01, NULL, 0 );
	else if( m_parameters.Map1Enable && m_parameters.Map2Enable )
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS_Map11, NULL, 0 );
	else
		GE_Core::getSingleton().getImmediateContext()->PSSetShader( m_pS, NULL, 0 );
	GE_Core::getSingleton().setScreenAlignQuadVertexDeclarationToContext();
	GE_Core::getSingleton().getImmediateContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	GE_Core::getSingleton().renderTriangleList( m_vB, sizeof( GE_Vec3 ), m_iB, m_numberOfIndices );

	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );
}

void GE_Sky::destroy()
{
	if( m_texture1 )
		GE_TextureManager::getSingleton().remove( &m_texture1 );
	if( m_texture2 )
		GE_TextureManager::getSingleton().remove( &m_texture2 );
	SAFE_RELEASE( m_vS );
	SAFE_RELEASE( m_pS );
	SAFE_RELEASE( m_pS_Map10 );
	SAFE_RELEASE( m_pS_Map01 );
	SAFE_RELEASE( m_pS_Map11 );
	SAFE_RELEASE( m_perFrame );
	SAFE_RELEASE( m_vB );
	SAFE_RELEASE( m_iB );
}
