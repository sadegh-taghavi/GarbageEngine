#pragma once

#include <QWidget>
#include <QListWidget>
#include <GE_Lens.h>
namespace Ui { class FlareProperty; }

class GEW_FlareProperty : public QWidget
{
	Q_OBJECT
	Ui::FlareProperty *m_ui;
	class GEW_Flare *m_flare;
	GE_LensFlare::Parameter m_copyParameter;

public:
	explicit GEW_FlareProperty( QWidget *i_parent = 0 );
	~GEW_FlareProperty( );
	void setFlare( class GEW_Flare *i_flare );
	class GEW_Flare *getFlare();
private slots:
	void on_Enable_toggled( bool i_checked );
	void on_BoundColor_signalColorChanged( QColor i_color );

	void on_DiffuseColor_signalColorChanged( QColor i_color );
	void on_DiffuseIntensity_valueChanged( double i_value );
	void on_Layer_valueChanged( double i_value );
	void on_AutoScale_toggled( bool i_checked );
	void on_AutoRotate_toggled( bool i_checked );
	void on_ScaleMulX_valueChanged( double i_value );
	void on_ScaleMulY_valueChanged( double i_value );
	void on_PositionMul_valueChanged( double i_value );
	void on_Rotation_valueChanged( double i_value );
	void on_VisiblePixelOffset_valueChanged( int i_value );
	void on_VisibleAngleOffset_valueChanged( double i_value );
	void on_PositionOffsetX_valueChanged( double i_value );
	void on_PositionOffsetY_valueChanged( double i_value );
	void on_PositionOffsetZ_valueChanged( double i_value );
	void on_Map_signalImageChanged( QString i_fileName );
	void on_FlareAdd_triggered();
	void on_FlareRemove_triggered();
	void on_FlareDuplicate_triggered();
	void on_FlareCopy_triggered();
	void on_FlarePaste_triggered();
	void on_FlareRename_triggered();
	void on_FlareList_signalItemOrderChanged( QList<QListWidgetItem*> &i_oldItems, QList<QListWidgetItem*> &i_newItems );
	void on_FlareList_itemChanged( class QListWidgetItem* i_item );
	void on_FlareList_itemClicked( QListWidgetItem* i_item );
	void on_FlareList_itemDoubleClicked( QListWidgetItem* i_item );
	void on_FlareList_customContextMenuRequested( QPoint );
	void on_FlareList_signalKeyPressed( Qt::Key );
};