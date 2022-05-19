#include "GradientSliderPlugin.h"
#include "GradientSlider.h"
#include <QtPlugin>

GradientSliderPlugin::GradientSliderPlugin( QObject *parent )
: QObject( parent ), initialized( false )
{
}


void GradientSliderPlugin::initialize( QDesignerFormEditorInterface * )
{
	if ( initialized )
		return;

	initialized = true;
}

bool GradientSliderPlugin::isInitialized() const
{
	return initialized;
}

QWidget *GradientSliderPlugin::createWidget( QWidget *parent )
{
	return new GradientSlider( parent );
}

QString GradientSliderPlugin::name() const
{
	return "GradientSlider";
}

QString GradientSliderPlugin::group() const
{
	return "GEngine Custom Widgets";
}

QIcon GradientSliderPlugin::icon() const
{
	GradientSlider w;
	w.resize( 64, 16 );
	QVector<QColor> cols;
	cols.push_back( Qt::green );
	cols.push_back( Qt::yellow );
	cols.push_back( Qt::red );
	w.setColors( cols );
	QPixmap pix( 64, 64 );
	pix.fill( Qt::transparent );
	w.render( &pix, QPoint( 0, 24 ) );
	return QIcon( pix );
}

QString GradientSliderPlugin::toolTip() const
{
	return "Slider over a gradient";
}

QString GradientSliderPlugin::whatsThis() const
{
	return toolTip();
}

bool GradientSliderPlugin::isContainer() const
{
	return false;
}

QString GradientSliderPlugin::domXml() const
{

	return 
		"<ui language=\"c++\">\n"
		"	<widget class=\"GradientSlider\" name=\"gradientSlider\">\n"
		"	</widget>\n"
		"</ui>\n";
}

QString GradientSliderPlugin::includeFile() const
{
	return "GradientSlider.h";
}


