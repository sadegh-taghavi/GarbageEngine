#include "GECustomWidgetsPluginCollection.h"
#include "PictureBoxPlugin.h"
#include "DoubleSpinBoxPlugin.h"
#include "SpinBoxPlugin.h"
#include "ScrollAreaPlugin.h"
#include "CurveEditPlugin.h"
#include "ColorWheelPlugin.h"
#include "GradientSliderPlugin.h"
#include "ColorPreviewPlugin.h"
#include "ColorSelectorPlugin.h"
#include "GroupBoxPlugin.h"
#include "ListWidgetPlugin.h"
#include "TreeWidgetPlugin.h"
#include "PxMaterialViewPlugin.h"

GECustomWidgetsPluginCollection::GECustomWidgetsPluginCollection( QObject *parent ) :
QObject( parent )
{
	widgets.push_back( new PictureBoxPlugin( this ) );
	widgets.push_back( new DoubleSpinBoxPlugin( this ) );
	widgets.push_back( new SpinBoxPlugin( this ) );
	widgets.push_back( new ScrollAreaPlugin( this ) );
	widgets.push_back( new CurveEditPlugin( this ) );
	widgets.push_back( new ColorWheelPlugin( this ) );
	widgets.push_back( new GradientSliderPlugin( this ) );
	widgets.push_back( new ColorPreviewPlugin( this ) );
	widgets.push_back( new ColorSelectorPlugin( this ) );
	widgets.push_back( new GroupBoxPlugin( this ) );
	widgets.push_back( new ListWidgetPlugin( this ) );
	widgets.push_back( new TreeWidgetPlugin( this ) );
	widgets.push_back( new PxMaterialViewPlugin( this ) );
}

QList<QDesignerCustomWidgetInterface *> GECustomWidgetsPluginCollection::customWidgets() const
{
	return widgets;
}
