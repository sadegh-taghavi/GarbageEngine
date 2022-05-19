#pragma once

#include <GE_Math.h>
#include <iostream>
#include <list>

class GE_Sprite;
class GE_SpriteInstance;
struct ID3D11ShaderResourceView;
struct ID3D11BlendState;

class GEW_Text
{
	GE_Sprite							*m_sprite;
	GE_Vec2								m_position;
	GE_Vec2								m_scale;
	float								m_rotation;
	float								m_charSize;
	float								m_charWidth[ 256 ];
	bool								m_change;
	bool								m_visible;
	float								m_charSpacing;
	float								m_lineSpacing;
	std::string							m_text;
	std::list<GE_SpriteInstance *>::iterator m_spriteInstances;
	std::list<GE_SpriteInstance *>::iterator m_tempSpriteInstances;
public:
	GEW_Text( std::string &i_fontGlyphFileName );
	~GEW_Text( void );

	void setText( const char *i_text );
	std::string &getText();
	float *getcharWidth();
	void setCharSpacing( float i_charSpacing );
	float &getCharSpacing();
	void setlineSpacing( float i_lineSpacing );
	float &getlineSpacing();

	void setGlyphSRV( ID3D11ShaderResourceView *i_fontGlyphSRV );
	ID3D11ShaderResourceView *getGlyphSRV();
	void setGlyphTexture( std::string &i_filePath );
	GE_Sprite *getSprite();
	void setEmissive( const GE_Vec3 &i_emissive );
	GE_Vec3 *getEmissive();
	void setDiffuse( const GE_Vec4 &i_diffuse );
	GE_Vec4 *getDiffuse();

	void setShimmerness( float i_shimmerness );
	float &getShimmerness();

	void setBlendState( ID3D11BlendState *i_blendState );
	ID3D11BlendState *getBlendState();

	void setVisible( bool i_visible );
	bool &isVisible();

	void setRotation( float i_rotation );
	float &getRotation();

	void setPosition( const GE_Vec2 &i_position );
	GE_Vec2 *getPosition();

	void setScale( const GE_Vec2 &i_scale );
	GE_Vec2 *getScale();

	void update();
};

