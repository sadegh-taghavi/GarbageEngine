#ifndef COLOR_SELECTOR_PLUGIN_HPP
#define COLOR_SELECTOR_PLUGIN_HPP

#include <QDesignerCustomWidgetInterface>

class ColorSelectorPlugin : public QObject, public QDesignerCustomWidgetInterface
{
	Q_OBJECT
		Q_INTERFACES( QDesignerCustomWidgetInterface )

public:
	ColorSelectorPlugin( QObject *parent = 0 );

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

#endif // COLOR_SELECTOR_PLUGIN_HPP
