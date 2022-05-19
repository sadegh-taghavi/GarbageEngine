#ifndef GGLYPHMAKER_H
#define GGLYPHMAKER_H
#include <stdint.h>
#include <QtWidgets/QMainWindow>
#include <QFontDialog>
#include <QColorDialog>
#include <QFileDialog>
#include <QGraphicsView>
#include <QFile>
#include "ui_gglyphmaker.h"

class GGlyphMaker : public QMainWindow
{
	Q_OBJECT

public:
	GGlyphMaker(QWidget *parent = 0);
	~GGlyphMaker();

private slots:

	void on_actionFont_triggered();

	void on_actionBackground_color_triggered();

	void on_actionTransparent_background_triggered( bool checked );

	void on_actionForeground_color_triggered();

	void on_actionExit_triggered();

	void on_actionSave_triggered();

	void on_glyphSize_currentIndexChanged( int index );

	void on_actionAuto_font_size_triggered( bool checked );
		
private:
	void reDraw();
	Ui::GGlyphMakerClass		ui;
	QFontDialog					*m_fontDialog;
	QColorDialog				m_colorDialog;
	QString						m_saveFile;
	QBrush						m_trasparentBrush;
	QColor						m_backgroundColor;
	QColor						m_foregroundColor;
	uint						m_size;
	QGraphicsScene				m_scene;
	QList<QGraphicsTextItem *>	m_items;
	bool						m_autoFontSize;
	float						m_fontSize;
};

#endif // GGLYPHMAKER_H
