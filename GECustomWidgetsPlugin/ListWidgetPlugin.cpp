#include "ListWidgetPlugin.h"
#include "ListWidget.h"
#include <QtPlugin>

ListWidgetPlugin::ListWidgetPlugin( QObject *parent )
    : QObject(parent), initialized(false)
{
}


void ListWidgetPlugin::initialize( QDesignerFormEditorInterface * )
{
    if (initialized)
        return;

    initialized = true;
}

bool ListWidgetPlugin::isInitialized() const
{
    return initialized;
}

QWidget *ListWidgetPlugin::createWidget( QWidget *parent )
{
	return new ListWidget( parent );
}

QString ListWidgetPlugin::name() const
{
    return "ListWidget";
}

QString ListWidgetPlugin::group() const
{
    return "GEngine Custom Widgets";
}

QIcon ListWidgetPlugin::icon() const
{
	return QIcon( ":/Resources/ListWidget.png" );
}

QString ListWidgetPlugin::toolTip() const
{
    return "List widget with key pressed signal.";
}

QString ListWidgetPlugin::whatsThis() const
{
    return toolTip();
}

bool ListWidgetPlugin::isContainer() const
{
    return false;
}

QString ListWidgetPlugin::domXml() const
{

    return 
		"<ui language=\"c++\">\n"
        "	<widget class=\"ListWidget\" name=\"listWidget\">\n"
        "	</widget>\n"
        "</ui>\n";
}

QString ListWidgetPlugin::includeFile() const
{
    return "ListWidget.h";
}


