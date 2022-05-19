#include "ColorDialog.h"
#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>
#include <QDesktopWidget>
#include <QMimeData>
#include <QtGui/QPainter>

void ColorDialog::on_button_pick_clicked()
{
	grabMouse( Qt::CrossCursor );
	m_pickFromScreen = true;
}

QString getColorText( QColor c )
{
	QString colorText;

	if ( c.red() < 16 )
		colorText += "0";
	colorText += QString::number( c.red(), 16 );

	if ( c.green() < 16 )
		colorText += "0";
	colorText += QString::number( c.green(), 16 );

	if ( c.blue() < 16 )
		colorText += "0";
	colorText += QString::number( c.blue(), 16 );

	if ( c.alpha() < 16 )
		colorText += "0";
	colorText += QString::number( c.alpha(), 16 );

	return colorText.toUpper();
}

ColorDialog::ColorDialog( QWidget *parent ) :
QDialog( parent ), m_pickFromScreen( false )
{
	setupUi( this );
	QVector<QColor> rainbow;
	for ( int i = 0; i < 360; i += 360 / 6 )
		rainbow.push_back( QColor::fromHsv( i, 255, 255 ) );
	rainbow.push_back( Qt::red );
	slide_hue->setColors( rainbow );
	setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
	setAcceptDrops( true );
	m_backGroundBrush.setTexture( QPixmap( QString( ":/Resources/Alphaback.png" ) ) );

	m_colorPallet = NULL;
}

QColor ColorDialog::color() const
{
	QColor col = wheel->color();
	col.setAlpha( slide_alpha->value() );
	return col;
}

QSize ColorDialog::sizeHint() const
{
	return QSize( 400, 0 );
}

void ColorDialog::setColorPallet( QList<QColor> *pallet )
{
	m_colorPallet = pallet;
	listWidget_pallet->blockSignals( true );
	listWidget_pallet->clear();
	if ( m_colorPallet )
	{
		foreach( QColor c, *m_colorPallet )
		{
			QPixmap pix( 16, 16 );
			QPainter p( &pix );
			p.fillRect( pix.rect(), m_backGroundBrush );
			p.fillRect( pix.rect(), c );
			new QListWidgetItem( pix, getColorText( c ), listWidget_pallet );
		}
	}
	listWidget_pallet->blockSignals( false );
}

void ColorDialog::slotSetColor( QColor c )
{
	if ( color() == c )
		return;
	wheel->slotSetColor( c );
	slide_alpha->setValue( c.alpha() );
	slotUpdateWidgets();
}


void ColorDialog::slotUpdateWidgets()
{
	blockSignals( true );
	foreach( QWidget* w, findChildren<QWidget*>() )
		w->blockSignals( true );

	QColor col = color();

	slide_red->setValue( col.red() );
	spin_red->setValue( slide_red->value() );
	slide_red->setFirstColor( QColor( 0, col.green(), col.blue() ) );
	slide_red->setLastColor( QColor( 255, col.green(), col.blue() ) );

	slide_green->setValue( col.green() );
	spin_green->setValue( slide_green->value() );
	slide_green->setFirstColor( QColor( col.red(), 0, col.blue() ) );
	slide_green->setLastColor( QColor( col.red(), 255, col.blue() ) );

	slide_blue->setValue( col.blue() );
	spin_blue->setValue( slide_blue->value() );
	slide_blue->setFirstColor( QColor( col.red(), col.green(), 0 ) );
	slide_blue->setLastColor( QColor( col.red(), col.green(), 255 ) );

	slide_hue->setValue( qRound( wheel->hue()*360.0 ) );
	spin_hue->setValue( slide_hue->value() );

	slide_saturation->setValue( qRound( wheel->saturation()*255.0 ) );
	spin_saturation->setValue( slide_saturation->value() );
	slide_saturation->setFirstColor( QColor::fromHsvF( wheel->hue(), 0, wheel->value() ) );
	slide_saturation->setLastColor( QColor::fromHsvF( wheel->hue(), 1, wheel->value() ) );

	slide_value->setValue( qRound( wheel->value()*255.0 ) );
	spin_value->setValue( slide_value->value() );
	slide_value->setFirstColor( QColor::fromHsvF( wheel->hue(), wheel->saturation(), 0 ) );
	slide_value->setLastColor( QColor::fromHsvF( wheel->hue(), wheel->saturation(), 1 ) );


	QColor apha_color = col;
	apha_color.setAlpha( 0 );
	slide_alpha->setFirstColor( apha_color );
	apha_color.setAlpha( 255 );
	slide_alpha->setLastColor( apha_color );
	spin_alpha->setValue( slide_alpha->value() );


	edit_hex->setText( col.name() );

	preview->slotSetColor( col );

	blockSignals( false );
	foreach( QWidget* w, findChildren<QWidget*>() )
		w->blockSignals( false );

	emit signalColorChanged( col );
}

void ColorDialog::slotSetHsv()
{
	if ( !signalsBlocked() )
	{
		wheel->slotSetColor( QColor::fromHsv(
			slide_hue->value(),
			slide_saturation->value(),
			slide_value->value()
			) );
		slotUpdateWidgets();
	}
}

void ColorDialog::slotSetRgb()
{
	if ( !signalsBlocked() )
	{
		QColor col( slide_red->value(), slide_green->value(), slide_blue->value() );
		if ( col.saturation() == 0 )
			col = QColor::fromHsv( slide_hue->value(), 0, col.value() );
		wheel->slotSetColor( col );
		slotUpdateWidgets();
	}
}


void ColorDialog::on_edit_hex_editingFinished()
{
	updateHex();
}

void ColorDialog::on_edit_hex_textEdited( const QString &arg1 )
{
	int cursor = edit_hex->cursorPosition();
	updateHex();
	//edit_hex->blockSignals(true);
	edit_hex->setText( arg1 );
	//edit_hex->blockSignals(false);
	edit_hex->setCursorPosition( cursor );
}

void ColorDialog::updateHex()
{
	QString xs = edit_hex->text().trimmed();
	xs.remove( '#' );

	if ( xs.isEmpty() )
		return;

	if ( xs.indexOf( QRegExp( "^[0-9a-fA-f]+$" ) ) == -1 )
	{
		QColor c( xs );
		if ( c.isValid() )
		{
			slotSetColor( c );
			return;
		}
	}

	if ( xs.size() == 3 )
	{
		slide_red->setValue( QString( 2, xs[ 0 ] ).toInt( 0, 16 ) );
		slide_green->setValue( QString( 2, xs[ 1 ] ).toInt( 0, 16 ) );
		slide_blue->setValue( QString( 2, xs[ 2 ] ).toInt( 0, 16 ) );
	} else
	{
		if ( xs.size() < 6 )
		{
			xs += QString( 6 - xs.size(), '0' );
		}
		slide_red->setValue( xs.mid( 0, 2 ).toInt( 0, 16 ) );
		slide_green->setValue( xs.mid( 2, 2 ).toInt( 0, 16 ) );
		slide_blue->setValue( xs.mid( 4, 2 ).toInt( 0, 16 ) );

		if ( xs.size() == 8 )
			slide_alpha->setValue( xs.mid( 6, 2 ).toInt( 0, 16 ) );
	}

	slotSetRgb();
}

void ColorDialog::dragEnterEvent( QDragEnterEvent *event )
{
	if ( event->mimeData()->hasColor() ||
		 ( event->mimeData()->hasText() && QColor( event->mimeData()->text() ).isValid() ) )
	{
		event->acceptProposedAction();
		event->accept();
	} else
		QDialog::dragEnterEvent( event );
}


void ColorDialog::dropEvent( QDropEvent *event )
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
		QDialog::dropEvent( event );
}

void ColorDialog::getScreenColor( QPoint global_pos )
{
	WId id = QApplication::desktop()->winId();
	QImage img = QPixmap::grabWindow( id, global_pos.x(), global_pos.y(), 1, 1 ).toImage();
	QColor color( img.pixel( 0, 0 ) );
	color.setAlpha( slide_alpha->value() );
	slotSetColor( color );
}

void ColorDialog::mouseReleaseEvent( QMouseEvent *event )
{
	if ( m_pickFromScreen )
	{
		event->accept();
		getScreenColor( event->globalPos() );
		m_pickFromScreen = false;
		releaseMouse();
		button_pick->setChecked( false );
	} else
		QDialog::mouseReleaseEvent( event );
}

void ColorDialog::mouseMoveEvent( QMouseEvent *event )
{
	if ( m_pickFromScreen )
	{
		event->accept();
		getScreenColor( event->globalPos() );
	} else
		QDialog::mouseMoveEvent( event );
}

QColor getColorFromText( QString str )
{
	QString rt = str.left( 2 ).right( 2 );
	QString gt = str.left( 4 ).right( 2 );
	QString bt = str.left( 6 ).right( 2 );
	QString at = str.left( 8 ).right( 2 );

	int r = rt.toUInt( NULL, 16 );
	int g = gt.toUInt( NULL, 16 );
	int b = bt.toUInt( NULL, 16 );
	int a = at.toUInt( NULL, 16 );

	return QColor( r, g, b, a );
}

void ColorDialog::on_button_add_clicked()
{
	bool found = false;
	QString colorText = getColorText( color() );
	for ( int i = 0; i < listWidget_pallet->count(); i++ )
	{
		if ( listWidget_pallet->item( i )->text() == colorText )
		{
			found = true;
			listWidget_pallet->blockSignals( true );
			listWidget_pallet->setCurrentRow( i );
			listWidget_pallet->blockSignals( false );
			break;
		}
	}
	if ( !found )
	{
		QPixmap pix( 16, 16 );
		QPainter p( &pix );
		p.fillRect( pix.rect(), m_backGroundBrush );
		p.fillRect( pix.rect(), color() );
		new QListWidgetItem( pix, colorText, listWidget_pallet );
	}
}

void ColorDialog::on_button_remove_clicked()
{
	if ( listWidget_pallet->currentItem() )
		delete listWidget_pallet->currentItem();
}

void ColorDialog::on_listWidget_pallet_currentRowChanged( int currentRow )
{
	if ( currentRow < 0 )
		return;
	QString colorText = listWidget_pallet->item( currentRow )->text();
	slotSetColor( getColorFromText( colorText ) );
}

void ColorDialog::on_listWidget_pallet_itemClicked( QListWidgetItem *item )
{
	Q_UNUSED( item )
		on_listWidget_pallet_currentRowChanged( listWidget_pallet->currentRow() );
}

void ColorDialog::on_ColorDialog_accepted()
{
	if ( m_colorPallet )
	{
		m_colorPallet->clear();
		for ( int i = 0; i < listWidget_pallet->count(); i++ )
			m_colorPallet->push_back( getColorFromText( listWidget_pallet->item( i )->text() ) );
	}
}
