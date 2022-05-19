#include "CurveEdit.h"

#include <QLineF>
#include <QVector>
#include <QList>
#include <QPair>
#include <QMenu>
#include <QAction>
#include <QtGui/QResizeEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QContextMenuEvent>

CurveEdit::CurveEdit( QWidget *parent /*= 0 */ )
: QWidget( parent ),
m_backColor( Qt::gray ),
m_lineColor( Qt::black ),
m_pointColor( Qt::white ),
m_selectColor( Qt::red ),
m_textColor( Qt::white ),
m_verticalRangeColor( Qt::darkGray ),
m_horizontalRangeColor( Qt::darkGray ),
m_rangeTextColor( Qt::yellow ),
m_selectedIndex( -1 ),
m_vertivalMode( true ),
m_limitedPoint( true ),
m_rangeLineVisible( false ),
m_rangeTextVisible( false ),
m_rangeX( 100 ),
m_rangeY( 100 ),
m_rangeXDivide( 10 ),
m_rangeYDivide( 10 ),
m_rangeXTextDivide( 10 ),
m_rangeYTextDivide( 10 )
{
	m_menu = new QMenu( this );
	QAction *action;
	action = m_menu->addAction( "Range Line Visible" );
	action->setCheckable( true );
	action->setChecked( false );
	action = m_menu->addAction( "Range Text Visible" );
	action->setCheckable( true );
	action->setChecked( false );
	m_menu->addSeparator();
	action = m_menu->addAction( "Vertical Mode" );
	action->setCheckable( true );
	action->setChecked( true );
	action = m_menu->addAction( "Limited Point" );
	action->setCheckable( true );
	action->setChecked( true );
	m_menu->addSeparator();
	action = m_menu->addAction( "Reset" );

	connect( m_menu, SIGNAL( triggered( QAction* ) ), this, SLOT( actionTriggered( QAction* ) ) );
}

void CurveEdit::setBackColor( QColor i_color )
{
	m_backColor = i_color;
	update();
}

QColor CurveEdit::getBackColor()
{
	return m_backColor;
}

void CurveEdit::setLineColor( QColor i_color )
{
	m_lineColor = i_color;
	update();
}

QColor CurveEdit::getLineColor()
{
	return m_lineColor;
}

void CurveEdit::setPointColor( QColor i_color )
{
	m_pointColor = i_color;
	update();
}

QColor CurveEdit::getPointColor()
{
	return m_pointColor;
}

void CurveEdit::setSelectColor( QColor i_color )
{
	m_selectColor = i_color;
	update();
}

QColor CurveEdit::getSelectColor()
{
	return m_selectColor;
}

void CurveEdit::setTextColor( QColor i_color )
{
	m_textColor = i_color;
}

QColor CurveEdit::getTextColor()
{
	return m_textColor;
}

void CurveEdit::setVerticalRangeColor( QColor i_color )
{
	m_verticalRangeColor = i_color;
	update();
}

QColor CurveEdit::getVerticalRangeColor()
{
	return m_verticalRangeColor;
}

void CurveEdit::setHorizontalRangeColor( QColor i_color )
{
	m_horizontalRangeColor = i_color;
	update();
}

QColor CurveEdit::getHorizontalRangeColor()
{
	return m_horizontalRangeColor;
}

void CurveEdit::setRangeTextColor( QColor i_color )
{
	m_rangeTextColor = i_color;
	update();
}

QColor CurveEdit::getRangeTextColor()
{
	return m_rangeTextColor;
}

void CurveEdit::setPointCount( int i_count )
{
	if ( i_count < 2 )
		return;

	m_selectedIndex = -1;
	m_points.resize( i_count );
	m_lins.resize( i_count - 1 );
	m_rects.resize( i_count );

	float distance = ( float ) width() / ( float ) ( i_count - 1 );
	for ( int i = 0; i < i_count; i++ )
	{
		m_points[ i ].setX( ( float ) i * distance );
		m_points[ i ].setY( ( float ) height() / 2.0f );
	}

	for ( int i = 0; i < i_count - 1; i++ )
	{
		m_lins[ i ].setPoints( m_points[ i ], m_points[ i + 1 ] );
	}

	for ( int i = 0; i < i_count; i++ )
	{
		m_rects[ i ].setSize( QSizeF( 5.0f, 5.0f ) );
		m_rects[ i ].moveCenter( m_points[ i ] );
	}
	update();
}

int CurveEdit::getPointCount()
{
	return m_points.size();
}

void CurveEdit::setVertivalMode( bool i_isVertical )
{
	m_vertivalMode = i_isVertical;
	m_menu->blockSignals( true );
	m_menu->actions()[ 3 ]->setChecked( i_isVertical );
	m_menu->blockSignals( false );
}

bool CurveEdit::isVertivalMode()
{
	return m_vertivalMode;
}

void CurveEdit::setLimitedPoint( bool i_isLimited )
{
	m_limitedPoint = i_isLimited;
	m_menu->blockSignals( true );
	m_menu->actions()[ 4 ]->setChecked( i_isLimited );
	m_menu->blockSignals( false );
}

bool CurveEdit::isLimitedPoint()
{
	return m_limitedPoint;
}

void CurveEdit::setRangeLineVisible( bool i_visible )
{
	m_rangeLineVisible = i_visible;
	m_menu->blockSignals( true );
	m_menu->actions()[ 0 ]->setChecked( i_visible );
	m_menu->blockSignals( false );
	createRangeX();
	createRangeY();
	update();
}

bool CurveEdit::isRangeLineVisible()
{
	return m_rangeLineVisible;
}

void CurveEdit::setRangeTextVisible( bool i_visible )
{
	m_rangeTextVisible = i_visible;
	m_menu->blockSignals( true );
	m_menu->actions()[ 1 ]->setChecked( i_visible );
	m_menu->blockSignals( false );
	createRangeX();
	createRangeY();
	update();
}

bool CurveEdit::isRangeTextVisible()
{
	return m_rangeTextVisible;
}

void CurveEdit::setRangeX( int i_range )
{
	m_rangeX = i_range;
	createRangeX();
	update();
}

int CurveEdit::getRangeX()
{
	return m_rangeX;
}

void CurveEdit::setRangeY( int i_range )
{
	m_rangeY = i_range;
	createRangeY();
	update();
}

int CurveEdit::getRangeY()
{
	return m_rangeY;
}

void CurveEdit::setRangeXDivide( int i_devide )
{
	m_rangeXDivide = i_devide;
	createRangeX();
	update();
}

int CurveEdit::getRangeXDivide()
{
	return m_rangeXDivide;
}

void CurveEdit::setRangeYDivide( int i_devide )
{
	m_rangeYDivide = i_devide;
	createRangeY();
	update();
}

int CurveEdit::getRangeYDivide()
{
	return m_rangeYDivide;
}

void CurveEdit::setRangeXTextDivide( int i_devide )
{
	m_rangeXTextDivide = i_devide;
	createRangeX();
	update();
}

int CurveEdit::getRangeXTextDivide()
{
	return m_rangeXTextDivide;
}

void CurveEdit::setRangeYTextDivide( int i_devide )
{
	m_rangeYTextDivide = i_devide;
	createRangeY();
	update();
}

int CurveEdit::getRangeYTextDivide()
{
	return m_rangeYTextDivide;
}

int CurveEdit::getCurrentIndex()
{
	return m_selectedIndex;
}

void CurveEdit::setCurrentIndex( int i_index )
{
	if ( i_index >= -1 && i_index < m_points.size() && i_index != m_selectedIndex )
	{
		m_selectedIndex = i_index;
		update();
		emit signalIndexChanged( m_selectedIndex );
	}
}

void CurveEdit::getPoints( QVector<QPointF>& i_points )
{
	if ( i_points.size() != m_points.size() )
		return;
	for ( int i = 0; i < m_points.size(); i++ )
	{
		i_points[ i ].setX( m_points[ i ].x() / width() );
		i_points[ i ].setY( 1.0f - m_points[ i ].y() / height() );
	}
}

void CurveEdit::setPoints( QVector<QPointF>& i_points )
{
	if ( i_points.size() != m_points.size() )
		return;

	for ( int i = 0; i < m_points.size(); i++ )
	{
		m_points[ i ].setX( i_points[ i ].x() * width() );
		m_points[ i ].setY( ( 1.0f - i_points[ i ].y() ) * height() );
		m_rects[ i ].moveCenter( m_points[ i ] );
	}
	for ( int i = 0; i < m_lins.size(); i++ )
		m_lins[ i ].setPoints( m_points[ i ], m_points[ i + 1 ] );

	update();
}

QPointF CurveEdit::getPoint( int i_index )
{
	QPointF pos;
	if ( i_index >= 0 && i_index < m_points.size() )
	{
		pos.setX( m_points[ i_index ].x() / width() );
		pos.setY( 1.0f - m_points[ i_index ].y() / height() );
	}
	return pos;
}

void CurveEdit::setPoint( int i_index, QPointF &i_point )
{
	if ( i_index < 0 || i_index >= m_points.size() )
		return;

	m_points[ i_index ].setX( i_point.x() * width() );
	m_points[ i_index ].setY( ( 1.0f - i_point.y() ) * height() );

	m_rects[ i_index ].moveCenter( m_points[ i_index ] );
	if ( i_index < m_lins.size() )
		m_lins[ i_index ].setP1( m_points[ i_index ] );
	if ( i_index > 0 )
		m_lins[ i_index - 1 ].setP2( m_points[ i_index ] );

	update();
}

void CurveEdit::createRangeX()
{
	int lineCount = m_rangeX / m_rangeXDivide + 1;
	float distance = ( float ) width() / ( float ) ( lineCount - 1 );

	m_verticalRangeLines.resize( lineCount * 2 );
	for ( int i = 0, j = 0; i < lineCount * 2; i += 2, j++ )
	{
		m_verticalRangeLines[ i ].setX( j * distance );
		m_verticalRangeLines[ i ].setY( height() );
		m_verticalRangeLines[ i + 1 ].setX( j * distance );
		m_verticalRangeLines[ i + 1 ].setY( 0 );
	}

	int textCount = m_rangeX / m_rangeXTextDivide - 1;
	distance = ( float ) width() / ( float ) ( textCount + 1 );
	m_verticalRangeTexts.resize( textCount );
	for ( int i = 0; i < textCount; i++ )
	{
		m_verticalRangeTexts[ i ].first.setX( ( float ) ( i + 1 ) * distance + 5.0f );
		m_verticalRangeTexts[ i ].first.setY( height() - 1 );
		m_verticalRangeTexts[ i ].second = tr( "%1" ).arg( ( i + 1 ) * ( m_rangeX / ( textCount + 1 ) ) );
	}
}

void CurveEdit::createRangeY()
{
	int lineCount = m_rangeY / m_rangeYDivide + 1;
	float distance = ( float ) height() / ( float ) ( lineCount - 1 );

	m_horizontalRangeLines.resize( lineCount * 2 );
	for ( int i = 0, j = 0; i < lineCount * 2; i += 2, j++ )
	{
		m_horizontalRangeLines[ i ].setX( 0 );
		m_horizontalRangeLines[ i ].setY( ( float ) height() - j * distance );
		m_horizontalRangeLines[ i + 1 ].setX( width() );
		m_horizontalRangeLines[ i + 1 ].setY( ( float ) height() - j * distance );
	}

	int textCount = m_rangeY / m_rangeYTextDivide - 1;
	distance = ( float ) height() / ( float ) ( textCount + 1 );
	m_horizontalRangeTexts.resize( textCount );
	for ( int i = 0; i < textCount; i++ )
	{
		m_horizontalRangeTexts[ i ].first.setX( 1 );
		m_horizontalRangeTexts[ i ].first.setY( ( float ) height() - ( ( float ) ( i + 1 ) * distance - 5.0f ) );
		m_horizontalRangeTexts[ i ].second = tr( "%1" ).arg( ( i + 1 ) * ( m_rangeY / ( textCount + 1 ) ) );
	}
}

void CurveEdit::paintEvent( QPaintEvent *i_ev )
{
	QPainter painter( this );
	painter.fillRect( rect(), m_backColor );

	if ( m_rangeLineVisible )
	{
		painter.setPen( m_verticalRangeColor );
		painter.drawLines( m_verticalRangeLines );
		painter.setPen( m_horizontalRangeColor );
		painter.drawLines( m_horizontalRangeLines );
	}

	if ( m_points.size() )
	{
		painter.setPen( m_lineColor );
		painter.drawLines( m_lins );

		for ( int i = 0; i < m_rects.size(); i++ )
			painter.fillRect( m_rects[ i ], m_pointColor );
		painter.setPen( m_lineColor );
		painter.drawRects( m_rects );

		if ( m_selectedIndex != -1 )
		{
			painter.setPen( m_selectColor );
			painter.drawRect( m_rects[ m_selectedIndex ] );

			painter.setPen( m_textColor );
			if ( m_rangeTextVisible )
			{
				painter.drawText( width() / 2 - 30, 10, tr( "X: %1" ).arg( ( int ) ( ( m_points[ m_selectedIndex ].x() / ( float ) width() ) * m_rangeX ) ) );
				painter.drawText( width() / 2 - 30, 20, tr( "Y: %1" ).arg( ( int ) ( ( 1.0f - m_points[ m_selectedIndex ].y() / ( float ) height() ) * m_rangeY ) ) );
			} else
			{
				painter.drawText( 1, 10, tr( "X: %1" ).arg( m_points[ m_selectedIndex ].x() / ( float ) width() ) );
				painter.drawText( 1, 20, tr( "Y: %1" ).arg( 1.0f - m_points[ m_selectedIndex ].y() / ( float ) height() ) );
			}
		}
	}

	if ( m_rangeTextVisible )
	{
		painter.setPen( m_rangeTextColor );
		for ( int i = 0; i < m_horizontalRangeTexts.size(); i++ )
			painter.drawText( m_horizontalRangeTexts[ i ].first, m_horizontalRangeTexts[ i ].second );

		for ( int i = 0; i < m_verticalRangeTexts.size(); i++ )
		{
			painter.save();
			painter.translate( m_verticalRangeTexts[ i ].first );
			painter.rotate( 270.0f );
			painter.drawText( 0, 0, m_verticalRangeTexts[ i ].second );
			painter.restore();
		}
	}

	i_ev->accept();
}

void CurveEdit::resizeEvent( QResizeEvent *i_ev )
{
	if ( i_ev->oldSize().width() == -1 || i_ev->oldSize().height() == -1 )
	{
		float distance = ( float ) width() / ( float ) ( m_points.size() - 1 );
		for ( int i = 0; i < m_points.size(); i++ )
		{
			m_points[ i ].setX( ( float ) i * distance );
			m_points[ i ].setY( ( float ) height() / 2.0f );
			m_rects[ i ].moveCenter( m_points[ i ] );
		}
		for ( int i = 0; i < m_lins.size(); i++ )
			m_lins[ i ].setPoints( m_points[ i ], m_points[ i + 1 ] );
	} else
	{
		for ( int i = 0; i < m_points.size(); i++ )
		{
			m_points[ i ].setX( m_points[ i ].x() * ( float ) i_ev->size().width() / ( float ) i_ev->oldSize().width() );
			m_points[ i ].setY( m_points[ i ].y() * ( float ) i_ev->size().height() / ( float ) i_ev->oldSize().height() );
			m_rects[ i ].moveCenter( m_points[ i ] );
		}
		for ( int i = 0; i < m_lins.size(); i++ )
			m_lins[ i ].setPoints( m_points[ i ], m_points[ i + 1 ] );

		createRangeX();
		createRangeY();
	}
	//update();
}

void CurveEdit::mousePressEvent( QMouseEvent *i_ev )
{
	if ( i_ev->buttons() & Qt::LeftButton )
	{
		i_ev->accept();
		//setCursor( Qt::CrossCursor );
		QPointF pos;
		pos.setX( ( float ) i_ev->pos().x() );
		pos.setY( ( float ) i_ev->pos().y() );

		m_selectedIndex = -1;
		for ( int i = 0; i < m_rects.size(); i++ )
		{
			if ( pos.x() >= m_rects[ i ].left() - 4 && 
				 pos.x() <= m_rects[ i ].right() + 4 && 
				 pos.y() >= m_rects[ i ].top() - 4 && 
				 pos.y() <= m_rects[ i ].bottom() + 4 )
			{
				if ( m_selectedIndex != i )
				{
					m_selectedIndex = i;
					update();
					emit signalIndexChanged( m_selectedIndex );
				}
				break;
			}
		}
	} else
		QWidget::mousePressEvent( i_ev );
}

void CurveEdit::mouseMoveEvent( QMouseEvent *i_ev )
{
	if ( m_selectedIndex != -1 &&
		 i_ev->buttons() & Qt::LeftButton )
	{
		i_ev->accept();

		QPointF pos = i_ev->pos();
		if ( pos.x() > width() )
			pos.setX( width() );
		else if ( pos.x() < 0 )
			pos.setX( 0 );

		if ( pos.y() > height() )
			pos.setY( height() );
		else if ( pos.y() < 0 )
			pos.setY( 0 );

		if ( m_limitedPoint )
		{
			if ( m_selectedIndex > 0 )
				if ( pos.x() <= m_points[ m_selectedIndex - 1 ].x() )
					pos.setX( m_points[ m_selectedIndex - 1 ].x() + 1.0f );
			if ( m_selectedIndex < m_rects.size() - 1 )
				if ( pos.x() >= m_points[ m_selectedIndex + 1 ].x() )
					pos.setX( m_points[ m_selectedIndex + 1 ].x() - 1.0f );
		}

		if ( m_vertivalMode )
			m_points[ m_selectedIndex ].setY( pos.y() );
		else
			m_points[ m_selectedIndex ] = pos;

		m_rects[ m_selectedIndex ].moveCenter( m_points[ m_selectedIndex ] );
		if ( m_selectedIndex < m_lins.size() )
			m_lins[ m_selectedIndex ].setP1( m_points[ m_selectedIndex ] );
		if ( m_selectedIndex > 0 )
			m_lins[ m_selectedIndex - 1 ].setP2( m_points[ m_selectedIndex ] );
		update();

		pos.setX( m_points[ m_selectedIndex ].x() / ( float ) width() );
		pos.setY( 1.0f - m_points[ m_selectedIndex ].y() / ( float ) height() );
		emit signalPointChanged( m_selectedIndex, pos );
	} else
		QWidget::mouseMoveEvent( i_ev );
}

void CurveEdit::keyPressEvent( QKeyEvent *i_ev )
{
	if ( i_ev->key() == Qt::Key_Up )
	{
		i_ev->accept();
	} else if ( i_ev->key() == Qt::Key_Down )
	{
		i_ev->accept();
	}else if ( i_ev->key() == Qt::Key_Right )
	{
		i_ev->accept();
	} else if ( i_ev->key() == Qt::Key_Left )
	{
		i_ev->accept();
	} else
		QWidget::keyPressEvent( i_ev );
}

void CurveEdit::contextMenuEvent( QContextMenuEvent *i_ev )
{
	i_ev->accept();
	m_menu->exec( mapToGlobal( i_ev->pos() ) );
}

void CurveEdit::actionTriggered( QAction* i_action )
{
	if ( i_action->text() == tr( "Range Line Visible" ) )
		setRangeLineVisible( i_action->isChecked() );
	else if ( i_action->text() == tr( "Range Text Visible" ) )
		setRangeTextVisible( i_action->isChecked() );
	else if ( i_action->text() == tr( "Vertical Mode" ) )
		setVertivalMode( i_action->isChecked() );
	else if ( i_action->text() == tr( "Limited Point" ) )
		setLimitedPoint( i_action->isChecked() );
	else if ( i_action->text() == tr( "Reset" ) )
		setPointCount( getPointCount() );
}

