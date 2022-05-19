#ifndef PXMATERIALVIEW_H
#define PXMATERIALVIEW_H

#include <QWidget>
namespace Ui { class PxMaterialView; }

class PxMaterialView : public QWidget
{
	Q_OBJECT
public:
	Ui::PxMaterialView *m_ui;
	PxMaterialView( QWidget *i_parent = 0 );
	~PxMaterialView();
private slots:
	void on_Materials_customContextMenuRequested( QPoint );
};

#endif