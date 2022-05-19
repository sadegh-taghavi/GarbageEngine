#include "PxMaterialViewPlugin.h"
#include "PxMaterialView.h"
#include <QtPlugin>

PxMaterialViewPlugin::PxMaterialViewPlugin( QObject *parent )
    : QObject(parent), initialized(false)
{
}

void PxMaterialViewPlugin::initialize( QDesignerFormEditorInterface * )
{
    if (initialized)
        return;

    initialized = true;
}

bool PxMaterialViewPlugin::isInitialized() const
{
    return initialized;
}

QWidget *PxMaterialViewPlugin::createWidget( QWidget *parent )
{
	return new PxMaterialView( parent );
}

QString PxMaterialViewPlugin::name() const
{
    return "PxMaterialView";
}

QString PxMaterialViewPlugin::group() const
{
    return "GEngine Custom Widgets";
}

QIcon PxMaterialViewPlugin::icon() const
{
	return QIcon( ":/Resources/MetroStyle/poo.png" );
}

QString PxMaterialViewPlugin::toolTip() const
{
    return "Physics Material viewer.";
}

QString PxMaterialViewPlugin::whatsThis() const
{
    return toolTip();
}

bool PxMaterialViewPlugin::isContainer() const
{
    return false;
}

QString PxMaterialViewPlugin::domXml() const
{
    return 
		"<ui language=\"c++\">\n"
        "	<widget class=\"PxMaterialView\" name=\"pxMaterialView\">\n"
        "	</widget>\n"
        "</ui>\n";
}

QString PxMaterialViewPlugin::includeFile() const
{
    return "PxMaterialView.h";
}


