#ifndef GROUPBOX_H
#define GROUPBOX_H

#include <QGroupBox>

class GroupBox : public QGroupBox
{
	Q_OBJECT
public:
	explicit GroupBox( QWidget *parent = 0 );

protected:
	virtual void childEvent( QChildEvent * event );

private:
	void mouseMoveEvent( QMouseEvent *i_ev );
};

#endif