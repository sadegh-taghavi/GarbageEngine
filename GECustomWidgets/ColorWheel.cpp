#include "ColorWheel.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QLineF>

ColorWheel::ColorWheel( QWidget *parent ) :
QWidget( parent ), m_huem( 0 ), m_saturation( 0 ), m_value( 0 ),
m_wheelWidth( 20 ), m_mouseStatus( Nothing )
{
}

QColor ColorWheel::color() const
{
	return QColor::fromHsvF( m_huem, m_saturation, m_value );
}

QSize ColorWheel::sizeHint() const
{
	return QSize( m_wheelWidth * 5, m_wheelWidth * 5 );
}

void ColorWheel::setWheelWidth( unsigned w )
{
	m_wheelWidth = w;
	renderRectangle();
	update();
}


void ColorWheel::paintEvent( QPaintEvent * )
{
	double selector_w = 6;

	QPainter painter( this );
	painter.setRenderHint( QPainter::Antialiasing );
	painter.translate( geometry().width() / 2, geometry().height() / 2 );

	// hue wheel
	if ( m_hueRing.isNull() )
		renderRing();

	painter.drawPixmap( -outerRadius(), -outerRadius(), m_hueRing );



	// hue selector
	painter.setPen( QPen( Qt::black, 3 ) );
	painter.setBrush( Qt::NoBrush );
	QLineF ray( 0, 0, outerRadius(), 0 );
	ray.setAngle( m_huem * 360 );
	QPointF h1 = ray.p2();
	ray.setLength( innerRadius() );
	QPointF h2 = ray.p2();
	painter.drawLine( h1, h2 );

	// lum-sat square
	if ( m_saturationValueSquare.isNull() )
		renderRectangle();

	painter.rotate( -m_huem * 360 - 45 );
	ray.setLength( innerRadius() );
	ray.setAngle( 135 );
	painter.drawImage( ray.p2(), m_saturationValueSquare );

	// lum-sat selector
	//painter.rotate(135);
	painter.setPen( QPen( m_value > 0.5 ? Qt::black : Qt::white, 3 ) );
	painter.setBrush( Qt::NoBrush );
	double max_dist = squareSize();
	painter.drawEllipse( QPointF( m_saturation*max_dist - max_dist / 2,
		m_value*max_dist - max_dist / 2
		),
		selector_w, selector_w );

}

void ColorWheel::mouseMoveEvent( QMouseEvent *ev )
{
	if ( m_mouseStatus == DragCircle )
	{
		ev->accept();
		m_huem = lineToPoint( ev->pos() ).angle() / 360.0;
		renderRectangle();

		emit signalColorSelected( color() );
		emit signalColorChanged( color() );
		update();
	} else if ( m_mouseStatus == DragSquare )
	{
		ev->accept();
		QLineF glob_mouse_ln = lineToPoint( ev->pos() );
		QLineF center_mouse_ln( QPointF( 0, 0 ),
								glob_mouse_ln.p2() - glob_mouse_ln.p1() );
		center_mouse_ln.setAngle( center_mouse_ln.angle() - m_huem * 360 - 45 );

		m_saturation = center_mouse_ln.x2() / squareSize() + 0.5;

		m_value = center_mouse_ln.y2() / squareSize() + 0.5;

		if ( m_saturation > 1 )
			m_saturation = 1;
		else if ( m_saturation < 0 )
			m_saturation = 0;

		if ( m_value > 1 )
			m_value = 1;
		else if ( m_value < 0 )
			m_value = 0;

		emit signalColorSelected( color() );
		emit signalColorChanged( color() );
		update();
	} else
		QWidget::mouseMoveEvent( ev );
}

void ColorWheel::mousePressEvent( QMouseEvent *ev )
{
	if ( ev->buttons() & Qt::LeftButton )
	{
		ev->accept();
		QLineF ray = lineToPoint( ev->pos() );
		if ( ray.length() <= innerRadius() )
			m_mouseStatus = DragSquare;
		else if ( ray.length() <= outerRadius() )
			m_mouseStatus = DragCircle;
	} else
		QWidget::mousePressEvent( ev );
}

void ColorWheel::mouseReleaseEvent( QMouseEvent *ev )
{
	if ( m_mouseStatus == DragCircle || m_mouseStatus == DragSquare )
	{
		mouseMoveEvent( ev );
		m_mouseStatus = Nothing;
	} else
		QWidget::mouseReleaseEvent( ev );
}

void ColorWheel::resizeEvent( QResizeEvent * )
{
	renderRing();
	renderRectangle();
}

void ColorWheel::renderRectangle()
{
	int sz = squareSize();
	m_saturationValueSquare = QImage( sz, sz, QImage::Format_RGB32 );

	for ( int i = 0; i < sz; ++i )
	{
		for ( int j = 0; j < sz; ++j )
		{
			m_saturationValueSquare.setPixel( i, j,
									 QColor::fromHsvF( m_huem, double( i ) / sz, double( j ) / sz ).rgb() );
		}
	}
}

void ColorWheel::renderRing()
{

	m_hueRing = QPixmap( outerRadius() * 2, outerRadius() * 2 );
	m_hueRing.fill( Qt::transparent );
	QPainter painter( &m_hueRing );
	painter.setRenderHint( QPainter::Antialiasing );
	painter.setCompositionMode( QPainter::CompositionMode_Source );


	const int hue_stops = 24;
	static QConicalGradient gradient_hue( 0, 0, 0 );
	if ( gradient_hue.stops().size() < hue_stops )
	{
		for ( double a = 0; a < 1.0; a += 1.0 / ( hue_stops - 1 ) )
		{
			gradient_hue.setColorAt( a, QColor::fromHsvF( a, 1, 1 ) );
		}
		gradient_hue.setColorAt( 1, QColor::fromHsvF( 0, 1, 1 ) );
	}

	painter.translate( outerRadius(), outerRadius() );

	painter.setPen( Qt::NoPen );
	painter.setBrush( QBrush( gradient_hue ) );
	painter.drawEllipse( QPointF( 0, 0 ), outerRadius(), outerRadius() );

	painter.setBrush( Qt::transparent );//palette().background());
	painter.drawEllipse( QPointF( 0, 0 ), innerRadius(), innerRadius() );

}


void ColorWheel::slotSetColor( QColor c )
{
	if ( color() == c )
		return;
	qreal oldh = m_huem;
	m_huem = c.hueF();
	if ( m_huem < 0 )
		m_huem = 0;
	m_saturation = c.saturationF();
	m_value = c.valueF();
	if ( !qFuzzyCompare( oldh + 1, m_huem + 1 ) )
		renderRectangle();
	update();
	emit signalColorChanged( c );
}

void ColorWheel::slotSetHue( qreal h )
{
	m_huem = qMax( 0.0, qMin( 1.0, h ) );
	renderRectangle();
	update();
}

void ColorWheel::slotSetSaturation( qreal s )
{
	m_saturation = qMax( 0.0, qMin( 1.0, s ) );
	update();
}

void ColorWheel::slotSetValue( qreal v )
{
	m_value = qMax( 0.0, qMin( 1.0, v ) );
	update();
}

