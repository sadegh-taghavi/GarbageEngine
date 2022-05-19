#pragma once
#include <stdint.h>
#include <GE_Math.h>
#include <QFile>

#define MAX_FILE_NAME 512

class GE_Texture;
class GEW_Light;

class GEW_Postprocess
{
	static GEW_Postprocess m_singleton;
	GEW_Postprocess();
	~GEW_Postprocess();
public:
	struct SkyParameters
	{
		bool	 Enable;
		uint32_t Samples;
		float    Radius;
		float    Intensity;
		GE_Vec3	 Wavelength;
		uint32_t LightID;
		float    Sharpness;
		float    Core;
		float    Highlight;
		float    Sun;
		bool	 Map1Enable;
		float    Map1ScaleZ;
		float    Map1PositionZ;
		GE_Vec3  Map1Rotation;
		bool	 Map1AnimateX;
		bool	 Map1AnimateY;
		bool	 Map1AnimateZ;
		char     Map1FileName[ MAX_FILE_NAME ];
		bool	 Map2Enable;
		float    Map2ScaleZ;
		float    Map2PositionZ;
		GE_Vec3  Map2Rotation;
		bool	 Map2AnimateX;
		bool	 Map2AnimateY;
		bool	 Map2AnimateZ;
		char     Map2FileName[ MAX_FILE_NAME ];
		SkyParameters( );
		void reset();
	}m_sky;

	struct LighScatterParameters
	{
		bool	Enable;
		float	Luminance;
		float	Cutoff;
		float	Exp;
		float	Density;
		float	Weight;
		float   Decay;
		LighScatterParameters();
		void reset();
	}m_lightScatter;

	struct AtmosphereFogParameters
	{
		bool	Enable;
		GE_Vec3	Color;
		float	Intensity;
		GE_Vec4	Depth_PositionFieldFalloffHardness;
		GE_Vec4	Height_PositionFieldFalloffHardness;
		AtmosphereFogParameters( );
		void reset();
	}m_atmosphereFog;

	struct LensDirtProperty
	{
		bool	Enable;
		float	Luminance;
		float	Cutoff;
		float	Exp;
		float	DirtExp;
		float	CoreExp;
		char    MapFileName[ MAX_FILE_NAME ];
		void	reset();
		LensDirtProperty();
	}m_lensDirt;

	struct MotionBlurParameters
	{
		bool Enable;
		float Damp;
		float Max;
		MotionBlurParameters( );
		void reset();
	}m_motionBlur;

	struct HDRParameters
	{
		bool  Enable;
		float WhiteCutoff;
		float LuminanceMax;
		float LuminanceMin;
		float Gamma;
		float AdaptationRate;
		char  GradientMap[ MAX_FILE_NAME ];
		HDRParameters();
		void reset();
	}m_hDR;

	struct AOParameters
	{
		bool  Enable;
		float Radius;
		float Depth;
		float DistanceStart;
		float Intensity;
		float DistanceEnd;
		float Normal;
		AOParameters();
		void reset();
		
	}m_aO;

	struct GlowParameters
	{
		bool  Enable;
		float Luminance;
		float Cutoff;
		float VScale;
		float HScale;
		GlowParameters( );
		void reset();
	}m_glow;

	struct GlowMappedParameters
	{
		bool  Enable;
		float Luminance;
		float Cutoff;
		float VScale;
		float HScale;
		GlowMappedParameters( );
		void reset();
	}m_glowMapped;

	GE_Texture *m_dirtMap;
	GEW_Light *m_skyLight;
	
	GE_Vec3 m_skyMap1Rotation;
	GE_Vec3 m_skyMap2Rotation;

	static GEW_Postprocess *getSingleton(){ return &m_singleton; }
	void update();
	void saveToFile( QFile &i_file, QString i_resourcePath );
	void loadFromFile( QFile &i_file, const QString &i_resourcePath );
	void loadDefault();
};

