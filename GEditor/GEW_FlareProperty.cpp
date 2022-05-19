#include "GEW_FlareProperty.h"
#include "ui_GEW_FlareProperty.h"
#include "GEW_Flare.h"
#include "GEW_Item.h"
#include <GE_RenderUtility.h>
#include <QInputDialog>
#include <QMenu>
#include <QToolBar>

GEW_FlareProperty::GEW_FlareProperty( QWidget *i_parent /*= 0 */ )
: QWidget( i_parent ),
m_ui( new Ui::FlareProperty )
{
	m_ui->setupUi( this );
	m_flare = NULL;
	QToolBar *toolBar = new QToolBar( this );
	toolBar->setIconSize( QSize( 16, 16 ) );
	toolBar->addAction( m_ui->FlareAdd );
	toolBar->addAction( m_ui->FlareRemove );
	toolBar->addSeparator();
	toolBar->addAction( m_ui->FlareRename );
	toolBar->addSeparator();
	toolBar->addAction( m_ui->FlareDuplicate );
	toolBar->addAction( m_ui->FlareCopy );
	toolBar->addAction( m_ui->FlarePaste );
	m_ui->ToolBarLayout->addWidget( toolBar );
}

GEW_FlareProperty::~GEW_FlareProperty()
{
	delete m_ui;
}


void GEW_FlareProperty::setFlare( GEW_Flare *i_flare )
{
	m_flare = NULL;
	if ( !i_flare )
		return;

	m_ui->Enable->setChecked( i_flare->m_enable );
	QColor cl;
	cl.setRedF( i_flare->m_boundColor.x );
	cl.setGreenF( i_flare->m_boundColor.y );
	cl.setBlueF( i_flare->m_boundColor.z );
	cl.setAlphaF( 1.0f );
	m_ui->BoundColor->slotSetColor( cl );
	m_ui->VisiblePixelOffset->setValue( i_flare->m_flare->getVisiblePixelOffset() );
	m_ui->VisibleAngleOffset->setValue( GE_ConvertToDegrees( i_flare->m_flare->getVisibleAngleOffset() ) );
	m_ui->PositionOffsetX->setValue( i_flare->m_flare->getOccluderPositionOffset()->x );
	m_ui->PositionOffsetY->setValue( i_flare->m_flare->getOccluderPositionOffset()->y );
	m_ui->PositionOffsetZ->setValue( i_flare->m_flare->getOccluderPositionOffset()->z );
	if ( i_flare->m_flare->getTexture() )
		m_ui->Map->slotLoad( i_flare->m_flare->getTexture()->getTextureName() );
	else
		m_ui->Map->slotClear();
	m_ui->FlareList->clear();
	for ( int32_t i = 0; i < i_flare->m_flareNames.size(); ++i )
		QListWidgetItem *item = new QListWidgetItem( GEW_Flare::m_iconFlare, i_flare->m_flareNames[ i ], m_ui->FlareList );

	m_flare = i_flare;
}

GEW_Flare * GEW_FlareProperty::getFlare()
{
	return m_flare;
}

void GEW_FlareProperty::on_FlareList_itemChanged( QListWidgetItem* i_item )
{
	on_FlareList_itemClicked( i_item );
}

void GEW_FlareProperty::on_FlareList_itemClicked( QListWidgetItem* i_item )
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;
	QColor cl;
	cl.setRedF( m_flare->m_flare->getParameter()->data()[ index ].Diffuse.x );
	cl.setGreenF( m_flare->m_flare->getParameter()->data()[ index ].Diffuse.y );
	cl.setBlueF( m_flare->m_flare->getParameter()->data()[ index ].Diffuse.z );
	cl.setAlphaF( m_flare->m_flare->getParameter()->data()[ index ].Diffuse.w );
	m_ui->DiffuseColor->slotSetColor( cl );
	m_ui->Layer->setValue( m_flare->m_flare->getParameter()->data()[ index ].Layer );
	m_ui->DiffuseIntensity->setValue( m_flare->m_flare->getParameter()->data()[ index ].DiffuseIntensity );
	m_ui->AutoRotate->setChecked( m_flare->m_flare->getParameter()->data()[ index ].AutoRotation );
	m_ui->AutoScale->setChecked( m_flare->m_flare->getParameter()->data()[ index ].AutoScale );
	m_ui->ScaleMulX->setValue( m_flare->m_flare->getParameter()->data()[ index ].ScaleMultiplier.x );
	m_ui->ScaleMulY->setValue( m_flare->m_flare->getParameter()->data()[ index ].ScaleMultiplier.y );
	m_ui->PositionMul->setValue( m_flare->m_flare->getParameter()->data()[ index ].PositionMultiplier );
	m_ui->Rotation->setValue( GE_ConvertToDegrees( m_flare->m_flare->getParameter()->data()[ index ].Rotation ) );
}

void GEW_FlareProperty::on_FlareList_itemDoubleClicked( QListWidgetItem* i_item )
{
	Q_UNUSED( i_item );
}

void GEW_FlareProperty::on_Enable_toggled( bool i_checked )
{
	if ( !m_flare )
		return;
	m_flare->m_enable = i_checked;
	m_flare->setItemCheckState( GEW_Item::Enable, i_checked );
}

void GEW_FlareProperty::on_BoundColor_signalColorChanged( QColor i_color )
{
	if ( !m_flare )
		return;
	m_flare->m_boundColor.x = i_color.redF();
	m_flare->m_boundColor.y = i_color.greenF();
	m_flare->m_boundColor.z = i_color.blueF();
}

void GEW_FlareProperty::on_DiffuseColor_signalColorChanged( QColor i_color )
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;
	m_flare->m_flare->getParameter()->data()[ index ].Diffuse =
		GE_Vec4( i_color.redF(), i_color.greenF(), i_color.blueF(), i_color.alphaF() );
}

void GEW_FlareProperty::on_DiffuseIntensity_valueChanged( double i_value )
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;
	m_flare->m_flare->getParameter()->data()[ index ].DiffuseIntensity = ( float )i_value;
}

void GEW_FlareProperty::on_Layer_valueChanged( double i_value )
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;
	m_flare->m_flare->getParameter()->data()[ index ].Layer = ( float )i_value;
}

void GEW_FlareProperty::on_AutoScale_toggled( bool i_checked )
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;
	m_flare->m_flare->getParameter()->data()[ index ].AutoScale = i_checked;
}

void GEW_FlareProperty::on_AutoRotate_toggled( bool i_checked )
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;
	m_flare->m_flare->getParameter()->data()[ index ].AutoRotation = i_checked;
}

void GEW_FlareProperty::on_ScaleMulX_valueChanged( double i_value )
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;
	m_flare->m_flare->getParameter()->data()[ index ].ScaleMultiplier.x = ( float )i_value;
}

void GEW_FlareProperty::on_ScaleMulY_valueChanged( double i_value )
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;
	m_flare->m_flare->getParameter()->data()[ index ].ScaleMultiplier.y = ( float )i_value;
}

void GEW_FlareProperty::on_PositionMul_valueChanged( double i_value )
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;
	m_flare->m_flare->getParameter()->data()[ index ].PositionMultiplier = ( float )i_value;
}

void GEW_FlareProperty::on_Rotation_valueChanged( double i_value )
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;
	m_flare->m_flare->getParameter()->data()[ index ].Rotation = GE_ConvertToRadians( ( float )i_value );
}

void GEW_FlareProperty::on_VisiblePixelOffset_valueChanged( int i_value )
{
	if ( !m_flare )
		return;
	m_flare->m_flare->setVisiblePixelOffset( i_value );
}

void GEW_FlareProperty::on_VisibleAngleOffset_valueChanged( double i_value )
{
	if ( !m_flare )
		return;
	m_flare->m_flare->setVisibleAngleOffset( GE_ConvertToRadians( ( float )i_value ) );
}

void GEW_FlareProperty::on_Map_signalImageChanged( QString i_fileName )
{
	if ( !m_flare )
		return;
	if ( i_fileName.isEmpty() )
		m_flare->m_flare->deleteTexture();
	else
	{
		m_flare->m_flare->setTexture( i_fileName.toStdString().c_str() );
		if ( m_flare->m_flare->getParameter()->size() )
			m_flare->m_flare->setNumberOfFlare( ( uint32_t )m_flare->m_flare->getParameter()->size() );
	}
}

void GEW_FlareProperty::on_FlareAdd_triggered()
{
	if ( !m_flare )
		return;
	QString flareName = QInputDialog::getText(
		this,
		tr( "Add flare" ),
		tr( "Flare name:" ),
		QLineEdit::Normal,
		tr( "Flare_%1" ).arg( m_ui->FlareList->count() ) );
	if ( flareName.isEmpty() )
		return;

	QListWidgetItem *it = new QListWidgetItem( GEW_Flare::m_iconFlare, flareName, m_ui->FlareList );
	m_flare->m_flareNames.push_back( flareName );
	m_flare->m_flare->addFlare();
}

void GEW_FlareProperty::on_FlareRemove_triggered()
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;
	QListWidgetItem *it = m_ui->FlareList->currentItem();
	delete it;
	m_flare->m_flareNames.removeAt( index );
	m_flare->m_flare->removeFlare( index );
}

void GEW_FlareProperty::on_FlareDuplicate_triggered()
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;

	QString flareName = m_ui->FlareList->currentItem()->text();

	QListWidgetItem *it = new QListWidgetItem( GEW_Flare::m_iconFlare, flareName, m_ui->FlareList );
	m_flare->m_flareNames.push_back( flareName );
	m_flare->m_flare->addFlare();
	m_flare->m_flare->getParameter()->data()[ m_flare->m_flare->getParameter()->size() - 1 ] = m_flare->m_flare->getParameter()->data()[ index ];
	m_ui->FlareList->setCurrentItem( it );
	on_FlareList_itemClicked( it );

}

void GEW_FlareProperty::on_FlareCopy_triggered()
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;

	m_copyParameter = m_flare->m_flare->getParameter()->data()[ index ];

}

void GEW_FlareProperty::on_FlarePaste_triggered()
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;
	m_flare->m_flare->getParameter()->data()[ index ] = m_copyParameter;
	on_FlareList_itemClicked( m_ui->FlareList->currentItem() );

}

void GEW_FlareProperty::on_FlareRename_triggered()
{
	if ( !m_flare )
		return;
	int32_t index = m_ui->FlareList->currentRow();
	if ( index < 0 )
		return;

	QString flareName = QInputDialog::getText(
		this,
		tr( "Flare" ),
		tr( "Flare name:" ),
		QLineEdit::Normal,
		m_ui->FlareList->item( index )->text() );
	if ( flareName.isEmpty() )
		return;

	m_ui->FlareList->item( index )->setText( flareName );
	m_flare->m_flareNames[ index ] = flareName;
}

void GEW_FlareProperty::on_FlareList_signalItemOrderChanged( QList<QListWidgetItem*> &i_oldItems, QList<QListWidgetItem*> &i_newItems )
{
	Q_UNUSED( i_oldItems );
	QVector<GE_LensFlare::Parameter> parList;
	parList.resize( i_newItems.size() );
	m_flare->m_flareNames.clear();
	for ( int32_t i = 0; i < i_newItems.size(); ++i )
	{
		m_flare->m_flareNames.push_back( i_newItems[ i ]->text() );
		parList.push_back( m_flare->m_flare->getParameter()->data()[ i ] );
		parList[ i ] = m_flare->m_flare->getParameter()->data()[ i_oldItems.indexOf( i_newItems[ i ] ) ];
	}
	for ( int32_t i = 0; i < i_newItems.size(); ++i )
		m_flare->m_flare->getParameter()->data()[ i ] = parList[ i ];
}

void GEW_FlareProperty::on_PositionOffsetX_valueChanged( double i_value )
{

	m_flare->m_flare->setOccluderPositionOffset( GE_Vec3( ( float )m_ui->PositionOffsetX->value(),
		( float )m_ui->PositionOffsetY->value(), ( float )m_ui->PositionOffsetZ->value() ) );
}

void GEW_FlareProperty::on_PositionOffsetY_valueChanged( double i_value )
{
	on_PositionOffsetX_valueChanged( i_value );
}

void GEW_FlareProperty::on_PositionOffsetZ_valueChanged( double i_value )
{
	on_PositionOffsetX_valueChanged( i_value );
}
void GEW_FlareProperty::on_FlareList_customContextMenuRequested( QPoint )
{
	QMenu menu( this );
	menu.addAction( m_ui->FlareRename );
	menu.addSeparator();
	menu.addAction( m_ui->FlareAdd );
	menu.addAction( m_ui->FlareRemove );
	menu.addSeparator();
	menu.addAction( m_ui->FlareDuplicate );
	menu.addSeparator();
	menu.addAction( m_ui->FlareCopy );
	menu.addAction( m_ui->FlarePaste );
	menu.exec( QCursor::pos() );
}

void GEW_FlareProperty::on_FlareList_signalKeyPressed( Qt::Key i_key )
{
	if ( i_key == Qt::Key_Delete )
		on_FlareRemove_triggered();
	else if ( i_key == Qt::Key_F2 )
		on_FlareRename_triggered();
}
