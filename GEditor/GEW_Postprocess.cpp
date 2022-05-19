#include "GEW_Postprocess.h"
#include <GE_Texture.h>
#include <GE_PostProcess.h>
#include <GE_Sky.h>
#include <GE_Core.h>
#include <GE_Light.h>
#include "GEW_Light.h"
#include "GEW_Utility.h"
#include "GEW_PostprocessProperty.h"

GEW_Postprocess GEW_Postprocess::m_singleton;

GEW_Postprocess::GEW_Postprocess()
{
	m_dirtMap = NULL;
	m_skyMap1Rotation = GE_Vec3( 0.0f, 0.0f, 0.0f );
	m_skyMap2Rotation = GE_Vec3( 0.0f, 0.0f, 0.0f );
}

GEW_Postprocess::~GEW_Postprocess()
{

}

void GEW_Postprocess::update()
{
	if ( m_sky.Enable )
	{
		GE_Quat rot;
		if ( m_sky.Map1Enable )
		{
			if ( m_sky.Map1AnimateX )
				m_skyMap1Rotation.x += GE_Core::getSingleton().getElapsedTime() * m_sky.Map1Rotation.x;
			else
				m_skyMap1Rotation.x = m_sky.Map1Rotation.x;

			if ( m_sky.Map1AnimateY )
				m_skyMap1Rotation.y += GE_Core::getSingleton().getElapsedTime() * m_sky.Map1Rotation.y;
			else
				m_skyMap1Rotation.y = m_sky.Map1Rotation.y;

			if ( m_sky.Map1AnimateZ )
				m_skyMap1Rotation.z += GE_Core::getSingleton().getElapsedTime() * m_sky.Map1Rotation.z;
			else
				m_skyMap1Rotation.z = m_sky.Map1Rotation.z;

			rot.rotationYPR( &m_skyMap1Rotation );
			GE_Sky::getSingleton().getParameters()->Map1Rotation.RotationQuaternion( &rot );
		}
		
		if ( m_sky.Map2Enable )
		{
			if ( m_sky.Map2AnimateX )
				m_skyMap2Rotation.x += GE_Core::getSingleton().getElapsedTime() * m_sky.Map2Rotation.x;
			else
				m_skyMap2Rotation.x = m_sky.Map2Rotation.x;

			if ( m_sky.Map2AnimateY )
				m_skyMap2Rotation.y += GE_Core::getSingleton().getElapsedTime() * m_sky.Map2Rotation.y;
			else
				m_skyMap2Rotation.y = m_sky.Map2Rotation.y;

			if ( m_sky.Map2AnimateZ )
				m_skyMap2Rotation.z += GE_Core::getSingleton().getElapsedTime() * m_sky.Map2Rotation.z;
			else
				m_skyMap2Rotation.z = m_sky.Map2Rotation.z;

			rot.rotationYPR( &m_skyMap2Rotation );
			GE_Sky::getSingleton().getParameters()->Map2Rotation.RotationQuaternion( &rot );
		}

		if ( m_skyLight )
			GE_Sky::getSingleton().getParameters()->LightDir = -*m_skyLight->m_light->getDirection();
	}
}

void GEW_Postprocess::saveToFile( QFile &i_file, QString i_resourcePath )
{
	std::string skyMap1;
	std::string skyMap2;
	std::string lensDirtMap;
	std::string hdrMap;
	if( m_sky.Map1FileName[ 0 ] )
	{
		skyMap1 = m_sky.Map1FileName;
		strcpy_s( m_sky.Map1FileName,
			GEW_Utility::copyFile( skyMap1.c_str(), i_resourcePath ).c_str() );
	}
	if( m_sky.Map2FileName[ 0 ] )
	{
		skyMap2 = m_sky.Map2FileName;
		strcpy_s( m_sky.Map2FileName,
			GEW_Utility::copyFile( skyMap2.c_str(), i_resourcePath ).c_str() );
	}
	if( m_lensDirt.MapFileName[ 0 ] )
	{
		lensDirtMap = m_lensDirt.MapFileName;
		strcpy_s( m_lensDirt.MapFileName,
			GEW_Utility::copyFile( lensDirtMap.c_str(), i_resourcePath ).c_str() );
	}
	if( m_hDR.GradientMap[ 0 ] )
	{
		hdrMap = m_hDR.GradientMap;
		strcpy_s( m_hDR.GradientMap,
			GEW_Utility::copyFile( hdrMap.c_str(), i_resourcePath ).c_str() );
	}

	i_file.write( ( const char* ) &m_sky, sizeof( SkyParameters ) );
	i_file.write( ( const char* ) &m_lightScatter, sizeof( LighScatterParameters ) );
	i_file.write( ( const char* ) &m_lensDirt, sizeof( LensDirtProperty ) );
	i_file.write( ( const char* ) &m_motionBlur, sizeof( MotionBlurParameters ) );
	i_file.write( ( const char* ) &m_hDR, sizeof( HDRParameters ) );
	i_file.write( ( const char* ) &m_aO, sizeof( AOParameters ) );
	i_file.write( ( const char* ) &m_glow, sizeof( GlowParameters ) );
	i_file.write( ( const char* ) &m_glowMapped, sizeof( GlowMappedParameters ) );
	i_file.write( ( const char* )&m_atmosphereFog, sizeof( AtmosphereFogParameters ) );
	i_file.write( ( const char* ) &m_skyMap1Rotation, sizeof( GE_Vec3 ) );
	i_file.write( ( const char* ) &m_skyMap2Rotation, sizeof( GE_Vec3 ) );

	if( skyMap1.length() )
		strcpy_s( m_sky.Map1FileName, skyMap1.c_str() );
	if( skyMap2.length() )
		strcpy_s( m_sky.Map2FileName, skyMap2.c_str() );
	if( lensDirtMap.length() )
		strcpy_s( m_lensDirt.MapFileName, lensDirtMap.c_str() );
	if( hdrMap.length() )
		strcpy_s( m_hDR.GradientMap, hdrMap.c_str() );
}

void GEW_Postprocess::loadFromFile( QFile &i_file, const QString &i_resourcePath )
{
	i_file.read( ( char* ) &m_sky, sizeof( SkyParameters ) );
	i_file.read( ( char* ) &m_lightScatter, sizeof( LighScatterParameters ) );
	i_file.read( ( char* ) &m_lensDirt, sizeof( LensDirtProperty ) );
	i_file.read( ( char* ) &m_motionBlur, sizeof( MotionBlurParameters ) );
	i_file.read( ( char* ) &m_hDR, sizeof( HDRParameters ) );
	i_file.read( ( char* ) &m_aO, sizeof( AOParameters ) );
	i_file.read( ( char* ) &m_glow, sizeof( GlowParameters ) );
	i_file.read( ( char* ) &m_glowMapped, sizeof( GlowMappedParameters ) );
	i_file.read( ( char* )&m_atmosphereFog, sizeof( AtmosphereFogParameters ) );
	i_file.read( ( char* ) &m_skyMap1Rotation, sizeof( GE_Vec3 ) );
	i_file.read( ( char* ) &m_skyMap2Rotation, sizeof( GE_Vec3 ) );

	m_skyLight = GEW_Light::find( m_sky.LightID );
	if( m_sky.Map1FileName[ 0 ] )
	{
		strcpy_s( m_sky.Map1FileName,
			GEW_Utility::absolutePath( m_sky.Map1FileName, i_resourcePath ).c_str() );
		GE_Sky::getSingleton().setMap1( m_sky.Map1FileName );
	} else
		GE_Sky::getSingleton().deleteMap1();

	if( m_sky.Map2FileName[ 0 ] )
	{
		strcpy_s( m_sky.Map2FileName,
			GEW_Utility::absolutePath( m_sky.Map2FileName, i_resourcePath ).c_str() );
		GE_Sky::getSingleton().setMap2( m_sky.Map2FileName );
	} else
		GE_Sky::getSingleton().deleteMap2();

	GE_Sky::Parameters *skyParam = GE_Sky::getSingleton().getParameters();
	skyParam->Samples = m_sky.Samples;
	skyParam->Radius = m_sky.Radius;
	skyParam->Intensity = m_sky.Intensity;
	skyParam->Wavelength = m_sky.Wavelength;
	skyParam->Sharpness = m_sky.Sharpness;
	skyParam->Core = m_sky.Core;
	skyParam->Highlight = m_sky.Highlight;
	skyParam->Sun = m_sky.Sun;
	skyParam->Map1Enable = m_sky.Map1Enable;
	skyParam->Map1ScaleZ = m_sky.Map1ScaleZ;
	skyParam->Map1PositionZ = m_sky.Map1PositionZ;
	skyParam->Map2Enable = m_sky.Map2Enable;
	skyParam->Map2ScaleZ = m_sky.Map2ScaleZ;
	skyParam->Map2PositionZ = m_sky.Map2PositionZ;

	GE_TextureManager::getSingleton().remove( &m_dirtMap );
	if( m_lensDirt.MapFileName[ 0 ] )
	{
		strcpy_s( m_lensDirt.MapFileName,
			GEW_Utility::absolutePath( m_lensDirt.MapFileName, i_resourcePath ).c_str() );
		m_dirtMap = GE_TextureManager::getSingleton().createTexture( m_lensDirt.MapFileName );
	}

	GE_PostProcess::getSingleton().deleteHDRGradientMap();
	if( m_hDR.GradientMap[ 0 ] )
	{
		strcpy_s( m_hDR.GradientMap,
			GEW_Utility::absolutePath( m_hDR.GradientMap, i_resourcePath ).c_str() );
		GE_PostProcess::getSingleton().loadHDRGradientMap( m_hDR.GradientMap );
	}
	GEW_PostprocessProperty::getSingleton()->updateUi();
}

void GEW_Postprocess::loadDefault()
{
	m_sky.reset();
	m_lightScatter.reset();
	m_lensDirt.reset();
	m_motionBlur.reset();
	m_hDR.reset();
	m_aO.reset();
	m_glow.reset();
	m_glowMapped.reset();
	m_atmosphereFog.reset( );

	GE_Sky::getSingleton().deleteMap1();
	GE_Sky::getSingleton().deleteMap2();
	if( m_dirtMap )
		GE_TextureManager::getSingleton().remove( &m_dirtMap );
	GE_PostProcess::getSingleton().deleteHDRGradientMap();
	GE_PostProcess::getSingleton().loadHDRGradientMap( m_hDR.GradientMap );
	m_skyMap1Rotation = GE_Vec3( 0, 0, 0 );
	m_skyMap2Rotation = GE_Vec3( 0, 0, 0 );
	m_skyLight = NULL;
	GEW_PostprocessProperty::getSingleton( )->updateUi( );
}

GEW_Postprocess::SkyParameters::SkyParameters()
{
	reset();
}

void GEW_Postprocess::SkyParameters::reset()
{
	Samples = 10;
	Radius = GE_ConvertToUnit( 4000.0f );
	Intensity = 1.0f;
	Wavelength.x = 0.650f;
	Wavelength.y = 0.570f;
	Wavelength.z = 0.475f;
	LightID = 0;
	Sharpness = 5.0f;
	Core = 1.0f;
	Highlight = 1.0f;
	Sun = 20.0f;
	Enable = false;

	Map1Enable = false;
	Map1ScaleZ = 1.0f;
	Map1PositionZ = 0.0f;
	Map1Rotation.x = 0.0f;
	Map1Rotation.y = 0.0f;
	Map1Rotation.z = 0.0f;
	Map1AnimateX = false;
	Map1AnimateY = false;
	Map1AnimateZ = false;
	Map1FileName[ 0 ] = 0;

	Map2Enable = false;
	Map2ScaleZ = 1.0f;
	Map2PositionZ = 0.0f;
	Map2Rotation.x = 0.0f;
	Map2Rotation.y = 0.0f;
	Map2Rotation.z = 0.0f;
	Map2AnimateX = false;
	Map2AnimateY = false;
	Map2AnimateZ = false;
	Map2FileName[ 0 ] = 0;

	GEW_Postprocess::getSingleton()->m_skyLight = NULL;
}

GEW_Postprocess::LighScatterParameters::LighScatterParameters()
{
	reset();
}

void GEW_Postprocess::LighScatterParameters::reset()
{
	Enable = false;
	Luminance = 2.1f;
	Cutoff = 1.5f;
	Exp = 0.5f;
	Density = 0.2f;
	Weight = 0.9f;
	Decay = 2.0f;
}

void GEW_Postprocess::LensDirtProperty::reset()
{
	Enable = false;
	Luminance = 2.34f;
	Cutoff = 2.0f;
	Exp = 1.5f;
	DirtExp = 3.0f;
	CoreExp = 1.0f;
	MapFileName[ 0 ] = 0;
}

GEW_Postprocess::LensDirtProperty::LensDirtProperty()
{
	reset();
}

void GEW_Postprocess::MotionBlurParameters::reset()
{
	Enable = false;
	Damp = 200.0f;
	Max = 0.1f;
}

GEW_Postprocess::MotionBlurParameters::MotionBlurParameters()
{
	reset();
}

GEW_Postprocess::HDRParameters::HDRParameters()
{
	reset( );
}

void GEW_Postprocess::HDRParameters::reset()
{
	Enable = false;
	WhiteCutoff = 2.5f;
	LuminanceMax = 0.3f;
	LuminanceMin = 1.0f;
	Gamma = 2.2f;
	AdaptationRate = 100.0f;
	strcpy_s( GradientMap, ( string( GE_TEXTURE_PATH ) + "RGBTable.dds" ).c_str( ) );
}

GEW_Postprocess::AOParameters::AOParameters()
{
	reset();
}

void GEW_Postprocess::AOParameters::reset()
{
	Enable = false;
	Radius = 0.0f;
	Depth = 0.0f;
	DistanceStart = GE_ConvertToUnit( 0.0f );
	DistanceEnd = GE_ConvertToUnit( 100.0f );
	Intensity = 2.0f;
	Normal = 0.1f;
}

GEW_Postprocess::GlowParameters::GlowParameters()
{
	reset();
}

void GEW_Postprocess::GlowParameters::reset()
{
	Enable = false;
	VScale = 2.0f;
	HScale = 2.0f;
	Luminance = 2.4f;
	Cutoff = 1.5f;
}

GEW_Postprocess::GlowMappedParameters::GlowMappedParameters( )
{
	reset(); 
}

void GEW_Postprocess::GlowMappedParameters::reset( )
{
	Enable = false;
	VScale = 2.0f;
	HScale = 2.0f;
	Luminance = 2.4f;
	Cutoff = 1.5f;
}

GEW_Postprocess::AtmosphereFogParameters::AtmosphereFogParameters()
{
	reset();
}

void GEW_Postprocess::AtmosphereFogParameters::reset()
{
	Enable = false;
	Color = GE_Vec3( 0.9f, 0.9f, 1.0f );
	Intensity = 1.0f;
	Depth_PositionFieldFalloffHardness.x = GE_ConvertToUnit( 4000.0f );
	Depth_PositionFieldFalloffHardness.y = GE_ConvertToUnit( 2000.0f );
	Depth_PositionFieldFalloffHardness.z = 0.0f;
	Depth_PositionFieldFalloffHardness.w = 0.0f;
	Height_PositionFieldFalloffHardness.x = GE_ConvertToUnit( 0.0f );
	Height_PositionFieldFalloffHardness.y = GE_ConvertToUnit( 2000.0f );
	Height_PositionFieldFalloffHardness.z = 0.0f;
	Height_PositionFieldFalloffHardness.w = 0.0f;
}
