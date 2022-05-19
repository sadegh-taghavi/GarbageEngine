#include "ColorSelectorPlugin.h"
#include "ColorSelector.h"
#include <QtPlugin>

ColorSelectorPlugin::ColorSelectorPlugin( QObject *parent )
: QObject( parent ), initialized( false )
{
}

void ColorSelectorPlugin::initialize( QDesignerFormEditorInterface * )
{
	if ( initialized )
		return;

	initialized = true;
}

bool ColorSelectorPlugin::isInitialized() const
{
	return initialized;
}

QWidget *ColorSelectorPlugin::createWidget( QWidget *parent )
{
	return new ColorSelector( parent );
}

QString ColorSelectorPlugin::name() const
{
	return "ColorSelector";
}

QString ColorSelectorPlugin::group() const
{
	return "GEngine Custom Widgets";
}

QIcon ColorSelectorPlugin::icon() const
{
	ColorSelector w;
	w.resize( 64, 32 );
	w.slotSetColor( QColor( 255, 0, 0, 64 ) );
	w.setAlphaMode( ColorPreview::SplitAlpha );
	w.setViewMode( ColorPreview::LeftToRight );
	QPixmap pix( 64, 64 );
	pix.fill( Qt::transparent );
	w.render( &pix, QPoint( 0, 16) );
	return QIcon( pix );
}

QString ColorSelectorPlugin::toolTip() const
{
	return "Display a color and opens a color dialog on click";
}

QString ColorSelectorPlugin::whatsThis() const
{
	return toolTip();
}

bool ColorSelectorPlugin::isContainer() const
{
	return false;
}

QString ColorSelectorPlugin::domXml() const
{

	return
		"<ui language=\"c++\">\n"
		"	<widget class=\"ColorSelector\" name=\"colorSelector\">\n"
		"	</widget>\n"
		"</ui>\n";
}

QString ColorSelectorPlugin::includeFile() const
{
	return "ColorSelector.h";
}



