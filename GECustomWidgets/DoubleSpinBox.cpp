#include "DoubleSpinBox.h"
#include <QtGui/QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>

DoubleSpinBox::DoubleSpinBox( QWidget *parent /*= 0 */ )
: QDoubleSpinBox( parent ), m_mouseGrabed( false ), m_accelerate( 1 )
{
}

void DoubleSpinBox::mousePressEvent( QMouseEvent *i_ev )
{
	if ( i_ev->buttons() & Qt::LeftButton )
	{
		setCursor( Qt::SizeVerCursor );
		m_mouseGrabed = true;
		m_lastY = QCursor::pos().y();
		i_ev->accept();
	}
}

void DoubleSpinBox::mouseReleaseEvent( QMouseEvent *i_ev )
{
	if ( m_mouseGrabed )
	{
		setCursor( Qt::ArrowCursor );
		m_mouseGrabed = false;
		i_ev->accept();
		emit editingFinished();
	}
}

void DoubleSpinBox::mouseMoveEvent( QMouseEvent *i_ev )
{
	Q_UNUSED( i_ev )
	if ( m_mouseGrabed )
	{
		int y = QCursor::pos().y();
		int dy = m_lastY - y;
		int maxY = QApplication::desktop()->height();
		m_lastY = y;

		if ( y == 0 )
		{
			QCursor::setPos( QCursor::pos().x(), maxY - 2 );
			m_lastY = maxY - 2;
		} else if ( y == maxY - 1 )
		{
			QCursor::setPos( QCursor::pos().x(), 1 );
			m_lastY = 1;
		}
		double val = dy * singleStep() * m_accelerate;
		setValue( value() + val );
	}
}

void DoubleSpinBox::keyPressEvent( QKeyEvent *i_ev )
{
	if ( i_ev->key() == Qt::Key_Shift )
	{
		m_accelerate = 10.0f;
		i_ev->accept();
	} else if ( i_ev->key() == Qt::Key_Control )
	{
		m_accelerate = 0.1f;
		i_ev->accept();
	} else
		QDoubleSpinBox::keyPressEvent( i_ev );
}

void DoubleSpinBox::keyReleaseEvent( QKeyEvent *i_ev )
{
	if ( i_ev->key() == Qt::Key_Shift || i_ev->key() == Qt::Key_Control )
	{
		m_accelerate = 1.0f;
		i_ev->accept();
	}else
		QDoubleSpinBox::keyReleaseEvent( i_ev );
}