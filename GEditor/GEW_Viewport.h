#ifndef GEW_VIEWPORT_H
#define GEW_VIEWPORT_H

#include <QWidget>
#include "GEW_Input.h"

struct GE_SwapChain;

class GEW_Viewport : public QWidget
{
	Q_OBJECT

	QPoint			m_lastPos;
public:
	GE_SwapChain *m_swapChain;
	GEW_Input::Data m_inputData;
	int				m_timerId;

	explicit GEW_Viewport( QWidget *i_parent = 0 );
	~GEW_Viewport();

	void setFps( float i_fps );
	void updateMouse();

signals:
	void signalRepaint();
	void signalResize( int i_width, int i_height );

private:
	inline virtual QPaintEngine *paintEngine() { return NULL; }
	virtual void paintEvent( QPaintEvent *i_ev );
	virtual void resizeEvent( QResizeEvent *i_ev );
	virtual void mousePressEvent( QMouseEvent *i_ev );
	virtual void mouseReleaseEvent( QMouseEvent *i_ev );
	virtual void mouseMoveEvent( QMouseEvent *i_ev );
	virtual void wheelEvent( QWheelEvent * );
	virtual void keyPressEvent( QKeyEvent *i_ev );
	virtual void keyReleaseEvent( QKeyEvent *i_ev );
	virtual void timerEvent( QTimerEvent * );
	virtual void update();
};

#endif