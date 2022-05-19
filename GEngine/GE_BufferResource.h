#pragma once
#include "GE_Prerequisites.h"

class _GE_BufferResource
{
public:
	uint32_t					m_lenght;
	uint32_t					m_sizeOfObj;
	ID3D11Buffer				*m_buffer;
	ID3D11ShaderResourceView	*m_sRV;
	void						*m_data;
	_GE_BufferResource();
	~_GE_BufferResource();
	void						createBuffer(
		uint32_t	i_lenght,
		uint32_t	i_sizeofobj = 1,
		void		*i_data = NULL,
		bool		i_isConst = true,
		DXGI_FORMAT i_format = DXGI_FORMAT_R32G32B32A32_FLOAT,
		uint32_t	i_width = 1
		);
	void						fill();
	void						fill( uint32_t i_customSize );
	void						release();
};

