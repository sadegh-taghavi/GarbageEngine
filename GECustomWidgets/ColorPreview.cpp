#include "ColorPreview.h"
#include <QStylePainter>
#include <QStyleOptionFrame>
#include "PaintBorder.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QDrag>
#include <QMimeData>

ColorPreview::ColorPreview( QWidget *parent ) :
QWidget( parent ), m_color( Qt::red ), m_backGroundBrush( Qt::darkGray, Qt::DiagCrossPattern ),
m_alphaMode( NoAlpha )
{
	m_backGroundBrush.setTexture( QPixmap( QString( ":/Resources/Alphaback.png" ) ) );
	m_alphaMode = SplitAlpha;
	m_viewMode = TopToBottom;
	//setPalette(QGuiApplication::palette());
}

QSize ColorPreview::sizeHint() const
{
	return QSize( 16, 16 );
}

void ColorPreview::paint( QPainter &painter, QRect rect ) const
{
	QColor noalpha = m_color;
	noalpha.setAlpha( 255 );

	painter.fillRect( 1, 1, rect.width() - 2, rect.height() - 2, m_backGroundBrush );

	if ( m_alphaMode == SplitAlpha )
	{
		if ( m_viewMode == TopToBottom )
		{
			int h = ( rect.height() - 2 ) / 2;
			painter.fillRect( 1, 1, rect.width() - 2, h, noalpha );
			painter.fillRect( 1, h, rect.width() - 2, rect.height() - h - 1, m_color );
		} else
		{
			int w = ( rect.width() - 2 ) / 2;
			painter.fillRect( 1, 1, w, rect.height() - 2, noalpha );
			painter.fillRect( w, 1, rect.width() - w - 1, rect.height() - 2, m_color );
		}
	} else if ( m_alphaMode == AllAlpha )
	{
		painter.fillRect( 1, 1, rect.width() - 2, rect.height() - 2, m_color );
	} else
	{
		painter.fillRect( 1, 1, rect.width() - 2, rect.height() - 2, noalpha );
	}
	/*int w = rect.width()-2;
	if ( alpha_mode == SplitAlpha )
	w /= 2;
	else if ( alpha_mode == AllAlpha )
	w = 0;
	painter.fillRect(1,1,w,rect.height()-2,noalpha);
	painter.fillRect(w,1,rect.width()-w-1,rect.height()-2,col);*/

	paintTlBorder( painter, size(), palette().color( QPalette::Mid ), 0 );
	paintTlBorder( painter, size(), palette().color( QPalette::Dark ), 1 );

	paintBrBorder( painter, size(), palette().color( QPalette::Midlight ), 1 );
	paintBrBorder( painter, size(), palette().color( QPalette::Button ), 0 );
}

void ColorPreview::slotSetColor( QColor c )
{
	if ( m_color == c )
		return;
	m_color = c;
	update();
	emit signalColorChanged( c );
}

void ColorPreview::paintEvent( QPaintEvent * ev )
{
	ev->accept();
	QStylePainter painter( this );
	//painter.setRenderHint(QPainter::Antialiasing);
	paint( painter, geometry() );
}

void ColorPreview::resizeEvent( QResizeEvent * ev )
{
	ev->accept();
	update();
}

void ColorPreview::mousePressEvent( QMouseEvent *ev )
{
	if ( ev->buttons() & Qt::LeftButton )
		ev->accept();
	else
		QWidget::mousePressEvent( ev );
}

void ColorPreview::mouseReleaseEvent( QMouseEvent * ev )
{
	if ( ev->button() == Qt::LeftButton && QRect( QPoint( 0, 0 ), size() ).contains( ev->pos() ) )
	{
		ev->accept();
		emit signalClicked();
	} else
		QWidget::mouseReleaseEvent( ev );
}

void ColorPreview::mouseMoveEvent( QMouseEvent *ev )
{
	if ( ev->buttons() & Qt::LeftButton && !QRect( QPoint( 0, 0 ), size() ).contains( ev->pos() ) )
	{
		ev->accept();
		QMimeData *data = new QMimeData;
		data->setColorData( m_color );
		/*data->setText(col.name());
		data->setData("application/x-oswb-color",
		QString("<paint><color name='%1'>""<sRGB r='%2' g='%3' b='%4' />"
		"</color></paint>")
		.arg(col.name()).arg(col.redF()).arg(col.greenF()).arg(col.blueF())
		.toUtf8()
		);*/

		QDrag* drag = new QDrag( this );
		drag->setMimeData( data );

		QPixmap preview( 16, 16 );
		preview.fill( m_color );
		drag->setPixmap( preview );

		drag->exec();
	} else
		QWidget::mouseMoveEvent( ev );
}

