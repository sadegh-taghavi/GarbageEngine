#include "GEW_Viewport.h"
#include <GE_Core.h>
#include <QPaintEvent>
#include <QResizeEvent>
#include "GEW_Scene.h"

GEW_Viewport::GEW_Viewport( QWidget *i_parent /*= 0 */ )
: QWidget( i_parent ),
m_timerId( -1 )
{
	setAttribute( Qt::WA_OpaquePaintEvent, true );
	setAttribute( Qt::WA_PaintOnScreen, true );
	setFocusPolicy( Qt::ClickFocus );
	setContextMenuPolicy( Qt::NoContextMenu );

	m_lastPos.setX( 0 );
	m_lastPos.setY( 0 );
	m_swapChain = new GE_SwapChain;
	m_swapChain->ViewPort.Width = 320;
	m_swapChain->ViewPort.Height = 240;
	m_swapChain->Hwnd = ( HWND ) winId();
	m_swapChain->Windowed = true;
	m_swapChain->ClearColor = GE_Vec4( 0.0f, 0.0f, 0.0f, 1.0f );

	GE_Core::getSingleton().createSwapChain( *m_swapChain );

	GEW_Scene::getSingleton().setViewPort( this );
}

GEW_Viewport::~GEW_Viewport()
{
	delete m_swapChain;
}

void GEW_Viewport::paintEvent( QPaintEvent *i_ev )
{
	emit signalRepaint();
	i_ev->accept();
}

void GEW_Viewport::resizeEvent( QResizeEvent *i_ev )
{
	if( i_ev->size().width() > 3 && i_ev->size().height() > 3 )
	{
		m_swapChain->ViewPort.Width = i_ev->size().width();
		m_swapChain->ViewPort.Height = i_ev->size().height();
		m_swapChain->resize();
		emit signalResize( i_ev->size().width(), i_ev->size().height() );
	}
	i_ev->accept();
}

void GEW_Viewport::mousePressEvent( QMouseEvent *i_ev )
{
	m_inputData.MButtons |= i_ev->button();
	m_lastPos = QCursor::pos();
	i_ev->accept();
}

void GEW_Viewport::mouseReleaseEvent( QMouseEvent *i_ev )
{
	m_inputData.MButtons &= ~i_ev->button();
	m_lastPos.setX( 0 );
	m_lastPos.setY( 0 );
	i_ev->accept();
}

void GEW_Viewport::mouseMoveEvent( QMouseEvent *i_ev )
{
	//m_inputData.X = QCursor::pos().x();
	//m_inputData.X = QCursor::pos().y();
	i_ev->accept();
}

void GEW_Viewport::wheelEvent( QWheelEvent *i_ev )
{
	m_inputData.Dw = i_ev->delta();
	i_ev->accept();
}

void GEW_Viewport::keyPressEvent( QKeyEvent *i_ev )
{
	switch( i_ev->key() )
	{
	case Qt::Key::Key_Z:
		m_inputData.Keyboard[ GUK_Z ] = true;		break;
	case Qt::Key::Key_X:
		m_inputData.Keyboard[ GUK_X ] = true;		break;
	case Qt::Key::Key_N:
		m_inputData.Keyboard[ GUK_N ] = true;		break;
	case Qt::Key::Key_M:
		m_inputData.Keyboard[ GUK_M ] = true;		break;
	case Qt::Key::Key_Up:
		m_inputData.Keyboard[ GUK_UP ] = true;		break;
	case Qt::Key::Key_Down:
		m_inputData.Keyboard[ GUK_DOWN ] = true;	break;
	case Qt::Key::Key_Left:
		m_inputData.Keyboard[ GUK_LEFT ] = true;	break;
	case Qt::Key::Key_Right:
		m_inputData.Keyboard[ GUK_RIGHT ] = true;	break;
	case Qt::Key::Key_Space:
		m_inputData.Keyboard[ GUK_SPACE ] = true;	break;
	case Qt::Key::Key_W:
		m_inputData.Keyboard[ GUK_W ] = true;		break;
	case Qt::Key::Key_A:
		m_inputData.Keyboard[ GUK_A ] = true;		break;
	case Qt::Key::Key_D:
		m_inputData.Keyboard[ GUK_D ] = true;		break;
	case Qt::Key::Key_S:
		m_inputData.Keyboard[ GUK_S ] = true;		break;
	case Qt::Key::Key_Shift:
		m_inputData.Keyboard[ GUK_LSHIFT ] = true;	break;
	case Qt::Key::Key_Control:
		m_inputData.Keyboard[GUK_LCONTROL] = true;	break;
	case Qt::Key::Key_Alt:
		m_inputData.Keyboard[GUK_LALT] = true;	break;
	case Qt::Key::Key_PageUp:
		m_inputData.Keyboard[ GUK_PGUP ] = true;	break;
	case Qt::Key::Key_PageDown:
		m_inputData.Keyboard[ GUK_PGDN ] = true;	break;
	case Qt::Key::Key_Plus:
		m_inputData.Keyboard[ GUK_NUMPADPLUS ] = true;	break;
	case Qt::Key::Key_Minus:
		m_inputData.Keyboard[ GUK_NUMPADMINUS ] = true;	break;
	}
	i_ev->accept();
}

void GEW_Viewport::keyReleaseEvent( QKeyEvent *i_ev )
{
	switch( i_ev->key() )
	{
	case Qt::Key::Key_Z:
		m_inputData.Keyboard[ GUK_Z ] = false;		break;
	case Qt::Key::Key_X:
		m_inputData.Keyboard[ GUK_X ] = false;		break;
	case Qt::Key::Key_N:
		m_inputData.Keyboard[ GUK_N ] = false;		break;
	case Qt::Key::Key_M:
		m_inputData.Keyboard[ GUK_M ] = false;		break;
	case Qt::Key::Key_Up:
		m_inputData.Keyboard[ GUK_UP ] = false;		break;
	case Qt::Key::Key_Down:
		m_inputData.Keyboard[ GUK_DOWN ] = false;	break;
	case Qt::Key::Key_Left:
		m_inputData.Keyboard[ GUK_LEFT ] = false;	break;
	case Qt::Key::Key_Right:
		m_inputData.Keyboard[ GUK_RIGHT ] = false;	break;
	case Qt::Key::Key_Space:
		m_inputData.Keyboard[ GUK_SPACE ] = false;	break;
	case Qt::Key::Key_W:
		m_inputData.Keyboard[ GUK_W ] = false;		break;
	case Qt::Key::Key_A:
		m_inputData.Keyboard[ GUK_A ] = false;		break;
	case Qt::Key::Key_D:
		m_inputData.Keyboard[ GUK_D ] = false;		break;
	case Qt::Key::Key_S:
		m_inputData.Keyboard[ GUK_S ] = false;		break;
	case Qt::Key::Key_Shift:
		m_inputData.Keyboard[ GUK_LSHIFT ] = false;	break;
	case Qt::Key::Key_Control:
		m_inputData.Keyboard[GUK_LCONTROL] = false;	break;
	case Qt::Key::Key_Alt:
		m_inputData.Keyboard[GUK_LALT] = false;	break;
	case Qt::Key::Key_PageUp:
		m_inputData.Keyboard[ GUK_PGUP ] = false;	break;
	case Qt::Key::Key_PageDown:
		m_inputData.Keyboard[ GUK_PGDN ] = false;	break;
	case Qt::Key::Key_Plus:
		m_inputData.Keyboard[ GUK_NUMPADPLUS ] = false;	break;
	case Qt::Key::Key_Minus:
		m_inputData.Keyboard[ GUK_NUMPADMINUS ] = false; break;
	}
	i_ev->accept();
}

void GEW_Viewport::timerEvent( QTimerEvent *i_ev )
{
	if( i_ev->timerId() == m_timerId )
	{
		i_ev->accept();
		update();
	} else
		QWidget::timerEvent( i_ev );
}

void GEW_Viewport::setFps( float i_fps )
{
	if( m_timerId != -1 )
		killTimer( m_timerId );

	if( i_fps == 0 )
		m_timerId = startTimer( 0, Qt::VeryCoarseTimer );
	else if( i_fps != -1.0f )
		m_timerId = startTimer( ( int ) ( 1000.0f / i_fps ), Qt::VeryCoarseTimer );
}

void GEW_Viewport::updateMouse()
{
	if( m_lastPos.x() == 0 || m_lastPos.y() == 0 )
		return;

	QPoint p = QCursor::pos();

	m_inputData.Dx = p.x() - m_lastPos.x();
	m_inputData.Dy = p.y() - m_lastPos.y();
	m_inputData.X += m_inputData.Dx;
	m_inputData.Y += m_inputData.Dy;

	if (m_inputData.MButtons & GUM_BUTTON_RIGHT || m_inputData.MButtons & GUM_BUTTON_MIDDLE)
		QCursor::setPos(m_lastPos);
}

void GEW_Viewport::update()
{
	if( width() < 5 || height() < 5 )
		return;
	updateMouse();
	emit signalRepaint();
	m_inputData.Dw = 0;
}
