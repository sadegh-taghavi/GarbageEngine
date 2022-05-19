#include "GEW_Text.h"

#include <GE_Sprite.h>
#include <GE_Utility.h>

GEW_Text::GEW_Text( std::string &i_fontGlyphFileName )
{
	m_sprite = new GE_Sprite();
	m_sprite->setDiffuseTexture( i_fontGlyphFileName.c_str() );
	m_sprite->setSize( GE_Vec2( 16.0f, 16.0f ) );
	m_charSpacing = 10.0f;
	m_lineSpacing = 13.0f;
	m_scale = GE_Vec2( 1.0f, 1.0f );
	m_position = GE_Vec2( 0, 0 );
	m_rotation = 0;
	m_change = true;
	m_visible = true;
	m_text = "";
	_GE_FileChunk fc;
	std::string fDes( i_fontGlyphFileName );
	fDes = fDes.substr( 0, fDes.length() - 3 );
	fDes += "des";
	m_charSize = m_sprite->getWidthHeight()->x / 16.0f;
	fc.load( fDes );
	for( uint32_t i = 0; i < 256; ++i )
		memcpy( &m_charWidth[ i ], &fc.Data[ i * sizeof( float ) ], sizeof( float ) );
}
GEW_Text::~GEW_Text( void )
{
	delete m_sprite;
}

void GEW_Text::update()
{
	if( !m_visible || !m_change )
		return;

	uint32_t len = ( uint32_t ) m_text.length();
	if( len > ( uint32_t ) m_sprite->getNumberOfInstances() )
		m_spriteInstances = m_sprite->createInstance( len );

	GE_Vec2 writeIndex( 0, 0 );
	uint32_t NofI = 0;
	m_tempSpriteInstances = m_spriteInstances;
	GE_Vec2 lastPos = m_position;
	for( uint32_t i = 0; i < len; ++i )
	{
		char c = m_text[ i ];

		( *m_tempSpriteInstances )->setScale( m_scale );
		( *m_tempSpriteInstances )->setRotation( m_rotation );

		if( c == '\n' )
		{
			writeIndex.y += 1.0f;
			writeIndex.x = 0.0f;
			GE_Vec2 down( cosf( m_rotation - GE_ConvertToRadians( 90.0f ) ) * m_scale.x, sinf( m_rotation - GE_ConvertToRadians( 90.0f ) ) * m_scale.y );
			down *= m_lineSpacing * writeIndex.y;
			lastPos = m_position + down;
		} else
		{
			float backWidth = 0;
			if( writeIndex.x > 0 )
				backWidth = m_charWidth[ m_text[ i - 1 ] ];

			GE_Vec2 fd( cosf( m_rotation ) * m_scale.x, sinf( m_rotation ) * m_scale.y );
			GE_Vec2 forward = fd * m_charSpacing * ( backWidth + m_charWidth[ c ] * 0.5f );
			lastPos += forward;

			( *m_tempSpriteInstances )->setPosition( lastPos );

			uint32_t y = ( uint32_t ) c / 16 * ( uint32_t ) m_charSize;
			uint32_t x = ( uint32_t ) fmod( c, 16 ) * ( uint32_t ) m_charSize;
			( *m_tempSpriteInstances )->setUvRectangle( GE_Vec4( ( float ) x, ( float ) y, ( float ) x + m_charSize, ( float ) y + m_charSize ) );
			m_tempSpriteInstances++;
			NofI++;
			writeIndex.x += 1.0f;
		}
	}

	m_sprite->setNumberOfRenderInstances( NofI );

	m_sprite->update();
	m_change = false;

}

void GEW_Text::setGlyphSRV( ID3D11ShaderResourceView *i_fontGlyphSRV )
{
	m_sprite->setDiffuseSRV( i_fontGlyphSRV );
	m_charSize = m_sprite->getWidthHeight()->x / 16.0f;
}

ID3D11ShaderResourceView * GEW_Text::getGlyphSRV()
{
	return m_sprite->getDiffuseSRV();
}

void GEW_Text::setGlyphTexture( std::string &i_filePath )
{
	m_sprite->setDiffuseTexture( i_filePath.c_str() );
	m_charSize = m_sprite->getWidthHeight()->x / 16.0f;
	_GE_FileChunk fc;
	std::string fDes( i_filePath );
	fDes += ".des";
	fc.load( fDes );
	for( uint32_t i = 0; i < 256; ++i )
		memcpy( &m_charWidth[ i ], &fc.Data[ i * sizeof( float ) ], sizeof( float ) );
}

void GEW_Text::setVisible( bool i_visible )
{
	m_visible = i_visible;
	m_sprite->setVisible( m_visible );
}

GE_Vec2 * GEW_Text::getScale()
{
	return &m_scale;
}

void GEW_Text::setScale( const GE_Vec2 &i_scale )
{
	m_scale = i_scale; m_change = true;
}

GE_Vec2 * GEW_Text::getPosition()
{
	return &m_position;
}

void GEW_Text::setPosition( const GE_Vec2 &i_position )
{
	m_position = i_position; m_change = true;
}

float & GEW_Text::getRotation()
{
	return m_rotation;
}

void GEW_Text::setRotation( float i_rotation )
{
	m_rotation = i_rotation; m_change = true;
}

bool & GEW_Text::isVisible()
{
	return m_visible;
}

ID3D11BlendState * GEW_Text::getBlendState()
{
	return m_sprite->getBlendState();
}

void GEW_Text::setBlendState( ID3D11BlendState *i_blendState )
{
	m_sprite->setBlendState( i_blendState );
}

float & GEW_Text::getShimmerness()
{
	return m_sprite->getShimmerness();
}

void GEW_Text::setShimmerness( float i_shimmerness )
{
	m_sprite->setShimmerness( i_shimmerness );
}

GE_Vec4 * GEW_Text::getDiffuse()
{
	return m_sprite->getDiffuse();
}

void GEW_Text::setDiffuse( const GE_Vec4 &i_diffuse )
{
	m_sprite->setDiffuse( i_diffuse );
}

GE_Vec3 * GEW_Text::getEmissive()
{
	return m_sprite->getEmissive();
}

void GEW_Text::setEmissive( const GE_Vec3 &i_emissive )
{
	m_sprite->setEmissive( i_emissive );
}

GE_Sprite * GEW_Text::getSprite()
{
	return m_sprite;
}

float & GEW_Text::getlineSpacing()
{
	return m_lineSpacing;
}

void GEW_Text::setlineSpacing( float i_lineSpacing )
{
	m_lineSpacing = i_lineSpacing;
}

float & GEW_Text::getCharSpacing()
{
	return m_charSpacing;
}

void GEW_Text::setCharSpacing( float i_charSpacing )
{
	m_charSpacing = i_charSpacing;
}

float * GEW_Text::getcharWidth()
{
	return m_charWidth;
}

std::string & GEW_Text::getText()
{
	return m_text;
}

void GEW_Text::setText( const char *i_text )
{
	m_text = i_text; 
	m_change = true;
}
