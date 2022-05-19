#ifndef CURVEEDIT_H
#define CURVEEDIT_H

#include <QWidget>

class QMenu;
class QAction;

class CurveEdit : public QWidget
{
	Q_OBJECT

	Q_PROPERTY( QColor	BackColor			READ getBackColor				WRITE setBackColor )
	Q_PROPERTY( QColor	LineColor			READ getLineColor				WRITE setLineColor )
	Q_PROPERTY( QColor	PointColor			READ getPointColor				WRITE setPointColor )
	Q_PROPERTY( QColor	SelectColor			READ getSelectColor				WRITE setSelectColor )
	Q_PROPERTY( QColor	TextColor			READ getTextColor				WRITE setTextColor )
	Q_PROPERTY( int		PointCount			READ getPointCount				WRITE setPointCount )
	Q_PROPERTY( bool	VertivalMode		READ isVertivalMode				WRITE setVertivalMode )
	Q_PROPERTY( bool	LimitedPoint		READ isLimitedPoint				WRITE setLimitedPoint )
	Q_PROPERTY( bool	RangeLineVisible	READ isRangeLineVisible			WRITE setRangeLineVisible )
	Q_PROPERTY( int		RangeX				READ getRangeX					WRITE setRangeX )
	Q_PROPERTY( int		RangeXDivide		READ getRangeXDivide			WRITE setRangeXDivide )
	Q_PROPERTY( int		RangeXTextDivide	READ getRangeXTextDivide		WRITE setRangeXTextDivide )
	Q_PROPERTY( QColor	VRangeColor			READ getVerticalRangeColor		WRITE setVerticalRangeColor )
	Q_PROPERTY( int		RangeY				READ getRangeY					WRITE setRangeY )
	Q_PROPERTY( int		RangeYDivide		READ getRangeYDivide			WRITE setRangeYDivide )
	Q_PROPERTY( int		RangeYTextDivide	READ getRangeYTextDivide		WRITE setRangeYTextDivide )
	Q_PROPERTY( QColor	HRangeColor			READ getHorizontalRangeColor	WRITE setHorizontalRangeColor )
	Q_PROPERTY( bool	RangeTextVisible	READ isRangeTextVisible			WRITE setRangeTextVisible )
	Q_PROPERTY( QColor	RangeTextColor		READ getRangeTextColor			WRITE setRangeTextColor )

	bool				m_vertivalMode;
	bool				m_limitedPoint;
	bool				m_rangeLineVisible;
	bool				m_rangeTextVisible;
	int					m_rangeX;
	int					m_rangeY;
	int					m_rangeXDivide;
	int					m_rangeYDivide;
	int					m_rangeXTextDivide;
	int					m_rangeYTextDivide;
	int					m_selectedIndex;
	QColor				m_backColor;
	QColor				m_lineColor;
	QColor				m_pointColor;
	QColor				m_selectColor;
	QColor				m_textColor;
	QColor				m_verticalRangeColor;
	QColor				m_horizontalRangeColor;
	QColor				m_rangeTextColor;
	QVector< QPointF >	m_points;
	QVector< QRectF >	m_rects;
	QVector< QLineF	>	m_lins;
	QVector< QPointF >	m_verticalRangeLines;
	QVector< QPointF >	m_horizontalRangeLines;
	QVector< QPair< QPointF, QString > > m_verticalRangeTexts;
	QVector< QPair< QPointF, QString > > m_horizontalRangeTexts;

	QMenu				*m_menu;
public:
	explicit CurveEdit( QWidget *parent = 0 );

	void setBackColor( QColor i_color );
	QColor getBackColor();
	void setLineColor( QColor i_color );
	QColor getLineColor();
	void setPointColor( QColor i_color );
	QColor getPointColor();
	void setSelectColor( QColor i_color );
	QColor getSelectColor();
	void setTextColor( QColor i_color );
	QColor getTextColor();
	void setVerticalRangeColor( QColor i_color );
	QColor getVerticalRangeColor();
	void setHorizontalRangeColor( QColor i_color );
	QColor getHorizontalRangeColor();
	void setRangeTextColor( QColor i_color );
	QColor getRangeTextColor();


	void setPointCount( int i_count );
	int getPointCount();

	void setVertivalMode( bool i_isVertical );
	bool isVertivalMode();

	void setLimitedPoint( bool i_isLimited );
	bool isLimitedPoint();

	void setRangeLineVisible( bool i_visible );
	bool isRangeLineVisible();

	void setRangeTextVisible( bool i_visible );
	bool isRangeTextVisible();

	void setRangeX( int i_range );
	int getRangeX();

	void setRangeY( int i_range );
	int getRangeY();

	void setRangeXDivide( int i_devide );
	int getRangeXDivide();

	void setRangeYDivide( int i_devide );
	int getRangeYDivide();

	void setRangeXTextDivide( int i_devide );
	int getRangeXTextDivide();

	void setRangeYTextDivide( int i_devide );
	int getRangeYTextDivide();

	int getCurrentIndex();
	void setCurrentIndex( int i_index );

	void getPoints( QVector<QPointF>& i_points );
	void setPoints( QVector<QPointF>& i_points );

	QPointF getPoint( int i_index );
	void setPoint( int i_index, QPointF &i_point );

signals:
	void signalPointChanged( int index, QPointF point );
	void signalIndexChanged( int index );

private:
	void createRangeX();
	void createRangeY();
	void paintEvent( QPaintEvent *i_ev );
	void resizeEvent( QResizeEvent *i_ev );
	void mousePressEvent( QMouseEvent *i_ev );
	void mouseMoveEvent( QMouseEvent *i_ev );
	void keyPressEvent( QKeyEvent *i_ev );
	void contextMenuEvent( QContextMenuEvent *i_ev );

private slots:
	void actionTriggered( QAction* );
};

#endif