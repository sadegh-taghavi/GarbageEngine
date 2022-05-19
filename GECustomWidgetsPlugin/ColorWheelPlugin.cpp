#include "ColorWheelPlugin.h"
#include "ColorWheel.h"

ColorWheelPlugin::ColorWheelPlugin( QObject *parent ) :
QObject( parent ), initialized( false )
{
}

void ColorWheelPlugin::initialize( QDesignerFormEditorInterface * )
{
	initialized = true;
}

bool ColorWheelPlugin::isInitialized() const
{
	return initialized;
}

QWidget *ColorWheelPlugin::createWidget( QWidget *parent )
{
	return new ColorWheel( parent );
}

QString ColorWheelPlugin::name() const
{
	return "ColorWheel";
}

QString ColorWheelPlugin::group() const
{
	return "GEngine Custom Widgets";
}

QIcon ColorWheelPlugin::icon() const
{
	ColorWheel w;
	w.resize( 64, 64 );
	w.setWheelWidth( 8 );
	QPixmap pix( 64, 64 );
	w.render( &pix );
	return QIcon( pix );
}

QString ColorWheelPlugin::toolTip() const
{
	return "Color selector";
}

QString ColorWheelPlugin::whatsThis() const
{
	return "A widget that allows an intuitive selection of HSL parameters for a QColor";
}

bool ColorWheelPlugin::isContainer() const
{
	return false;
}

QString ColorWheelPlugin::domXml() const
{
	return 
		"<ui language=\"c++\">\n"
		"	<widget class=\"ColorWheel\" name=\"colorWheel\">\n"
		"		<property name=\"sizePolicy\">\n"
		"			<sizepolicy hsizetype=\"Minimum\" vsizetype=\"Minimum\">\n"
		"				<horstretch>0</horstretch>\n"
		"				<verstretch>0</verstretch>\n"
		"			</sizepolicy>\n"
		"		</property>\n"
		"	</widget>\n"
		"</ui>\n";
}

QString ColorWheelPlugin::includeFile() const
{
	return "ColorWheel.h";
}


