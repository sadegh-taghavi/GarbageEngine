#include "gglyphmaker.h"
#include <QGraphicsRectItem>
#include <QPalette>

GGlyphMaker::GGlyphMaker( QWidget *parent )
: QMainWindow( parent )
{
	ui.setupUi( this );
	QFont fnt( "Tahoma", 18 );
	fnt.setBold( true );
	m_fontDialog = new QFontDialog( fnt, this );
	m_trasparentBrush = QBrush( Qt::red );
	m_trasparentBrush.setTexture( QPixmap( ":/GGlyphMaker/transparent.png" ) );
	ui.glyphView->setScene( &m_scene );
	ui.glyphView->setFont( fnt );
	m_scene.setFont( fnt );
	m_backgroundColor = Qt::black;
	m_foregroundColor = Qt::white;
	on_glyphSize_currentIndexChanged( 1 );
	on_actionAuto_font_size_triggered( true );
	on_actionTransparent_background_triggered( true );
}

GGlyphMaker::~GGlyphMaker()
{

}

void GGlyphMaker::on_actionFont_triggered()
{
	if ( m_fontDialog->exec() == QFontDialog::Accepted )
		m_scene.setFont( m_fontDialog->selectedFont() );
	reDraw();
}

void GGlyphMaker::on_actionBackground_color_triggered()
{
	if ( m_colorDialog.exec() == QColorDialog::Accepted )
	{
		m_backgroundColor = m_colorDialog.selectedColor();
		ui.glyphView->setBackgroundBrush( QBrush( m_backgroundColor ) );
		m_scene.setBackgroundBrush( QBrush( m_backgroundColor ) );
	}
	reDraw();
}

void GGlyphMaker::on_actionTransparent_background_triggered( bool checked )
{
	if ( checked )
	{
		ui.glyphView->setBackgroundBrush( m_trasparentBrush );
		m_scene.setBackgroundBrush( QBrush(Qt::transparent) );
	}
	else
	{
		ui.glyphView->setBackgroundBrush( QBrush( m_backgroundColor ) );
		m_scene.setBackgroundBrush( QBrush( m_backgroundColor ) );
	}
	ui.actionBackground_color->setEnabled( !checked );
	reDraw();
}

void GGlyphMaker::on_actionForeground_color_triggered()
{
	if ( m_colorDialog.exec() == QColorDialog::Accepted )
		m_foregroundColor = m_colorDialog.selectedColor();
	reDraw();
}

void GGlyphMaker::on_actionExit_triggered()
{
	exit( 0 );
}

void GGlyphMaker::on_actionSave_triggered()
{
	m_saveFile = QFileDialog::getSaveFileName( this, "Save glyph", m_saveFile, "PNG file(*.png)" );
	if ( !m_saveFile.isEmpty() )
	{
		QImage img( m_size, m_size, QImage::Format_ARGB32 );
		img.fill( Qt::transparent );

		QPainter p( &img );
		p.setRenderHint( QPainter::SmoothPixmapTransform, true );
		p.setRenderHint( QPainter::TextAntialiasing, true );
		p.setRenderHint( QPainter::HighQualityAntialiasing, true );
		p.setRenderHint( QPainter::Antialiasing, true );
		m_scene.render( &p );

		p.end();

		img.save( m_saveFile );
		QFile file( m_saveFile.left( m_saveFile.length() - 3 ) + "des" );
		file.open( QIODevice::WriteOnly );
		for ( int i = 0; i < 256; i++ )
		{
			float val = m_items[ i ]->boundingRect().width() / ( ( float )m_size / 16.0f );
			file.write( ( char * )&val, sizeof( float ) );
		}
		file.close();
		ui.statusBar->showMessage( "Successfully saved!", 5000 );
	}
}

void GGlyphMaker::on_glyphSize_currentIndexChanged( int index )
{
	m_size = ( uint32_t )1 << ( index + 8 );
	on_actionAuto_font_size_triggered( m_autoFontSize );
	reDraw();
}

void GGlyphMaker::on_actionAuto_font_size_triggered( bool checked )
{
	m_fontSize = m_size / 16.0f / 1.5f;
	m_autoFontSize = checked;
	reDraw();
}

void GGlyphMaker::reDraw()
{
	m_scene.clear();
	m_scene.setSceneRect( 0, 0, m_size, m_size );
	float mw = ( float )m_size / 16.0f;
	m_items.clear();
	QFont font = m_scene.font();
	if ( m_autoFontSize )
		font.setPixelSize( m_fontSize );
	uint i = 0;
	for ( uint yy = 0; yy < 16; yy++ )
	{
		for ( uint xx = 0; xx < 16; xx++ )
		{
			QGraphicsTextItem *item = new QGraphicsTextItem( QString( i ) );
			item->setFont( font );
			item->setDefaultTextColor( m_foregroundColor );
			qreal w = item->boundingRect().width();
			qreal h = item->boundingRect().height();
			item->setPos( ( float )xx * mw - ( w / 2.0f ) + ( mw / 2.0f ), ( float )yy * mw - ( h / 2.0f ) + ( mw / 2.0f ) );
			m_items.append( item );
			m_scene.addItem( item );
			i++;
		}
	}
}
