#include "ColorSelector.h"
#include "ColorDialog.h"
#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>
#include <QMimeData>

QList<QColor> ColorSelector::m_colorPallet;

ColorSelector::ColorSelector( QWidget *parent ) :
ColorPreview( parent ), m_dialog( new ColorDialog( this ) )
{
	setUpdateMode( Continuous );
	m_oldColor = color();
	connect( this, SIGNAL( signalClicked() ), this, SLOT( slotShowDialog() ) );
	connect( this, SIGNAL( signalColorChanged( QColor ) ), this, SLOT( slotUpdateOldColor( QColor ) ) );
	connect( m_dialog, SIGNAL( accepted() ), this, SLOT( slotAcceptDialog() ) );
	connect( m_dialog, SIGNAL( rejected() ), this, SLOT( slotRejectDialog() ) );
	setAcceptDrops( true );
}

void ColorSelector::setUpdateMode( ColorSelector::UpdateMode m )
{
	m_updateMode = m;
}

Qt::WindowModality ColorSelector::dialogModality() const
{
	return m_dialog->windowModality();
}

void ColorSelector::setDialogModality( Qt::WindowModality m )
{
	m_dialog->setWindowModality( m );
}

void ColorSelector::slotShowDialog()
{
	m_oldColor = color();
	m_dialog->slotSetColor( color() );
	m_dialog->setColorPallet( &m_colorPallet );
	connectDialog();
	m_dialog->show();
}

void ColorSelector::connectDialog()
{
	if ( m_updateMode == Continuous )
		connect( m_dialog, SIGNAL( signalColorChanged( QColor ) ), this, SLOT( slotSetColor( QColor ) ) );
	else
		disconnectDialog();
}

void ColorSelector::disconnectDialog()
{
	disconnect( m_dialog, SIGNAL( signalColorChanged( QColor ) ), this, SLOT( slotSetColor( QColor ) ) );
}

void ColorSelector::slotAcceptDialog()
{
	disconnectDialog();
	slotSetColor( m_dialog->color() );
}

void ColorSelector::slotRejectDialog()
{
	slotSetColor( m_oldColor );
}

void ColorSelector::slotUpdateOldColor( QColor c )
{
	if ( !m_dialog->isVisible() )
		m_oldColor = c;
}

void ColorSelector::dragEnterEvent( QDragEnterEvent *event )
{
	if ( event->mimeData()->hasColor() ||
		 ( event->mimeData()->hasText() && QColor( event->mimeData()->text() ).isValid() ) )
	{
		event->acceptProposedAction();
		event->accept();
	} else
		ColorPreview::dragEnterEvent( event );
}


void ColorSelector::dropEvent( QDropEvent *event )
{
	if ( event->mimeData()->hasColor() )
	{
		slotSetColor( event->mimeData()->colorData().value<QColor>() );
		event->accept();
	} else if ( event->mimeData()->hasText() )
	{
		QColor col( event->mimeData()->text() );
		if ( col.isValid() )
		{
			slotSetColor( col );
			event->accept();
		}
	} else
		ColorPreview::dropEvent( event );
}
