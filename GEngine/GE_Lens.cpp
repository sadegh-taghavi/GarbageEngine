#include "GE_Lens.h"
#include "GE_Sprite.h"
#include "GE_Math.h"
#include "GE_Core.h"
#include "GE_Occluder.h"
#include "GE_View.h"
#include <boost\foreach.hpp>

using namespace std;

void GE_LensFlare::setBlendState( ID3D11BlendState *i_blendState )
{
	m_sprite->setBlendState( i_blendState );
}
ID3D11BlendState *GE_LensFlare::getBlendState()
{
	return m_sprite->getBlendState();
}
GE_LensFlare::GE_LensFlare()
{
	m_visiblePixelOffset = 10;
	m_visibleAngleOffset = GE_ConvertToRadians( 180.0f );
	m_visible = true;
	m_sprite = new GE_Sprite();
	m_sprite->setBlendState( GE_Core::getSingleton().getRenderStates().BlendStateAlphaOne );
	m_visible = true;
	m_occluder = new GE_Occluder();
	m_lightPositionOffset = GE_Vec3( 0.0f, 0.0f, 0.0f );
	m_lightPosition = GE_Vec3( 0.0f, 0.0f, 0.0f );
}
void GE_LensFlare::setNumberOfFlare( uint32_t i_numberOfFlare )
{
	if ( !i_numberOfFlare )
		return;
	m_spriteInstances = m_sprite->createInstance( i_numberOfFlare );
	m_parameters.resize( i_numberOfFlare );
}
void GE_LensFlare::setTexture( const char *i_filename )
{
	m_sprite->setDiffuseTexture( i_filename );
}
void GE_LensFlare::setSRV( ID3D11ShaderResourceView *i_srv )
{
	m_sprite->setDiffuseSRV( i_srv );
}
GE_LensFlare::~GE_LensFlare( void )
{
	SAFE_DELETE( m_sprite );
	SAFE_DELETE( m_occluder );
}

void GE_LensFlare::update( const GE_View *i_view )
{
	if( !m_visible )
		return;
	GE_Vec3 vecPos = ( ( GE_View * ) i_view )->m_position - m_lightPosition;
	float invViewDistance = 1.0f / ( vecPos.length() + 1.0f ) * 100.0f;
	vecPos.normalize();

	m_occluder->Position = m_lightPosition + m_lightPositionOffset;
	m_occluder->update();

	GE_Vec3 dirOffset = GE_Vec3( 0.0f, 0.0f, 1.0f );
	GE_Mat4x4 tr;
	tr.RotationQuaternion( &m_occluder->Rotation );
	dirOffset.transform( &tr );

	float width = GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Width;
	float height = GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Height;

	float h = width / height;

	GE_Vec4 _2dpos;
	_2dpos.To2DLeftUpPosition( m_lightPosition, i_view->m_matViewProjection );

	if( m_occluder->NumberOfPixels < m_visiblePixelOffset || acos( vecPos.dot( &dirOffset ) ) > m_visibleAngleOffset )
	{
		m_sprite->setVisible( false );
		return;
	} else
		m_sprite->setVisible( m_visible );

	GE_Vec2 dir = GE_Vec2( 0.5f, 0.5f ) - GE_Vec2( _2dpos.x, _2dpos.y );
	float len = dir.length();
	dir.normalize();
	GE_Vec2 vpos;

	m_tempSpriteInstances = m_spriteInstances;
	int i = 0;
	BOOST_FOREACH( Parameter &param, m_parameters )
	{
		( *m_tempSpriteInstances )->setDiffuse( param.Diffuse * param.DiffuseIntensity );
		( *m_tempSpriteInstances )->setEmissive( param.Emissive * param.EmissiveIntensity );
		( *m_tempSpriteInstances )->setLayer( param.Layer );

		vpos.x = ( _2dpos.x + dir.x * param.PositionMultiplier * len ) * width;

		vpos.x -= 0.5f * width;
		vpos.y = 0.5f * height;

		vpos.y -= ( _2dpos.y + dir.y * param.PositionMultiplier * len ) * height;

		if( param.AutoRotation )
			( *m_tempSpriteInstances )->setRotation( -atan2( dir.y, dir.x ) + param.Rotation );
		else
			( *m_tempSpriteInstances )->setRotation( param.Rotation );

		if( param.AutoScale )
			( *m_tempSpriteInstances )->setScale( GE_Vec2( param.ScaleMultiplier.x * invViewDistance, param.ScaleMultiplier.y * invViewDistance ) );
		else
			( *m_tempSpriteInstances )->setScale( GE_Vec2( param.ScaleMultiplier.x, param.ScaleMultiplier.y ) );

		( *m_tempSpriteInstances )->setPosition( vpos );


		m_tempSpriteInstances++;
		i++;
	}
	m_sprite->update();
}

void GE_LensFlare::setVisible( bool i_visible )
{
	m_visible = i_visible;
	m_sprite->setVisible( m_visible );
}

void GE_LensFlare::deleteTexture()
{
	m_sprite->deleteDiffuseTexture();
}

void GE_LensFlare::addFlare()
{
	setNumberOfFlare( ( uint32_t ) m_parameters.size() + 1 );
}

void GE_LensFlare::removeFlare( uint32_t i_index )
{
	if( i_index >= m_parameters.size() )
		return;
	vector< GE_LensFlare::Parameter >::iterator it = m_parameters.begin();
	for( uint32_t i = 0; i < i_index; i++ )
		it++;
	vector< GE_LensFlare::Parameter >::iterator ite = it;
	ite++;
	m_parameters.erase( it, ite );
	m_spriteInstances = m_sprite->createInstance( ( uint32_t ) m_parameters.size() );
}

GE_Occluder * GE_LensFlare::getOccluder()
{
	return m_occluder;
}

void GE_LensFlare::setOccluderPositionOffset( const GE_Vec3 &i_offset )
{
	m_lightPositionOffset = i_offset;
}

GE_Vec3 * GE_LensFlare::getOccluderPositionOffset()
{
	return &m_lightPositionOffset;
}

void GE_LensFlare::setOccluderSize( const GE_Vec3 &i_size )
{
	m_occluder->Size = i_size;
}

GE_Vec3 * GE_LensFlare::getOccluderSize()
{
	return &m_occluder->Size;
}

void GE_LensFlare::setOccluderRotation( const GE_Quat &i_rotation )
{
	m_occluder->Rotation = i_rotation;
}

GE_Quat * GE_LensFlare::getOccluderRotation()
{
	return &m_occluder->Rotation;
}

GE_Texture * GE_LensFlare::getTexture()
{
	return m_sprite->getDiffuseTexture();
}

GE_LensFlare::Parameter::Parameter()
{
	Diffuse = GE_Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	Emissive = GE_Vec3( 0, 0, 0 );
	EmissiveIntensity = 1.0f;
	DiffuseIntensity = 1.0f;
	ScaleMultiplier = GE_Vec2( 1.0f, 1.0f );
	Rotation = 0;
	Layer = 0;
	PositionMultiplier = 0.0f;
	AutoRotation = true;
	AutoScale = false;
}
