#include "GE_Light.h"
#include "GE_Math.h"
#include "GE_Texture.h"
#include "GE_RenderTarget.h"
#include "GE_Core.h"
#include "GE_Lens.h"
#include "GE_View.h"

GE_Light::GE_Light( const Parameters &i_parameters )
{
	m_cubeFilterRotate.identity();
	m_filterTexture = NULL;
	m_filterSRV = NULL;
	m_parameters = i_parameters;
	for ( uint32_t i = 0; i < LIGHT_PARAM_SIZE; i++ )
		m_shadowMap[ i ] = NULL;
	createShadowMap();
	m_pointTargets[ 0 ] = GE_Vec3( -1.0f, 0, 0 );
	m_pointTargets[ 1 ] = GE_Vec3( 1.0f, 0, 0 );
	m_pointTargets[ 2 ] = GE_Vec3( 0, -1.0f, 0 );
	m_pointTargets[ 3 ] = GE_Vec3( 0, 1.0f, 0 );
	m_pointTargets[ 4 ] = GE_Vec3( 0, 0, -1.0f );
	m_pointTargets[ 5 ] = GE_Vec3( 0, 0, 1.0f );

	m_linkToView = NULL;
}

void GE_Light::setFilterMap( const char *i_fileName )
{
	if ( !i_fileName )
		return;
	if ( !strlen( i_fileName ) )
		return;
	if ( m_filterTexture )
		GE_TextureManager::getSingleton().remove( &m_filterTexture );
	m_filterTexture = GE_TextureManager::getSingleton().createTexture( i_fileName );
}

ID3D11ShaderResourceView *GE_Light::getFilterSRV()
{
	if ( m_filterSRV )
		return m_filterSRV;
	if ( m_filterTexture )
		return m_filterTexture->getTextureSRV();
	return NULL;
}
void GE_Light::update()
{
	switch ( m_parameters.LightType )
	{
	case Directional:
	{

						if ( !m_parameters.TargetMode )
						{
							if ( m_linkToView )
								m_parameters.Position = m_linkToView->m_position;
							m_parameters.Target = GE_Vec3( cosf( m_parameters.Pitch ) * cosf( m_parameters.Yaw ) * 10.0f,
								sinf( m_parameters.Pitch ) * 10.0f,
								sinf( m_parameters.Yaw ) * cosf( m_parameters.Pitch ) * 10.0f ) + m_parameters.Position;
						}
						m_parameters.Direction = m_parameters.Target - m_parameters.Position;
						m_parameters.Direction.normalize();

						if ( !m_parameters.TargetMode )
						{
							if ( m_linkToView )
								m_parameters.Position = m_parameters.Direction * -m_parameters.DirectionalTop + m_linkToView->m_position;
							else
								m_parameters.Position = m_parameters.Direction * -m_parameters.DirectionalTop;
						}

						if ( m_parameters.HasShadow && m_parameters.DoShadow )
						{
							GE_Vec3 lookAt;
							if ( m_parameters.Position == m_parameters.Target )
								m_parameters.Target.x += 0.001f;
							if ( abs( m_parameters.Direction.z ) == 1.0f )
							{
								lookAt.x = 0.0f;
								lookAt.y = 1.0f;
								lookAt.z = 0.0f;
							}
							else
							{
								lookAt.x = 0.0f;
								lookAt.y = 0.0f;
								lookAt.z = 1.0f;
							}

							float lastWidth = m_parameters.ShadowFrustum[ 0 ].x * 0.5f;
							for ( uint32_t i = 0; i < m_parameters.NumberOfCascade && m_linkToView; i++ )
							{
								GE_Vec3 offset = m_linkToView->m_target - m_linkToView->m_position;
								offset.normalize();
								GE_Vec2 offset2D = GE_Vec2( offset.x, offset.y );
								offset2D.normalize();
								offset.x = offset2D.x;
								offset.y = offset2D.y;
								offset.z = 0;
								offset *= lastWidth;
								lastWidth += m_parameters.ShadowFrustum[ i ].x;
								m_view[ i ].lookAtRH( &( m_parameters.Position + offset ), &( m_parameters.Target + offset ), &lookAt );

								m_projection[ i ].orthoRH( m_parameters.ShadowFrustum[ i ].x,
									m_parameters.ShadowFrustum[ i ].y,
									m_parameters.ShadowFrustum[ i ].z,
									m_parameters.ShadowFrustum[ i ].w );
								GE_Mat4x4Multiply( &m_viewProjection[ i ], &m_view[ i ], &m_projection[ i ] );

								GE_Vec3 org = GE_Vec3( 0.0f, 0.0f, 0.0f );
								float shadowSize = m_parameters.ShadowMapSize[ i ].x * 0.5f;
								org.transform( &m_viewProjection[ i ] );
								org *= shadowSize;
								GE_Vec3 roundOrg = GE_Vec3( ( float )round( org.x ), ( float )round( org.y ), 0.0f );
								GE_Vec3 roundPos = roundOrg - org;
								roundPos /= shadowSize;
								roundPos.z = roundOrg.z;
								GE_Mat4x4 matTrans;
								matTrans.transform( roundPos );
								m_viewProjection[ i ] *= matTrans;
							}
						}
						break;
	}
	case Spot:
	{
				 if ( !m_parameters.TargetMode )
					 m_parameters.Target = GE_Vec3( cosf( m_parameters.Pitch ) * cosf( m_parameters.Yaw ) * 10.0f,
					 sinf( m_parameters.Pitch ) * 10.0f,
					 sinf( m_parameters.Yaw ) * cosf( m_parameters.Pitch ) * 10.0f ) + m_parameters.Position;
				 GE_Quat qt;
				 qt.identity();
				 m_world.srp( m_parameters.Position, qt, GE_Vec3( m_parameters.Range, m_parameters.Range, m_parameters.Range ) );
				 if ( m_parameters.Position == m_parameters.Target )
					 m_parameters.Target.x += 0.001f;
				 if ( abs( m_parameters.Target.z ) == 1.0f )
					 m_view[ 0 ].lookAtRH( &m_parameters.Position, &m_parameters.Target, &GE_Vec3( 0, 0, 1.0f ) );
				 else
					 m_view[ 0 ].lookAtRH( &m_parameters.Position, &m_parameters.Target, &GE_Vec3( 0, 1.0f, 0 ) );
				 m_projection[ 0 ].perspectiveFovRH( m_parameters.SpotSpread, 1.0f, GE_ConvertToUnit( 0.01f ), m_parameters.Range );
				 GE_Mat4x4Multiply( &m_viewProjection[ 0 ], &m_view[ 0 ], &m_projection[ 0 ] );

				 m_parameters.Direction = m_parameters.Target - m_parameters.Position;
				 m_parameters.Direction.normalize();
				 break;
	}
	case Point:
	{
				  GE_Quat qt;
				  qt.identity();
				  m_world.srp( m_parameters.Position, qt, GE_Vec3( m_parameters.Range, m_parameters.Range, m_parameters.Range ) );
				  for ( uint32_t i = 0; i < LIGHT_PARAM_SIZE; i++ )
				  {
					  GE_Vec3 lookAt = GE_Vec3( 0, 0, 1.0f );
					  if ( lookAt == m_pointTargets[ i ] || -m_pointTargets[ i ] == lookAt )
						  lookAt = GE_Vec3( 0.0f, 1.0f, 0.0f );

					  m_view[ i ].lookAtRH( &m_parameters.Position, &( m_pointTargets[ i ] + m_parameters.Position ), &lookAt );
					  m_projection[ i ].perspectiveFovRH( GE_ConvertToRadians( 91.0f ), 1.0f, GE_ConvertToUnit( 0.01f ), m_parameters.Range );
					  GE_Mat4x4Multiply( &m_viewProjection[ i ], &m_view[ i ], &m_projection[ i ] );
				  }
				  break;
	}
	}
}

GE_Light::~GE_Light()
{
	if ( m_filterTexture )
		GE_TextureManager::getSingleton().remove( &m_filterTexture );
	for ( int i = 0; i < LIGHT_PARAM_SIZE; i++ )
		SAFE_DELETE( m_shadowMap[ i ] );
}

void GE_Light::createShadowMap()
{
	if ( !m_parameters.HasShadow )
		return;

	for ( int i = 0; i < LIGHT_PARAM_SIZE; i++ )
		SAFE_DELETE( m_shadowMap[ i ] );

	if ( m_parameters.LightType == GE_Light::Spot )
	{
		GE_RenderTarget::Parameters rtp( 1 );
		rtp.Format[ 0 ] = DXGI_FORMAT_R8_UNORM;
		rtp.Width[ 0 ] = ( uint32_t )m_parameters.ShadowMapSize[ 0 ].x;
		rtp.Height[ 0 ] = ( uint32_t )m_parameters.ShadowMapSize[ 0 ].y;
		m_shadowMap[ 0 ] = new GE_RenderTarget( rtp );
	}
	else if ( m_parameters.LightType == GE_Light::Point )
	{
		GE_RenderTarget::Parameters rtp( 1 );
		rtp.Format[ 0 ] = DXGI_FORMAT_R8_UNORM;
		rtp.Width[ 0 ] = ( uint32_t )m_parameters.ShadowMapSize[ 0 ].x;
		rtp.Height[ 0 ] = ( uint32_t )m_parameters.ShadowMapSize[ 0 ].y;
		for ( int i = 0; i < LIGHT_PARAM_SIZE; i++ )
		{
			m_shadowMap[ i ] = new GE_RenderTarget( rtp );
		}
	}
	else
	{
		GE_RenderTarget::Parameters rtp( 1 );
		for ( uint32_t i = 0; i < m_parameters.NumberOfCascade; i++ )
		{
			rtp.Format[ 0 ] = DXGI_FORMAT_R8_UNORM;
			rtp.Width[ 0 ] = ( uint32_t )m_parameters.ShadowMapSize[ i ].x;
			rtp.Height[ 0 ] = ( uint32_t )m_parameters.ShadowMapSize[ i ].y;
			m_shadowMap[ i ] = new GE_RenderTarget( rtp );
		}
	}
}

void GE_Light::setRotateCubeFilter( const GE_Quat &i_rotate )
{
	m_parameters.RotateCubeFilter = i_rotate;
	m_cubeFilterRotate.RotationQuaternion( &m_parameters.RotateCubeFilter );
}

void GE_Light::setTarget( const GE_Vec3 &i_target )
{
	if ( m_parameters.TargetMode )
		m_parameters.Target = i_target;
}

void GE_Light::deleteFilterMap()
{
	if ( m_filterTexture )
		GE_TextureManager::getSingleton().remove( &m_filterTexture );
}

GE_Light::Parameters::Parameters()
{

	Dynamic = true;
	Enable = true;
	VisualizeCascades = false;
	HasShadow = false;
	LightType = Directional;
	NumberOfCascade = 2;
	DirectionalTop = 1000.0f;
	for ( uint32_t i = 0; i < LIGHT_PARAM_SIZE; i++ )
	{
		ShadowMapSize[ i ] = GE_Vec2( 256.0f, 256.0f );
		ShadowOffsetBias[ i ] = GE_Vec2( GE_ConvertToUnit( 0.2f ), 0.0f );
		ShadowFrustum[ i ] = GE_Vec4( GE_ConvertToUnit( 10.0f * ( i + 1 ) ),
			GE_ConvertToUnit( 5.0f * ( i + 1 ) ),
			GE_ConvertToUnit( 1.0f ),
			GE_ConvertToUnit( 1000.0f ) );
	}
	Intensity = 1.0f;
	TargetMode = false;
	Yaw = GE_ConvertToRadians( -45.0f );
	Pitch = 0.0f;
	RotateCubeFilter.identity();
	Attenuation = 0.1f;
	Range = GE_ConvertToUnit( 10.0f );
	SpotSpread = GE_ConvertToRadians( 60.0f );
	SpotSharpness = GE_ConvertToRadians( 1.0f );
	Position = GE_Vec3( 0.0f, 0.0f, 0.0f );
	Diffuse = GE_Vec3( 1.0f, 1.0f, 1.0f );
	Specular = GE_Vec3( 1.0f, 1.0f, 1.0f );
	Ambient = GE_Vec3( 0.3f, 0.3f, 0.3f );
	Target = GE_Vec3( 0.0f, -5.0f, -10.0f );
	DoShadow = true;
	NumberOfCascade = 1;
	Direction = GE_Vec3( 0.0f, 1.0f, 0.0f );
}
