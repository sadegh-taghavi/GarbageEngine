#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QScrollArea>

class ScrollArea : public QScrollArea
{
	Q_OBJECT
	bool	m_mouseGrabed;
	QPoint	m_lastPos;
public:
	explicit ScrollArea( QWidget *parent = 0 );

protected:
	virtual void childEvent( QChildEvent * event );

private:
	void mousePressEvent( QMouseEvent *i_ev );
	void mouseReleaseEvent( QMouseEvent *i_ev );
	void mouseMoveEvent( QMouseEvent *i_ev );
};

#endif