#ifndef SPINBOX_H
#define SPINBOX_H

#include <QSpinBox>

class SpinBox : public QSpinBox
{
	Q_OBJECT

	bool	m_mouseGrabed;
	int		m_lastY;
	float	m_accelerate;
public:
	explicit SpinBox( QWidget *parent = 0 );

private:
	void mousePressEvent( QMouseEvent *i_ev );
	void mouseReleaseEvent( QMouseEvent *i_ev );
	void mouseMoveEvent( QMouseEvent *i_ev );
	void keyPressEvent( QKeyEvent *i_ev );
	void keyReleaseEvent( QKeyEvent *i_ev );
};

#endif