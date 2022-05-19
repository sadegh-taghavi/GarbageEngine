#include "SpinBoxPlugin.h"
#include "SpinBox.h"
#include <QtPlugin>

SpinBoxPlugin::SpinBoxPlugin( QObject *parent )
    : QObject(parent), initialized(false)
{
}


void SpinBoxPlugin::initialize( QDesignerFormEditorInterface * )
{
    if (initialized)
        return;

    initialized = true;
}

bool SpinBoxPlugin::isInitialized() const
{
    return initialized;
}

QWidget *SpinBoxPlugin::createWidget( QWidget *parent )
{
	return new SpinBox( parent );
}

QString SpinBoxPlugin::name() const
{
    return "SpinBox";
}

QString SpinBoxPlugin::group() const
{
    return "GEngine Custom Widgets";
}

QIcon SpinBoxPlugin::icon() const
{
	return QIcon( ":/Resources/SpinBox.png" );
}

QString SpinBoxPlugin::toolTip() const
{
    return "Spin box with mouse control.";
}

QString SpinBoxPlugin::whatsThis() const
{
    return toolTip();
}

bool SpinBoxPlugin::isContainer() const
{
    return false;
}

QString SpinBoxPlugin::domXml() const
{

    return "<ui language=\"c++\">\n"
           " <widget class=\"SpinBox\" name=\"spinBox\">\n"
           " </widget>\n"
            "</ui>\n";
}

QString SpinBoxPlugin::includeFile() const
{
    return "SpinBox.h";
}


