#ifndef PAINTBORDER_H
#define PAINTBORDER_H

#include <QWidget>
#include <QtGui/QPainter>

inline void paintTlBorder( QPainter &painter, QSize sz, QColor c, int inpx )
{
	//c.setAlpha(0.5);
	painter.setPen( c );
	painter.drawLine( inpx, inpx, sz.width() - 1 - inpx, inpx );
	painter.drawLine( inpx, inpx, inpx, sz.height() - 1 - inpx );
}

inline void paintBrBorder( QPainter &painter, QSize sz, QColor c, int inpx )
{
	//c.setAlpha(0.5);
	painter.setPen( c );
	painter.drawLine( sz.width() - 1 - inpx, inpx,
					  sz.width() - 1 - inpx, sz.height() - 1 - inpx );
	painter.drawLine( inpx, sz.height() - 1 - inpx,
					  sz.width() - 1 - inpx, sz.height() - 1 - inpx );
}

#endif // PAINTBORDER_H
