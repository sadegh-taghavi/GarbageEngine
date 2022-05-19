#include "GE_BufferResource.h"
#include "GE_Core.h"

_GE_BufferResource::_GE_BufferResource()
{
	m_buffer = NULL;
	m_sRV = NULL;
	m_lenght = 0;
	m_data = NULL;
}

_GE_BufferResource::~_GE_BufferResource()
{
	release();
}

void _GE_BufferResource::createBuffer( 
	uint32_t	i_lenght, 
	uint32_t	i_sizeofobj	/*=1*/, 
	void		*i_data		/*=NULL*/, 
	bool		i_isConst	/*=true*/, 
	DXGI_FORMAT i_format	/*=DXGI_FORMAT_R32G32B32A32_FLOAT*/,
	uint32_t	i_width		/*=1*/ )
{
	release();
	m_sizeOfObj = i_sizeofobj;
	m_lenght = i_lenght;
	m_data = i_data;
	m_buffer = NULL;
	m_sRV = NULL;
	D3D11_BUFFER_DESC bdesc;
	memset(&bdesc, 0, sizeof(D3D11_BUFFER_DESC));
	bdesc.ByteWidth = m_lenght * m_sizeOfObj;
	bdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bdesc.MiscFlags = 0;
	if(i_isConst)
	{
		bdesc.CPUAccessFlags = 0;
		bdesc.Usage = D3D11_USAGE_DEFAULT;
	}else
	{
		bdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bdesc.Usage = D3D11_USAGE_DYNAMIC;
	}
	D3D11_SUBRESOURCE_DATA InitData;
	memset(&InitData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	InitData.pSysMem = m_data;
	InitData.SysMemPitch = 0;
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	memset( &SRVDesc, 0, sizeof( D3D11_SHADER_RESOURCE_VIEW_DESC ) );
	if(m_data)
		GE_Core::getSingleton().getDevice()->CreateBuffer( &bdesc, &InitData, &m_buffer );
	else
		GE_Core::getSingleton().getDevice()->CreateBuffer( &bdesc, NULL, &m_buffer );
	
	SRVDesc.Format = i_format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.ElementOffset = 0;
	SRVDesc.Buffer.ElementWidth = m_lenght * i_width ;
	GE_Core::getSingleton().getDevice()->CreateShaderResourceView( m_buffer, &SRVDesc, &m_sRV );
}

void _GE_BufferResource::fill()
{
	D3D11_MAPPED_SUBRESOURCE ptr;
	GE_Core::getSingleton().getImmediateContext()->Map( m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ptr);
	memcpy( ptr.pData, m_data, m_sizeOfObj * m_lenght);
	GE_Core::getSingleton().getImmediateContext()->Unmap( m_buffer, 0);
}

void _GE_BufferResource::fill( uint32_t i_customSize )
{
	D3D11_MAPPED_SUBRESOURCE ptr;
	GE_Core::getSingleton().getImmediateContext()->Map( m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ptr);
	memcpy( ptr.pData, m_data, m_sizeOfObj * i_customSize);
	GE_Core::getSingleton().getImmediateContext()->Unmap( m_buffer, 0);
}

void _GE_BufferResource::release()
{
	m_lenght = 0;
	SAFE_RELEASE( m_sRV );
	SAFE_RELEASE( m_buffer );
}

