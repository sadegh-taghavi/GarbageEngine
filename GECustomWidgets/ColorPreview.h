#ifndef COLORPREVIEW_H
#define COLORPREVIEW_H

#include <QWidget>

/**
Simple widget that shows a preview of a color
*/
class ColorPreview : public QWidget
{
	Q_OBJECT
		Q_PROPERTY( QColor color READ color WRITE slotSetColor NOTIFY signalColorChanged DESIGNABLE true )
		Q_PROPERTY( AlphaMode m_alphaMode READ alphaMode WRITE setAlphaMode DESIGNABLE true )
		Q_PROPERTY( ViewMode m_viewMode READ viewMode WRITE setViewMode DESIGNABLE true )
		Q_PROPERTY( QBrush background READ getBackground WRITE setBackground DESIGNABLE true )
		Q_ENUMS( AlphaMode )
		Q_ENUMS( ViewMode )
public:
	enum AlphaMode
	{
		NoAlpha,    ///< Show only solid color
		SplitAlpha, ///< Show both solid and transparent side by side
		AllAlpha    ///< show only transparent
	};
	enum ViewMode  ///< Works when alpha mode is SplitAlpha
	{
		LeftToRight,
		RightToLeft,
		TopToBottom,
		BottomToTop
	};

private:
	QColor m_color; ///< color to be viewed
	QBrush m_backGroundBrush;///< Background brush, visible on transaprent color
	AlphaMode m_alphaMode; ///< How transparent colors are handled
	ViewMode  m_viewMode; ///< How to split colors

public:
	explicit ColorPreview( QWidget *parent = 0 );

	/// Change the background visible under transparent colors
	void setBackground( QBrush bk )
	{
		m_backGroundBrush = bk;
		update();
	}

	/// Get the background visible under transparent colors
	QBrush getBackground() const
	{
		return m_backGroundBrush;
	}

	/// Get how transparent colors are handled
	AlphaMode alphaMode() const
	{
		return m_alphaMode;
	}
	/// Set how transparent colors are handled
	void setAlphaMode( AlphaMode am )
	{
		m_alphaMode = am;
		update();
	}

	ViewMode viewMode() const
	{
		return m_viewMode;
	}
	void setViewMode( ViewMode vm )
	{
		m_viewMode = vm;
		update();
	}

	/// Get current color
	QColor color() const
	{
		return m_color;
	}

	QSize sizeHint() const;

	void paint( QPainter &painter, QRect rect ) const;

public slots:
	/// Set current color
	void slotSetColor( QColor c );

signals:
	/// Emitted when the user clicks on the widget
	void signalClicked();

	/// Emitted on setColor
	void signalColorChanged( QColor );

protected:
	void paintEvent( QPaintEvent * );
	void resizeEvent( QResizeEvent * );
	void mousePressEvent( QMouseEvent *ev );
	void mouseReleaseEvent( QMouseEvent *ev );
	void mouseMoveEvent( QMouseEvent *ev );
};

#endif // COLORPREVIEW_H
