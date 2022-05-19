#pragma once

#include <QWidget>
#include <GE_Math.h>
namespace Ui { class DecalProperty; }

class GEW_DecalProperty : public QWidget
{
	Q_OBJECT

	Ui::DecalProperty *m_ui;
	class GEW_Decal *m_decal;

	struct
	{
		bool		Enable;
		GE_Vec3		BoundColor;
		float		VisibleDistance;
		GE_Vec3		Position;
		GE_Vec3		Scale;
		GE_Quat		Rotation;
		float		MaxAngle;
		float		Bumpiness;
		float		Glowness;
		float		Roughness;
		float		Reflectmask;
		float		Layer;
		float		RotationF;
		GE_Vec2		UVTile;
		GE_Vec2		UVMove;
		GE_Vec4     DiffuseColor;
	}m_copyData;

public:
	explicit GEW_DecalProperty( QWidget *i_parent );
	~GEW_DecalProperty();
	void setDecal( class GEW_Decal *i_decal );
private slots:
	void on_Enable_toggled( bool i_checked );
	void on_DecalEnable_toggled( bool i_checked );
	void on_DecalAdd_triggered();
	void on_DecalRemove_triggered();
	void on_DecalRename_triggered();
	void on_DecalDuplicate_triggered();
	void on_DecalCopy_triggered();
	void on_DecalPaste_triggered();

	void on_BoundColor_signalColorChanged( QColor i_color );
	void on_DiffuseColor_signalColorChanged( QColor i_color );
	void on_VisibleDistanceFromCamera_clicked( );
	void on_VisibleDistance_valueChanged( double i_value );
	void on_Layer_valueChanged( double i_value );
	void on_UTile_valueChanged( double i_value );
	void on_VTile_valueChanged( double i_value );
	void on_UMove_valueChanged( double i_value );
	void on_VMove_valueChanged( double i_value );
	void on_MapRotation_valueChanged( double i_value );
	void on_MaxAngle_valueChanged( double i_value );
	void on_Glowness_valueChanged( double i_value );
	void on_Bumpiness_valueChanged( double i_value );
	void on_Roughness_valueChanged( double i_value );
	void on_Reflectmask_valueChanged( double i_value );
	void on_DiffuseMap_signalImageChanged( QString i_fileName );
	void on_NormalMap_signalImageChanged( QString i_fileName );
	void on_GlowSpecularReflectMap_signalImageChanged( QString i_fileName );
	void on_ReflectMap_signalImageChanged( QString i_fileName );
	void on_DecalList_itemChanged( class QListWidgetItem* i_item );
	void on_DecalList_itemClicked( QListWidgetItem* i_item );
	void on_DecalList_currentRowChanged( int i_row );
	void on_DecalList_itemDoubleClicked( QListWidgetItem* i_item );
	void on_DecalList_customContextMenuRequested( QPoint );
	void on_DecalList_signalKeyPressed( Qt::Key );
};

