#include "PictureBoxPlugin.h"
#include "PictureBox.h"
#include <QtPlugin>

PictureBoxPlugin::PictureBoxPlugin(QObject *parent)
    : QObject(parent), initialized(false)
{
}


void PictureBoxPlugin::initialize(QDesignerFormEditorInterface *)
{
    if (initialized)
        return;

    initialized = true;
}

bool PictureBoxPlugin::isInitialized() const
{
    return initialized;
}

QWidget *PictureBoxPlugin::createWidget(QWidget *parent)
{
    return new PictureBox(parent);
}

QString PictureBoxPlugin::name() const
{
    return "PictureBox";
}

QString PictureBoxPlugin::group() const
{
    return "GEngine Custom Widgets";
}

QIcon PictureBoxPlugin::icon() const
{
	return QIcon( ":/Resources/PictureBox.png" );
}

QString PictureBoxPlugin::toolTip() const
{
    return "Supports DDS, TGA and so on.";
}

QString PictureBoxPlugin::whatsThis() const
{
    return toolTip();
}

bool PictureBoxPlugin::isContainer() const
{
    return false;
}

QString PictureBoxPlugin::domXml() const
{

    return 
		"<ui language=\"c++\">\n"
		"	<widget class=\"PictureBox\" name=\"pictureBox\">\n"
		"		<property name=\"geometry\">\n"
		"			<rect>\n"
		"				<x>0</x>\n"
		"				<y>0</y>\n"
		"				<width>100</width>\n"
		"				<height>100</height>\n"
		"			</rect>\n"
		"		</property>\n"
		"		<property name=\"sizePolicy\">\n"
		"			<sizepolicy hsizetype=\"Ignored\" vsizetype=\"Ignored\">\n"
		"				<horstretch>0</horstretch>\n"
		"				<verstretch>0</verstretch>\n"
		"			</sizepolicy>\n"
		"		</property>\n"
		"		<property name=\"acceptDrops\">\n"
		"			<bool>true</bool>\n"
		"		</property>\n"
		"		<property name=\"frameShape\">\n"
		"			<enum>QFrame::Box</enum>\n"
		"		</property>\n"
		"		<property name = \"scaledContents\">\n"
		"			<bool>true</bool>\n"
		"		</property>\n"
		"	</widget>\n"
		"</ui>\n";
}

QString PictureBoxPlugin::includeFile() const
{
    return "PictureBox.h";
}


