#include "ColorPreviewPlugin.h"
#include "ColorPreview.h"
#include <QtPlugin>

ColorPreviewPlugin::ColorPreviewPlugin( QObject *parent )
: QObject( parent ), initialized( false )
{
}


void ColorPreviewPlugin::initialize( QDesignerFormEditorInterface * )
{
	if ( initialized )
		return;

	initialized = true;
}

bool ColorPreviewPlugin::isInitialized() const
{
	return initialized;
}

QWidget *ColorPreviewPlugin::createWidget( QWidget *parent )
{
	return new ColorPreview( parent );
}

QString ColorPreviewPlugin::name() const
{
	return "ColorPreview";
}

QString ColorPreviewPlugin::group() const
{
	return "GEngine Custom Widgets";
}

QIcon ColorPreviewPlugin::icon() const
{
	ColorPreview w;
	w.resize( 64, 32 );
	w.slotSetColor( QColor( 255, 0, 0, 64 ) );
	w.setAlphaMode( ColorPreview::SplitAlpha );
	w.setViewMode( ColorPreview::LeftToRight );
	QPixmap pix( 64, 64 );
	pix.fill( Qt::transparent );
	w.render( &pix, QPoint( 0, 16 ) );
	return QIcon( pix );
}

QString ColorPreviewPlugin::toolTip() const
{
	return "Display a color";
}

QString ColorPreviewPlugin::whatsThis() const
{
	return toolTip();
}

bool ColorPreviewPlugin::isContainer() const
{
	return false;
}

QString ColorPreviewPlugin::domXml() const
{

	return 
		"<ui language=\"c++\">\n"
		"	<widget class=\"ColorPreview\" name=\"colorPreview\">\n"
		"	</widget>\n"
		"</ui>\n";
}

QString ColorPreviewPlugin::includeFile() const
{
	return "ColorPreview.h";
}

//Q_EXPORT_PLUGIN2(color_widgets, Color_Preview_Plugin);



