#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"
#include "GE_View.h"

class GE_RenderTarget;
class GE_Light;
class GE_Model;

class GE_EnvironmentProbe
{
	bool						m_enable;
	GE_Vec3						m_position;
	GE_Vec3						m_scale;
	GE_Vec2						m_mapSize;
	GE_View						m_view[ 6 ];
	GE_RenderTarget				*m_renderTarget[ 6 ];
	GE_RenderTarget				*m_cubeTarget;
	friend class GE_EnvironmentProbeRenderer;
	struct ShadowModels
	{
		GE_Light								*Light;
		std::list< std::list<GE_Model *> *>		ModelLists;
		ShadowModels( )
		{
			Light = NULL;
		}
		~ShadowModels( );
	};
public:
	GE_EnvironmentProbe();
	~GE_EnvironmentProbe();

	void	render( std::list<GE_Model*> &i_modelList, std::list<GE_Light*> &i_lightList );
	void	saveToFile( std::string i_path, std::string i_name );
	void	setEnable( bool i_enable );
	void	setPosition( GE_Vec3 &i_position );
	void	setScale( GE_Vec3 &i_scale );
	void	setMapSize( GE_Vec2 &i_mapSize );
	bool	isEnable();
	GE_Vec3 &getPosition();
	GE_Vec3 &getScale();
	GE_Vec2 &getMapSize();

	ID3D11ShaderResourceView *getSRV();
};

//Renderer-----------------------------------------------------------------
class GE_EnvironmentProbeRenderer
{
	static GE_EnvironmentProbeRenderer m_singleton;
	ID3D11VertexShader   *m_vS;
	ID3D11VertexShader   *m_vS_Cube;
	ID3D11GeometryShader *m_gS_Cube;
	ID3D11PixelShader	 *m_pS;
	ID3D11PixelShader    *m_pS_Cube;
	ID3D11Buffer		 *m_perFrame;
	ID3D11Buffer		 *m_perObject;
	uint32_t			 m_indicesCount;
	ID3D11Buffer		 *m_sphereVB;
	ID3D11Buffer		 *m_sphereIB;
	struct PerFrame
	{
		GE_Vec4		ViewPosition;
		GE_Mat4x4	InverseProjection;

	}m_cPerFrame;

	struct PerObject
	{
		GE_Mat4x4	World;
		GE_Mat4x4	WorldViewProjection;

	}m_cPerObject;
	
	GE_EnvironmentProbeRenderer();
	~GE_EnvironmentProbeRenderer();
	friend class GE_EnvironmentProbe;
	void generateCube( GE_EnvironmentProbe *i_prob );
public:
	static GE_EnvironmentProbeRenderer& getSingleton(){ return m_singleton; }
	void init();
	void render( std::list< GE_EnvironmentProbe * > *i_probsList, const GE_View *i_view, ID3D11ShaderResourceView *i_depth );
};

#include "GE_EnvironmentProbe.inl"