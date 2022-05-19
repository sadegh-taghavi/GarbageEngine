#ifndef GRADIENTSLIDER_H
#define GRADIENTSLIDER_H

#include <QSlider>

/**
* \brief A slider that mover on top of a gradient
*/
class GradientSlider : public QSlider
{
	Q_OBJECT
		Q_PROPERTY( QBrush background READ background WRITE setBackground )
		Q_PROPERTY( QVector<QColor> colors READ colors WRITE setColors )
		Q_PROPERTY( QLinearGradient gradient READ gradient WRITE setGradient )
		Q_PROPERTY( QColor firstColor READ firstColor WRITE setFirstColor )
		Q_PROPERTY( QColor lastColor READ lastColor WRITE setLastColor )

private:
	QVector<QColor> m_colorList;
	QBrush m_backGroundBrush;

public:
	explicit GradientSlider( QWidget *parent = 0 );

	/// Get the background, it's visible for transparent gradient stops
	QBrush background() const { return m_backGroundBrush; }
	/// Set the background, it's visible for transparent gradient stops
	void setBackground( QBrush bg );

	/// Get the colors that make up the gradient
	QVector<QColor> colors() const { return m_colorList; }
	/// Set the colors that make up the gradient
	void setColors( QVector<QColor> bg );

	/**
	* \brief Set the gradient
	* \note  Only the color order is preserved, all stops will be equally spaced
	*/
	void setGradient( QLinearGradient bg );
	/// Get the gradient
	QLinearGradient gradient() const;

	/**
	* Set the first color of the gradient
	*
	* If the gradient is currently empty it will create a stop with the given color
	*/
	void setFirstColor( QColor c );
	/**
	* Set the last color of the gradient
	*
	* If the gradient is has less than two colors,
	* it will create a stop with the given color
	*/
	void setLastColor( QColor c );
	/**
	* Get the first color
	*
	* Returns QColor() con empty gradient
	*/
	QColor firstColor() const;
	/**
	* Get the last color
	*
	* Returns QColor() con empty gradient
	*/
	QColor lastColor() const;

protected:
	void paintEvent( QPaintEvent *ev );
};

#endif // GRADIENTSLIDER_H
