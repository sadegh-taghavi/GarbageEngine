#include "TreeWidget.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>

TreeWidget::TreeWidget( QWidget *parent /*= 0 */ )
: QTreeWidget( parent )
{
}

void colapsAllItems( QTreeWidgetItem * i_root )
{
	if( i_root )
	{
		i_root->setExpanded( false );
		for( int i = 0; i < i_root->childCount(); ++i )
			colapsAllItems( i_root->child( i ) );
	}
}

void expandAllItems( QTreeWidgetItem * i_root )
{
	if( i_root )
	{
		i_root->setExpanded( true );
		for( int i = 0; i < i_root->childCount(); ++i )
			expandAllItems( i_root->child( i ) );
	}
}

void TreeWidget::keyPressEvent( QKeyEvent *event )
{
	Qt::Key key = ( Qt::Key )event->key();
	if( key == Qt::Key_Minus )
		colapsAllItems( currentItem() );
	else if( key == Qt::Key_Plus )
		expandAllItems( currentItem() );
	else if( key == Qt::Key_Slash )
		colapsAllItems( invisibleRootItem() );
	else if( key == Qt::Key_Asterisk )
		expandAllItems( invisibleRootItem() );

	if( key == Qt::Key_Right ||
		key == Qt::Key_Left ||
		key == Qt::Key_Up ||
		key == Qt::Key_Down ||
		key == Qt::Key_PageUp ||
		key == Qt::Key_PageDown ||
		key == Qt::Key_Home ||
		key == Qt::Key_End )
	{
		QTreeWidget::keyPressEvent( event );
	}

	emit signalKeyPressed( key );
	event->accept();
}

void TreeWidget::mouseMoveEvent( QMouseEvent *event )
{
	if( event->buttons() & Qt::MidButton )
		QWidget::mouseMoveEvent( event );
	else
		QTreeWidget::mouseMoveEvent( event );
}

void TreeWidget::mousePressEvent( QMouseEvent *event )
{
	if( event->button() == Qt::MidButton )
		QWidget::mousePressEvent( event );
	else
		QTreeWidget::mousePressEvent( event );
}

void TreeWidget::mouseReleaseEvent( QMouseEvent *event )
{
	if( event->button() == Qt::MidButton )
		QWidget::mouseReleaseEvent( event );
	else
		QTreeWidget::mouseReleaseEvent( event );
}
