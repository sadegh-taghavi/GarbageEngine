#include "GroupBox.h"
#include <QtGui/QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QScrollBar>
#include <QChildEvent>

GroupBox::GroupBox( QWidget *parent /*= 0 */ )
: QGroupBox( parent )
{
}

void GroupBox::mouseMoveEvent( QMouseEvent *i_ev )
{
	QWidget::mouseMoveEvent( i_ev );
}

void GroupBox::childEvent( QChildEvent * event )
{
	QGroupBox::changeEvent( event );
}
