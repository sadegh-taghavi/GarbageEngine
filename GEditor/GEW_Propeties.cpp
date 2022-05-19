#include "GEW_Propeties.h"
#include "ui_GEW_Propeties.h"
#include "GEW_Item.h"
#include "GEW_Light.h"
#include "GEW_Flare.h"
#include "GEW_Decal.h"
#include "GEW_Entity.h"
#include <QToolBar>

GEW_Propeties * GEW_Propeties::m_singleton = NULL;

GEW_Propeties::GEW_Propeties( QWidget *i_parent /*= 0 */ )
: QWidget( i_parent ),
m_ui( new Ui::GEW_Propeties )
{
	m_singleton = this;
	m_ui->setupUi( this );
	m_ui->stackedWidget->setCurrentWidget( m_ui->EmptyPage );
	QToolBar *toolBar = new QToolBar( this );
	toolBar->setIconSize( QSize( 16, 16 ) );
	m_ui->ToolBarLayout->addWidget( toolBar );
	toolBar->addAction( m_ui->PostProcess );
	toolBar->addAction(m_ui->Terrain);
	toolBar->addAction(m_ui->Screenshot);
	toolBar->addAction( m_ui->DefaultCamera );
}

GEW_Propeties::~GEW_Propeties()
{
	delete m_ui;
}

void GEW_Propeties::slotSetItem( GEW_Item *i_item )
{
	if ( !i_item )
	{
		m_ui->stackedWidget->setCurrentWidget( m_ui->EmptyPage );
		m_ui->NoItem->setText( "No Item Selected" );
		return;
	}

	switch ( i_item->type() )
	{
	case GEW_Item::Model:
		m_ui->ModelProperty->setModel( ( GEW_Model* )i_item );
		m_ui->stackedWidget->setCurrentWidget( m_ui->ModelPropertyPage );
		break;
	case GEW_Item::Light:
		m_ui->LightProperty->setLight( ( GEW_Light * )i_item );
		m_ui->stackedWidget->setCurrentWidget( m_ui->LightPropertyPage );
		break;
	case GEW_Item::Flare:
		m_ui->FlareProperty->setFlare( ( GEW_Flare * )i_item );
		m_ui->stackedWidget->setCurrentWidget( m_ui->FlarePropertyPage );
		break;
	case GEW_Item::Decal:
		m_ui->DecalProperty->setDecal( ( GEW_Decal * )i_item );
		m_ui->stackedWidget->setCurrentWidget( m_ui->DecalPropertyPage );
		break;
	case GEW_Item::Entity:
		m_ui->EntityProperty->setEntity( ( GEW_Entity * ) i_item );
		m_ui->stackedWidget->setCurrentWidget( m_ui->EntityPropertyPage );
		break;
	default:
		m_ui->stackedWidget->setCurrentWidget( m_ui->EmptyPage );
		m_ui->NoItem->setText( "This item has no property." );
		break;
	}
}

void GEW_Propeties::slotSetPage( int i_index )
{
	if ( i_index < m_ui->stackedWidget->count() )
		m_ui->stackedWidget->setCurrentIndex( i_index );
}

void GEW_Propeties::on_PostProcess_triggered()
{
	m_ui->stackedWidget->setCurrentWidget( m_ui->PostprocessPropertyPage );
}

void GEW_Propeties::on_Terrain_triggered()
{
	m_ui->stackedWidget->setCurrentWidget( m_ui->TerrainPropertyPage );
}

void GEW_Propeties::on_Screenshot_triggered()
{
	m_ui->stackedWidget->setCurrentWidget(m_ui->ScreenshotPage);
}

void GEW_Propeties::on_DefaultCamera_triggered()
{
	m_ui->stackedWidget->setCurrentWidget( m_ui->DefaultCameraPage );
}
