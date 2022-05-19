#pragma once

#include <QWidget>

namespace Ui { class LightProperty; }

class GEW_LightProperty : public QWidget
{
	Q_OBJECT
	Ui::LightProperty *m_ui;
	class GEW_Light *m_light;
public:
	void setLight( class GEW_Light *i_light );
	class GEW_Light *getLight();
	explicit GEW_LightProperty( QWidget *i_parent = 0 );
	~GEW_LightProperty();
private slots:
	void on_Enable_toggled( bool i_checked );
	void on_Dynamic_toggled( bool i_checked );
	void on_Intensity_valueChanged( double i_value );
	void on_Type_currentIndexChanged( int i_index );

	void on_Ambient_signalColorChanged( QColor i_color );
	void on_Diffuse_signalColorChanged( QColor i_color );
	void on_Specular_signalColorChanged( QColor i_color );
	void on_BoundColor_signalColorChanged( QColor i_color );

	void on_DirectionalTarget_toggled( bool i_checked );
	void on_DirectionalDirectionYaw_valueChanged( double i_value );
	void on_DirectionalDirectionPitch_valueChanged( double i_value );
	void on_DirectionalTargetX_valueChanged( double i_value );
	void on_DirectionalTargetY_valueChanged( double i_value );
	void on_DirectionalTargetZ_valueChanged( double i_value );

	void on_DirectionalHasShadow_toggled( bool i_checked );
	void on_DirectionalVisualizeCascade_toggled( bool i_checked );
	void on_DirectionalNumberOfCascade_currentIndexChanged( int i_index );

	void on_DirectionalCascade1Offset_valueChanged( double i_value );
	void on_DirectionalCascade1Bias_valueChanged( double i_value );
	void on_DirectionalCascade1MapSize_currentIndexChanged( int i_index );
	void on_DirectionalCascade1OrthoSize_valueChanged( int i_value );

	void on_DirectionalCascade2Offset_valueChanged( double i_value );
	void on_DirectionalCascade2Bias_valueChanged( double i_value );
	void on_DirectionalCascade2MapSize_currentIndexChanged( int i_index );
	void on_DirectionalCascade2OrthoSize_valueChanged( int i_value );

	void on_DirectionalCascade3Offset_valueChanged( double i_value );
	void on_DirectionalCascade3Bias_valueChanged( double i_value );
	void on_DirectionalCascade3MapSize_currentIndexChanged( int i_index );
	void on_DirectionalCascade3OrthoSize_valueChanged( int i_value );

	void on_DirectionalCascade4Offset_valueChanged( double i_value );
	void on_DirectionalCascade4Bias_valueChanged( double i_value );
	void on_DirectionalCascade4MapSize_currentIndexChanged( int i_index );
	void on_DirectionalCascade4OrthoSize_valueChanged( int i_value );

	void on_SpotAttenuation_valueChanged( double i_value );
	void on_SpotSperead_valueChanged( double i_value );
	void on_SpotRange_valueChanged( double i_value );
	void on_SpotSharpness_valueChanged( double i_value );
	void on_SpotHasShadow_toggled( bool i_checked );
	void on_SpotMapSize_currentIndexChanged( int );
	void on_SpotOffset_valueChanged( double i_value );
	void on_SpotBias_valueChanged( double i_value );
	void on_SpotMap_signalImageChanged( QString i_fileName );
	void on_SpotOffDistance_valueChanged( double i_value );
	void on_SpotOffDistanceFromCamera_clicked( );
	void on_SpotShadowDistance_valueChanged( double i_value );
	void on_SpotShadowDistanceFromCamera_clicked( );
	
	void on_SpotTarget_toggled( bool i_checked );
	void on_SpotDirectionYaw_valueChanged( double i_value );
	void on_SpotDirectionPitch_valueChanged( double i_value );
	void on_SpotTargetX_valueChanged( double i_value );
	void on_SpotTargetY_valueChanged( double i_value );
	void on_SpotTargetZ_valueChanged( double i_value );

	void on_PointAttenuation_valueChanged( double i_value );
	void on_PointRange_valueChanged( double i_value );
	void on_PointHasShadow_toggled( bool i_checked );
	void on_PointMapSize_currentIndexChanged( int );
	void on_PointOffset_valueChanged( double i_value );
	void on_PointBias_valueChanged( double i_value );
	void on_PointYaw_valueChanged( double i_value );
	void on_PointPitch_valueChanged( double i_value );
	void on_PointRoll_valueChanged( double i_value );
	void on_PointCubeMap_signalImageChanged( QString i_fileName );
	void on_PointOffDistance_valueChanged( double i_value );
	void on_PointOffDistanceFromCamera_clicked( );
	void on_PointShadowDistance_valueChanged( double i_value );
	void on_PointShadowDistanceFromCamera_clicked( );
};
