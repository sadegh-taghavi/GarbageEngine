#include "GEW_Viewport.h"
#include <GE_Core.h>
#include <QPaintEvent>
#include <QResizeEvent>

GEW_Viewport::GEW_Viewport( QWidget *i_parent /*= 0 */ )
: QWidget( i_parent )
{
	setAttribute( Qt::WA_OpaquePaintEvent, true );
	setAttribute( Qt::WA_PaintOnScreen, true );
	setFocusPolicy( Qt::ClickFocus );
	setContextMenuPolicy( Qt::NoContextMenu );

	m_swapChain = new GE_SwapChain;
	m_swapChain->ViewPort.Width = 1024;
	m_swapChain->ViewPort.Height = 768;
	m_swapChain->Hwnd = ( HWND ) winId();
	m_swapChain->Windowed = true;
	m_swapChain->ClearColor = GE_Vec4( 0.0f, 0.0f, 0.0f, 1.0f );

	GE_Core::getSingleton().createSwapChain( *m_swapChain );
}

GEW_Viewport::~GEW_Viewport()
{
	delete m_swapChain;
}

void GEW_Viewport::paintEvent( QPaintEvent *i_ev )
{
	i_ev->accept();
	emit signalRepaint();
}

void GEW_Viewport::resizeEvent( QResizeEvent *i_ev )
{
	i_ev->accept();
	if( i_ev->size().width() > 5 || i_ev->size().height() > 5 )
	{
		m_swapChain->ViewPort.Width = i_ev->size().width();
		m_swapChain->ViewPort.Height = i_ev->size().height();
		m_swapChain->resize();
		emit signalResize( i_ev->size().width(), i_ev->size().height() );
	}
}

void GEW_Viewport::mousePressEvent( QMouseEvent *i_ev )
{
	m_inputData.MButtons |= i_ev->button();
	i_ev->accept();
}

void GEW_Viewport::mouseReleaseEvent( QMouseEvent *i_ev )
{
	m_inputData.MButtons &= ~i_ev->button();
	i_ev->accept();
}

void GEW_Viewport::mouseMoveEvent( QMouseEvent *i_ev )
{
	m_inputData.X = QCursor::pos().x();
	m_inputData.X = QCursor::pos().y();
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
	}
	i_ev->accept();
}
