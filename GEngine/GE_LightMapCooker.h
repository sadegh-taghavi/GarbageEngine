#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"

class GE_LightMapCooker
{
	GE_LightMapCooker( void );
	~GE_LightMapCooker( void );
	static GE_LightMapCooker	m_singleton;
	bool						m_overWriteLightMaps;
	uint32_t					m_size;
	uint32_t					m_depthSize;
	std::string					m_path;
	ID3D11VertexShader			*m_vS;
	ID3D11PixelShader			*m_pS_Dir;
	ID3D11PixelShader			*m_pS_Spot;
	ID3D11PixelShader			*m_pS_Point;
	class GE_RenderTarget		*m_rt[ 2 ];
	class GE_RenderTarget		*m_dirDepth;
	std::vector<class GE_RenderTarget *>	m_rtLightDepth;
	struct PerObject
	{
		float		LightRange;
		float		LightAttenuation;
		float		LightSpotSpread;
		float		LightSpotSharpness;
		GE_Vec4		LightPosition;
		GE_Vec4		LightAmbient;
		GE_Vec4		LightDirection;
		GE_Vec4		ViewPosition;
		GE_Vec4		LightDiffuse;
		GE_Vec4		ShadowOffsetBias;
		GE_Mat4x4	LightViewProjection[ 6 ];
		GE_Mat4x4	World;
	}						m_cPerObject;
	ID3D11Buffer			*m_perObject;
public:
	static GE_LightMapCooker &getSingleton();
	void		init();
	void		cook( std::list<class GE_Light *> &i_lights, std::list<class GE_Model *> &i_models );
	void		setLightMapSize( uint32_t i_size );
	void		setDepthMapSize( uint32_t i_size );
	uint32_t	&getLightMapSize();
	uint32_t	&getDepthMapSize();
	void		setPath( const std::string &i_path );
	void		setOverWriteLightMaps( bool i_overWrite );
	std::string &getPath();
};

#include "GE_LightMapCooker.inl"