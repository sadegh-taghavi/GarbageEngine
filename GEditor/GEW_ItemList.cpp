#include "GEW_ItemList.h"
#include "ui_GEW_ItemList.h"
#include "GEW_Item.h"
#include <QTreeWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QCursor>
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include "GEW_Group.h"
#include "GEW_Entity.h"
#include "GEW_Light.h"
#include "GEW_LightProperty.h"
#include "GEW_Model.h"
#include "GEW_Item.h"
#include "GEW_Flare.h"
#include "GEW_Utility.h"
#include "GEW_Material.h"
#include "GEW_Decal.h"
#include "GEW_Scene.h"
#include <QToolButton>

GEW_ItemList *GEW_ItemList::m_singleton = NULL;

GEW_Item * GEW_ItemList::findItem( unsigned int i_id )
{
	return m_root->findItem( i_id );
}

uint32_t genName(
	QString &i_outName,
	GEW_Item *i_root,
	const QString &i_baseName,
	uint32_t i_baseId )
{
	while( true )
	{
		i_outName = i_baseName + QString( "%1" ).arg( i_baseId );
		if( i_root->findItem( i_outName.toLower() ) == NULL )
			break;
		i_baseId++;
	}
	return i_baseId;
}

QString GEW_ItemList::generateName( GEW_Item *i_root, const QString &i_baseName )
{
	int id = 0;
	QString outName;
	while( true )
	{
		id = genName( outName, i_root, i_baseName, id ) + 1;
		outName = QInputDialog::getText(
			this,
			tr( "Name" ),
			tr( "Name:" ),
			QLineEdit::Normal,
			outName );

		if( outName.isEmpty() )
			break;

		if( m_root->findItem( outName.toLower() ) )
		{
			if( QMessageBox::warning(
				this,
				tr( "Name" ),
				tr( "This name is already exist!\nDo you want to use this name any way?" ),
				QMessageBox::Yes,
				QMessageBox::No ) == QMessageBox::Yes )
				break;
		} else
			break;
	}
	return outName;
}

GEW_ItemList::GEW_ItemList( QWidget *i_parent /*= 0 */ )
: QWidget( i_parent ),
m_ui( new Ui::GEW_ItemList )
{
	m_ui->setupUi( this );
	m_ui->ItemList->header()->setStretchLastSection( false );
	m_ui->ItemList->header()->setSectionResizeMode( 0, QHeaderView::Stretch );
	m_ui->ItemList->header()->setSectionResizeMode( 1, QHeaderView::Fixed );
	m_ui->ItemList->header()->setSectionResizeMode( 2, QHeaderView::Fixed );
	m_ui->ItemList->header()->setSectionResizeMode( 3, QHeaderView::Fixed );
	m_ui->ItemList->blockSignals( true );
	m_root = new GEW_Item( m_ui->ItemList, GEW_Item::Root );
	m_ui->ItemList->setCurrentItem( m_root );
	m_ui->ItemList->setRootIndex( m_ui->ItemList->currentIndex() );
	m_ui->ItemList->blockSignals( false );

	m_menu = new QMenu( this );
	QMenu *basicMenu = m_menu->addMenu( m_ui->actionAddBasic->icon(), m_ui->actionAddBasic->text() );
	basicMenu->addAction( m_ui->AddModel );
	basicMenu->addAction( m_ui->AddLight );
	basicMenu->addAction( m_ui->AddFlare );
	basicMenu->addAction( m_ui->AddEnvironmentProbe );
	basicMenu->addAction( m_ui->AddDecal );
	basicMenu->addAction( m_ui->AddCamera );
	basicMenu->addAction( m_ui->AddSound );
	QMenu *entityMenu = m_menu->addMenu( m_ui->actionAddEntity->icon(), m_ui->actionAddEntity->text() );
	entityMenu->addAction( m_ui->AddVehicle );
	entityMenu->addAction( m_ui->AddTree );
	m_menu->addSeparator();
	m_menu->addAction( m_ui->AddGroup );
	m_menu->addAction( m_ui->AddEntity );

	QToolBar *toolBar = new QToolBar( this );
	toolBar->setIconSize( QSize( 16, 16 ) );
	QToolButton *basicToolButton = new QToolButton( this );
	basicToolButton->setIcon( m_ui->actionAddBasic->icon() );
	basicToolButton->setToolTip( "Basic Items" );
	basicToolButton->setMenu( basicMenu );
	basicToolButton->setPopupMode( QToolButton::InstantPopup );
	toolBar->addWidget( basicToolButton );
	QToolButton *entityToolButton = new QToolButton( this );
	entityToolButton->setIcon( m_ui->actionAddEntity->icon() );
	entityToolButton->setToolTip( "Entities" );
	entityToolButton->setMenu( entityMenu );
	entityToolButton->setPopupMode( QToolButton::InstantPopup );
	toolBar->addWidget( entityToolButton );
	toolBar->addSeparator();
	toolBar->addAction( m_ui->AddGroup );
	toolBar->addAction( m_ui->AddEntity );
	m_ui->MenuBarLayout->addWidget( toolBar );

	m_singleton = this;
	m_lastSelected = NULL;
}

GEW_ItemList::~GEW_ItemList()
{
	delete m_ui;
	m_singleton = NULL;
}

void GEW_ItemList::blockItemSignal( bool i_block )
{
	m_ui->ItemList->blockSignals( i_block );
}

bool GEW_ItemList::isItemSignalBlocked()
{
	return m_ui->ItemList->signalsBlocked();
}

void GEW_ItemList::saveToFile( QFile &i_file, const QString &i_resourcePath )
{
	uint32_t count = GEW_Item::getCount();
	i_file.write( ( const char* ) &count, sizeof( uint32_t ) );
	m_root->saveToFile( i_file, i_resourcePath );
}

void GEW_ItemList::LoadFromFile( QFile &i_file, const QString &i_resourcePath )
{
	uint32_t count;
	i_file.read( ( char* ) &count, sizeof( uint32_t ) );
	m_ui->ItemList->blockSignals( true );
	while( count-- )
	{
		GEW_Item::SaveHeader header;
		if( i_file.read( ( char* ) &header, sizeof( GEW_Item::SaveHeader ) ) != sizeof( GEW_Item::SaveHeader ) )
			break;

		GEW_Item *parent = m_root->findItem( header.ParentId );

		switch( header.MyType )
		{
		case GEW_Item::Group:
		{
			GEW_Group *gewGroup = GEW_Group::CreateFromFile( parent, i_file, i_resourcePath );
			gewGroup->setId( header.MyId );
		}break;

		case GEW_Item::Entity:
		{
			GEW_Entity *gewEntity = GEW_Entity::CreateFromFile( parent, i_file, i_resourcePath );
			gewEntity->setId( header.MyId );
		}break;

		case GEW_Item::Model:
		{
			GEW_Model *gewModel = GEW_Model::CreateFromFile( parent, i_file, i_resourcePath );
			gewModel->setId( header.MyId );
		}break;

		case GEW_Item::Light:
		{
			GEW_Light *gewLight = GEW_Light::CreateFromFile( parent, i_file, i_resourcePath );
			gewLight->setId( header.MyId );
		}break;

		case GEW_Item::Flare:
		{
			GEW_Flare *gewFlare = GEW_Flare::CreateFromFile( parent, i_file, i_resourcePath );
			gewFlare->setId( header.MyId );
		}break;

		case GEW_Item::Decal:
		{
			GEW_Decal *gewDecal = GEW_Decal::createFromFile( parent, i_file, i_resourcePath );
			gewDecal->setId( header.MyId );
		}break;

		default:
			break;
		}
	}

	m_ui->ItemList->blockSignals( false );
}

void GEW_ItemList::clearAll()
{
	m_ui->ItemList->blockSignals( true );
	emit signalItemChanged( NULL );
	if( m_lastSelected )
		m_lastSelected->deSelect();
	m_lastSelected = NULL;
	int cc = m_root->childCount();
	for( int i = 0; i < cc; ++i )
	{
		GEW_Item *item = ( GEW_Item* ) m_root->child(0 );
		delete item;
	}
	m_ui->ItemList->blockSignals( false );
}

void GEW_ItemList::on_ItemList_currentItemChanged( QTreeWidgetItem *i_new, QTreeWidgetItem *i_old )
{
	Q_UNUSED( i_old );

	GEW_Item *item = ( GEW_Item* ) i_new;

	if( m_lastSelected )
		m_lastSelected->deSelect();
	m_lastSelected = item;

	m_ui->UnicID->clear();

	if( item )
	{
		item->select();
		m_ui->UnicID->setText( tr( "%1" ).arg( item->getId() ) );
	}

	emit signalItemChanged( item );
}

void GEW_ItemList::on_ItemList_itemClicked( QTreeWidgetItem *i_item, int i_column )
{
	Q_UNUSED( i_column );
	on_ItemList_currentItemChanged( i_item, NULL );
}

void GEW_ItemList::on_ItemList_itemDoubleClicked( QTreeWidgetItem *i_item, int i_column )
{
	if( i_column == 0 ) // Rename
	{
		m_ui->ItemList->blockSignals( true );
		GEW_Item *item = ( GEW_Item* ) i_item;
		QString itemName;

		while( true )
		{
			itemName = QInputDialog::getText(
				this,
				tr( "Rename" ),
				tr( "Name:" ),
				QLineEdit::Normal,
				item->text( 0 ) );

			if( itemName.isEmpty() || itemName == item->text( 0 ) )
				break;

			if( m_root->findItem( itemName.toLower() ) )
			{
				if( QMessageBox::warning(
					this,
					tr( "Name" ),
					tr( "This name is already exist!\nDo you want to use this name any way?" ),
					QMessageBox::Yes,
					QMessageBox::No ) == QMessageBox::Yes )
				{
					item->setText( 0, itemName );
					break;
				}
			} else
			{
				item->setText( 0, itemName );
				break;
			}
		}
		m_ui->ItemList->blockSignals( false );
	}
}

void GEW_ItemList::on_ItemList_itemChanged( QTreeWidgetItem *i_item, int i_column )
{
	m_ui->ItemList->blockSignals( true );
	( ( GEW_Item* ) i_item )->checkStateChanged( ( GEW_Item::CheckIndex )i_column );
	m_ui->ItemList->blockSignals( false );
}

void GEW_ItemList::on_ItemList_signalKeyPressed( Qt::Key i_key )
{
	if( i_key == Qt::Key_Delete )
	{
		if(
			QMessageBox::warning(
			this,
			tr( "Deleting objects..." ),
			tr( "Are you sure?" ),
			QMessageBox::Yes,
			QMessageBox::No ) == QMessageBox::Yes
			)
		{
			QList<QTreeWidgetItem*> itemList = m_ui->ItemList->selectedItems();
			QList<QTreeWidgetItem*> deleteItemList;
			foreach( QTreeWidgetItem *it, itemList )
			{
				bool reject = false;
				QTreeWidgetItem *parent = it->parent();
				while( parent && parent != m_root )
				{
					if( itemList.count( parent ) != 0 )
					{
						reject = true;
						break;
					}
					parent = parent->parent();
				}
				if( !reject ) //item has no parent in this list, so delete it
				{
					deleteItemList.push_back( it );
				}
			}
			m_ui->ItemList->blockSignals( true );
			foreach( QTreeWidgetItem *item, deleteItemList )
				delete ( ( GEW_Item* ) item );
			m_ui->ItemList->blockSignals( false );
			m_lastSelected = NULL;
			emit signalItemChanged( NULL );
		}
	} else if( i_key == Qt::Key_F2 )
	{
		if( m_ui->ItemList->currentItem() )
			on_ItemList_itemDoubleClicked( m_ui->ItemList->currentItem(), 0 );
	}
}

void GEW_ItemList::on_ItemList_customContextMenuRequested( QPoint i_pos )
{
	Q_UNUSED( i_pos );
	m_menu->popup( QCursor::pos() );
}

void GEW_ItemList::on_Search_clicked()
{

}

void GEW_ItemList::on_ObjectVisible_toggled( bool i_checked )
{
	m_root->setObjectVisible( i_checked );
	GEW_Item *item = ( GEW_Item* ) m_ui->ItemList->currentItem();
	if( !i_checked &&
		item &&
		item->type() != GEW_Item::Group &&
		item->type() != GEW_Item::Entity )
	{
		m_ui->ItemList->setCurrentItem( item->parent() );
	}
}

void GEW_ItemList::on_AddModel_triggered()
{
	GEW_Item *parent = ( GEW_Item* ) m_ui->ItemList->currentItem();
	if( !parent )
		parent = m_root;

	while( true )
	{
		if( parent->type() == GEW_Item::Root ||
			parent->type() == GEW_Item::Group ||
			parent->type() == GEW_Item::Vehicle ||
			parent->type() == GEW_Item::Entity )
			break;
		parent = ( GEW_Item * ) parent->parent();
	}

	if( parent->type() != GEW_Item::Vehicle && parent->type() != GEW_Item::Entity )
	{
		QString entityName = generateName( parent, tr( "ModelEntity_" ) );
		if( entityName.isEmpty() )
			return;
		m_ui->ItemList->blockSignals( true );
		GEW_Entity *gewEntity = new GEW_Entity( entityName, parent );
		m_ui->ItemList->blockSignals( false );
		parent = gewEntity;
	}

	QString dir = GEW_Model::m_lastFileName.left(
		GEW_Model::m_lastFileName.length() -
		GEW_Model::m_lastFileName.split( "/" ).last().length() );

	QStringList files = QFileDialog::getOpenFileNames(
		this,
		tr( "Select model files" ),
		dir,
		tr( "GEngine Model (*.GModel)" ) );
	if( !files.count() )
		return;

	m_ui->ItemList->blockSignals( true );
	foreach( QString fileName, files )
	{
		GEW_Model *gewModel = new GEW_Model( fileName, parent );
		gewModel->setObjectVisible( m_ui->ObjectVisible->isChecked() );
	}
	m_ui->ItemList->blockSignals( false );
	GEW_Model::m_lastFileName = files.last();
}

void GEW_ItemList::on_AddLight_triggered()
{
	GEW_Item *parent = ( GEW_Item* ) m_ui->ItemList->currentItem();
	if( !parent )
		parent = m_root;

	while( true )
	{
		if( parent->type() == GEW_Item::Root ||
			parent->type() == GEW_Item::Group ||
			parent->type() == GEW_Item::Vehicle ||
			parent->type() == GEW_Item::Entity )
			break;
		parent = ( GEW_Item * ) parent->parent();
	}

	if( parent->type() != GEW_Item::Vehicle && parent->type() != GEW_Item::Entity )
	{
		QString entityName = generateName( parent, tr( "LightEntity_" ) );
		if( entityName.isEmpty() )
			return;
		m_ui->ItemList->blockSignals( true );
		GEW_Entity *gewEntity = new GEW_Entity( entityName, parent );
		m_ui->ItemList->blockSignals( false );
		parent = gewEntity;
	}

	QString lightName = generateName( parent, tr( "Light_" ) );
	if( lightName.isEmpty() )
		return;
	m_ui->ItemList->blockSignals( true );
	GEW_Light *gewLight = new GEW_Light( lightName, parent );
	gewLight->setObjectVisible( m_ui->ObjectVisible->isChecked() );
	m_ui->ItemList->blockSignals( false );
}

void GEW_ItemList::on_AddFlare_triggered()
{
	GEW_Item *parent = ( GEW_Item* ) m_ui->ItemList->currentItem();
	if( !parent )
		parent = m_root;

	while( true )
	{
		if( parent->type() == GEW_Item::Root ||
			parent->type() == GEW_Item::Group ||
			parent->type() == GEW_Item::Vehicle ||
			parent->type() == GEW_Item::Entity )
			break;
		parent = ( GEW_Item * ) parent->parent();
	}

	if( parent->type() != GEW_Item::Vehicle && parent->type() != GEW_Item::Entity )
	{
		QString entityName = generateName( parent, tr( "FlareEntity_" ) );
		if( entityName.isEmpty() )
			return;
		m_ui->ItemList->blockSignals( true );
		GEW_Entity *gewEntity = new GEW_Entity( entityName, parent );
		m_ui->ItemList->blockSignals( false );
		parent = gewEntity;
	}

	QString flareName = generateName( parent, tr( "Flare_" ) );
	if( flareName.isEmpty() )
		return;
	m_ui->ItemList->blockSignals( true );
	GEW_Flare *gewFlare = new GEW_Flare( flareName, parent );
	gewFlare->setObjectVisible( m_ui->ObjectVisible->isChecked() );
	m_ui->ItemList->blockSignals( false );
}

void GEW_ItemList::on_AddEnvironmentProbe_triggered()
{

}

void GEW_ItemList::on_AddDecal_triggered()
{
	GEW_Item *parent = ( GEW_Item* ) m_ui->ItemList->currentItem();
	if( !parent )
		parent = m_root;

	while( true )
	{
		if( parent->type() == GEW_Item::Root ||
			parent->type() == GEW_Item::Group ||
			parent->type() == GEW_Item::Vehicle ||
			parent->type() == GEW_Item::Entity )
			break;
		parent = ( GEW_Item * ) parent->parent();
	}

	if( parent->type() != GEW_Item::Vehicle && parent->type() != GEW_Item::Entity )
	{
		QString entityName = generateName( parent, tr( "DecalEntity_" ) );
		if( entityName.isEmpty() )
			return;
		m_ui->ItemList->blockSignals( true );
		GEW_Entity *gewEntity = new GEW_Entity( entityName, parent );
		m_ui->ItemList->blockSignals( false );
		parent = gewEntity;
	}

	QString decalName = generateName( parent, tr( "Decal_" ) );
	if( decalName.isEmpty() )
		return;
	m_ui->ItemList->blockSignals( true );
	GEW_Decal *gewDecal = new GEW_Decal( decalName, parent );
	gewDecal->setObjectVisible( m_ui->ObjectVisible->isChecked() );
	m_ui->ItemList->blockSignals( false );
}

void GEW_ItemList::on_AddCamera_triggered()
{

}

void GEW_ItemList::on_AddSound_triggered()
{

}

void GEW_ItemList::on_AddEntity_triggered()
{
	GEW_Item *parent = ( GEW_Item* ) m_ui->ItemList->currentItem();
	if( !parent )
		parent = m_root;

	while( true )
	{
		if( parent->type() == GEW_Item::Root || parent->type() == GEW_Item::Group )
			break;
		parent = ( GEW_Item * ) parent->parent();
	}

	QString entityName = generateName( parent, tr( "Entity_" ) );
	if( entityName.isEmpty() )
		return;

	m_ui->ItemList->blockSignals( true );
	GEW_Entity *gewEntity = new GEW_Entity( entityName, parent );
	m_ui->ItemList->blockSignals( false );
}

void GEW_ItemList::on_AddVehicle_triggered()
{

}

void GEW_ItemList::on_AddTree_triggered()
{

}

void GEW_ItemList::on_AddGroup_triggered()
{
	GEW_Item *parent = ( GEW_Item* ) m_ui->ItemList->currentItem();
	if( !parent )
		parent = m_root;

	while( true )
	{
		if( parent->type() == GEW_Item::Root || parent->type() == GEW_Item::Group )
			break;
		parent = ( GEW_Item * ) parent->parent();
	}

	QString groupName = generateName( parent, tr( "Group_" ) );
	if( groupName.isEmpty() )
		return;

	m_ui->ItemList->blockSignals( true );
	GEW_Group *gewGroup = new GEW_Group( groupName, parent );
	m_ui->ItemList->blockSignals( false );
}

