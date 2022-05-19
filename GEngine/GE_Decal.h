#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"

class GE_Texture;
class GE_View;

class GE_Decal
{
public:
	struct Parameter
	{
		float		Visible;
		float		MaxAngle;
		float		Bumpiness;
		float		Glowness;
		float		Roughness;
		float		Reflectmask;
		float		Layer;
		float		Rotation;
		GE_Vec2		UVTile;
		GE_Vec2		UVMove;
		GE_Vec4     Color;
		GE_Mat4x4	Transform;
		GE_Mat4x4	Projection;
		Parameter();
	};
	GE_Decal();
	~GE_Decal();
	void update();
	void setDiffuseTexture( const char *i_fileName );
	class GE_Texture* getDiffuseTexture();
	void deleteDiffuseTexture();
	void setNormalTexture( const char *i_fileName );
	class GE_Texture* getNormalTexture();
	void deleteNormalTexture();
	void setGlowSpecularTexture( const char *i_fileName );
	class GE_Texture*  getGlowSpecularTexture();
	void deleteGlowSpecularTexture();
	void setReflectTexture( const char *i_fileName );
	class GE_Texture*  getReflectTexture();
	void deleteReflectTexture();
	void addDecal();
	void removeDecal( uint32_t i_index );
	void setNumberOfDecal( uint32_t i_numberOfDecal );
	vector<Parameter> *beginEditParameters();
	void endEditParameters();

private:
	friend class GE_DecalRenderer;

	vector<Parameter>	m_parameters;
	class GE_Texture *m_diffuseTexture;
	class GE_Texture *m_normalTexture;
	class GE_Texture *m_glowSpecularTexture;
	class GE_Texture *m_reflectTexture;

	class _GE_BufferResource *m_instanceData;

	bool m_needFill;
};

//----------------------------------------------------------------------------------------------------------------------------------
class GE_DecalRenderer
{
	uint32_t	 m_indicesCount;
	ID3D11Buffer *m_boxVB, *m_boxIB;
	ID3D11Buffer *m_perFrame, *m_perObject;

	ID3D11VertexShader *m_vS;
	ID3D11GeometryShader *m_gS;
	ID3D11PixelShader *m_pS_D;
	ID3D11PixelShader *m_pS_DN;
	ID3D11PixelShader *m_pS_DM;
	ID3D11PixelShader *m_pS_DR;
	ID3D11PixelShader *m_pS_DNM;
	ID3D11PixelShader *m_pS_DNR;
	ID3D11PixelShader *m_pS_DMR;
	ID3D11PixelShader *m_pS_DNMR;
	
	struct PerFrame
	{
		GE_Mat4x4 ViewProjection;
		GE_Mat4x4 ViewProjectionInv;
		GE_Vec3	  ViewPosition;
		float	  Padding;
	}m_cPerFrame;

	struct PerObject
	{
		float	EnvironmentRadiusInv;
		GE_Vec3 EnvironmentPosition;
		GE_Vec4	NumberOfMips;
	}m_cPerObject;

	GE_DecalRenderer();
	~GE_DecalRenderer();
	static GE_DecalRenderer m_singleton;
public:
	static GE_DecalRenderer &getSingleton(){ return m_singleton; }
	void init();
	void render( std::list<GE_Decal *> *i_decalList, GE_View *i_view );
	void destroy();
};