#include "GEW_ModelProperty.h"
#include "ui_GEW_ModelProperty.h"
#include <QListWidgetItem>
#include "GE_Model.h"
#include "GEW_Model.h"
#include "GEW_Material.h"
#include <GEW_MaterialEditorDialog.h>
#include <GE_Material.h>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>
#include <QToolBar>
#include "GEW_Scene.h"
#include "GEW_Camera.h"
#include "GEW_Physics.h"
#include <QPixmap>
#include <QPainter>

GEW_ModelProperty::GEW_ModelProperty( QWidget *i_parent )
: QWidget( i_parent ),
m_ui( new Ui::GEW_ModelProperty ),
m_model( NULL ),
m_materialCopy( NULL )
{
	m_ui->setupUi( this );
	m_ui->frame_2->setVisible( false );
	m_ui->ModelPxForceFrame->setVisible( false );
	m_ui->frame_3->setVisible( false );
	m_dialog = new GEW_MaterialEditorDialog( this );

	QToolBar *materialToolBar = new QToolBar( this );
	materialToolBar->setIconSize( QSize( 16, 16 ) );
	materialToolBar->addAction( m_ui->MaterialItemAdd );
	materialToolBar->addAction( m_ui->MaterialItemDelete );
	materialToolBar->addSeparator();
	materialToolBar->addAction( m_ui->MaterialItemRename );
	materialToolBar->addAction( m_ui->MaterialItemSearch );
	materialToolBar->addSeparator();
	materialToolBar->addAction( m_ui->MaterialItemDuplicate );
	materialToolBar->addAction( m_ui->MaterialItemCopy );
	materialToolBar->addAction( m_ui->MaterialItemPaste );
	m_ui->MaterialToolBarLayout->addWidget( materialToolBar );

	QToolBar *subsetToolBar = new QToolBar( this );
	subsetToolBar->setIconSize( QSize( 16, 16 ) );
	subsetToolBar->addAction( m_ui->SubsetItemSetMaterial );
	subsetToolBar->addAction( m_ui->SubsetItemUnSetMaterial );
	subsetToolBar->addSeparator( );
	subsetToolBar->addAction( m_ui->SubsetItemRename );
	subsetToolBar->addSeparator( );
	subsetToolBar->addAction( m_ui->SubsetItemSetMaterialFromGroup );
	m_ui->SubsetToolBarLayout->addWidget( subsetToolBar );

	QToolBar *pxMaterialToolBar = new QToolBar( this );
	pxMaterialToolBar->setIconSize( QSize( 16, 16 ) );
	pxMaterialToolBar->addAction( m_ui->PxMaterialSet );
	pxMaterialToolBar->addAction( m_ui->PxMaterialUnSet );
	pxMaterialToolBar->addAction( m_ui->PxMaterialSetToAllShapes );
	pxMaterialToolBar->addSeparator();
	pxMaterialToolBar->addAction( m_ui->PxMaterialAdd );
	pxMaterialToolBar->addAction( m_ui->PxMaterialRemove );
	pxMaterialToolBar->addAction( m_ui->PxMaterialEdit );
	pxMaterialToolBar->addAction( m_ui->PxMaterialDuplicate );
	m_ui->PxMaterialToolBar->addWidget( pxMaterialToolBar );

	QToolBar *InstanceToolBar = new QToolBar( this );
	InstanceToolBar->setIconSize( QSize( 16, 16 ) );
	InstanceToolBar->addAction( m_ui->InstanceAdd );
	InstanceToolBar->addAction( m_ui->InstanceDelete );
	InstanceToolBar->addSeparator( );
	InstanceToolBar->addAction( m_ui->InstanceRename );
	InstanceToolBar->addAction( m_ui->InstanceSearch );
	InstanceToolBar->addSeparator( );
	InstanceToolBar->addAction( m_ui->InstanceDuplicate );
	InstanceToolBar->addAction( m_ui->InstanceCopy );
	InstanceToolBar->addAction( m_ui->InstancePaste );
	m_ui->InstanceToolBarLayout->addWidget( InstanceToolBar );
}

GEW_ModelProperty::~GEW_ModelProperty()
{
	delete m_ui;
	delete m_dialog;
}

void GEW_ModelProperty::setModel( GEW_Model *i_model )
{
	m_model = i_model;
	if( m_model )
	{
		GEW_Material::getSingleton().selectMaterial( NULL );
		GE_Model *model = m_model->m_model;

		m_ui->ModelDynamic->blockSignals( true );
		m_ui->ModelDynamic->setChecked( m_model->isDynamic() );
		m_ui->ModelDynamic->blockSignals( false );

		m_ui->ModelTransparent->blockSignals(true);
		m_ui->ModelTransparent->setChecked(m_model->isTransparent());
		m_ui->ModelTransparent->blockSignals(false);

		m_ui->ModelVisible->blockSignals( true );
		m_ui->ModelVisible->setChecked( model->isVisible() );
		m_ui->ModelVisible->blockSignals( false );

		m_ui->ModelSubsetList->blockSignals( true );
		m_ui->ModelSubsetList->clear();
		for( uint32_t i = 0; i < model->getNumberOfSubsets(); ++i )
		{
			if( GEW_Material::getSingleton().isNoMaterial( model->getMaterial( i ) ) || 
				GEW_Material::getSingleton().isSelectMaterial( model->getMaterial( i ) ) )
				new QListWidgetItem( GEW_Model::m_iconSubsetNoMaterial, m_model->getSubsetName( i ), m_ui->ModelSubsetList );
			else
				new QListWidgetItem( GEW_Model::m_iconSubset, m_model->getSubsetName( i ), m_ui->ModelSubsetList );
		}
		m_ui->ModelSubsetList->setCurrentRow( 0 );
		m_ui->ModelSubsetList->blockSignals( false );

		m_ui->ModelLOD->blockSignals( true );
		m_ui->ModelLOD->clear();
		for( uint32_t i = 0; i < model->getNumberOfLod(); ++i )
			m_ui->ModelLOD->addItem( GEW_Model::m_iconModel, tr( "%1" ).arg( i ) );
		m_ui->ModelLOD->setCurrentIndex( model->getLod() );
		m_ui->ModelLOD->blockSignals( false );

		m_ui->ModelLodDistance->blockSignals( true );
		m_ui->ModelLodDistance->setValue( GE_ConvertToMeter( model->getLodDistance( model->getLod() ) ) );
		m_ui->ModelLodDistance->blockSignals( false );

		m_ui->ModelVisibleDistance->blockSignals( true );
		m_ui->ModelVisibleDistance->setValue( GE_ConvertToMeter( model->getVisibleDistance() ) );
		m_ui->ModelVisibleDistance->blockSignals( false );

		m_ui->ModelTwoSideShade->blockSignals( true );
		m_ui->ModelTwoSideShade->setChecked(m_model->isTwoSideShade());
		m_ui->ModelTwoSideShade->blockSignals( false );

		m_ui->ModelTwoSideDepth->blockSignals( true );
		m_ui->ModelTwoSideDepth->setChecked(m_model->isTwoSideDepth());
		m_ui->ModelTwoSideDepth->blockSignals( false );

		m_ui->ModelShadowCaster->blockSignals( true );
		m_ui->ModelShadowCaster->setChecked( m_model->isShadowCaster() );
		m_ui->ModelShadowCaster->blockSignals( false );

		QColor color;
		GE_Vec3 c = m_model->m_renderUtilityBB->getColor();
		color.setRedF( c.x );
		color.setGreenF( c.y );
		color.setBlueF( c.z );
		m_ui->ModelBoundColor->blockSignals( true );
		m_ui->ModelBoundColor->slotSetColor( color );
		m_ui->ModelBoundColor->blockSignals( false );

		m_ui->ModelHas2ndUV->setChecked( model->hasAdditionTexcoord() );

		updateMaterialGroup();
		on_ModelSubsetList_currentRowChanged( 0 );

		updatePxUi();
	}
}

void GEW_ModelProperty::updateMaterialGroup()
{
	int groupId = m_ui->MaterialGroup->currentIndex();
	int materialId = m_ui->MaterialList->currentRow();

	m_ui->MaterialGroup->blockSignals( true );
	m_ui->MaterialList->blockSignals( true );

	m_ui->MaterialGroup->clear();
	for( int i = 0; i < GEW_Material::getSingleton().nbGroup(); ++i )
		m_ui->MaterialGroup->addItem(
			GEW_Material::m_groupIcon,
			GEW_Material::getSingleton().getGroup( i )->Name );
	m_ui->MaterialGroup->setCurrentIndex( 0 );
	m_ui->MaterialList->clear();

	if( groupId != -1 && 
		materialId != -1 &&
		GEW_Material::getSingleton().getGroup( groupId ) &&
		GEW_Material::getSingleton().getGroup( groupId )->getMaterial( materialId )
		)
	{
		m_ui->MaterialGroup->setCurrentIndex( groupId );
		on_MaterialGroup_currentIndexChanged( groupId );
		m_ui->MaterialList->setCurrentRow( materialId );
	}

	m_ui->MaterialGroup->blockSignals( false );
	m_ui->MaterialList->blockSignals( false );
}

void GEW_ModelProperty::on_ModelVisible_toggled( bool i_checked )
{
	if( !m_model )
		return;
	m_model->m_model->setVisible( i_checked );
	m_model->setItemCheckState( GEW_Item::Enable, i_checked );
}

void GEW_ModelProperty::on_ModelDynamic_toggled( bool i_checked )
{
	if( !m_model )
		return;
	m_model->setDynamic( i_checked );
	m_model->setItemCheckState( GEW_Item::Dynamic, i_checked );
}

void GEW_ModelProperty::on_ModelTwoSideShade_toggled( bool i_checked )
{
	if( !m_model )
		return;

	m_model->setTwoSideShade( i_checked );
}

void GEW_ModelProperty::on_ModelTwoSideDepth_toggled( bool i_checked )
{
	if( !m_model )
		return;

	m_model->setTwoSideDepth( i_checked );
}

void GEW_ModelProperty::on_ModelShadowCaster_toggled( bool i_checked )
{
	if( !m_model )
		return;

	m_model->setShadowCaster( i_checked );
}

void GEW_ModelProperty::on_ModelTransparent_toggled(bool i_checked)
{
	if (!m_model)
		return;

	m_model->setTransparent(i_checked);
}



void GEW_ModelProperty::on_ModelBoundColor_signalColorChanged( QColor i_color )
{
	if( !m_model )
		return;

	GE_Vec3 color;
	color.x = i_color.redF();
	color.y = i_color.greenF();
	color.z = i_color.blueF();
	m_model->m_renderUtilityBB->setColor( color );
}

void GEW_ModelProperty::on_ModelLOD_currentIndexChanged( int i_index )
{
	if( !m_model )
		return;

	GE_Model *model = m_model->m_model;
	model->setLod( i_index );

	m_ui->ModelLodDistance->blockSignals( true );
	m_ui->ModelLodDistance->setValue( GE_ConvertToMeter( model->getLodDistance( i_index ) ) );
	m_ui->ModelLodDistance->blockSignals( false );

	if( m_ui->ModelSubsetList->currentRow() < 0 )
		on_ModelSubsetList_currentRowChanged( 0 );
	else
		on_ModelSubsetList_currentRowChanged( m_ui->ModelSubsetList->currentRow() );
}

void GEW_ModelProperty::on_ModelLODAdd_clicked()
{
	if( !m_model )
		return;

	GE_Model *model = m_model->m_model;
	QString dir = GEW_Model::m_lastFileName.left(
		GEW_Model::m_lastFileName.length() -
		GEW_Model::m_lastFileName.split( "/" ).last().length() );
	QString fileName = QFileDialog::getOpenFileName(
		this,
		tr( "Add Model" ),
		dir,
		tr( "GEngine Model (*.GModel)" ) );
	if( fileName.isEmpty() )
		return;

	if( model->addLod( fileName.toStdString() ) )
	{
		m_ui->ModelLOD->addItem( GEW_Model::m_iconModel, tr( "%1" ).arg( m_ui->ModelLOD->count() ) );
		m_ui->ModelLOD->setCurrentIndex( m_ui->ModelLOD->count() - 1 );
	} else
	{
		QMessageBox::critical(
			this,
			tr( "Add LOD" ),
			tr( "Failed to add LOD!\n"
			"The subset count must be the same as the fists LOD." ) );
	}
}

void GEW_ModelProperty::on_ModelLODRemove_clicked()
{
	int lodIndex = m_ui->ModelLOD->currentIndex();
	if( !m_model || lodIndex < 1 )
		return;

	if( QMessageBox::warning(
		this,
		tr( "Remove LOD" ),
		tr( "Are you sure?" ),
		QMessageBox::Yes,
		QMessageBox::No ) != QMessageBox::Yes )
	{
		return;
	}

	GE_Model *model = m_model->m_model;

	model->removeLod( lodIndex );
	m_ui->ModelLOD->removeItem( m_ui->ModelLOD->count() - 1 );
	m_ui->ModelLOD->setCurrentIndex( lodIndex - 1 );
}

void GEW_ModelProperty::on_ModelLodDistance_valueChanged( double i_value )
{
	if( !m_model )
		return;

	GE_Model *model = m_model->m_model;
	model->setLodDistance( m_ui->ModelLOD->currentIndex(), GE_ConvertToUnit( ( float ) i_value ) );
}

void GEW_ModelProperty::on_ModelLodDistanceFromCamera_clicked()
{
	if( !m_model )
		return;
	GE_Model *model = m_model->m_model;
	float distance = ( GEW_Scene::getSingleton().m_camera->m_position - model->getPosition() ).length();
	m_ui->ModelLodDistance->setValue( GE_ConvertToMeter( abs( distance ) ) );
}

void GEW_ModelProperty::on_ModelVisibleDistance_valueChanged( double i_value )
{
	if( !m_model )
		return;

	GE_Model *model = m_model->m_model;
	model->setVisibleDistance( GE_ConvertToUnit( ( float ) i_value ) );
}

void GEW_ModelProperty::on_ModelVisibleDistanceFromCamera_clicked()
{
	if( !m_model )
		return;
	GE_Model *model = m_model->m_model;
	float distance = ( GEW_Scene::getSingleton().m_camera->m_position - model->getPosition() ).length();
	m_ui->ModelVisibleDistance->setValue( GE_ConvertToMeter( abs( distance ) ) );
}

void GEW_ModelProperty::on_ModelSubsetList_itemDoubleClicked( QListWidgetItem *i_item )
{
	Q_UNUSED( i_item );
}

void GEW_ModelProperty::on_ModelSubsetList_itemClicked( QListWidgetItem *i_item )
{
	Q_UNUSED( i_item );
	on_ModelSubsetList_currentRowChanged( m_ui->ModelSubsetList->currentRow() );
}

void GEW_ModelProperty::on_ModelSubsetList_currentRowChanged( int i_row )
{
	int subsetIndex = i_row;
	if( !m_model || subsetIndex < 0 )
		return;

	GE_Model *model = m_model->m_model;
	GE_Model::Subset *subset = model->getSubset( subsetIndex );

	uint32_t lodIndex = model->getLod();
	uint32_t Vs = model->getNumberOfVertices( lodIndex );
	uint32_t V = model->getNumberOfSubsetVertices( subsetIndex, lodIndex );
	uint32_t Is = model->getNumberOfIndices( lodIndex ) / 3;
	uint32_t I = model->getNumberOfSubsetIndices( subsetIndex, lodIndex ) / 3;

	m_ui->ModelInfo->setText( tr( "V: %1/%2    I: %3/%4" ).arg( V ).arg( Vs ).arg( I ).arg( Is ) );

	m_ui->ModelUseVertexColor->blockSignals( true );
	m_ui->ModelUseVertexColor->setChecked( subset->UseVertexColor );
	m_ui->ModelUseVertexColor->blockSignals( false );

	m_ui->ModelLayerMultiply->blockSignals( true );
	m_ui->ModelLayerMultiply->setValue( subset->LayerMultiply );
	m_ui->ModelLayerMultiply->blockSignals( false );


	m_ui->ModelLayerOffset->blockSignals( true );
	m_ui->ModelLayerOffset->setValue( subset->LayerOffset );
	m_ui->ModelLayerOffset->blockSignals( false );

	m_ui->ModelSubsetLightMap->blockSignals( true );
	if( model->getLightMap( subsetIndex ) )
		m_ui->ModelSubsetLightMap->loadFromFile( model->getLightMap( subsetIndex )->getTextureName() );
	else
		m_ui->ModelSubsetLightMap->slotClear();
	m_ui->ModelSubsetLightMap->blockSignals( false );

	int groupId;
	int materialId;
	GEW_Material::getSingleton().getMaterialId(
		groupId,
		materialId,
		subset->Material );
	
	if( groupId != 0 )
	{
		m_ui->MaterialGroup->blockSignals( true );
		m_ui->MaterialGroup->setCurrentIndex( groupId );
		on_MaterialGroup_currentIndexChanged( groupId );
		m_ui->MaterialGroup->blockSignals( false );

		m_ui->MaterialList->blockSignals( true );
		m_ui->MaterialList->setCurrentRow( materialId );
		on_MaterialList_currentRowChanged( materialId );
		m_ui->MaterialList->blockSignals( false );
	}

	GEW_Material::getSingleton().selectMaterial( &subset->Material );
}

void GEW_ModelProperty::on_ModelSubsetList_signalKeyPressed( Qt::Key key )
{
	if( key == Qt::Key_F2 )
		on_SubsetItemRename_triggered();
}

void GEW_ModelProperty::on_ModelSubsetList_customContextMenuRequested( QPoint )
{
	QMenu menu( this );
	menu.addAction( m_ui->SubsetItemRename );
	menu.addSeparator();
	menu.addAction( m_ui->SubsetItemSetMaterial );
	menu.addAction( m_ui->SubsetItemUnSetMaterial );
	menu.addAction( m_ui->SubsetItemSetMaterialFromGroup );
	menu.exec( QCursor::pos() );
}

void GEW_ModelProperty::on_SubsetItemRename_triggered()
{
	int subsetIndex = m_ui->ModelSubsetList->currentRow();
	if( !m_model || subsetIndex < 0 )
		return;

	QString subsetName = m_model->getSubsetName( subsetIndex );
	while( true )
	{
		subsetName = QInputDialog::getText(
			this,
			tr( "Subset Name" ),
			tr( "Name:" ),
			QLineEdit::Normal,
			subsetName );
		if( subsetName.isEmpty() || subsetName == m_model->getSubsetName( subsetIndex ) )
			return;
		if( m_model->getSubsetIndex( subsetName.toLower() ) != -1 )
		{
			if( QMessageBox::warning(
				this,
				tr( "Rename subset" ),
				tr( "This name is already exist.\nDo you want to use this name any way?" ),
				QMessageBox::Yes,
				QMessageBox::No ) == QMessageBox::Yes )
			{
				break;
			}
		} else
		{
			break;
		}
	}

	m_model->setSubsetName( subsetIndex, subsetName );
	m_ui->ModelSubsetList->currentItem( )->setText( subsetName );
}

void GEW_ModelProperty::on_SubsetItemSetMaterial_triggered()
{
	int subsetIndex = m_ui->ModelSubsetList->currentRow();
	int groupId = m_ui->MaterialGroup->currentIndex();
	int materialId = m_ui->MaterialList->currentRow();
	if( !m_model || subsetIndex < 0 || groupId < 1 || materialId < 0 )
		return;

	GE_Model *model = m_model->m_model;
	GE_Model::Subset *subset = model->getSubset( subsetIndex );

	subset->Material = GEW_Material::getSingleton().getMaterial( groupId, materialId );
	m_ui->ModelSubsetList->item( subsetIndex )->setIcon( GEW_Model::m_iconSubset );
}

void GEW_ModelProperty::on_SubsetItemUnSetMaterial_triggered()
{
	int subsetIndex = m_ui->ModelSubsetList->currentRow();
	if( !m_model || subsetIndex < 0 )
		return;

	GE_Model *model = m_model->m_model;
	GE_Model::Subset *subset = model->getSubset( subsetIndex );

	subset->Material = GEW_Material::getSingleton().getNullMaterial();
	m_ui->ModelSubsetList->item( subsetIndex )->setIcon( GEW_Model::m_iconSubsetNoMaterial );
}

void GEW_ModelProperty::on_SubsetItemSetMaterialFromGroup_triggered()
{
	int groupId = m_ui->MaterialGroup->currentIndex();
	if( !m_model || groupId < 1 )
		return;

	GEW_Material::Group *group = GEW_Material::getSingleton( ).getGroup( groupId );
	GE_Model *model = m_model->m_model;

	if( ( int ) model->getNumberOfSubsets() > group->List.size() )
	{
		if( QMessageBox::warning(
			this,
			tr( "Warning" ),
			tr( "This group has lees material than subset count.\n"
			"Remaining subsets won't take effect.\n"
			"Do you want to continue any way?" ),
			QMessageBox::Yes,
			QMessageBox::No ) == QMessageBox::No )
			return;
	}

	for( int i = 0; i < ( int ) model->getNumberOfSubsets() && i < group->List.size(); ++i )
	{
		model->setMaterial( i, group->getMaterial( i ) );
		m_ui->ModelSubsetList->item( i )->setIcon( GEW_Model::m_iconSubset );
	}
}

void GEW_ModelProperty::on_ModelUseVertexColor_toggled( bool i_checked )
{
	int subsetIndex = m_ui->ModelSubsetList->currentRow();
	if( !m_model || subsetIndex < 0 )
		return;

	GE_Model *model = m_model->m_model;
	GE_Model::Subset *subset = model->getSubset( subsetIndex );
	
	subset->UseVertexColor = i_checked;
}

void GEW_ModelProperty::on_ModelLayerMultiply_valueChanged( double i_value )
{
	int subsetIndex = m_ui->ModelSubsetList->currentRow();
	if( !m_model || subsetIndex < 0 )
		return;

	GE_Model *model = m_model->m_model;
	GE_Model::Subset *subset = model->getSubset( subsetIndex );

	subset->LayerMultiply = ( float ) i_value;
}

void GEW_ModelProperty::on_ModelLayerOffset_valueChanged( double i_value )
{
	int subsetIndex = m_ui->ModelSubsetList->currentRow();
	if( !m_model || subsetIndex < 0 )
		return;

	GE_Model *model = m_model->m_model;
	GE_Model::Subset *subset = model->getSubset( subsetIndex );

	subset->LayerOffset = ( float ) i_value;
}

void GEW_ModelProperty::on_ModelSubsetLightMap_signalImageChanged( QString i_filePath )
{
	int subsetIndex = m_ui->ModelSubsetList->currentRow();
	if( !m_model || subsetIndex < 0 )
		return;

	GE_Model *model = m_model->m_model;
	GE_Model::Subset *subset = model->getSubset( subsetIndex );

	if( subset->LightMap )
		GE_TextureManager::getSingleton().remove( &subset->LightMap );
	if( !i_filePath.isEmpty() )
		subset->LightMap = GE_TextureManager::getSingleton().createTexture( i_filePath.toStdString().c_str() );
}

void GEW_ModelProperty::on_MaterialGroup_currentIndexChanged( int i_index )
{
	GEW_Material::Group *group = GEW_Material::getSingleton().getGroup( i_index );
	m_ui->MaterialList->blockSignals( true );
	m_ui->MaterialList->clear();
	if( i_index != 0 )
		foreach( GE_Material *material, group->List )
			new QListWidgetItem( GEW_Material::m_materialIcon, material->Name.c_str(), m_ui->MaterialList );
	m_ui->MaterialList->blockSignals( false );
}

void GEW_ModelProperty::on_MaterialGroup_customContextMenuRequested( QPoint )
{
	QMenu menu( this );
	menu.addAction( m_ui->MaterialRenameGroup );
	menu.addSeparator();
	menu.addAction( m_ui->MaterialOpenGroup );
	menu.addSeparator();
	menu.addAction( m_ui->MaterialSaveThisGroup );
	menu.addAction( m_ui->MaterialSaveAllGroups );
	menu.addSeparator();
	menu.addAction( m_ui->MaterialClearAllGroups );
	menu.exec( QCursor::pos() );
}

void GEW_ModelProperty::on_MaterialRenameGroup_triggered()
{
	int groupId = m_ui->MaterialGroup->currentIndex();
	if( groupId < 1 )
		return;

	GEW_Material::Group *group = GEW_Material::getSingleton().getGroup( groupId );

	QString name = QInputDialog::getText(
		this,
		tr( "Rename group" ),
		tr( "name:" ),
		QLineEdit::Normal,
		group->Name );
	if( name.isEmpty() || name == group->Name )
		return;
	
	if( GEW_Material::getSingleton().getGroupId( name.toLower() ) != -1 )
	{
		QMessageBox::critical(
			this,
			tr( "Rename group" ),
			tr( "This name is already exist." ) );
		return;
	}

	group->Name = name;
	m_ui->MaterialGroup->blockSignals( true );
	m_ui->MaterialGroup->setItemText( groupId, name );
	m_ui->MaterialGroup->blockSignals( false );
}

void GEW_ModelProperty::on_MaterialOpenGroup_triggered()
{
	QString fileName = QFileDialog::getOpenFileName( 
		this,
		QString(),
		QString(),
		tr( "Material file (*.GMat)" ) );
	if( fileName.isEmpty() )
		return;
	QString resourcePath = fileName.left( fileName.length() - 5 ) + "_resources/";
	QFile file( fileName );

	if( file.open( QIODevice::ReadOnly ) )
	{
		GEW_Material::getSingleton().loadFromFile( file, resourcePath );
		updateMaterialGroup();
		QMessageBox::information(
			this,
			tr( "Loading material" ),
			tr( "Materials has loaded successfully." ) );
	} else
	{
		QMessageBox::critical(
			this,
			tr( "Loading material" ),
			tr( "Unable to open file or file is corrupted" ) );
	}
}

void GEW_ModelProperty::on_MaterialSaveThisGroup_triggered()
{
	int groupId = m_ui->MaterialGroup->currentIndex();
	if( groupId < 1 )
		return;
	
	QString fileName = QFileDialog::getSaveFileName(
		this,
		QString(),
		QString(),
		tr( "Material file (*.GMat)" ) );
	if( fileName.isEmpty() )
		return;
	QString resourcePath = fileName.left( fileName.length() - 5 ) + "_resources/";
	QFile file( fileName );
	QDir dir;

	if( dir.mkdir( resourcePath ) && file.open( QIODevice::WriteOnly ) )
	{
		GEW_Material::getSingleton().saveToFile( file, resourcePath, groupId );
	} else
	{
		QMessageBox::critical(
			this,
			tr( "Saving material" ),
			tr( "Unable to create file or directory." ) );
	}
}

void GEW_ModelProperty::on_MaterialSaveAllGroups_triggered()
{
	QString fileName = QFileDialog::getSaveFileName(
		this,
		QString(),
		QString(),
		tr( "Material file (*.GMat)" ) );
	if( fileName.isEmpty() )
		return;
	QString resourcePath = fileName.left( fileName.length() - 5 ) + "_resources/";
	QFile file( fileName );
	QDir dir;

	if( dir.mkdir( resourcePath ) && file.open( QIODevice::WriteOnly ) )
	{
		GEW_Material::getSingleton().saveToFile( file, resourcePath );
	} else
	{
		QMessageBox::critical(
			this,
			tr( "Saving material" ),
			tr( "Unable to create file or directory." ) );
	}
}

void GEW_ModelProperty::on_MaterialClearAllGroups_triggered()
{
	if( QMessageBox::warning(
		this,
		tr( "Remove group" ),
		tr( "Are you sure?" ),
		QMessageBox::Yes,
		QMessageBox::No ) == QMessageBox::No )
	{
		return;
	}

	GEW_Material::getSingleton().selectMaterial( NULL );
	GE_Material *nullMaterial = GEW_Material::getSingleton().getNullMaterial();

	for( int groupId = 1; groupId < GEW_Material::getSingleton().nbGroup(); ++groupId )
	{
		GEW_Material::Group *group = GEW_Material::getSingleton().getGroup( groupId );
		foreach( GEW_Model *gewModel, GEW_Model::m_list )
		{
			foreach( GE_Material *material, group->List )
			{
				gewModel->replaceMaterial( material, nullMaterial );
			}

			if( gewModel == m_model )
			{
				GE_Model *model = m_model->m_model;
				for( uint32_t i = 0; i < model->getNumberOfSubsets(); ++i )
				{
					if( model->getSubset( i )->Material == nullMaterial )
						m_ui->ModelSubsetList->item( i )->setIcon( GEW_Model::m_iconSubsetNoMaterial );
				}
			}
		}
		GEW_Material::getSingleton().removeGroup( groupId );
	}
	
	m_ui->MaterialGroup->blockSignals( true );
	m_ui->MaterialGroup->clear();
	m_ui->MaterialGroup->addItem( GEW_Material::m_groupIcon, tr( "NULL" ) );
	m_ui->MaterialGroup->setCurrentIndex( 0 );
	m_ui->MaterialGroup->blockSignals( false );
	m_ui->MaterialList->blockSignals( true );
	m_ui->MaterialList->clear();
	m_ui->MaterialList->blockSignals( false );
}

void GEW_ModelProperty::on_MaterialGroupAdd_clicked()
{
	QString groupName;
	while( true )
	{
		groupName = tr( "Group_%1" ).arg( GEW_Material::m_groupNewId );
		if( GEW_Material::getSingleton().getGroupId( groupName.toLower() ) == -1)
			break;
		GEW_Material::m_groupNewId++;
	}
	groupName = QInputDialog::getText(
		this,
		tr( "Group Name" ),
		tr( "Name:" ),
		QLineEdit::Normal,
		groupName );
	if( groupName.isEmpty() )
		return;

	if( GEW_Material::getSingleton().getGroupId( groupName.toLower() ) != -1 )
	{
		QMessageBox::critical(
			this,
			tr( "Add Material Group" ),
			tr( "Group name already exist! " ) );
		return;
	}
	GEW_Material::m_groupNewId++;

	GEW_Material::getSingleton().addGroup( groupName );
	m_ui->MaterialGroup->blockSignals( true );
	m_ui->MaterialGroup->addItem( GEW_Material::m_groupIcon, groupName );
	m_ui->MaterialGroup->setCurrentText( groupName );
	on_MaterialGroup_currentIndexChanged( m_ui->MaterialGroup->currentIndex() );
	m_ui->MaterialGroup->blockSignals( false );
}

void GEW_ModelProperty::on_MaterialGroupRemove_clicked()
{
	int index = m_ui->MaterialGroup->currentIndex();
	if( index < 1 )
		return;

	if( QMessageBox::warning(
		this,
		tr( "Remove group" ),
		tr( "Are you sure?" ),
		QMessageBox::Yes,
		QMessageBox::No ) == QMessageBox::No )
	{
		return;
	}

	GEW_Material::getSingleton().selectMaterial( NULL );
	GEW_Material::Group *group = GEW_Material::getSingleton().getGroup( index );
	GE_Material *nullMaterial = GEW_Material::getSingleton().getNullMaterial();
	foreach( GEW_Model *gewModel, GEW_Model::m_list )
	{
		foreach( GE_Material *material, group->List )
		{
			gewModel->replaceMaterial( material, nullMaterial );
		}
		
		if( gewModel == m_model )
		{
			GE_Model *model = m_model->m_model;
			for( uint32_t i = 0; i < model->getNumberOfSubsets(); ++i )
			{
				if( model->getSubset( i )->Material == nullMaterial )
					m_ui->ModelSubsetList->item( i )->setIcon( GEW_Model::m_iconSubsetNoMaterial );
			}
		}
	}
	GEW_Material::getSingleton().removeGroup( index );
	m_ui->MaterialGroup->setCurrentIndex( index - 1 );
	m_ui->MaterialGroup->removeItem( index );
}

void GEW_ModelProperty::on_MaterialList_signalKeyPressed( Qt::Key key )
{
	if( key == Qt::Key_Delete )
		on_MaterialItemDelete_triggered();
	else if( key == Qt::Key_F2 )
		on_MaterialItemRename_triggered();
}

void GEW_ModelProperty::on_MaterialList_itemClicked( QListWidgetItem *i_item )
{
	Q_UNUSED( i_item );
	on_MaterialList_currentRowChanged( m_ui->MaterialList->currentRow() );
}

void GEW_ModelProperty::on_MaterialList_itemDoubleClicked( QListWidgetItem *i_item )
{
	Q_UNUSED( i_item );
	int groupId = m_ui->MaterialGroup->currentIndex();
	int materialId = m_ui->MaterialList->currentRow();
	
	m_dialog->setMaterial( GEW_Material::getSingleton().getMaterial( groupId, materialId ) );
	m_dialog->show();
}

void GEW_ModelProperty::on_MaterialList_currentRowChanged( int i_row )
{
	int groupId = m_ui->MaterialGroup->currentIndex();
	int materialId = i_row;

	GE_Material *material = GEW_Material::getSingleton().getMaterial( groupId, materialId );
	if( m_dialog->isVisible() && m_dialog->getMaterial( ) != material )
		m_dialog->setMaterial( material );
}

void GEW_ModelProperty::on_MaterialList_signalItemOrderChanged( QList<QListWidgetItem*> &i_oldItems, QList<QListWidgetItem*> &i_newItems )
{
	Q_UNUSED( i_oldItems );
	GEW_Material::Group *group = GEW_Material::getSingleton().getGroup( m_ui->MaterialGroup->currentIndex() );
	QList<GE_Material*> newList;
	
	foreach( QListWidgetItem *item, i_newItems )
		newList.push_back( group->getMaterial( group->getMaterialId( item->text().toLower() ) ) );
	
	group->List.clear();
	group->List = newList;
}

void GEW_ModelProperty::on_MaterialList_customContextMenuRequested( QPoint )
{
	QMenu menu( this );
	menu.addAction( m_ui->MaterialItemRename );
	menu.addAction( m_ui->MaterialItemSearch );
	menu.addSeparator();
	menu.addAction( m_ui->MaterialItemAdd );
	menu.addAction( m_ui->MaterialItemDelete );
	menu.addSeparator();
	menu.addAction( m_ui->MaterialItemDuplicate );
	menu.addSeparator();
	menu.addAction( m_ui->MaterialItemCopy );
	menu.addAction( m_ui->MaterialItemPaste );
	menu.exec( QCursor::pos() );
}

void GEW_ModelProperty::on_MaterialItemAdd_triggered()
{
	if( m_ui->MaterialGroup->currentIndex() < 1 )
		return;
	GEW_Material::Group *group = GEW_Material::getSingleton().getGroup( m_ui->MaterialGroup->currentIndex() );
	QString materialName;
	int index = group->List.size();
	while( true )
	{
		materialName = tr( "Material_%1" ).arg( index );
		if( group->getMaterialId( materialName.toLower() ) == -1 )
			break;
		index++;
	}
	materialName = QInputDialog::getText(
		this,
		tr( "Material Name" ),
		tr( "Name:" ),
		QLineEdit::Normal,
		materialName );
	if( materialName.isEmpty() )
		return;

	if( group->getMaterialId( materialName.toLower() ) != -1 )
	{
		QMessageBox::critical(
			this,
			tr( "Add Material" ),
			tr( "Material name already exist! " ) );
		return;
	}

	GEW_Material::getSingleton().addMaterial( m_ui->MaterialGroup->currentIndex(), materialName );
	on_MaterialGroup_currentIndexChanged( m_ui->MaterialGroup->currentIndex() );
	m_ui->MaterialList->setCurrentRow( m_ui->MaterialList->count() - 1 );
}

void GEW_ModelProperty::on_MaterialItemDelete_triggered()
{
	int groupIndex = m_ui->MaterialGroup->currentIndex();
	QList<QListWidgetItem*> items = m_ui->MaterialList->selectedItems();
	if( groupIndex < 1 || items.isEmpty() )
		return;

	if( QMessageBox::warning(
		this,
		tr( "Remove Material" ),
		tr( "Are you sure?" ),
		QMessageBox::Yes,
		QMessageBox::No ) == QMessageBox::No )
	{
		return;
	}

	if( m_dialog->isVisible() )
		m_dialog->close();

	GEW_Material::getSingleton().selectMaterial( NULL );
	GEW_Material::Group *group = GEW_Material::getSingleton().getGroup( groupIndex );
	GE_Material *nullMaterial = GEW_Material::getSingleton().getNullMaterial();
	m_ui->MaterialList->blockSignals( true );
	foreach( QListWidgetItem *item, items )
	{
		int materialId = group->getMaterialId( item->text().toLower() );
		GE_Material *material = group->getMaterial( materialId );
		foreach( GEW_Model *gewModel, GEW_Model::m_list )
		{
			gewModel->replaceMaterial( material, nullMaterial );
			if( gewModel == m_model )
			{
				GE_Model *model = m_model->m_model;
				for( uint32_t i = 0; i < model->getNumberOfSubsets(); ++i )
				{
					if( model->getSubset( i )->Material == nullMaterial )
						m_ui->ModelSubsetList->item( i )->setIcon( GEW_Model::m_iconSubsetNoMaterial );
				}
			}
		}
		group->removeMaterial( materialId );
		delete item;
	}
	m_ui->MaterialList->blockSignals( false );
}

void GEW_ModelProperty::on_MaterialItemRename_triggered()
{
	if( m_ui->MaterialGroup->currentIndex() < 1 || !m_ui->MaterialList->currentItem() )
		return;

	GEW_Material::Group *group = GEW_Material::getSingleton().getGroup( m_ui->MaterialGroup->currentIndex() );
	GE_Material *material = group->getMaterial( m_ui->MaterialList->currentRow() );

	QString materialName = QInputDialog::getText(
		this,
		tr( "Material Name" ),
		tr( "Name:" ),
		QLineEdit::Normal,
		material->Name.c_str() );
	if( materialName.isEmpty() )
		return;
	if( group->getMaterialId( materialName.toLower() ) != -1 )
	{
		QMessageBox::critical(
			this,
			tr( "Add Material" ),
			tr( "Material name already exist! " ) );
		return;
	}

	material->Name = materialName.toStdString();
	m_ui->MaterialList->currentItem()->setText( materialName );
}

void GEW_ModelProperty::on_MaterialItemDuplicate_triggered()
{

}

void GEW_ModelProperty::on_MaterialItemCopy_triggered()
{
	int groupId = m_ui->MaterialGroup->currentIndex();
	int materialId = m_ui->MaterialList->currentRow();
	if( groupId < 1 || materialId < 0 )
		return;

	m_materialCopy = GEW_Material::getSingleton().getMaterial( groupId, materialId );
}

void GEW_ModelProperty::on_MaterialItemPaste_triggered()
{
	int groupId = m_ui->MaterialGroup->currentIndex();
	int materialId = m_ui->MaterialList->currentRow();
	if( groupId < 1 || materialId < 0 )
		return;

	GE_Material *material = GEW_Material::getSingleton().getMaterial( groupId, materialId );
	if( material == m_materialCopy )
		return;
	material->copyFrom( *m_materialCopy );
	if( m_dialog->isVisible() && m_dialog->getMaterial() == material )
		m_dialog->setMaterial( material );
}

void GEW_ModelProperty::on_MaterialItemSearch_triggered()
{
	QString searchName = QInputDialog::getText(
		this,
		tr( "Search Material" ),
		tr( "Name:" ) );
	if( searchName.isEmpty() )
		return;

	int groupId;
	int materialId;
	GEW_Material::getSingleton().getMaterialId( groupId, materialId, searchName.toLower() );
	if( groupId == -1 || materialId == -1 )
	{
		QMessageBox::critical(
			this,
			tr( "Search material" ),
			tr( "Material not found!" ) );
	} else
	{
		m_ui->MaterialGroup->setCurrentIndex( groupId );
		m_ui->MaterialList->setCurrentRow( materialId );
	}
}

//Physics------------------------------------------------------------------------------------------------------------------------------------
void GEW_ModelProperty::updatePxUi()
{
	if( !m_model )
		return;

	m_ui->ModelPxEnable->blockSignals( true );
	m_ui->ModelPxEnable->setChecked( m_model->m_pxActorData.Enable );
	m_ui->ModelPxEnable->blockSignals( false );

	m_ui->ModelPxActorType->blockSignals( true );
	m_ui->ModelPxActorType->setCurrentIndex( ( int ) m_model->m_pxActorData.MyType );
	m_ui->ModelPxActorType->blockSignals( false );

	m_ui->ModelPxUseMass->blockSignals( true );
	m_ui->ModelPxUseMass->setChecked( m_model->m_pxActorData.UseMass );
	m_ui->ModelPxUseMass->blockSignals( false );

	m_ui->ModelPxMass->blockSignals( true );
	m_ui->ModelPxMass->setValue( m_model->m_pxActorData.Mass );
	m_ui->ModelPxMass->blockSignals( false );

	m_ui->ModelPxShowCenterOfMass->blockSignals( true );
	m_ui->ModelPxShowCenterOfMass->setChecked( m_model->m_pxActorData.ShowCenterOfMass );
	m_ui->ModelPxShowCenterOfMass->blockSignals( false );

	m_ui->ModelPxLinearDamping->blockSignals( true );
	m_ui->ModelPxLinearDamping->setValue( GE_ConvertToMeter( m_model->m_pxActorData.LinearDamping ) );
	m_ui->ModelPxLinearDamping->blockSignals( false );

	m_ui->ModelPxAngularDamping->blockSignals( true );
	m_ui->ModelPxAngularDamping->setValue( GE_ConvertToDegrees( m_model->m_pxActorData.AngularDampin ) );
	m_ui->ModelPxAngularDamping->blockSignals( false );

	m_ui->ModelPxCenterOfMassGroupBox->blockSignals( true );
	m_ui->ModelPxCenterOfMassGroupBox->setChecked( m_model->m_pxActorData.UseCenterOfMass );
	m_ui->ModelPxCenterOfMassX->setEnabled( m_model->m_pxActorData.UseCenterOfMass );
	m_ui->ModelPxCenterOfMassY->setEnabled( m_model->m_pxActorData.UseCenterOfMass );
	m_ui->ModelPxCenterOfMassZ->setEnabled( m_model->m_pxActorData.UseCenterOfMass );
	m_ui->label_137->setEnabled( m_model->m_pxActorData.UseCenterOfMass );
	m_ui->label_136->setEnabled( m_model->m_pxActorData.UseCenterOfMass );
	m_ui->label_125->setEnabled( m_model->m_pxActorData.UseCenterOfMass );
	m_ui->ModelPxCenterOfMassGroupBox->blockSignals( false );

	m_ui->ModelPxCenterOfMassX->blockSignals( true );
	m_ui->ModelPxCenterOfMassX->setValue( GE_ConvertToMeter( m_model->m_pxActorData.CenterOfMass.x ) );
	m_ui->ModelPxCenterOfMassX->blockSignals( false );

	m_ui->ModelPxCenterOfMassY->blockSignals( true );
	m_ui->ModelPxCenterOfMassY->setValue( GE_ConvertToMeter( m_model->m_pxActorData.CenterOfMass.y ) );
	m_ui->ModelPxCenterOfMassY->blockSignals( false );

	m_ui->ModelPxCenterOfMassZ->blockSignals( true );
	m_ui->ModelPxCenterOfMassZ->setValue( GE_ConvertToMeter( m_model->m_pxActorData.CenterOfMass.z ) );
	m_ui->ModelPxCenterOfMassZ->blockSignals( false );

	m_ui->ModelPxShowCenterOfMass->blockSignals( true );
	m_ui->ModelPxShowCenterOfMass->setChecked( m_model->m_pxActorData.ShowCenterOfMass );
	m_ui->ModelPxShowCenterOfMass->blockSignals( false );

	m_ui->ModelPxeENABLE_CCD->blockSignals( true );
	m_ui->ModelPxeENABLE_CCD->setChecked( m_model->m_pxActorData.RigidBodyFlags.eENABLE_CCD );
	m_ui->ModelPxeENABLE_CCD->blockSignals( false );

	m_ui->ModelPxeENABLE_CCD_FRICTION->blockSignals( true );
	m_ui->ModelPxeENABLE_CCD_FRICTION->setChecked( m_model->m_pxActorData.RigidBodyFlags.eENABLE_CCD_FRICTION );
	m_ui->ModelPxeENABLE_CCD_FRICTION->blockSignals( false );

	m_ui->ModelPxeKINEMATIC->blockSignals( true );
	m_ui->ModelPxeKINEMATIC->setChecked( m_model->m_pxActorData.RigidBodyFlags.eKINEMATIC );
	m_ui->ModelPxeKINEMATIC->blockSignals( false );

	m_ui->ModelPxeUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES->blockSignals( true );
	m_ui->ModelPxeUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES->setChecked( m_model->m_pxActorData.RigidBodyFlags.eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES );
	m_ui->ModelPxeUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES->blockSignals( false );

	m_ui->ModelPxActoreVISUALIZATION->blockSignals( true );
	m_ui->ModelPxActoreVISUALIZATION->setChecked( m_model->m_pxActorData.ActorFlags.eVISUALIZATION );
	m_ui->ModelPxActoreVISUALIZATION->blockSignals( false );

	m_ui->ModelPxActoreDISABLE_GRAVITY->blockSignals( true );
	m_ui->ModelPxActoreDISABLE_GRAVITY->setChecked( m_model->m_pxActorData.ActorFlags.eDISABLE_GRAVITY );
	m_ui->ModelPxActoreDISABLE_GRAVITY->blockSignals( false );

	m_ui->ModelPxActoreSEND_SLEEP_NOTIFIES->blockSignals( true );
	m_ui->ModelPxActoreSEND_SLEEP_NOTIFIES->setChecked( m_model->m_pxActorData.ActorFlags.eSEND_SLEEP_NOTIFIES );
	m_ui->ModelPxActoreSEND_SLEEP_NOTIFIES->blockSignals( false );

	m_ui->ModelPxActoreDISABLE_SIMULATION->blockSignals( true );
	m_ui->ModelPxActoreDISABLE_SIMULATION->setChecked( m_model->m_pxActorData.ActorFlags.eDISABLE_SIMULATION );
	m_ui->ModelPxActoreDISABLE_SIMULATION->blockSignals( false );

	m_ui->ModelPxMaterials->blockSignals( true );
	m_ui->ModelPxMaterials->clear();
	m_ui->ModelPxMaterials->addItem( GEW_Physics::m_iconMaterialNull, "NULL" );
	for( uint32_t i = 1; i < GEW_Physics::getSingleton().getNbMaterial(); ++i )
		m_ui->ModelPxMaterials->addItem( GEW_Physics::m_iconMaterial, GEW_Physics::getSingleton().getMaterialName( i ) );
	m_ui->ModelPxMaterials->setCurrentIndex( 0 );
	m_ui->ModelPxMaterials->blockSignals( false );

	m_ui->ModelPxShapeList->blockSignals( true );
	m_ui->ModelPxShapeList->clear();
	for( uint32_t i = 0; i < m_model->getPxShapeCount(); ++i )
	{
		QListWidgetItem *item = new QListWidgetItem( m_model->getPxShapeName( i ), m_ui->ModelPxShapeList );
		item->setCheckState( m_model->m_pxShapeData[ i ].Use ? Qt::Checked : Qt::Unchecked );
		updateModelPxShapeListIcon( ( int ) i );
	}
	if( m_model->getPxShapeCount() )
	{
		m_ui->ModelPxShapeList->setCurrentRow( 0 );
		on_ModelPxShapeList_currentRowChanged( 0 );
	}
	m_ui->ModelPxShapeList->blockSignals( false );
}

void GEW_ModelProperty::updateModelPxShapeListIcon( int i_itemIndex )
{
	QPixmap pix( QSize( 36, 18 ) );
	QPainter painter( &pix );
	painter.fillRect( 0, 0, 36, 18, Qt::white );
	if( m_model->m_pxShapeData[ i_itemIndex ].MaterialID == 0 )
		GEW_Physics::m_iconMaterialNull.paint( &painter, 0, 0, 18, 18 );
	else
		GEW_Physics::m_iconMaterial.paint( &painter, 0, 0, 18, 18 );
	m_ui->ModelPxSurface->itemIcon( m_model->m_pxShapeData[ i_itemIndex ].MySurface ).paint( &painter, 18, 0, 18, 18 );
	m_ui->ModelPxShapeList->blockSignals( true );
	m_ui->ModelPxShapeList->item( i_itemIndex )->setIcon( QIcon( pix ) );
	m_ui->ModelPxShapeList->blockSignals( false );
}

void GEW_ModelProperty::on_ModelPxEnable_toggled( bool i_checked )
{
	if( !m_model )
		return;

	if( i_checked )
	{
		if( !m_model->createPhysics() )
		{
			QMessageBox::critical( 
				this,
				tr( "Creating Physics Actor" ),
				tr( "Can not create Physics Actor!!!" ) );
			m_ui->ModelPxEnable->blockSignals( true );
			m_ui->ModelPxEnable->setChecked( false );
			m_ui->ModelPxEnable->blockSignals( false );
			m_model->setItemCheckState( GEW_Item::Physics, false );
		} else
		{
			m_model->m_pxActorData.Enable = true;
			m_model->setItemCheckState( GEW_Item::Physics, true );
		}
	} else
	{
		m_model->destroyPhysics();
		m_model->m_pxActorData.Enable = false;
		m_model->setItemCheckState( GEW_Item::Physics, false );
	}
}

void GEW_ModelProperty::on_ModelPxActorType_currentIndexChanged( int i_index )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.MyType = ( GEW_Model::ActorData::Type ) i_index;
}

void GEW_ModelProperty::on_ModelPxUseMass_toggled( bool i_checked )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.UseMass = i_checked;
}

void GEW_ModelProperty::on_ModelPxMass_valueChanged( double i_value )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.Mass = ( float ) i_value;
}

void GEW_ModelProperty::on_ModelPxShowCenterOfMass_toggled( bool i_checked )
{
	if( !m_model )
		return;
	m_model->setShowCenterOfMass( i_checked );
}

void GEW_ModelProperty::on_ModelPxLinearDamping_valueChanged( double i_value )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.LinearDamping = GE_ConvertToUnit( ( float ) i_value );
}

void GEW_ModelProperty::on_ModelPxAngularDamping_valueChanged( double i_value )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.AngularDampin = GE_ConvertToRadians( ( float ) i_value );
}

void GEW_ModelProperty::on_ModelPxCenterOfMassGroupBox_toggled( bool i_checked )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.UseCenterOfMass = i_checked;
}

void GEW_ModelProperty::on_ModelPxCenterOfMassX_valueChanged( double i_value )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.CenterOfMass.x = GE_ConvertToUnit( ( float ) i_value );
}

void GEW_ModelProperty::on_ModelPxCenterOfMassY_valueChanged( double i_value )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.CenterOfMass.y = GE_ConvertToUnit( ( float ) i_value );
}

void GEW_ModelProperty::on_ModelPxCenterOfMassZ_valueChanged( double i_value )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.CenterOfMass.z = GE_ConvertToUnit( ( float ) i_value );
}

void GEW_ModelProperty::on_ModelPxeENABLE_CCD_toggled( bool i_checked )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.RigidBodyFlags.eENABLE_CCD = i_checked;
}

void GEW_ModelProperty::on_ModelPxeENABLE_CCD_FRICTION_toggled( bool i_checked )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.RigidBodyFlags.eENABLE_CCD_FRICTION = i_checked;
}

void GEW_ModelProperty::on_ModelPxeKINEMATIC_toggled( bool i_checked )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.RigidBodyFlags.eKINEMATIC = i_checked;
}

void GEW_ModelProperty::on_ModelPxeUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES_toggled( bool i_checked )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.RigidBodyFlags.eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES = i_checked;
}

void GEW_ModelProperty::on_ModelPxActoreVISUALIZATION_toggled( bool i_checked )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.ActorFlags.eVISUALIZATION = i_checked;
}

void GEW_ModelProperty::on_ModelPxActoreDISABLE_GRAVITY_toggled( bool i_checked )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.ActorFlags.eDISABLE_GRAVITY = i_checked;
}

void GEW_ModelProperty::on_ModelPxActoreSEND_SLEEP_NOTIFIES_toggled( bool i_checked )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.ActorFlags.eSEND_SLEEP_NOTIFIES = i_checked;
}

void GEW_ModelProperty::on_ModelPxActoreDISABLE_SIMULATION_toggled( bool i_checked )
{
	if( !m_model )
		return;
	m_model->m_pxActorData.ActorFlags.eDISABLE_SIMULATION = i_checked;
}

void GEW_ModelProperty::on_ModelPxShapeList_currentRowChanged( int i_row )
{
	int shapeIndex = i_row;
	if( !m_model || shapeIndex < 0 )
		return;

	m_ui->ModelPxeVISUALIZATION->blockSignals( true );
	m_ui->ModelPxeVISUALIZATION->setChecked( m_model->m_pxShapeData[ shapeIndex ].ShapeFlags.eVISUALIZATION );
	m_ui->ModelPxeVISUALIZATION->blockSignals( false );

	m_ui->ModelPxeTRIGGER_SHAPE->blockSignals( true );
	m_ui->ModelPxeTRIGGER_SHAPE->setChecked( m_model->m_pxShapeData[ shapeIndex ].ShapeFlags.eTRIGGER_SHAPE );
	m_ui->ModelPxeTRIGGER_SHAPE->blockSignals( false );

	m_ui->ModelPxeSIMULATION_SHAPE->blockSignals( true );
	m_ui->ModelPxeSIMULATION_SHAPE->setChecked( m_model->m_pxShapeData[ shapeIndex ].ShapeFlags.eSIMULATION_SHAPE );
	m_ui->ModelPxeSIMULATION_SHAPE->blockSignals( false );

	m_ui->ModelPxeSCENE_QUERY_SHAPE->blockSignals( true );
	m_ui->ModelPxeSCENE_QUERY_SHAPE->setChecked( m_model->m_pxShapeData[ shapeIndex ].ShapeFlags.eSCENE_QUERY_SHAPE );
	m_ui->ModelPxeSCENE_QUERY_SHAPE->blockSignals( false );

	m_ui->ModelPxePARTICLE_DRAIN->blockSignals( true );
	m_ui->ModelPxePARTICLE_DRAIN->setChecked( m_model->m_pxShapeData[ shapeIndex ].ShapeFlags.ePARTICLE_DRAIN );
	m_ui->ModelPxePARTICLE_DRAIN->blockSignals( false );

	m_ui->ModelPxSurface->blockSignals( true );
	m_ui->ModelPxSurface->setCurrentIndex( ( int ) m_model->m_pxShapeData[ shapeIndex ].MySurface );
	m_ui->ModelPxSurface->blockSignals( false );

	m_ui->ModelPxBoundVisible->blockSignals( true );
	m_ui->ModelPxBoundVisible->setChecked( m_model->m_pxShapeData[ shapeIndex ].BoundVisible );
	m_ui->ModelPxBoundVisible->blockSignals( false );

	QColor color;
	color.setRedF( m_model->m_pxShapeData[ shapeIndex ].BoundColor.x );
	color.setGreenF( m_model->m_pxShapeData[ shapeIndex ].BoundColor.y );
	color.setBlueF( m_model->m_pxShapeData[ shapeIndex ].BoundColor.z );
	m_ui->ModelPxBoundColor->blockSignals( true );
	m_ui->ModelPxBoundColor->slotSetColor( color );
	m_ui->ModelPxBoundColor->blockSignals( false );

	m_ui->ModelPxMaterials->setCurrentIndex( m_model->m_pxShapeData[ shapeIndex ].MaterialID );
}

void GEW_ModelProperty::on_ModelPxShapeList_itemChanged( QListWidgetItem * i_item )
{
	int shapeIndex = m_ui->ModelPxShapeList->currentRow();
	if( !m_model || shapeIndex < 0 )
		return;

	if( m_model->m_pxActorData.Enable )
	{
		m_ui->ModelPxShapeList->blockSignals( true );
		i_item->setCheckState( i_item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked );
		m_ui->ModelPxShapeList->blockSignals( false );
	} else
		m_model->m_pxShapeData[ shapeIndex ].Use = i_item->checkState() == Qt::Checked;
}

void GEW_ModelProperty::on_ModelPxeVISUALIZATION_toggled( bool i_checked )
{
	int shapeIndex = m_ui->ModelPxShapeList->currentRow();
	if( !m_model || shapeIndex < 0 )
		return;
	m_model->m_pxShapeData[ shapeIndex ].ShapeFlags.eVISUALIZATION = i_checked;
}

void GEW_ModelProperty::on_ModelPxeTRIGGER_SHAPE_toggled( bool i_checked )
{
	int shapeIndex = m_ui->ModelPxShapeList->currentRow();
	if( !m_model || shapeIndex < 0 )
		return;
	m_model->m_pxShapeData[ shapeIndex ].ShapeFlags.eTRIGGER_SHAPE = i_checked;
}

void GEW_ModelProperty::on_ModelPxeSIMULATION_SHAPE_toggled( bool i_checked )
{
	int shapeIndex = m_ui->ModelPxShapeList->currentRow();
	if( !m_model || shapeIndex < 0 )
		return;
	m_model->m_pxShapeData[ shapeIndex ].ShapeFlags.eSIMULATION_SHAPE = i_checked;
}

void GEW_ModelProperty::on_ModelPxeSCENE_QUERY_SHAPE_toggled( bool i_checked )
{
	int shapeIndex = m_ui->ModelPxShapeList->currentRow();
	if( !m_model || shapeIndex < 0 )
		return;
	m_model->m_pxShapeData[ shapeIndex ].ShapeFlags.eSCENE_QUERY_SHAPE = i_checked;
}

void GEW_ModelProperty::on_ModelPxePARTICLE_DRAIN_toggled( bool i_checked )
{
	int shapeIndex = m_ui->ModelPxShapeList->currentRow();
	if( !m_model || shapeIndex < 0 )
		return;
	m_model->m_pxShapeData[ shapeIndex ].ShapeFlags.ePARTICLE_DRAIN = i_checked;
}

void GEW_ModelProperty::on_ModelPxSurface_currentIndexChanged( int i_index )
{
	int shapeIndex = m_ui->ModelPxShapeList->currentRow();
	if( !m_model || shapeIndex < 0 )
		return;
	m_model->setSurfaceType( shapeIndex, ( GEW_Model::ShapeData::Surface ) i_index );
	updateModelPxShapeListIcon( shapeIndex );
}

void GEW_ModelProperty::on_ModelPxSetSurfaceToAllShapes_triggered()
{
	if( !m_model )
		return;
	GEW_Model::ShapeData::Surface surface = ( GEW_Model::ShapeData::Surface )m_ui->ModelPxSurface->currentIndex();
	for( uint32_t i = 0; i < m_model->getPxShapeCount(); ++i )
	{
		m_model->setSurfaceType( i, surface );
		updateModelPxShapeListIcon( i );
	}
}

void GEW_ModelProperty::on_ModelPxSurface_customContextMenuRequested( QPoint )
{
	QMenu menu( this );
	menu.addAction( m_ui->ModelPxSetSurfaceToAllShapes );
	menu.exec( QCursor::pos() );
}

void GEW_ModelProperty::on_ModelPxBoundVisible_toggled( bool i_checked )
{
	int shapeIndex = m_ui->ModelPxShapeList->currentRow();
	if( !m_model || shapeIndex < 0 )
		return;
	m_model->m_pxShapeData[ shapeIndex ].BoundVisible = i_checked;
	m_model->m_pxRenderUtilityObjects[ shapeIndex ]->setVisible( i_checked );
}

void GEW_ModelProperty::on_ModelPxBoundColor_signalColorChanged( QColor i_color )
{
	int shapeIndex = m_ui->ModelPxShapeList->currentRow();
	if( !m_model || shapeIndex < 0 )
		return;
	GE_Vec3 color;
	color.x = i_color.redF();
	color.y = i_color.greenF();
	color.z = i_color.blueF();
	m_model->m_pxShapeData[ shapeIndex ].BoundColor = color;
	m_model->m_pxRenderUtilityObjects[ shapeIndex ]->setColor( color );
}

void GEW_ModelProperty::on_ModelPxBoundAllVisible_clicked()
{
	if( !m_model )
		return;
	for( uint32_t i = 0; i < m_model->getPxShapeCount(); ++i )
	{
		m_model->m_pxShapeData[ i ].BoundVisible = true;
		m_model->m_pxRenderUtilityObjects[ i ]->setVisible( true );
	}
	m_ui->ModelPxBoundVisible->blockSignals( true );
	m_ui->ModelPxBoundVisible->setChecked( true );
	m_ui->ModelPxBoundVisible->blockSignals( false );
}

void GEW_ModelProperty::on_ModelPxBoundAllInvisible_clicked()
{
	if( !m_model )
		return;
	for( uint32_t i = 0; i < m_model->getPxShapeCount(); ++i )
	{
		m_model->m_pxShapeData[ i ].BoundVisible = false;
		m_model->m_pxRenderUtilityObjects[ i ]->setVisible( false );
	}
	m_ui->ModelPxBoundVisible->blockSignals( true );
	m_ui->ModelPxBoundVisible->setChecked( false );
	m_ui->ModelPxBoundVisible->blockSignals( false );
}

void GEW_ModelProperty::on_ModelPxForceApply_clicked()
{
	if( !m_model )
		return;
	GE_Vec3 linearDirection;
	linearDirection.x = ( float ) m_ui->ModelPxLinearVelocityX->value();
	linearDirection.y = ( float ) m_ui->ModelPxLinearVelocityY->value();
	linearDirection.z = ( float ) m_ui->ModelPxLinearVelocityZ->value();

	GE_Vec3 angularDirection;
	angularDirection.x = ( float ) m_ui->ModelPxAngularVelocityX->value();
	angularDirection.y = ( float ) m_ui->ModelPxAngularVelocityY->value();
	angularDirection.z = ( float ) m_ui->ModelPxAngularVelocityZ->value();

	m_model->setLinearVelocity( linearDirection );
	m_model->setAngularVelocity( angularDirection );
}

void GEW_ModelProperty::on_ModelPxShapeList_customContextMenuRequested( QPoint )
{
	QMenu menu( this );
	menu.addAction( m_ui->PxMaterialSet );
	menu.addAction( m_ui->PxMaterialUnSet );
	menu.addAction( m_ui->PxMaterialSetToAllShapes );
	menu.addAction( m_ui->ModelPxSetSurfaceToAllShapes );
	menu.exec( QCursor::pos() );
}

void GEW_ModelProperty::on_ModelPxMaterials_customContextMenuRequested( QPoint )
{
	QMenu menu( this );
	menu.addAction( m_ui->PxMaterialEdit );
	menu.addAction( m_ui->PxMaterialRename );
	menu.addSeparator();
	menu.addAction( m_ui->PxMaterialDuplicate );
	menu.addAction( m_ui->PxMaterialCopy );
	menu.addAction( m_ui->PxMaterialPaste );
	menu.addSeparator();
	menu.addAction( m_ui->PxMaterialAdd );
	menu.addAction( m_ui->PxMaterialRemove );
	menu.addAction( m_ui->PxMaterialClearAll );
	menu.addSeparator();
	menu.addAction( m_ui->PxMaterialSaveThis );
	menu.addAction( m_ui->PxMaterialSaveAll );
	menu.addAction( m_ui->PxMaterialOpen );
	menu.exec( QCursor::pos() );
}

void GEW_ModelProperty::on_PxMaterialAdd_triggered()
{

}

void GEW_ModelProperty::on_PxMaterialRemove_triggered()
{

}

void GEW_ModelProperty::on_PxMaterialRename_triggered()
{

}

void GEW_ModelProperty::on_PxMaterialSet_triggered()
{
	int shapeIndex = m_ui->ModelPxShapeList->currentRow();
	int materialIndex = m_ui->ModelPxMaterials->currentIndex();
	if( !m_model || shapeIndex < 0 || materialIndex < 0 )
		return;
	m_model->m_pxShapeData[ shapeIndex ].MaterialID = materialIndex;
	updateModelPxShapeListIcon( shapeIndex );
}

void GEW_ModelProperty::on_PxMaterialUnSet_triggered()
{
	int shapeIndex = m_ui->ModelPxShapeList->currentRow();
	if( !m_model || shapeIndex < 0 )
		return;
	m_model->m_pxShapeData[ shapeIndex ].MaterialID = 0;
	updateModelPxShapeListIcon( shapeIndex );
	m_ui->ModelPxMaterials->setCurrentIndex( 0 );
}

void GEW_ModelProperty::on_PxMaterialSetToAllShapes_triggered()
{
	int materialIndex = m_ui->ModelPxMaterials->currentIndex();
	if( !m_model || materialIndex < 0 )
		return;
	for( uint32_t i = 0; i < m_model->getPxShapeCount(); ++i )
	{
		m_model->m_pxShapeData[ i ].MaterialID = materialIndex;
		updateModelPxShapeListIcon( i );
	}
}

void GEW_ModelProperty::on_PxMaterialSaveThis_triggered()
{

}

void GEW_ModelProperty::on_PxMaterialSaveAll_triggered()
{

}

void GEW_ModelProperty::on_PxMaterialClearAll_triggered()
{

}

void GEW_ModelProperty::on_PxMaterialOpen_triggered()
{

}

void GEW_ModelProperty::on_PxMaterialDuplicate_triggered()
{

}

void GEW_ModelProperty::on_PxMaterialCopy_triggered()
{

}

void GEW_ModelProperty::on_PxMaterialPaste_triggered()
{

}

void GEW_ModelProperty::on_PxMaterialEdit_triggered()
{

}

void GEW_ModelProperty::on_Instances_toggled( bool i_checked )
{

}

void GEW_ModelProperty::on_InstanceAdd_triggered()
{

}

void GEW_ModelProperty::on_InstanceDelete_triggered()
{

}

void GEW_ModelProperty::on_InstanceRename_triggered()
{

}

void GEW_ModelProperty::on_InstanceSearch_triggered()
{

}

void GEW_ModelProperty::on_InstanceCopy_triggered()
{

}

void GEW_ModelProperty::on_InstancePaste_triggered()
{

}

void GEW_ModelProperty::on_InstanceDuplicate_triggered()
{

}
