#include "CurveEditPlugin.h"
#include "CurveEdit.h"
#include <QtPlugin>

CurveEditPlugin::CurveEditPlugin( QObject *parent )
    : QObject(parent), initialized(false)
{
}

void CurveEditPlugin::initialize( QDesignerFormEditorInterface * )
{
    if (initialized)
        return;

    initialized = true;
}

bool CurveEditPlugin::isInitialized() const
{
    return initialized;
}

QWidget *CurveEditPlugin::createWidget( QWidget *parent )
{
	return new CurveEdit( parent );
}

QString CurveEditPlugin::name() const
{
    return "CurveEdit";
}

QString CurveEditPlugin::group() const
{
    return "GEngine Custom Widgets";
}

QIcon CurveEditPlugin::icon() const
{
	return QIcon( ":/Resources/CurveEdit.png" );
}

QString CurveEditPlugin::toolTip() const
{
    return "Curve Edit with mouse control.";
}

QString CurveEditPlugin::whatsThis() const
{
    return toolTip();
}

bool CurveEditPlugin::isContainer() const
{
    return false;
}

QString CurveEditPlugin::domXml() const
{
    return 
		"<ui language=\"c++\">\n"
		"	<widget class=\"CurveEdit\" name=\"curveEdit\">\n"
		"		<property name=\"geometry\">\n"
		"			<rect>\n"
		"				<x>0</x>\n"
		"				<y>0</y>\n"
		"				<width>100</width>\n"
		"				<height>100</height>\n"
		"			</rect>\n"
		"		</property>\n"
		"	</widget>\n"
		"</ui>\n";
}

QString CurveEditPlugin::includeFile() const
{
    return "CurveEdit.h";
}


