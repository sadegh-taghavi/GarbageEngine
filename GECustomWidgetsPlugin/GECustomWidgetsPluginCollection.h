#ifndef COLOR_WIDGET_PLUGIN_COLLECTION_HPP
#define COLOR_WIDGET_PLUGIN_COLLECTION_HPP

#include <QDesignerCustomWidgetCollectionInterface>

class GECustomWidgetsPluginCollection : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface")
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

public:
    explicit GECustomWidgetsPluginCollection(QObject *parent = 0);

    QList<QDesignerCustomWidgetInterface*> customWidgets() const;

   private:
       QList<QDesignerCustomWidgetInterface*> widgets;
    
};

#endif // COLOR_WIDGET_PLUGIN_COLLECTION_HPP
