#include "GEW_DecalProperty.h"
#include "ui_GEW_DecalProperty.h"
#include "GEW_Decal.h"
#include <GE_Texture.h>
#include <GE_Decal.h>
#include <QInputDialog>
#include <QToolBar>
#include <QMenu>
#include <GE_RenderUtility.h>
#include "GEW_Camera.h"
#include "GEW_Scene.h"
#include "GEW_Prs.h"

GEW_DecalProperty::GEW_DecalProperty( QWidget *i_parent )
: QWidget( i_parent ), m_ui( new Ui::DecalProperty )
{
	m_ui->setupUi( this );
	m_decal = NULL;
	QToolBar *toolBar = new QToolBar( this );
	toolBar->setIconSize( QSize( 16, 16 ) );
	toolBar->addAction( m_ui->DecalAdd );
	toolBar->addAction( m_ui->DecalRemove );
	toolBar->addSeparator();
	toolBar->addAction( m_ui->DecalRename );
	toolBar->addSeparator();
	toolBar->addAction( m_ui->DecalDuplicate );
	toolBar->addAction( m_ui->DecalCopy );
	toolBar->addAction( m_ui->DecalPaste );
	m_ui->ToolBarLayout->addWidget( toolBar );
}

GEW_DecalProperty::~GEW_DecalProperty()
{
	delete m_ui;
}

void GEW_DecalProperty::on_Enable_toggled( bool i_checked )
{
	if( !m_decal )
		return;
	m_decal->m_enable = i_checked;
	if( i_checked )
	{
		for( int32_t i = 0; i < m_decal->m_properties.size(); ++i )
			m_decal->m_properties[ i ]->Bound->setColor( m_decal->m_properties[ i ]->BoundColor );
	} else
		for( int32_t i = 0; i < m_decal->m_properties.size(); ++i )
			m_decal->m_properties[ i ]->Bound->setColor( GE_Vec3( 0.2f, 0.2f, 0.2f ) );
}

void GEW_DecalProperty::on_DecalAdd_triggered()
{
	if( !m_decal )
		return;
	QString decalName = QInputDialog::getText(
		this,
		tr( "Add decal" ),
		tr( "decal name:" ),
		QLineEdit::Normal,
		tr( "decal_%1" ).arg( m_ui->DecalList->count() ) );
	if( decalName.isEmpty() )
		return;

	QListWidgetItem *it = new QListWidgetItem( GEW_Decal::m_iconDecal, decalName, m_ui->DecalList );
	GEW_Decal::Property *pr = new GEW_Decal::Property();
	pr->Name = decalName;
	m_decal->m_properties.push_back( pr );
	m_decal->m_decal->addDecal();
}

void GEW_DecalProperty::on_DecalRemove_triggered()
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	QListWidgetItem *it = m_ui->DecalList->currentItem();
	delete it;
	GEW_Decal::Property *pr = m_decal->m_properties[ index ];
	m_decal->m_properties.removeAt( index );
	delete pr;
	m_decal->m_decal->removeDecal( index );
	GEW_Decal::m_selectedItem = -1;
}

void GEW_DecalProperty::on_DecalRename_triggered()
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;

	QString decalName = QInputDialog::getText(
		this,
		tr( "Decal" ),
		tr( "Decal name:" ),
		QLineEdit::Normal,
		m_ui->DecalList->item( index )->text() );
	if( decalName.isEmpty() )
		return;

	m_ui->DecalList->item( index )->setText( decalName );
	m_decal->m_properties[ index ]->Name = decalName;
}

void GEW_DecalProperty::on_DecalDuplicate_triggered()
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;

	QString decalName = m_ui->DecalList->currentItem()->text();

	QListWidgetItem *it = new QListWidgetItem( GEW_Decal::m_iconDecal, decalName, m_ui->DecalList );
	GEW_Decal::Property *property = new GEW_Decal::Property();
	GEW_Decal::Property * gwProperty = m_decal->m_properties[ index ];
	property->Bound->setColor( gwProperty->BoundColor );
	property->BoundColor = gwProperty->BoundColor;
	property->Name = gwProperty->Name;
	property->VisibleDistance = gwProperty->VisibleDistance;
	property->Enable = gwProperty->Enable;
	property->Position = gwProperty->Position;
	property->Rotation = gwProperty->Rotation;
	property->Scale = gwProperty->Scale;
	property->Bound->setPosition( gwProperty->Position );
	property->Bound->setRotation( gwProperty->Rotation );
	property->Bound->setScale( gwProperty->Scale );
	m_decal->m_properties.push_back( property );
	m_decal->m_decal->addDecal();
	vector< GE_Decal::Parameter > *dl = m_decal->m_decal->beginEditParameters();
	dl->data()[ dl->size() - 1 ] = dl->data()[ index ];
	m_decal->m_decal->endEditParameters();
	m_decal->update();
	m_ui->DecalList->setCurrentRow( ( int ) dl->size() - 1 );
}

void GEW_DecalProperty::on_DecalCopy_triggered()
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;

	GEW_Decal::Property *property = m_decal->m_properties[ index ];
	GE_Decal::Parameter *parameter = &m_decal->m_decal->beginEditParameters()->data()[ index ];
	m_copyData.Enable = property->Enable;
	m_copyData.BoundColor = property->BoundColor;
	m_copyData.VisibleDistance = property->VisibleDistance;
	m_copyData.Position = property->Position;
	m_copyData.Rotation = property->Rotation;
	m_copyData.Scale = property->Scale;
	m_copyData.MaxAngle = parameter->MaxAngle;
	m_copyData.Bumpiness = parameter->Bumpiness;
	m_copyData.Glowness = parameter->Glowness;
	m_copyData.Roughness = parameter->Roughness;
	m_copyData.Reflectmask = parameter->Reflectmask;
	m_copyData.Layer = parameter->Layer;
	m_copyData.RotationF = parameter->Rotation;
	m_copyData.UVTile = parameter->UVTile;
	m_copyData.UVMove = parameter->UVMove;
	m_copyData.DiffuseColor = parameter->Color;
}

void GEW_DecalProperty::on_DecalPaste_triggered()
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	GEW_Decal::Property * gwProperty = m_decal->m_properties[ index ];
	GE_Decal::Parameter *parameter = &m_decal->m_decal->beginEditParameters()->data()[ index ];
	gwProperty->Bound->setColor( m_copyData.BoundColor );
	gwProperty->BoundColor = m_copyData.BoundColor;
	gwProperty->VisibleDistance = m_copyData.VisibleDistance;
	gwProperty->Enable = m_copyData.Enable;
	gwProperty->Position = m_copyData.Position;
	gwProperty->Rotation = m_copyData.Rotation;
	gwProperty->Scale = m_copyData.Scale;
	gwProperty->Bound->setPosition( m_copyData.Position );
	gwProperty->Bound->setRotation( m_copyData.Rotation );
	gwProperty->Bound->setScale( m_copyData.Scale );
	parameter->Bumpiness = m_copyData.Bumpiness;
	parameter->Color = m_copyData.DiffuseColor;
	parameter->Glowness = m_copyData.Glowness;
	parameter->Layer = m_copyData.Layer;
	parameter->MaxAngle = m_copyData.MaxAngle;
	parameter->Reflectmask = m_copyData.Reflectmask;
	parameter->Rotation = m_copyData.RotationF;
	parameter->Roughness = m_copyData.Roughness;
	parameter->UVMove = m_copyData.UVMove;
	parameter->UVTile = m_copyData.UVTile;
	parameter->Visible = m_copyData.VisibleDistance;
	GEW_Decal::m_selectedItem = index;
	m_decal->updateTransform();
	m_decal->update();
	m_ui->DecalList->setCurrentRow( -1 );
	m_ui->DecalList->setCurrentRow( index );
}

void GEW_DecalProperty::on_BoundColor_signalColorChanged( QColor i_color )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_properties[ index ]->BoundColor.x = i_color.redF();
	m_decal->m_properties[ index ]->BoundColor.y = i_color.greenF();
	m_decal->m_properties[ index ]->BoundColor.z = i_color.blueF();
	m_decal->m_properties[ index ]->Bound->setColor( m_decal->m_properties[ index ]->BoundColor );
}

void GEW_DecalProperty::on_Layer_valueChanged( double i_value )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_decal->beginEditParameters()->data()[ index ].Layer = ( float ) i_value;
	m_decal->m_decal->endEditParameters();
}

void GEW_DecalProperty::on_UTile_valueChanged( double i_value )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_decal->beginEditParameters()->data()[ index ].UVTile.x = ( float ) i_value;
	m_decal->m_decal->endEditParameters();
}

void GEW_DecalProperty::on_VTile_valueChanged( double i_value )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_decal->beginEditParameters()->data()[ index ].UVTile.y = ( float ) i_value;
	m_decal->m_decal->endEditParameters();
}

void GEW_DecalProperty::on_UMove_valueChanged( double i_value )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_decal->beginEditParameters()->data()[ index ].UVMove.x = ( float ) i_value;
	m_decal->m_decal->endEditParameters();
}

void GEW_DecalProperty::on_VMove_valueChanged( double i_value )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_decal->beginEditParameters()->data()[ index ].UVMove.y = ( float ) i_value;
	m_decal->m_decal->endEditParameters();
}

void GEW_DecalProperty::on_MapRotation_valueChanged( double i_value )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_decal->beginEditParameters()->data()[ index ].Rotation = GE_ConvertToRadians( ( float ) i_value );
	m_decal->m_decal->endEditParameters();
}

void GEW_DecalProperty::on_MaxAngle_valueChanged( double i_value )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_decal->beginEditParameters()->data()[ index ].MaxAngle = GE_ConvertToRadians( ( float ) i_value );
	m_decal->m_decal->endEditParameters();
}

void GEW_DecalProperty::on_Bumpiness_valueChanged( double i_value )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_decal->beginEditParameters()->data()[ index ].Bumpiness = ( float ) i_value;
	m_decal->m_decal->endEditParameters();
}

void GEW_DecalProperty::on_Roughness_valueChanged( double i_value )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_decal->beginEditParameters()->data()[ index ].Roughness = ( float ) i_value;
	m_decal->m_decal->endEditParameters();
}

void GEW_DecalProperty::on_Reflectmask_valueChanged( double i_value )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_decal->beginEditParameters()->data()[ index ].Reflectmask = ( float ) i_value;
	m_decal->m_decal->endEditParameters();
}

void GEW_DecalProperty::on_DiffuseMap_signalImageChanged( QString i_fileName )
{
	if( !m_decal )
		return;
	if( i_fileName.isEmpty() )
		m_decal->m_decal->deleteDiffuseTexture();
	else
		m_decal->m_decal->setDiffuseTexture( i_fileName.toStdString().c_str() );
}

void GEW_DecalProperty::on_NormalMap_signalImageChanged( QString i_fileName )
{
	if( !m_decal )
		return;
	if( i_fileName.isEmpty() )
		m_decal->m_decal->deleteNormalTexture();
	else
		m_decal->m_decal->setNormalTexture( i_fileName.toStdString().c_str() );
}

void GEW_DecalProperty::on_GlowSpecularReflectMap_signalImageChanged( QString i_fileName )
{
	if( !m_decal )
		return;
	if( i_fileName.isEmpty() )
		m_decal->m_decal->deleteGlowSpecularTexture();
	else
		m_decal->m_decal->setGlowSpecularTexture( i_fileName.toStdString().c_str() );
}

void GEW_DecalProperty::on_ReflectMap_signalImageChanged( QString i_fileName )
{
	if( !m_decal )
		return;
	if( i_fileName.isEmpty() )
		m_decal->m_decal->deleteReflectTexture();
	else
		m_decal->m_decal->setReflectTexture( i_fileName.toStdString().c_str() );
}

void GEW_DecalProperty::on_DecalList_itemChanged( class QListWidgetItem* i_item )
{
	on_DecalList_itemClicked( i_item );
}

void GEW_DecalProperty::on_DecalList_itemClicked( QListWidgetItem* i_item )
{
	if( !m_decal )
		return;
	on_DecalList_currentRowChanged( m_ui->DecalList->currentRow() );
}

void GEW_DecalProperty::on_DecalList_currentRowChanged( int i_row )
{
	if( !m_decal )
		return;
	int32_t index = i_row;
	if( index < 0 )
		return;
	m_decal->updateTransform();
	if( GEW_Decal::m_selectedItem >= 0 )
		m_decal->m_properties[ GEW_Decal::m_selectedItem ]->Bound->setVisible( false );
	GEW_Decal::m_selectedItem = index;
	m_decal->m_properties[ GEW_Decal::m_selectedItem ]->Bound->setVisible( true );
	m_decal->updateTransform();
	m_ui->DecalEnable->blockSignals( true );
	m_ui->DecalEnable->setChecked( m_decal->m_properties[ index ]->Enable );
	m_ui->DecalEnable->blockSignals( false );

	QColor cl;
	cl.setRedF( m_decal->m_properties[ index ]->BoundColor.x );
	cl.setGreenF( m_decal->m_properties[ index ]->BoundColor.y );
	cl.setBlueF( m_decal->m_properties[ index ]->BoundColor.z );
	m_ui->BoundColor->blockSignals( true );
	m_ui->BoundColor->slotSetColor( cl );
	m_ui->BoundColor->blockSignals( false );

	cl.setRedF( m_decal->m_decal->beginEditParameters()->data()[ index ].Color.x );
	cl.setGreenF( m_decal->m_decal->beginEditParameters()->data()[ index ].Color.y );
	cl.setBlueF( m_decal->m_decal->beginEditParameters()->data()[ index ].Color.z );
	cl.setAlphaF( m_decal->m_decal->beginEditParameters()->data()[ index ].Color.w );
	m_ui->DiffuseColor->blockSignals( true );
	m_ui->DiffuseColor->slotSetColor( cl );
	m_ui->DiffuseColor->blockSignals( false );

	m_ui->Layer->blockSignals( true );
	m_ui->Layer->setValue( m_decal->m_decal->beginEditParameters()->data()[ index ].Layer );
	m_ui->Layer->blockSignals( false );

	m_ui->MapRotation->blockSignals( true );
	m_ui->MapRotation->setValue( GE_ConvertToDegrees( m_decal->m_decal->beginEditParameters()->data()[ index ].Rotation ) );
	m_ui->MapRotation->blockSignals( false );

	m_ui->MaxAngle->blockSignals( true );
	m_ui->MaxAngle->setValue( GE_ConvertToDegrees( m_decal->m_decal->beginEditParameters()->data()[ index ].MaxAngle ) );
	m_ui->MaxAngle->blockSignals( false );

	m_ui->Bumpiness->blockSignals( true );
	m_ui->Bumpiness->setValue( m_decal->m_decal->beginEditParameters()->data()[ index ].Bumpiness );
	m_ui->Bumpiness->blockSignals( false );

	m_ui->Glowness->blockSignals( true );
	m_ui->Glowness->setValue( m_decal->m_decal->beginEditParameters()->data()[ index ].Glowness );
	m_ui->Glowness->blockSignals( false );

	m_ui->Roughness->blockSignals( true );
	m_ui->Roughness->setValue( m_decal->m_decal->beginEditParameters()->data()[ index ].Roughness );
	m_ui->Roughness->blockSignals( false );

	m_ui->Reflectmask->blockSignals( true );
	m_ui->Reflectmask->setValue( m_decal->m_decal->beginEditParameters()->data()[ index ].Reflectmask );
	m_ui->Reflectmask->blockSignals( false );

	m_ui->UTile->blockSignals( true );
	m_ui->UTile->setValue( m_decal->m_decal->beginEditParameters()->data()[ index ].UVTile.x );
	m_ui->UTile->blockSignals( false );

	m_ui->VTile->blockSignals( true );
	m_ui->VTile->setValue( m_decal->m_decal->beginEditParameters()->data()[ index ].UVTile.y );
	m_ui->VTile->blockSignals( false );

	m_ui->UMove->blockSignals( true );
	m_ui->UMove->setValue( m_decal->m_decal->beginEditParameters()->data()[ index ].UVMove.x );
	m_ui->UMove->blockSignals( false );

	m_ui->VMove->blockSignals( true );
	m_ui->VMove->setValue( m_decal->m_decal->beginEditParameters()->data()[ index ].UVMove.y );
	m_ui->VMove->blockSignals( false );

	m_ui->VisibleDistance->blockSignals( true );
	m_ui->VisibleDistance->setValue( GE_ConvertToMeter( m_decal->m_properties[ index ]->VisibleDistance ) );
	m_ui->VisibleDistance->blockSignals( false );

	GEW_Prs::getSingleton().updateUi();
}

void GEW_DecalProperty::on_DecalList_itemDoubleClicked( QListWidgetItem* i_item )
{

}

void GEW_DecalProperty::on_DecalList_customContextMenuRequested( QPoint )
{
	QMenu menu( this );
	menu.addAction( m_ui->DecalRename );
	menu.addSeparator();
	menu.addAction( m_ui->DecalAdd );
	menu.addAction( m_ui->DecalRemove );
	menu.addSeparator();
	menu.addAction( m_ui->DecalDuplicate );
	menu.addSeparator();
	menu.addAction( m_ui->DecalCopy );
	menu.addAction( m_ui->DecalPaste );
	menu.exec( QCursor::pos() );
}

void GEW_DecalProperty::on_DecalList_signalKeyPressed( Qt::Key i_key )
{
	if( i_key == Qt::Key_Delete )
		on_DecalRemove_triggered();
	else if( i_key == Qt::Key_F2 )
		on_DecalRename_triggered();
}

void GEW_DecalProperty::on_DecalEnable_toggled( bool i_checked )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_properties[ index ]->Enable = i_checked;
	if( i_checked )
		m_decal->m_properties[ index ]->Bound->setColor( m_decal->m_properties[ index ]->BoundColor );
	else
		m_decal->m_properties[ index ]->Bound->setColor( GE_Vec3( 0.2f, 0.2f, 0.2f ) );
	m_decal->m_decal->beginEditParameters()->data()[ index ].Visible = ( i_checked ? true : false );
	m_decal->m_decal->endEditParameters();
}

void GEW_DecalProperty::on_Glowness_valueChanged( double i_value )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_decal->beginEditParameters()->data()[ index ].Glowness = ( float ) i_value;
	m_decal->m_decal->endEditParameters();
}

void GEW_DecalProperty::on_VisibleDistance_valueChanged( double i_value )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_properties[ index ]->VisibleDistance = GE_ConvertToUnit( ( float ) i_value );
}

void GEW_DecalProperty::on_VisibleDistanceFromCamera_clicked()
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_ui->VisibleDistance->setValue(
		GE_ConvertToMeter( GEW_Scene::getSingleton().m_camera->m_position - m_decal->m_properties[ index ]->Position ).length() );
}

void GEW_DecalProperty::setDecal( GEW_Decal *i_decal )
{
	m_decal = NULL;
	m_ui->DecalList->clear();

	if( !i_decal )
		return;
	m_ui->Enable->setChecked( i_decal->m_enable );
	if( i_decal->m_decal->getDiffuseTexture() )
		m_ui->DiffuseMap->slotLoad( i_decal->m_decal->getDiffuseTexture()->getTextureName() );
	else
		m_ui->DiffuseMap->slotClear();

	if( i_decal->m_decal->getNormalTexture() )
		m_ui->NormalMap->slotLoad( i_decal->m_decal->getNormalTexture()->getTextureName() );
	else
		m_ui->NormalMap->slotClear();

	if( i_decal->m_decal->getGlowSpecularTexture() )
		m_ui->GlowSpecularReflectMap->slotLoad( i_decal->m_decal->getGlowSpecularTexture()->getTextureName() );
	else
		m_ui->GlowSpecularReflectMap->slotClear();

	if( i_decal->m_decal->getReflectTexture() )
		m_ui->ReflectMap->slotLoad( i_decal->m_decal->getReflectTexture()->getTextureName() );
	else
		m_ui->ReflectMap->slotClear();

	for( int32_t i = 0; i < i_decal->m_properties.size(); ++i )
		QListWidgetItem *item = new QListWidgetItem(
		m_decal->m_iconDecal,
		i_decal->m_properties[ i ]->Name,
		m_ui->DecalList );
	m_decal = i_decal;

}

void GEW_DecalProperty::on_DiffuseColor_signalColorChanged( QColor i_color )
{
	if( !m_decal )
		return;
	int32_t index = m_ui->DecalList->currentRow();
	if( index < 0 )
		return;
	m_decal->m_decal->beginEditParameters()->data()[ index ].Color.x = i_color.redF();
	m_decal->m_decal->beginEditParameters()->data()[ index ].Color.y = i_color.greenF();
	m_decal->m_decal->beginEditParameters()->data()[ index ].Color.z = i_color.blueF();
	m_decal->m_decal->beginEditParameters()->data()[ index ].Color.w = i_color.alphaF();
	m_decal->m_decal->endEditParameters();
}


