#ifndef COLOR_WHEEL_PLUGIN_H
#define COLOR_WHEEL_PLUGIN_H

#include <QObject>
#include <QDesignerCustomWidgetInterface>

class ColorWheelPlugin : public QObject, public QDesignerCustomWidgetInterface
{
	Q_OBJECT
		Q_INTERFACES( QDesignerCustomWidgetInterface )

public:
	ColorWheelPlugin( QObject *parent = 0 );

	void initialize( QDesignerFormEditorInterface *core );
	bool isInitialized() const;

	QWidget *createWidget( QWidget *parent );

	QString name() const;
	QString group() const;
	QIcon icon() const;
	QString toolTip() const;
	QString whatsThis() const;
	bool isContainer() const;

	QString domXml() const;

	QString includeFile() const;

private:
	bool initialized;
};


#endif // COLOR_WHEEL_PLUGIN_HPP
