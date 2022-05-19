#ifndef COLORSELECTOR_H
#define COLORSELECTOR_H

#include "ColorPreview.h"

/**
* Color preview that opens a color dialog
*/
class ColorSelector : public ColorPreview
{
	Q_OBJECT
		Q_ENUMS( UpdateMode )
		Q_PROPERTY( UpdateMode updateMode READ updateMode WRITE setUpdateMode )
		Q_PROPERTY( Qt::WindowModality dialogModality READ dialogModality WRITE setDialogModality )

public:
	enum UpdateMode
	{
		Confirm, ///< Update color only after the dialog has been accepted
		Continuous ///< Update color as it's being modified in the dialog
	};

private:
	UpdateMode  m_updateMode;
	class ColorDialog* m_dialog;
	QColor m_oldColor;
	static QList<QColor> m_colorPallet;

public:
	explicit ColorSelector( QWidget *parent = 0 );

	void setUpdateMode( UpdateMode m );
	UpdateMode updateMode() const { return m_updateMode; }

	Qt::WindowModality dialogModality() const;
	void setDialogModality( Qt::WindowModality m );

public slots:
	void slotShowDialog();

private:
	/// Connect/Disconnect colorChanged based on Update_Mode
	void connectDialog();

	/// Disconnect from dialog update
	void disconnectDialog();

private slots:
	void slotAcceptDialog();
	void slotRejectDialog();
	void slotUpdateOldColor( QColor c );

protected:
	void dragEnterEvent( QDragEnterEvent *event );
	void dropEvent( QDropEvent * event );

};

#endif // COLORSELECTOR_H
