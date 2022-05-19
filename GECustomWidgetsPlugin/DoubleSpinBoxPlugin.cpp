#include "DoubleSpinBoxPlugin.h"
#include "DoubleSpinBox.h"
#include <QtPlugin>

DoubleSpinBoxPlugin::DoubleSpinBoxPlugin( QObject *parent )
    : QObject(parent), initialized(false)
{
}


void DoubleSpinBoxPlugin::initialize( QDesignerFormEditorInterface * )
{
    if (initialized)
        return;

    initialized = true;
}

bool DoubleSpinBoxPlugin::isInitialized() const
{
    return initialized;
}

QWidget *DoubleSpinBoxPlugin::createWidget( QWidget *parent )
{
	return new DoubleSpinBox( parent );
}

QString DoubleSpinBoxPlugin::name() const
{
    return "DoubleSpinBox";
}

QString DoubleSpinBoxPlugin::group() const
{
    return "GEngine Custom Widgets";
}

QIcon DoubleSpinBoxPlugin::icon() const
{
	return QIcon( ":/Resources/DoubleSpinBox.png" );
}

QString DoubleSpinBoxPlugin::toolTip() const
{
    return "Double spin box with mouse control.";
}

QString DoubleSpinBoxPlugin::whatsThis() const
{
    return toolTip();
}

bool DoubleSpinBoxPlugin::isContainer() const
{
    return false;
}

QString DoubleSpinBoxPlugin::domXml() const
{

    return "<ui language=\"c++\">\n"
           " <widget class=\"DoubleSpinBox\" name=\"doubleSpinBox\">\n"
           " </widget>\n"
            "</ui>\n";
}

QString DoubleSpinBoxPlugin::includeFile() const
{
    return "DoubleSpinBox.h";
}


