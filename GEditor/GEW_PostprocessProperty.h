#pragma once

#include <QWidget>

namespace Ui { class PostprocessProperty; }

class GEW_PostprocessProperty : public QWidget
{
	Q_OBJECT
	Ui::PostprocessProperty *m_ui;
public:
	explicit GEW_PostprocessProperty( QWidget *i_parent = 0 );
	~GEW_PostprocessProperty( );
	static GEW_PostprocessProperty *m_singleton;
	static GEW_PostprocessProperty *getSingleton();
	void init();
	void removeLight( class GEW_Light *i_light );
	void updateUi();

private slots:
	
	//Sky-------------------------------------------------------------
	void updateSkyUIValues();
	void on_SkyEnable_toggled( bool i_checked );
	void on_SkySamples_valueChanged( int i_value );
	void on_SkyRadius_valueChanged( double i_value );
	void on_SkyIntensity_valueChanged( double i_value );
	void on_SkyWavelength_signalColorChanged( QColor i_color );
	void on_SkyReset_clicked( );
	void on_SkyLightSelect_clicked();
	void on_SkySharpness_valueChanged( double i_value );
	void on_SkyCore_valueChanged( double i_value );
	void on_SkyHighlight_valueChanged( double i_value );
	void on_SkySun_valueChanged( double i_value );

	void on_SkyMap1Enable_toggled( bool i_checked );
	void on_SkyMap1ScaleZ_valueChanged( double i_value );
	void on_SkyMap1PositionZ_valueChanged( double i_value );
	void on_SkyMap1RotationX_valueChanged( double i_value );
	void on_SkyMap1RotationY_valueChanged( double i_value );
	void on_SkyMap1RotationZ_valueChanged( double i_value );
	void on_SkyMap1RotationXAnimate_toggled( bool i_checked );
	void on_SkyMap1RotationYAnimate_toggled( bool i_checked );
	void on_SkyMap1RotationZAnimate_toggled( bool i_checked );
	void on_SkyMap1Picture_signalImageChanged( QString i_fileName );

	void on_SkyMap2Enable_toggled( bool i_checked );
	void on_SkyMap2ScaleZ_valueChanged( double i_value );
	void on_SkyMap2PositionZ_valueChanged( double i_value );
	void on_SkyMap2RotationX_valueChanged( double i_value );
	void on_SkyMap2RotationY_valueChanged( double i_value );
	void on_SkyMap2RotationZ_valueChanged( double i_value );
	void on_SkyMap2RotationXAnimate_toggled( bool i_checked );
	void on_SkyMap2RotationYAnimate_toggled( bool i_checked );
	void on_SkyMap2RotationZAnimate_toggled( bool i_checked );
	void on_SkyMap2Picture_signalImageChanged( QString i_fileName );

	//LightScatter
	void updateLightScatterUIValues( );
	void on_LightScatterEnable_toggled( bool i_checked );
	void on_LightScatterLuminance_valueChanged( double i_value );
	void on_LightScatterCutoff_valueChanged( double i_value );
	void on_LightScatterExp_valueChanged( double i_value );
	void on_LightScatterDensity_valueChanged( double i_value );
	void on_LightScatterWeight_valueChanged( double i_value );
	void on_LightScatterDecay_valueChanged(double i_value);
	void on_LightScatterReset_clicked( );

	//Lens Dirt
	void updateLensDirtUIValues();
	void on_LensDirtEnable_toggled( bool i_checked );
	void on_LensDirtLuminance_valueChanged( double i_value );
	void on_LensDirtCutoff_valueChanged( double i_value );
	void on_LensDirtExp_valueChanged( double i_value );
	void on_LensDirtDirtExp_valueChanged( double i_value );
	void on_LensDirtCoreExp_valueChanged( double i_value );
	void on_LensDirtMap_signalImageChanged( QString i_fileName );
	void on_LensDirtReset_clicked( );

	//Motion Blur
	void updateMotionBlurUIValues( );
	void on_MotionBlurEnable_toggled( bool i_checked );
	void on_MotionBlurDamp_valueChanged( double i_value );
	void on_MotionBlurMax_valueChanged( double i_value );
	void on_MotionBlurReset_clicked( );

	//HDR
	void updateHDRUIValues( );
	void on_HDREnable_toggled( bool i_checked );
	void on_HDRWhiteCutoff_valueChanged( double i_value );
	void on_HDRLuminanceMax_valueChanged( double i_value );
	void on_HDRLuminanceMin_valueChanged( double i_value );
	void on_HDRGamma_valueChanged( double i_value );
	void on_HDRAdaptationRate_valueChanged( double i_value );
	void on_HDRReset_clicked( );
	void on_HDRGradientMap_signalImageChanged( QString i_fileName );

	//AO
	void updateAOUIValues();
	void on_AOEnable_toggled( bool i_checked );
	void on_AORadius_valueChanged( double i_value );
	void on_AODepth_valueChanged( double i_value );
	void on_AODistanceStart_valueChanged( double i_value );
	void on_AOIntensity_valueChanged( double i_value );
	void on_AODistanceEnd_valueChanged( double i_value );
	void on_AONormal_valueChanged( double i_value );
	void on_AOReset_clicked( );

	//Glow
	void updateGlowUIValues();
	void on_GlowEnable_toggled( bool i_checked );
	void on_GlowVScale_valueChanged( double i_value );
	void on_GlowHScale_valueChanged( double i_value );
	void on_GlowLuminance_valueChanged( double i_value );
	void on_GlowCutoff_valueChanged( double i_value );
	void on_GlowReset_clicked( );

	//GlowMapped
	void updateGlowMappedUIValues( );
	void on_GlowMappedEnable_toggled( bool i_checked );
	void on_GlowMappedVScale_valueChanged( double i_value );
	void on_GlowMappedHScale_valueChanged( double i_value );
	void on_GlowMappedLuminance_valueChanged( double i_value );
	void on_GlowMappedCutoff_valueChanged( double i_value );
	void on_GlowMappedReset_clicked( );

	//AtmosphereFog
	void updateAtmosphereFogUIValues();
	void on_AtmosphereFogEnable_toggled( bool i_checked );
	void on_AtmosphereFogIntensity_valueChanged( double i_value );
	void on_AtmosphereFogDepthPosition_valueChanged( double i_value );
	void on_AtmosphereFogDepthField_valueChanged( double i_value );
	void on_AtmosphereFogDepthFalloff_valueChanged( double i_value );
	void on_AtmosphereFogDepthHardness_valueChanged( double i_value );
	void on_AtmosphereFogHeightPosition_valueChanged( double i_value );
	void on_AtmosphereFogHeightField_valueChanged( double i_value );
	void on_AtmosphereFogHeightFalloff_valueChanged( double i_value );
	void on_AtmosphereFogHeightHardness_valueChanged( double i_value );
	void on_AtmosphereFogColor_signalColorChanged( QColor i_color );
	void on_AtmosphereFogReset_clicked( );


	

};

