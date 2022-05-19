#include "GroupBoxPlugin.h"
#include "GroupBox.h"
#include <QtPlugin>

GroupBoxPlugin::GroupBoxPlugin( QObject *parent )
    : QObject(parent), initialized(false)
{
}

void GroupBoxPlugin::initialize( QDesignerFormEditorInterface * )
{
    if (initialized)
        return;

    initialized = true;
}

bool GroupBoxPlugin::isInitialized() const
{
    return initialized;
}

QWidget *GroupBoxPlugin::createWidget( QWidget *parent )
{
	return new GroupBox( parent );
}

QString GroupBoxPlugin::name() const
{
    return "GroupBox";
}

QString GroupBoxPlugin::group() const
{
    return "GEngine Custom Widgets";
}

QIcon GroupBoxPlugin::icon() const
{
	return QIcon( ":/Resources/GroupBox.png" );
}

QString GroupBoxPlugin::toolTip() const
{
    return "Group box dosn't block mouse move event.";
}

QString GroupBoxPlugin::whatsThis() const
{
    return toolTip();
}

bool GroupBoxPlugin::isContainer() const
{
    return true;
}

QString GroupBoxPlugin::domXml() const
{
	return
		"<ui language=\"c++\">\n"
		"	<widget class=\"GroupBox\" name=\"groupBox\">\n"
		"		<property name=\"geometry\">\n"
		"			<rect>\n"
		"				<x>90</x>\n"
		"				<y>110</y>\n"
		"				<width>171</width>\n"
		"				<height>131</height>\n"
		"			</rect>\n"
		"		</property>\n"
		"		<property name=\"title\">\n"
		"			<string>GroupBox</string>\n"
		"		</property>\n"
		"	</widget>\n"
		"</ui>\n";
}

QString GroupBoxPlugin::includeFile() const
{
    return "GroupBox.h";
}


