#include "GradientSlider.h"
#include <QtGui/QPainter>
#include <QStyleOptionSlider>
#include "PaintBorder.h"

GradientSlider::GradientSlider( QWidget *parent ) :
QSlider( parent ), m_backGroundBrush( Qt::darkGray, Qt::DiagCrossPattern )
{
	m_colorList.push_back( Qt::black );
	m_colorList.push_back( Qt::white );

	m_backGroundBrush.setTexture( QPixmap( QString( ":/Resources/Alphaback.png" ) ) );
	setOrientation( Qt::Horizontal );
}

void GradientSlider::setBackground( QBrush bg )
{
	m_backGroundBrush = bg;
	update();
}

void GradientSlider::setColors( QVector<QColor> bg )
{
	m_colorList = bg;
	update();
}

void GradientSlider::setGradient( QLinearGradient bg )
{
	m_colorList.clear();
	foreach( const QGradientStop& gs, bg.stops() )
	{
		m_colorList.push_back( gs.second );
	}
	update();
}

QLinearGradient GradientSlider::gradient() const
{
	int ior = orientation() == Qt::Horizontal ? 1 : 0;
	QLinearGradient lg( 0, 0, ior, 1 - ior );
	lg.setCoordinateMode( QGradient::StretchToDeviceMode );
	for ( int i = 0; i < m_colorList.size(); i++ )
		lg.setColorAt( double( i ) / ( m_colorList.size() - 1 ), m_colorList[ i ] );
	return lg;
}

void GradientSlider::setFirstColor( QColor c )
{
	if ( m_colorList.empty() )
		m_colorList.push_back( c );
	else
		m_colorList.front() = c;
	update();
}

void GradientSlider::setLastColor( QColor c )
{

	if ( m_colorList.size() < 2 )
		m_colorList.push_back( c );
	else
		m_colorList.back() = c;
	update();
}

QColor GradientSlider::firstColor() const
{
	return m_colorList.empty() ? QColor() : m_colorList.front();
}

QColor GradientSlider::lastColor() const
{
	return m_colorList.empty() ? QColor() : m_colorList.back();
}


void GradientSlider::paintEvent( QPaintEvent * )
{
	QPainter painter( this );


	painter.setPen( Qt::NoPen );
	painter.setBrush( m_backGroundBrush );
	painter.drawRect( 1, 1, geometry().width() - 2, geometry().height() - 2 );
	painter.setBrush( gradient() );
	painter.drawRect( 1, 1, geometry().width() - 2, geometry().height() - 2 );

	paintTlBorder( painter, size(), palette().color( QPalette::Mid ), 0 );
	/*paint_tl_border(painter,size(),palette().color(QPalette::Dark),1);

	paint_br_border(painter,size(),palette().color(QPalette::Light),1);*/
	paintBrBorder( painter, size(), palette().color( QPalette::Midlight ), 0 );

	QStyleOptionSlider opt_slider;
	initStyleOption( &opt_slider );
	opt_slider.subControls = QStyle::SC_SliderHandle;
	if ( isSliderDown() )
		opt_slider.state |= QStyle::State_Sunken;
	style()->drawComplexControl( QStyle::CC_Slider, &opt_slider, &painter, this );


	/*QStyleOptionFrameV3 opt_frame;
	opt_frame.init(this);
	opt_frame.frameShape = QFrame::StyledPanel;
	opt_frame.rect = geometry();
	opt_frame.state = QStyle::State_Sunken;

	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::NoBrush);
	painter.translate(-geometry().topLeft());
	style()->drawControl(QStyle::CE_ShapedFrame, &opt_frame, &painter, this);*/
}
