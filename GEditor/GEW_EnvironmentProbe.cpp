#include "GEW_EnvironmentProbe.h"
#include <GE_Texture.h>
#include <GE_EnvironmentProbe.h>
#include <GE_RenderUtility.h>

GEW_EnvironmentProbe::GEW_EnvironmentProbe()
{
	m_map = NULL;
	m_probe = new GE_EnvironmentProbe();
	m_bound = new GE_RenderUtilityObject();
	m_bound->createSphere( 1.0f, GE_Vec3( 0.0f, 0.0f, 0.0f ), 2, 2 );
	m_radius = GE_ConvertToUnit( 50.0f );
	m_bound->setScale( GE_Vec3( m_radius, m_radius, m_radius ) );
	m_boundColor = GE_Vec3( ( float )( rand( ) % 128 + 128 ) / 255.0f, ( float )( rand( ) % 128 + 128 ) / 255.0f, ( float )( rand( ) % 128 + 128 ) / 255.0f );
}

GEW_EnvironmentProbe::~GEW_EnvironmentProbe()
{
	GE_TextureManager::getSingleton().remove( &m_map );
	delete m_probe;
	delete m_bound;
}

void GEW_EnvironmentProbe::update()
{
	//m_bound->setPosition( m_probe->getPosition() );
	if ( m_probe->isEnable() )
		m_bound->setColor( GE_Vec3( 0.0f, 0.0f, 0.0f ) );
	else
		m_bound->setColor( m_boundColor );
	m_bound->update();
}
