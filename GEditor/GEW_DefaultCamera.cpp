#include "GEW_DefaultCamera.h"
#include <GE_Math.h>
#include <QSettings>
#include "GEW_Path.h"

GEW_DefaultCamera::GEW_DefaultCamera()
{
}


GEW_DefaultCamera::~GEW_DefaultCamera()
{
}

GEW_DefaultCamera GEW_DefaultCamera::m_singleton;

void GEW_DefaultCamera::Settings::reset()
{
	Near = GE_ConvertToUnit( 0.3f );
	Far = GE_ConvertToUnit( 4000.0f );
	FOV = GE_ConvertToRadians( 60.0f );
}

GEW_DefaultCamera::Settings::Settings()
{
	QSettings settings( GEW_SETTINGS_FILE, QSettings::IniFormat );
	if ( settings.value( "DefaultCameraFOV" ).isValid() )
	{
		FOV = settings.value( "DefaultCameraFOV" ).toFloat();
		Near = settings.value( "DefaultCameraNear", Near ).toFloat();
		Far = settings.value( "DefaultCameraFar", Far ).toFloat();
	}
	else
		reset();
}

GEW_DefaultCamera::Settings::~Settings()
{
	QSettings settings( GEW_SETTINGS_FILE, QSettings::IniFormat );
	settings.setValue( "DefaultCameraFOV", FOV );
	settings.setValue( "DefaultCameraNear", Near );
	settings.setValue( "DefaultCameraFar", Far );
}
