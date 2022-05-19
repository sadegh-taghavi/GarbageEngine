#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QListWidget>

class ListWidget : public QListWidget
{
	Q_OBJECT
public:
	explicit ListWidget( QWidget *parent = 0 );

signals:
	void signalKeyPressed( Qt::Key key );
	void signalItemOrderChanged( QList<QListWidgetItem*> &oldItems, QList<QListWidgetItem*> &newItems );

private:
	void keyPressEvent( QKeyEvent *event );
	void mouseMoveEvent( QMouseEvent *event );
	void mousePressEvent( QMouseEvent *event );
	void mouseReleaseEvent( QMouseEvent *event );
	void dropEvent( QDropEvent *event );
};

#endif