#include "ScrollAreaPlugin.h"
#include "ScrollArea.h"
#include <QtPlugin>

ScrollAreaPlugin::ScrollAreaPlugin( QObject *parent )
    : QObject(parent), initialized(false)
{
}

void ScrollAreaPlugin::initialize( QDesignerFormEditorInterface * )
{
    if (initialized)
        return;

    initialized = true;
}

bool ScrollAreaPlugin::isInitialized() const
{
    return initialized;
}

QWidget *ScrollAreaPlugin::createWidget( QWidget *parent )
{
	return new ScrollArea( parent );
}

QString ScrollAreaPlugin::name() const
{
    return "ScrollArea";
}

QString ScrollAreaPlugin::group() const
{
    return "GEngine Custom Widgets";
}

QIcon ScrollAreaPlugin::icon() const
{
	return QIcon( ":/Resources/ScrollArea.png" );
}

QString ScrollAreaPlugin::toolTip() const
{
    return "Scroll Area with mouse control.";
}

QString ScrollAreaPlugin::whatsThis() const
{
    return toolTip();
}

bool ScrollAreaPlugin::isContainer() const
{
    return true;
}

QString ScrollAreaPlugin::domXml() const
{
	return
		"<ui language=\"c++\">\n"
		"	<widget class=\"ScrollArea\" name=\"scrollArea\">\n"
		"		<property name=\"geometry\">\n"
		"			<rect>\n"
		"				<x>90</x>\n"
		"				<y>110</y>\n"
		"				<width>171</width>\n"
		"				<height>131</height>\n"
		"			</rect>\n"
		"		</property>\n"
		"		<property name=\"widgetResizable\">\n"
		"			<bool>true</bool>\n"
		"		</property>\n"
		"		<widget class=\"QWidget\" name=\"scrollAreaWidgetContents\">\n"
		"			<property name=\"geometry\">\n"
		"				<rect>\n"
		"					<x>0</x>\n"
		"					<y>0</y>\n"
		"					<width>169</width>\n"
		"					<height>129</height>\n"
		"				</rect>\n"
		"			</property>\n"
		"		</widget>\n"
		"	</widget>\n"
		"</ui>\n";
}

QString ScrollAreaPlugin::includeFile() const
{
    return "ScrollArea.h";
}


