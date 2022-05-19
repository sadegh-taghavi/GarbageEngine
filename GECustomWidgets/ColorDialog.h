#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include "ui_ColorDialog.h"

class ColorDialog : public QDialog, private Ui::ColorDialog
{
	Q_OBJECT

public:
	explicit ColorDialog( QWidget *parent = 0 );

	/**
	* Get currently selected color
	*/
	QColor color() const;

	QSize sizeHint() const;

	void setColorPallet( QList<QColor> *pallet );

public slots:
	/**
	* Change color
	*/
	void slotSetColor( QColor c );

signals:
	/**
	* Emitted when the user selects a color or setColor is called
	*/
	void signalColorChanged( QColor );

protected slots:
	/// Update all the Ui elements to match the selected color
	void slotUpdateWidgets();
	/// Update from HSV sliders
	void slotSetHsv();
	/// Update from RGB sliders
	void slotSetRgb();

private slots:
	void on_edit_hex_editingFinished();
	void on_edit_hex_textEdited( const QString &arg1 );

	void on_button_pick_clicked();

	void on_button_add_clicked();

	void on_button_remove_clicked();

	void on_listWidget_pallet_currentRowChanged( int currentRow );

	void on_listWidget_pallet_itemClicked( QListWidgetItem *item );

	void on_ColorDialog_accepted();

private:
	void updateHex();
	void getScreenColor( QPoint global_pos );

protected:
	void dragEnterEvent( QDragEnterEvent *event );
	void dropEvent( QDropEvent * event );
	void mouseReleaseEvent( QMouseEvent *event );
	void mouseMoveEvent( QMouseEvent *event );

private:
	bool m_pickFromScreen;
	bool m_topMust;
	QBrush m_backGroundBrush;
	QList<QColor> *m_colorPallet;
};

#endif // COLORDIALOG_H
