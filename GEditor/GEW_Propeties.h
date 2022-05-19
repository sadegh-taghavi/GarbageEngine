#pragma once

#include <QWidget>

namespace Ui { class GEW_Propeties; }
class GEW_Item;

class GEW_Propeties : public QWidget
{
	Q_OBJECT
	Ui::GEW_Propeties *m_ui;
	static GEW_Propeties *m_singleton;

public:
	explicit GEW_Propeties( QWidget *i_parent = 0 );
	~GEW_Propeties();
	static GEW_Propeties &getSingleton() { return *m_singleton; }

public slots:
	void slotSetItem( GEW_Item *i_item );
	void slotSetPage( int i_index );

private slots:
	void on_PostProcess_triggered();
	void on_Terrain_triggered();
	void on_Screenshot_triggered();
	void on_DefaultCamera_triggered();
};