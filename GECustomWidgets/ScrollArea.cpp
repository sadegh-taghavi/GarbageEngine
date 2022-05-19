#include "ScrollArea.h"
#include <QtGui/QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QScrollBar>
#include <QChildEvent>

ScrollArea::ScrollArea( QWidget *parent /*= 0 */ )
: QScrollArea( parent ), 
m_mouseGrabed( false )
{
}

void ScrollArea::mousePressEvent( QMouseEvent *i_ev )
{
	if ( i_ev->buttons() & Qt::MidButton )
	{
		setCursor( Qt::ClosedHandCursor );
		m_mouseGrabed = true;
		m_lastPos = QCursor::pos();
		i_ev->accept();
	} else
		QScrollArea::mousePressEvent( i_ev );
}

void ScrollArea::mouseReleaseEvent( QMouseEvent *i_ev )
{
	if ( m_mouseGrabed )
	{
		setCursor( Qt::ArrowCursor );
		m_mouseGrabed = false;
		i_ev->accept();
	} else
		QScrollArea::mouseReleaseEvent( i_ev );
}

void ScrollArea::mouseMoveEvent( QMouseEvent *i_ev )
{
	if ( m_mouseGrabed )
	{
		QPoint newPos = QCursor::pos();
		int maxY = QApplication::desktop()->height();
		int maxX = QApplication::desktop()->width();
		QPoint dXY = newPos - m_lastPos;
		m_lastPos = newPos;

		if ( newPos.y() == 0 )
		{
			QCursor::setPos( QCursor::pos().x(), maxY - 2 );
			m_lastPos.setY( maxY - 2 );
		} else if ( newPos.y() == maxY - 1 )
		{
			QCursor::setPos( QCursor::pos().x(), 1 );
			m_lastPos.setY( 1 );
		}

		if ( newPos.x() == 0 )
		{
			QCursor::setPos( maxX - 2, QCursor::pos().y() );
			m_lastPos.setX( maxX - 2 );
		} else if ( newPos.x() == maxX - 1 )
		{
			QCursor::setPos( 1, QCursor::pos().y() );
			m_lastPos.setX( 1 );
		}

		dXY *= -1.0;
		verticalScrollBar()->setSliderPosition( verticalScrollBar()->sliderPosition() + dXY.y() );
		horizontalScrollBar()->setSliderPosition( horizontalScrollBar()->sliderPosition() + dXY.x() );
		i_ev->accept();
	} else
		QScrollArea::mouseMoveEvent( i_ev );
}

void ScrollArea::childEvent( QChildEvent * event )
{
	QScrollArea::changeEvent( event );
}
