#pragma once

#include <QWidget>

namespace Ui { class EntityPtoperty; }
class GEW_Entity;

class GEW_EntityPtoperty : public QWidget
{
	Q_OBJECT
public:
	explicit GEW_EntityPtoperty( QWidget *i_parent = 0 );
	~GEW_EntityPtoperty();

	void setEntity( GEW_Entity *i_entity );
	GEW_Entity *getEntity();
private slots:
	void on_Add_triggered();
	void on_Remove_triggered();
	void on_Search_triggered();

private:
	Ui::EntityPtoperty *m_ui;
	GEW_Entity *m_entity;
};
