#ifndef COLORWHEEL_H
#define COLORWHEEL_H

#include <QWidget>
#include <qmath.h>

/**
* \brief Display an analog widget that allows the selection of a HSV color
*
* It has an outer wheel to select the Hue and an intenal square to select
* Saturation and Lightness.
*/
class ColorWheel : public QWidget
{
	Q_OBJECT

	Q_PROPERTY( QColor color READ color WRITE slotSetColor NOTIFY signalColorChanged DESIGNABLE true STORED false )
	Q_PROPERTY( qreal hue READ hue WRITE slotSetHue DESIGNABLE false )
	Q_PROPERTY( qreal saturation READ saturation WRITE slotSetSaturation DESIGNABLE false )
	Q_PROPERTY( qreal value READ value WRITE slotSetValue DESIGNABLE false )
	Q_PROPERTY( unsigned wheelWidth READ wheelWidth WRITE setWheelWidth DESIGNABLE true )

private:
	qreal m_huem, m_saturation, m_value;
	unsigned m_wheelWidth;
	enum MouseStatus
	{
		Nothing,
		DragCircle,
		DragSquare
	} m_mouseStatus;
	QPixmap m_hueRing;
	QImage m_saturationValueSquare;


public:
	explicit ColorWheel( QWidget *parent = 0 );

	/// Get current color
	QColor color() const;

	QSize sizeHint() const;

	/// Get current hue in the range [0-1]
	qreal hue() const { return m_huem; }

	/// Get current saturation in the range [0-1]
	qreal saturation() const { return m_saturation; }

	/// Get current value in the range [0-1]
	qreal value() const { return m_value; }

	/// Get the width in pixels of the outer wheel
	unsigned wheelWidth() const { return m_wheelWidth; }

	/// Set the width in pixels of the outer wheel
	void setWheelWidth( unsigned w );

public slots:
	/// Set current color
	void slotSetColor( QColor c );

	/**
	* @param h Hue [0-1]
	*/
	void slotSetHue( qreal h );
	/**
	* @param s Saturation [0-1]
	*/
	void slotSetSaturation( qreal s );
	/**
	* @param v Value [0-1]
	*/
	void slotSetValue( qreal v );

signals:
	/**
	* Emitted when the user selects a color or setColor is called
	*/
	void signalColorChanged( QColor );
	/**
	* Emitted when the user selects a color
	*/
	void signalColorSelected( QColor );

protected:
	void paintEvent( QPaintEvent * );
	void mouseMoveEvent( QMouseEvent * );
	void mousePressEvent( QMouseEvent * );
	void mouseReleaseEvent( QMouseEvent * );
	void resizeEvent( QResizeEvent * );

private:
	/// Calculate outer wheel radius from idget center
	qreal outerRadius() const
	{
		return qMin( geometry().width(), geometry().height() ) / 2;
	}
	/// Calculate inner wheel radius from idget center
	qreal innerRadius() const
	{
		return outerRadius() - m_wheelWidth;
	}
	/// return line from center to given point
	QLineF lineToPoint( QPoint p ) const
	{
		return QLineF( geometry().width() / 2, geometry().height() / 2, p.x(), p.y() );
	}
	/// Calculate the edge length of the inner square
	qreal squareSize() const
	{
		return innerRadius()*qSqrt( 2 );
	}
	/**
	* Updates the inner square that displays the saturation-value selector
	*/
	void renderRectangle();
	/**
	* Updates the outer ring that displays the hue selector
	*/
	void renderRing();

};

#endif // COLORWHEEL_H
