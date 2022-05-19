#pragma once

#include <QWidget>

namespace Ui { class GEW_ModelProperty; }

class QListWidgetItem;
class GEW_Model;
class GEW_MaterialEditorDialog;
struct GE_Material;

class GEW_ModelProperty : public QWidget
{
	Q_OBJECT

	Ui::GEW_ModelProperty *m_ui;
	GEW_Model *m_model;
	GEW_MaterialEditorDialog *m_dialog;
	GE_Material* m_materialCopy;
public:
	GEW_ModelProperty( QWidget *i_parent );
	~GEW_ModelProperty();

	void setModel( GEW_Model *i_model );
	void updateMaterialGroup();

private slots:
	void on_ModelVisible_toggled( bool i_checked );
	void on_ModelTransparent_toggled(bool i_checked);
	void on_ModelDynamic_toggled( bool i_checked );
	void on_ModelTwoSideShade_toggled( bool i_checked );
	void on_ModelTwoSideDepth_toggled( bool i_checked );
	void on_ModelShadowCaster_toggled( bool i_checked );
	void on_ModelBoundColor_signalColorChanged( QColor i_color );
	void on_ModelLOD_currentIndexChanged( int i_index );
	void on_ModelLODAdd_clicked();
	void on_ModelLODRemove_clicked();
	void on_ModelLodDistance_valueChanged( double i_value );
	void on_ModelLodDistanceFromCamera_clicked();
	void on_ModelVisibleDistance_valueChanged( double i_value );
	void on_ModelVisibleDistanceFromCamera_clicked();

	void on_ModelSubsetList_itemDoubleClicked( QListWidgetItem *i_item );
	void on_ModelSubsetList_itemClicked( QListWidgetItem *i_item );
	void on_ModelSubsetList_currentRowChanged( int i_row );
	void on_ModelSubsetList_signalKeyPressed( Qt::Key key );
	void on_ModelSubsetList_customContextMenuRequested( QPoint );
	void on_SubsetItemRename_triggered( );
	void on_SubsetItemSetMaterial_triggered( );
	void on_SubsetItemUnSetMaterial_triggered( );
	void on_SubsetItemSetMaterialFromGroup_triggered( );
	void on_ModelUseVertexColor_toggled( bool i_checked );
	void on_ModelLayerMultiply_valueChanged( double i_value );
	void on_ModelLayerOffset_valueChanged( double i_value );
	void on_ModelSubsetLightMap_signalImageChanged( QString i_filePath );

	void on_MaterialGroup_currentIndexChanged( int i_index );
	void on_MaterialGroup_customContextMenuRequested( QPoint );
	void on_MaterialRenameGroup_triggered();
	void on_MaterialOpenGroup_triggered();
	void on_MaterialSaveThisGroup_triggered( );
	void on_MaterialSaveAllGroups_triggered( );
	void on_MaterialClearAllGroups_triggered( );
	void on_MaterialGroupAdd_clicked();
	void on_MaterialGroupRemove_clicked();
	void on_MaterialList_signalKeyPressed( Qt::Key key );
	void on_MaterialList_itemClicked( QListWidgetItem *i_item );
	void on_MaterialList_itemDoubleClicked( QListWidgetItem *i_item );
	void on_MaterialList_currentRowChanged( int i_row );
	void on_MaterialList_signalItemOrderChanged( QList<QListWidgetItem*> &i_oldItems, QList<QListWidgetItem*> &i_newItems );
	void on_MaterialList_customContextMenuRequested( QPoint );
	void on_MaterialItemAdd_triggered();
	void on_MaterialItemDelete_triggered();
	void on_MaterialItemRename_triggered();
	void on_MaterialItemDuplicate_triggered();
	void on_MaterialItemCopy_triggered();
	void on_MaterialItemPaste_triggered();
	void on_MaterialItemSearch_triggered();

	void on_Instances_toggled( bool i_checked );
	void on_InstanceAdd_triggered();
	void on_InstanceDelete_triggered( );
	void on_InstanceRename_triggered( );
	void on_InstanceSearch_triggered( );
	void on_InstanceCopy_triggered( );
	void on_InstancePaste_triggered( );
	void on_InstanceDuplicate_triggered( );
		
	//physics
	void updatePxUi();
	void updateModelPxShapeListIcon( int i_itemIndex );
	void on_ModelPxEnable_toggled( bool i_checked );
	void on_ModelPxActorType_currentIndexChanged( int i_index );
	void on_ModelPxUseMass_toggled( bool i_checked );
	void on_ModelPxMass_valueChanged( double i_value );
	void on_ModelPxShowCenterOfMass_toggled( bool i_checked );
	void on_ModelPxLinearDamping_valueChanged( double i_value );
	void on_ModelPxAngularDamping_valueChanged( double i_value );
	void on_ModelPxCenterOfMassGroupBox_toggled( bool i_checked );
	void on_ModelPxCenterOfMassX_valueChanged( double i_value );
	void on_ModelPxCenterOfMassY_valueChanged( double i_value );
	void on_ModelPxCenterOfMassZ_valueChanged( double i_value );
	void on_ModelPxeENABLE_CCD_toggled( bool i_checked );
	void on_ModelPxeENABLE_CCD_FRICTION_toggled( bool i_checked );
	void on_ModelPxeKINEMATIC_toggled( bool i_checked );
	void on_ModelPxeUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES_toggled( bool i_checked );
	void on_ModelPxActoreVISUALIZATION_toggled( bool i_checked );
	void on_ModelPxActoreDISABLE_GRAVITY_toggled( bool i_checked );
	void on_ModelPxActoreSEND_SLEEP_NOTIFIES_toggled( bool i_checked );
	void on_ModelPxActoreDISABLE_SIMULATION_toggled( bool i_checked );

	void on_ModelPxShapeList_currentRowChanged( int );
	void on_ModelPxShapeList_itemChanged( QListWidgetItem * i_item );
	void on_ModelPxeVISUALIZATION_toggled( bool i_checked );
	void on_ModelPxeTRIGGER_SHAPE_toggled( bool i_checked );
	void on_ModelPxeSIMULATION_SHAPE_toggled( bool i_checked );
	void on_ModelPxeSCENE_QUERY_SHAPE_toggled( bool i_checked );
	void on_ModelPxePARTICLE_DRAIN_toggled( bool i_checked );
	void on_ModelPxSurface_currentIndexChanged( int i_index );
	void on_ModelPxSetSurfaceToAllShapes_triggered();
	void on_ModelPxSurface_customContextMenuRequested( QPoint );
	void on_ModelPxBoundVisible_toggled( bool i_checked );
	void on_ModelPxBoundColor_signalColorChanged( QColor i_color );
	void on_ModelPxBoundAllVisible_clicked();
	void on_ModelPxBoundAllInvisible_clicked();
	void on_ModelPxForceApply_clicked();
	void on_ModelPxShapeList_customContextMenuRequested( QPoint );
	void on_ModelPxMaterials_customContextMenuRequested( QPoint );
	void on_PxMaterialAdd_triggered();
	void on_PxMaterialRemove_triggered();
	void on_PxMaterialRename_triggered();
	void on_PxMaterialSet_triggered();
	void on_PxMaterialUnSet_triggered();
	void on_PxMaterialSetToAllShapes_triggered();
	void on_PxMaterialSaveThis_triggered();
	void on_PxMaterialSaveAll_triggered();
	void on_PxMaterialClearAll_triggered();
	void on_PxMaterialOpen_triggered();
	void on_PxMaterialDuplicate_triggered();
	void on_PxMaterialCopy_triggered();
	void on_PxMaterialPaste_triggered();
	void on_PxMaterialEdit_triggered();
};

