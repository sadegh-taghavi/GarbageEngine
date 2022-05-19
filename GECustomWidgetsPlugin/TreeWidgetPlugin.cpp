#include "TreeWidgetPlugin.h"
#include "TreeWidget.h"
#include <QtPlugin>

TreeWidgetPlugin::TreeWidgetPlugin( QObject *parent )
    : QObject(parent), initialized(false)
{
}


void TreeWidgetPlugin::initialize( QDesignerFormEditorInterface * )
{
    if (initialized)
        return;

    initialized = true;
}

bool TreeWidgetPlugin::isInitialized() const
{
    return initialized;
}

QWidget *TreeWidgetPlugin::createWidget( QWidget *parent )
{
	return new TreeWidget( parent );
}

QString TreeWidgetPlugin::name() const
{
    return "TreeWidget";
}

QString TreeWidgetPlugin::group() const
{
    return "GEngine Custom Widgets";
}

QIcon TreeWidgetPlugin::icon() const
{
	return QIcon( ":/Resources/TreeWidget.png" );
}

QString TreeWidgetPlugin::toolTip() const
{
    return "Tree widget with key pressed signal.";
}

QString TreeWidgetPlugin::whatsThis() const
{
    return toolTip();
}

bool TreeWidgetPlugin::isContainer() const
{
    return false;
}

QString TreeWidgetPlugin::domXml() const
{

    return 
		"<ui language=\"c++\">\n"
        "	<widget class=\"TreeWidget\" name=\"treeWidget\">\n"
        "	</widget>\n"
        "</ui>\n";
}

QString TreeWidgetPlugin::includeFile() const
{
    return "TreeWidget.h";
}


