#pragma once

#include <QWidget>
#include <QFile>

namespace Ui { class GEW_ItemList; }
class QTreeWidgetItem;

class GEW_ItemList : public QWidget
{
	Q_OBJECT
	Ui::GEW_ItemList *m_ui;
	class GEW_Item *m_root;
	class QMenu *m_menu;
	static GEW_ItemList *m_singleton;
	GEW_Item *m_lastSelected;

	QString generateName( GEW_Item *i_root, const QString &i_baseName );
	class GEW_Item *findItem( unsigned int i_id );
public:
	explicit GEW_ItemList( QWidget *i_parent = 0 );
	~GEW_ItemList();

	static GEW_ItemList &getSingleton() { return *m_singleton; }
	GEW_Item *getRoot() { return m_root; }
	void blockItemSignal( bool i_block );
	bool isItemSignalBlocked();
	void saveToFile( QFile &i_file, const QString &i_resourcePath );
	void LoadFromFile( QFile &i_file, const QString &i_resourcePath );
	void clearAll();

signals:
	void signalItemChanged( GEW_Item *item );

private slots:
	void on_ItemList_currentItemChanged( QTreeWidgetItem *i_new, QTreeWidgetItem *i_old );
	void on_ItemList_itemClicked( QTreeWidgetItem *i_item, int i_column );
	void on_ItemList_itemDoubleClicked( QTreeWidgetItem *i_item, int i_column );
	void on_ItemList_itemChanged( QTreeWidgetItem *i_item, int i_column );
	void on_ItemList_signalKeyPressed( Qt::Key i_key );
	void on_ItemList_customContextMenuRequested( QPoint i_pos );

	void on_Search_clicked();
	void on_ObjectVisible_toggled( bool i_checked );

	void on_AddModel_triggered();
	void on_AddLight_triggered();
	void on_AddFlare_triggered();
	void on_AddEnvironmentProbe_triggered();
	void on_AddDecal_triggered();
	void on_AddCamera_triggered();
	void on_AddSound_triggered();

	void on_AddEntity_triggered();
	void on_AddVehicle_triggered();
	void on_AddTree_triggered();

	void on_AddGroup_triggered();
};