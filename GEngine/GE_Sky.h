#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"

class GE_Texture;
class GE_View;

class GE_Sky
{
public:
	struct Parameters
	{
		GE_Vec3		LightDir;
		float		OuterRadius;
		float		InnerRadius;
		uint32_t	Samples;
		float		Radius;
		float		Intensity;
		GE_Vec3		Wavelength;
		float		Sharpness;
		float		Core;
		float		Height;
		float		Highlight;
		float		Sun;
		bool		Map1Enable;
		float		Map1ScaleZ;
		float		Map1PositionZ;
		GE_Mat4x4	Map1Rotation;
		bool		Map2Enable;
		float		Map2ScaleZ;
		float		Map2PositionZ;
		GE_Mat4x4	Map2Rotation;
	};

protected:
	ID3D11VertexShader	*m_vS;
	ID3D11PixelShader	*m_pS;
	ID3D11PixelShader	*m_pS_Map10;
	ID3D11PixelShader	*m_pS_Map01;
	ID3D11PixelShader	*m_pS_Map11;
	ID3D11Buffer		*m_perFrame;
	Parameters			m_parameters;
	GE_Texture			*m_texture1;
	GE_Texture			*m_texture2;
	ID3D11Buffer		*m_vB;
	ID3D11Buffer		*m_iB;
	uint32_t			m_numberOfIndices;
	struct PerFrame
	{
		float		CameraHeight;
		float		CameraHeight2;
		float		OuterRadius;
		float		OuterRadius2;
		float		InnerRadius;
		float		InnerRadius2;
		float		SharpnessSun;
		float		HighlightSun;
		float		Sharpness4PI;
		float		Highlight4PI;
		float		InvScale;
		float		ScaleDepth;
		float		ScaleOverScaleDepth;
		float		Map1ScaleZ;
		float		Map1PositionZ;
		float		Map2ScaleZ;
		float		Map2PositionZ;
		float		Core;
		float		Core2;
		float		Scale;
		uint32_t	NumberOfSamples;
		uint32_t	Padding[ 3 ];
		GE_Vec4		CameraPos;
		GE_Vec4		LightDir;
		GE_Vec4		Position;
		GE_Vec4		InvWavelength_Intensity;
		GE_Mat4x4	ViewProjection;
		GE_Mat4x4	Map1Rotation;
		GE_Mat4x4	Map2Rotation;
	}m_cPerFrame;

	GE_Sky( void ){ memset( this, 0, sizeof( GE_Sky ) ); }
	~GE_Sky( void ){ destroy(); }
	static GE_Sky m_singleton;
public:
	static GE_Sky &getSingleton(){ return m_singleton; }
	Parameters *getParameters(){ return &m_parameters; }
	void setParameters( Parameters &i_parameters ){ m_parameters = i_parameters; }
	void init();
	void setMap1( const char *i_map1FileName );
	void deleteMap1();
	GE_Texture *getMap1(){ return m_texture1; }
	void setMap2( const char *i_map2FileName );
	void deleteMap2();
	GE_Texture *getMap2(){ return m_texture2; }
	void render( GE_View *i_view );
	void destroy();
};

