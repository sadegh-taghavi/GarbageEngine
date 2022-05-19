#include "GEW_EntityPtoperty.h"
#include "ui_GEW_EntityPtoperty.h"
#include <QToolBar>
#include "GEW_Entity.h"
#include "GEW_EntityBindDialog.h"
#include <QTableWidgetItem>

GEW_EntityPtoperty::GEW_EntityPtoperty( QWidget *i_parent /*= 0 */ ) : QWidget( i_parent ), m_ui( new Ui::EntityPtoperty )
{
	m_ui->setupUi( this );
	m_ui->BindTable->horizontalHeader()->setStretchLastSection( false );
	m_ui->BindTable->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );
	m_ui->BindTable->horizontalHeader()->setSectionResizeMode( 1, QHeaderView::ResizeToContents );
	m_ui->BindTable->horizontalHeader()->setSectionResizeMode( 2, QHeaderView::Stretch );
	m_ui->BindTable->horizontalHeader()->setSectionResizeMode( 3, QHeaderView::Stretch );
	m_ui->BindTable->horizontalHeader()->setSectionResizeMode( 4, QHeaderView::Fixed );
	QToolBar *toolBar = new QToolBar( this );
	toolBar->setIconSize( QSize( 16, 16 ) );
	toolBar->addAction( m_ui->Add );
	toolBar->addAction( m_ui->Remove );
	toolBar->addSeparator();
	toolBar->addAction( m_ui->Search );
	m_ui->ToolbarLayout->addWidget( toolBar );
	m_entity = NULL;
}

GEW_EntityPtoperty::~GEW_EntityPtoperty()
{
	delete m_ui;
}

void GEW_EntityPtoperty::setEntity( class GEW_Entity *i_entity )
{
	m_entity = i_entity;
	if( !m_entity )
		return;
}

class GEW_Entity * GEW_EntityPtoperty::getEntity()
{
	return m_entity;
}

void GEW_EntityPtoperty::on_Add_triggered()
{
	if( !m_entity )
		return;

	QStringList parentList;
	for( int i = 0; i < m_entity->childCount(); ++i )
	{
		GEW_Item *item = ( GEW_Item * ) m_entity->child( i );
		if( item->type() != GEW_Item::Type::Group ||
			item->type() != GEW_Item::Type::Entity ||
			item->type() != GEW_Item::Type::Root ||
			item->type() != GEW_Item::Type::Vehicle )
		{
			parentList << item->text( 0 );
		}
	}

	GEW_EntityBindDialog dialog( this );
	dialog.init( parentList, parentList, parentList );
	if( dialog.exec() == QDialog::Accepted )
	{
		int row = m_ui->BindTable->rowCount();
		m_ui->BindTable->insertRow( row );
		QTableWidgetItem *item;

		item = new QTableWidgetItem( tr( "Parent - %1" ).arg( row ) );
		item->setFlags( item->flags() & ~Qt::ItemIsEditable );
		m_ui->BindTable->setItem( row, 0, item );

		item = new QTableWidgetItem( tr( "Bound Type" ) );
		item->setFlags( item->flags() & ~Qt::ItemIsEditable );
		m_ui->BindTable->setItem( row, 1, item );

		item = new QTableWidgetItem( tr( "BoundPos - %1" ).arg( row ) );
		item->setFlags( item->flags() & ~Qt::ItemIsEditable );
		m_ui->BindTable->setItem( row, 2, item );

		item = new QTableWidgetItem( tr( "Child - %1" ).arg( row ) );
		item->setFlags( item->flags() & ~Qt::ItemIsEditable );
		m_ui->BindTable->setItem( row, 3, item );

		item = new QTableWidgetItem();
		item->setFlags( item->flags() & ~( Qt::ItemIsEditable | Qt::ItemIsUserCheckable ) );
		item->setCheckState( row % 2 ? Qt::Unchecked : Qt::Checked );
		m_ui->BindTable->setItem( row, 4, item );

		m_ui->BindTable->selectRow( row );
	}
}

void GEW_EntityPtoperty::on_Remove_triggered()
{
	int row = m_ui->BindTable->currentRow();
	if( row < 0 )
		return;

	m_ui->BindTable->removeRow( row );
}

void GEW_EntityPtoperty::on_Search_triggered()
{

}

