#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QTreeWidget>

class TreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	explicit TreeWidget( QWidget *parent = 0 );

signals:
	void signalKeyPressed( Qt::Key key );

private:
	void keyPressEvent( QKeyEvent *event );
	void mouseMoveEvent( QMouseEvent *event );
	void mousePressEvent( QMouseEvent *event );
	void mouseReleaseEvent( QMouseEvent *event );
};

#endif