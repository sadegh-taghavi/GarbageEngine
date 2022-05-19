#include "GE_Texture.h"
#include "GE_Core.h"
#include <DirectXTex.h>

GE_Texture::GE_Texture()
{
	m_outSRV = NULL;
	m_texture = NULL;
	m_numberOfUsers = 0;
}

GE_Texture::~GE_Texture()
{
	SAFE_RELEASE( m_texture );
}

void GE_Texture::load( const char *i_textureFileName )
{
	m_numberOfUsers = 1;
	m_name = i_textureFileName;

	std::string postFix( i_textureFileName + ( strlen( i_textureFileName ) - 3 ) );
	std::transform( postFix.begin(), postFix.end(), postFix.begin(), ::tolower );
	CString str = i_textureFileName;
	DirectX::ScratchImage img;
	DirectX::TexMetadata md;
	if( postFix == "dds" )
		DirectX::LoadFromDDSFile( str.GetString(), 0, &md, img );
	else if( postFix == "tga" )
		DirectX::LoadFromTGAFile( str.GetString(), &md, img );
	else
	{
		DirectX::LoadFromWICFile( str.GetString(), 0, &md, img );
	}
	m_numberOfMips = ( uint32_t ) md.mipLevels;
	m_width = ( uint32_t )md.width;
	m_height = ( uint32_t )md.height;
	DirectX::CreateShaderResourceView( GE_Core::getSingleton().getDevice(), img.GetImages(), img.GetImageCount(), md, &m_texture );
}

void GE_Texture::reload()
{
	SAFE_RELEASE( m_texture );
	std::string postFix( m_name.c_str() + ( m_name.length() - 3 ) );
	std::transform( postFix.begin(), postFix.end(), postFix.begin(), ::tolower );
	CString str = m_name.c_str();
	DirectX::ScratchImage img;
	DirectX::TexMetadata md;
	if( postFix == "dds" )
		DirectX::LoadFromDDSFile( str.GetString(), 0, &md, img );
	else if( postFix == "tga" )
		DirectX::LoadFromTGAFile( str.GetString(), &md, img );
	else
	{
		DirectX::LoadFromWICFile( str.GetString(), 0, &md, img );
	}
	m_numberOfMips = ( uint32_t ) md.mipLevels;
	m_width = ( uint32_t )md.width;
	m_height = ( uint32_t )md.height;
	DirectX::CreateShaderResourceView( GE_Core::getSingleton().getDevice(),
		img.GetImages(),
		img.GetImageCount(),
		md,
		&m_texture );
}

void GE_Texture::unLoad()
{
	if( m_numberOfUsers == 0 )
		return;
	m_numberOfUsers--;
	if( m_numberOfUsers != 0 )
		return;
	SAFE_RELEASE( m_texture );
	m_name = "";
}

const char* GE_Texture::getTextureName()
{
	return m_name.data();
}

//GE_TextureManager----------------------------------

GE_TextureManager GE_TextureManager::m_singleton;

GE_Texture* GE_TextureManager::createTexture( const char* textureFileName )
{
	GE_Texture* temp = getTexture( textureFileName );
	if( temp )
	{
		temp->m_numberOfUsers++;
		return temp;
	}
	temp = new GE_Texture();
	temp->load( textureFileName );
	m_textures.push_back( temp );
	return temp;
}

GE_Texture*	GE_TextureManager::getTexture( const char* textureFileName )
{
	std::vector<GE_Texture *>::iterator i = m_textures.begin();
	for( ; i != m_textures.end(); i++ )
		if( ( *i )->m_name.compare( textureFileName ) == 0 )
			return *i;
	return NULL;
}

GE_Texture*	GE_TextureManager::getTexture( ID3D11ShaderResourceView *pTexture )
{
	std::vector<GE_Texture *>::iterator i = m_textures.begin();
	for( ; i != m_textures.end(); i++ )
		if( ( *i )->m_texture == pTexture )
			return *i;
	return NULL;
}

void GE_TextureManager::remove( const char* textureFileName )
{

	std::vector<GE_Texture *>::iterator i = m_textures.begin();
	for( ; i != m_textures.end(); i++ )
	{
		if( ( *i )->m_name.compare( textureFileName ) == 0 )
		{
			( *i )->unLoad();
			if( ( *i )->m_numberOfUsers == 0 )
			{
				delete ( *i );
				m_textures.erase( i );
			}
			return;
		}
	}
}

bool GE_TextureManager::find( GE_Texture **ppTexture )
{
	if( !m_textures.size() )
		return false;
	std::vector<GE_Texture *>::iterator i = m_textures.begin();
	for( ; i != m_textures.end(); i++ )
	{
		if( ( *i ) == ( *ppTexture ) )
		{
			return true;
		}
	}
	return false;
}

void GE_TextureManager::remove( GE_Texture **ppTexture )
{
	if( ( *ppTexture ) == NULL )
		return;

	if( !find( ppTexture ) )
	{
		( *ppTexture ) = NULL;
		return;
	}

	( *ppTexture )->unLoad();
	if( ( *ppTexture )->m_numberOfUsers == 0 )
	{
		std::vector<GE_Texture *>::iterator i = m_textures.begin();
		for( ; i != m_textures.end(); i++ )
		{
			if( ( *i ) == ( *ppTexture ) )
			{
				m_textures.erase( i );
				SAFE_DELETE( *ppTexture );
				return;
			}
		}
	}
	( *ppTexture ) = NULL;
}

void GE_TextureManager::release()
{
	std::vector<GE_Texture *>::iterator i = m_textures.begin();
	for( ; i != m_textures.end(); i++ )
	{
		SAFE_DELETE( ( *i ) );
	}
	m_textures.clear();
}

GE_TextureManager::~GE_TextureManager()
{
	release();
}

void GE_TextureManager::reloadTextures()
{
	std::vector<GE_Texture *>::iterator i = m_textures.begin();
	for( ; i != m_textures.end(); i++ )
	{
		( *i )->reload();
	}
}

GE_TextureManager::GE_TextureManager()
{
	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
}

GE_MemoryTexture::GE_MemoryTexture()
{
	m_image = new DirectX::ScratchImage();
}

GE_MemoryTexture::~GE_MemoryTexture()
{
	DirectX::ScratchImage *image = (DirectX::ScratchImage *)m_image;
	image->Release();
	SAFE_DELETE(m_image);
}

void GE_MemoryTexture::save(ID3D11Resource *i_inResource)
{
	DirectX::ScratchImage *image = (DirectX::ScratchImage *)m_image;
	image->Release();
	DirectX::CaptureTexture(GE_Core::getSingleton().getDevice(), GE_Core::getSingleton().getImmediateContext(), i_inResource, *image);
}

void GE_MemoryTexture::load(ID3D11ShaderResourceView **i_outSRV)
{
	DirectX::ScratchImage *image = (DirectX::ScratchImage *)m_image;
	DirectX::CreateShaderResourceView(GE_Core::getSingleton().getDevice(), image->GetImages(),
		image->GetImageCount(), image->GetMetadata(), i_outSRV);
}

bool GE_MemoryTexture::isValid()
{
	DirectX::ScratchImage *image = (DirectX::ScratchImage *)m_image;
	return (image->GetImageCount() > 0);
}

void GE_MemoryTexture::clear()
{
	DirectX::ScratchImage *image = (DirectX::ScratchImage *)m_image;
	image->Release();
}
