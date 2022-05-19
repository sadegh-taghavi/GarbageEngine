#pragma once 
#include "GE_Prerequisites.h"

class GE_Texture
{
	friend class GE_TextureManager;
	ID3D11ShaderResourceView	*m_texture;
	ID3D11ShaderResourceView	*m_outSRV;
	std::string					m_name;
	uint32_t					m_numberOfUsers;
	uint32_t					m_numberOfMips;
	uint32_t					m_width;
	uint32_t					m_height;
	GE_Texture();
	~GE_Texture();
	void load( const char * i_textureFileName );
	void unLoad();
public:
	void setOutSrv( ID3D11ShaderResourceView *i_srv ) { m_outSRV = i_srv; }
	void reload();
	uint32_t getNumberOfMips() { return m_numberOfMips; }
	uint32_t getWidth( ) { return m_width; }
	uint32_t getHeight( ) { return m_height; }
	const char* getTextureName();
	ID3D11ShaderResourceView *getTextureSRV()
	{
		if( m_outSRV )
			return m_outSRV;
		return m_texture;
	}
};

class GE_MemoryTexture
{
	void *m_image;
public:
	GE_MemoryTexture();
	~GE_MemoryTexture();
	void save(ID3D11Resource *i_inResource);
	void load(ID3D11ShaderResourceView **i_outSRV);
	void clear();
	bool isValid();
};

class GE_TextureManager
{
	std::vector<GE_Texture *> m_textures;
	static GE_TextureManager m_singleton;

	GE_TextureManager();
	~GE_TextureManager();
	bool find( GE_Texture **ppTexture );
public:
	static GE_TextureManager &getSingleton() { return m_singleton; }
	GE_Texture*	createTexture( const char* i_textureFileName );
	GE_Texture*	getTexture( const char* i_textureFileName );
	GE_Texture*	getTexture( ID3D11ShaderResourceView *i_pTexture );
	uint32_t	getCount() { return ( uint32_t ) m_textures.size(); }
	void		remove( const char* i_textureFileName );
	void		remove( GE_Texture **i_ppTexture );
	void		reloadTextures();
	void		release();
	std::vector< GE_Texture *> &getTextures() { return m_textures; }
};