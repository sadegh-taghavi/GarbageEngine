#include "ListWidget.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>

ListWidget::ListWidget( QWidget *parent /*= 0 */ )
: QListWidget( parent )
{
}

void ListWidget::keyPressEvent( QKeyEvent *event )
{
	Qt::Key key = ( Qt::Key )event->key();

	if( key == Qt::Key_Right ||
		key == Qt::Key_Left ||
		key == Qt::Key_Up ||
		key == Qt::Key_Down ||
		key == Qt::Key_PageUp ||
		key == Qt::Key_PageDown ||
		key == Qt::Key_Home || 
		key == Qt::Key_End )
	{
		QListWidget::keyPressEvent( event );
	}

	emit signalKeyPressed( key );
	event->accept();
}

void ListWidget::mouseMoveEvent( QMouseEvent *event )
{
	if( event->buttons() & Qt::MidButton )
		QWidget::mouseMoveEvent( event );
	else
		QListWidget::mouseMoveEvent( event );
}

void ListWidget::mousePressEvent( QMouseEvent *event )
{
	if( event->button() == Qt::MidButton )
		QWidget::mousePressEvent( event );
	else
		QListWidget::mousePressEvent( event );
}

void ListWidget::mouseReleaseEvent( QMouseEvent *event )
{
	if( event->button() == Qt::MidButton )
		QWidget::mouseReleaseEvent( event );
	else
		QListWidget::mouseReleaseEvent( event );
}

void ListWidget::dropEvent( QDropEvent *event )
{
	QList<QListWidgetItem *> oldItems;
	QList<QListWidgetItem *> newItems;

	for( int i = 0; i < count(); i++ )
		oldItems.push_back( item( i ) );

	QListWidget::dropEvent( event );

	for( int i = 0; i < count(); i++ )
		newItems.push_back( item( i ) );

	if( oldItems.count() != newItems.count() )
		emit signalItemOrderChanged( oldItems, newItems );
	else
	{
		for( int i = 0; i < oldItems.count(); i++ )
		{
			if( oldItems[ i ] != newItems[ i ] )
			{
				emit signalItemOrderChanged( oldItems, newItems );
				break;
			}
		}
	}
	event->accept();
}
