#ifndef GEDITOR_H
#define GEDITOR_H

#include <QtWidgets/QMainWindow>
#include "ui_geditor.h"

class QListWidgetItem;
class QToolButton;
class SpinBox;
class QAction;

class GEditor : public QMainWindow
{
	Q_OBJECT
	QString m_lastProject;
public:
	GEditor( QWidget *parent = 0 );
	~GEditor();
	void openFromCmd( QString &i_fileName );
private slots:
	void on_Viewport_signalRepaint();
	void on_Viewport_signalResize( int i_width, int i_height );
	void on_New_triggered();
	void on_Open_triggered();
	void on_Save_triggered();
	void on_SaveAs_triggered();
private:
	Ui::GEditorClass ui;
};

#endif // GEDITOR_H
