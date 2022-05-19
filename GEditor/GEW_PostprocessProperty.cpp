#include "ui_GEW_PostprocessProperty.h"
#include "GEW_PostprocessProperty.h"
#include "GEW_Postprocess.h"
#include <GE_Sky.h>
#include <GE_PostProcess.h>
#include <QMessageBox>
#include <GE_Texture.h>
#include "GEW_SelectItemDialog.h"
#include "GEW_Light.h"
#include <QListWidget>

GEW_PostprocessProperty * GEW_PostprocessProperty::m_singleton = NULL;

GEW_PostprocessProperty * GEW_PostprocessProperty::getSingleton()
{
	return m_singleton;
}

GEW_PostprocessProperty::GEW_PostprocessProperty( QWidget *i_parent /*= 0 */ )
: QWidget( i_parent ),
m_ui( new Ui::PostprocessProperty )
{
	m_ui->setupUi( this );
	m_singleton = this;
}

void GEW_PostprocessProperty::updateSkyUIValues()
{
	m_ui->SkyEnable->setChecked( GEW_Postprocess::getSingleton()->m_sky.Enable );
	m_ui->SkySamples->setValue( GEW_Postprocess::getSingleton()->m_sky.Samples );
	m_ui->SkyRadius->setValue( GE_ConvertToMeter( GEW_Postprocess::getSingleton()->m_sky.Radius ) );
	m_ui->SkyIntensity->setValue( GEW_Postprocess::getSingleton()->m_sky.Intensity );
	QColor cl;
	cl.setRedF( GEW_Postprocess::getSingleton()->m_sky.Wavelength.x );
	cl.setGreenF( GEW_Postprocess::getSingleton()->m_sky.Wavelength.y );
	cl.setBlueF( GEW_Postprocess::getSingleton()->m_sky.Wavelength.z );
	m_ui->SkyWavelength->slotSetColor( cl );
	m_ui->SkySharpness->setValue( GEW_Postprocess::getSingleton()->m_sky.Sharpness );
	m_ui->SkyCore->setValue( GEW_Postprocess::getSingleton()->m_sky.Core );
	m_ui->SkyHighlight->setValue( GEW_Postprocess::getSingleton()->m_sky.Highlight );
	m_ui->SkySun->setValue( GEW_Postprocess::getSingleton()->m_sky.Sun );
	m_ui->SkyMap1Enable->setChecked( GEW_Postprocess::getSingleton()->m_sky.Map1Enable );
	m_ui->SkyMap1ScaleZ->setValue( GEW_Postprocess::getSingleton()->m_sky.Map1ScaleZ );
	m_ui->SkyMap1PositionZ->setValue( GEW_Postprocess::getSingleton()->m_sky.Map1PositionZ );
	m_ui->SkyMap1RotationX->setValue( GEW_Postprocess::getSingleton()->m_sky.Map1Rotation.x );
	m_ui->SkyMap1RotationY->setValue( GEW_Postprocess::getSingleton()->m_sky.Map1Rotation.y );
	m_ui->SkyMap1RotationZ->setValue( GEW_Postprocess::getSingleton()->m_sky.Map1Rotation.z );
	m_ui->SkyMap1RotationXAnimate->setChecked( GEW_Postprocess::getSingleton()->m_sky.Map1AnimateX );
	m_ui->SkyMap1RotationYAnimate->setChecked( GEW_Postprocess::getSingleton()->m_sky.Map1AnimateY );
	m_ui->SkyMap1RotationZAnimate->setChecked( GEW_Postprocess::getSingleton()->m_sky.Map1AnimateZ );
	if( GEW_Postprocess::getSingleton()->m_sky.Map1FileName[ 0 ] )
		m_ui->SkyMap1Picture->loadFromFile( GEW_Postprocess::getSingleton()->m_sky.Map1FileName );
	else
		m_ui->SkyMap1Picture->slotClear();
	m_ui->SkyMap2Enable->setChecked( GEW_Postprocess::getSingleton()->m_sky.Map2Enable );
	m_ui->SkyMap2ScaleZ->setValue( GEW_Postprocess::getSingleton()->m_sky.Map2ScaleZ );
	m_ui->SkyMap2PositionZ->setValue( GEW_Postprocess::getSingleton()->m_sky.Map2PositionZ );
	m_ui->SkyMap2RotationX->setValue( GEW_Postprocess::getSingleton()->m_sky.Map2Rotation.x );
	m_ui->SkyMap2RotationY->setValue( GEW_Postprocess::getSingleton()->m_sky.Map2Rotation.y );
	m_ui->SkyMap2RotationZ->setValue( GEW_Postprocess::getSingleton()->m_sky.Map2Rotation.z );
	m_ui->SkyMap2RotationXAnimate->setChecked( GEW_Postprocess::getSingleton()->m_sky.Map2AnimateX );
	m_ui->SkyMap2RotationYAnimate->setChecked( GEW_Postprocess::getSingleton()->m_sky.Map2AnimateY );
	m_ui->SkyMap2RotationZAnimate->setChecked( GEW_Postprocess::getSingleton()->m_sky.Map2AnimateZ );
	if( GEW_Postprocess::getSingleton()->m_sky.Map2FileName[ 0 ] )
		m_ui->SkyMap2Picture->loadFromFile( GEW_Postprocess::getSingleton()->m_sky.Map2FileName );
	else
		m_ui->SkyMap2Picture->slotClear();

	if ( GEW_Postprocess::getSingleton()->m_skyLight )
		m_ui->SkyLight->setText( GEW_Postprocess::getSingleton()->m_skyLight->text( 0 ) );
	else
		m_ui->SkyLight->clear();
}

GEW_PostprocessProperty::~GEW_PostprocessProperty()
{
	delete m_ui;
}

void GEW_PostprocessProperty::on_SkyEnable_toggled( bool i_checked )
{
	GEW_Postprocess::getSingleton()->m_sky.Enable = i_checked;
}

void GEW_PostprocessProperty::on_SkySamples_valueChanged( int i_value )
{
	GE_Sky::getSingleton( ).getParameters( )->Samples = GEW_Postprocess::getSingleton( )->m_sky.Samples = i_value;
}

void GEW_PostprocessProperty::on_SkyRadius_valueChanged( double i_value )
{
	GE_Sky::getSingleton( ).getParameters( )->Radius = GEW_Postprocess::getSingleton( )->m_sky.Radius = ( float )GE_ConvertToUnit( i_value );	
}

void GEW_PostprocessProperty::on_SkyIntensity_valueChanged( double i_value )
{
	GE_Sky::getSingleton( ).getParameters( )->Intensity = GEW_Postprocess::getSingleton( )->m_sky.Intensity = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkyWavelength_signalColorChanged( QColor i_color )
{
	GE_Sky::getSingleton( ).getParameters()->Wavelength.x = GEW_Postprocess::getSingleton( )->m_sky.Wavelength.x = ( float )i_color.redF( );
	GE_Sky::getSingleton( ).getParameters()->Wavelength.y = GEW_Postprocess::getSingleton( )->m_sky.Wavelength.y = ( float )i_color.greenF( );
	GE_Sky::getSingleton( ).getParameters()->Wavelength.z = GEW_Postprocess::getSingleton( )->m_sky.Wavelength.z = ( float )i_color.blueF( );	
}

void GEW_PostprocessProperty::on_SkyReset_clicked()
{
	if ( QMessageBox::question( this, tr( "Reset" ), tr( "Are you sure?" ) ) == QMessageBox::No )
		return;
	GE_Sky::getSingleton( ).getParameters( )->LightDir = GE_Vec3( 0.0f, 0.0f, 0.0f );
	GEW_Postprocess::getSingleton()->m_sky.reset();
	updateSkyUIValues();
}

void GEW_PostprocessProperty::on_SkyLightSelect_clicked()
{
	GEW_SelectItemDialog *dialog = new GEW_SelectItemDialog( this );
	dialog->setWindowTitle( "Choose sky sunlight" );
	foreach( GEW_Light *light, GEW_Light::m_list )
	{
		dialog->addItem( light->icon( 0 ), light->text( 0 ), light->getId() );
	}
	if ( dialog->exec( ) == QDialog::Accepted && dialog->getCurrentItem( ) )
	{
		m_ui->SkyLight->setText( dialog->getCurrentItem()->text() );
		GEW_Postprocess::getSingleton()->m_sky.LightID = dialog->getCurrentItem()->data( Qt::UserRole ).toUInt();
		GEW_Postprocess::getSingleton( )->m_skyLight = GEW_Light::find( GEW_Postprocess::getSingleton( )->m_sky.LightID );
	}
	delete dialog;
}

void GEW_PostprocessProperty::on_SkySharpness_valueChanged( double i_value )
{
	GE_Sky::getSingleton( ).getParameters( )->Sharpness = GEW_Postprocess::getSingleton( )->m_sky.Sharpness = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkyCore_valueChanged( double i_value )
{
	GE_Sky::getSingleton( ).getParameters( )->Core = GEW_Postprocess::getSingleton( )->m_sky.Core = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkyHighlight_valueChanged( double i_value )
{
	GE_Sky::getSingleton( ).getParameters( )->Highlight = GEW_Postprocess::getSingleton( )->m_sky.Highlight = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkySun_valueChanged( double i_value )
{
	GE_Sky::getSingleton( ).getParameters( )->Sun = GEW_Postprocess::getSingleton( )->m_sky.Sun = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkyMap1Enable_toggled( bool i_checked )
{
	GE_Sky::getSingleton( ).getParameters( )->Map1Enable = GEW_Postprocess::getSingleton( )->m_sky.Map1Enable = i_checked;
}

void GEW_PostprocessProperty::on_SkyMap1ScaleZ_valueChanged( double i_value )
{
	GE_Sky::getSingleton( ).getParameters( )->Map1ScaleZ = GEW_Postprocess::getSingleton( )->m_sky.Map1ScaleZ = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkyMap1PositionZ_valueChanged( double i_value )
{
	GE_Sky::getSingleton( ).getParameters( )->Map1PositionZ = GEW_Postprocess::getSingleton( )->m_sky.Map1PositionZ = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkyMap1RotationX_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_sky.Map1Rotation.x = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkyMap1RotationY_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_sky.Map1Rotation.y = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkyMap1RotationZ_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_sky.Map1Rotation.z = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkyMap1RotationXAnimate_toggled( bool i_checked )
{
	GEW_Postprocess::getSingleton()->m_sky.Map1AnimateX = i_checked;
}

void GEW_PostprocessProperty::on_SkyMap1RotationYAnimate_toggled( bool i_checked )
{
	GEW_Postprocess::getSingleton()->m_sky.Map1AnimateY = i_checked;
}

void GEW_PostprocessProperty::on_SkyMap1RotationZAnimate_toggled( bool i_checked )
{
	GEW_Postprocess::getSingleton()->m_sky.Map1AnimateZ = i_checked;
}

void GEW_PostprocessProperty::on_SkyMap1Picture_signalImageChanged( QString i_fileName )
{
	if ( i_fileName.isEmpty() )
	{
		GEW_Postprocess::getSingleton()->m_sky.Map1FileName[ 0 ] = 0;
		GE_Sky::getSingleton().deleteMap1();
	}

	strcpy_s( GEW_Postprocess::getSingleton()->m_sky.Map1FileName, i_fileName.toStdString().c_str() );
	GE_Sky::getSingleton().setMap1( GEW_Postprocess::getSingleton()->m_sky.Map1FileName );
}

void GEW_PostprocessProperty::on_SkyMap2Enable_toggled( bool i_checked )
{
	GE_Sky::getSingleton( ).getParameters( )->Map2Enable = GEW_Postprocess::getSingleton( )->m_sky.Map2Enable = i_checked;
}

void GEW_PostprocessProperty::on_SkyMap2ScaleZ_valueChanged( double i_value )
{
	GE_Sky::getSingleton( ).getParameters( )->Map2ScaleZ = GEW_Postprocess::getSingleton( )->m_sky.Map2ScaleZ = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkyMap2PositionZ_valueChanged( double i_value )
{
	GE_Sky::getSingleton( ).getParameters( )->Map2PositionZ = GEW_Postprocess::getSingleton( )->m_sky.Map2PositionZ = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkyMap2RotationX_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_sky.Map2Rotation.x = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkyMap2RotationY_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_sky.Map2Rotation.y = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkyMap2RotationZ_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_sky.Map2Rotation.z = ( float )i_value;
}

void GEW_PostprocessProperty::on_SkyMap2RotationXAnimate_toggled( bool i_checked )
{
	GEW_Postprocess::getSingleton()->m_sky.Map2AnimateX = i_checked;
}

void GEW_PostprocessProperty::on_SkyMap2RotationYAnimate_toggled( bool i_checked )
{
	GEW_Postprocess::getSingleton()->m_sky.Map2AnimateY = i_checked;
}

void GEW_PostprocessProperty::on_SkyMap2RotationZAnimate_toggled( bool i_checked )
{
	GEW_Postprocess::getSingleton()->m_sky.Map2AnimateZ = i_checked;
}

void GEW_PostprocessProperty::on_SkyMap2Picture_signalImageChanged( QString i_fileName )
{
	if ( i_fileName.isEmpty() )
	{
		GEW_Postprocess::getSingleton()->m_sky.Map2FileName[ 0 ] = 0;
		GE_Sky::getSingleton().deleteMap2();
	}

	strcpy_s( GEW_Postprocess::getSingleton()->m_sky.Map2FileName, i_fileName.toStdString().c_str() );
	GE_Sky::getSingleton().setMap2( GEW_Postprocess::getSingleton()->m_sky.Map2FileName );
}

//LightScatter

void GEW_PostprocessProperty::updateLightScatterUIValues()
{
	m_ui->LightScatterEnable->blockSignals( true );
	m_ui->LightScatterEnable->setChecked( GEW_Postprocess::getSingleton()->m_lightScatter.Enable );
	m_ui->LightScatterEnable->blockSignals( false );

	m_ui->LightScatterLuminance->blockSignals( true );
	m_ui->LightScatterLuminance->setValue( GEW_Postprocess::getSingleton()->m_lightScatter.Luminance );
	m_ui->LightScatterLuminance->blockSignals( false );

	m_ui->LightScatterCutoff->blockSignals( true );
	m_ui->LightScatterCutoff->setValue( GEW_Postprocess::getSingleton()->m_lightScatter.Cutoff );
	m_ui->LightScatterCutoff->blockSignals( false );

	m_ui->LightScatterExp->blockSignals( true );
	m_ui->LightScatterExp->setValue( GEW_Postprocess::getSingleton()->m_lightScatter.Exp );
	m_ui->LightScatterExp->blockSignals( false );

	m_ui->LightScatterDensity->blockSignals( true );
	m_ui->LightScatterDensity->setValue( GEW_Postprocess::getSingleton()->m_lightScatter.Density );
	m_ui->LightScatterDensity->blockSignals( false );

	m_ui->LightScatterWeight->blockSignals( true );
	m_ui->LightScatterWeight->setValue( GEW_Postprocess::getSingleton()->m_lightScatter.Weight );
	m_ui->LightScatterWeight->blockSignals( false );

	m_ui->LightScatterDecay->blockSignals(true);
	m_ui->LightScatterDecay->setValue(GEW_Postprocess::getSingleton()->m_lightScatter.Decay);
	m_ui->LightScatterDecay->blockSignals(false);
}


void GEW_PostprocessProperty::on_LightScatterEnable_toggled( bool i_checked )
{
	GEW_Postprocess::getSingleton()->m_lightScatter.Enable = i_checked;
}

void GEW_PostprocessProperty::on_LightScatterLuminance_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_lightScatter.Luminance = ( float )i_value;
}

void GEW_PostprocessProperty::on_LightScatterCutoff_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_lightScatter.Cutoff = ( float )i_value;
}

void GEW_PostprocessProperty::on_LightScatterExp_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_lightScatter.Exp = ( float )i_value;
}

void GEW_PostprocessProperty::on_LightScatterDensity_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_lightScatter.Density = ( float )i_value;
}

void GEW_PostprocessProperty::on_LightScatterWeight_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_lightScatter.Weight = ( float )i_value;
}

void GEW_PostprocessProperty::on_LightScatterDecay_valueChanged(double i_value)
{
	GEW_Postprocess::getSingleton()->m_lightScatter.Decay = (float)i_value;
}

void GEW_PostprocessProperty::on_LightScatterReset_clicked()
{
	if ( QMessageBox::question( this, tr( "Reset" ), tr( "Are you sure?" ) ) == QMessageBox::No )
		return;
	GEW_Postprocess::getSingleton()->m_lightScatter.reset();
	updateLightScatterUIValues();
}

//Motion Blur

void GEW_PostprocessProperty::updateMotionBlurUIValues()
{
	m_ui->MotionBlurEnable->blockSignals( true );
	m_ui->MotionBlurEnable->setChecked( GEW_Postprocess::getSingleton()->m_motionBlur.Enable );
	m_ui->MotionBlurEnable->blockSignals( false );

	m_ui->MotionBlurDamp->blockSignals( true );
	m_ui->MotionBlurDamp->setValue( GEW_Postprocess::getSingleton()->m_motionBlur.Damp );
	m_ui->MotionBlurDamp->blockSignals( false );

	m_ui->MotionBlurMax->blockSignals( true );
	m_ui->MotionBlurMax->setValue( GEW_Postprocess::getSingleton()->m_motionBlur.Max );
	m_ui->MotionBlurMax->blockSignals( false );

}

void GEW_PostprocessProperty::on_MotionBlurEnable_toggled( bool i_checked )
{
	GEW_Postprocess::getSingleton()->m_motionBlur.Enable = i_checked;
}

void GEW_PostprocessProperty::on_MotionBlurDamp_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_motionBlur.Damp = ( float )i_value;
}

void GEW_PostprocessProperty::on_MotionBlurMax_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_motionBlur.Max = ( float )i_value;
}

void GEW_PostprocessProperty::on_MotionBlurReset_clicked()
{
	if ( QMessageBox::question( this, tr( "Reset" ), tr( "Are you sure?" ) ) == QMessageBox::No )
		return;
	GEW_Postprocess::getSingleton()->m_motionBlur.reset();
	updateMotionBlurUIValues();
}

//HDR

void GEW_PostprocessProperty::updateHDRUIValues()
{
	m_ui->HDREnable->setChecked( GEW_Postprocess::getSingleton()->m_hDR.Enable );
	GE_PostProcess::getSingleton( ).setEnableHDR( GEW_Postprocess::getSingleton( )->m_hDR.Enable );
	m_ui->HDRWhiteCutoff->setValue( GEW_Postprocess::getSingleton()->m_hDR.WhiteCutoff );
	GE_PostProcess::getSingleton( ).setHDRWhiteCutoff( GEW_Postprocess::getSingleton( )->m_hDR.WhiteCutoff );
	m_ui->HDRLuminanceMax->setValue( GEW_Postprocess::getSingleton()->m_hDR.LuminanceMax );
	GE_PostProcess::getSingleton( ).setHDRLuminanceMax( GEW_Postprocess::getSingleton( )->m_hDR.LuminanceMax );
	m_ui->HDRLuminanceMin->setValue( GEW_Postprocess::getSingleton()->m_hDR.LuminanceMin );
	GE_PostProcess::getSingleton( ).setHDRLuminanceMin( GEW_Postprocess::getSingleton( )->m_hDR.LuminanceMin );
	m_ui->HDRGamma->setValue( GEW_Postprocess::getSingleton()->m_hDR.Gamma );
	GE_PostProcess::getSingleton( ).setHDRGamma( GEW_Postprocess::getSingleton( )->m_hDR.Gamma );
	m_ui->HDRAdaptationRate->setValue( GEW_Postprocess::getSingleton()->m_hDR.AdaptationRate );
	GE_PostProcess::getSingleton( ).setHDRAdaptationRate( GEW_Postprocess::getSingleton( )->m_hDR.AdaptationRate );
	if( GEW_Postprocess::getSingleton()->m_hDR.GradientMap[ 0 ] )
		m_ui->HDRGradientMap->slotLoad( GEW_Postprocess::getSingleton()->m_hDR.GradientMap );
	else
		m_ui->HDRGradientMap->slotClear();
}

void GEW_PostprocessProperty::on_HDREnable_toggled( bool i_checked )
{
	GE_PostProcess::getSingleton().setEnableHDR( GEW_Postprocess::getSingleton()->m_hDR.Enable = i_checked );
}

void GEW_PostprocessProperty::on_HDRWhiteCutoff_valueChanged( double i_value )
{
	GE_PostProcess::getSingleton( ).setHDRWhiteCutoff( GEW_Postprocess::getSingleton( )->m_hDR.WhiteCutoff = ( float )i_value );
}

void GEW_PostprocessProperty::on_HDRLuminanceMax_valueChanged( double i_value )
{
	GE_PostProcess::getSingleton( ).setHDRLuminanceMax( GEW_Postprocess::getSingleton( )->m_hDR.LuminanceMax = ( float )i_value );
}

void GEW_PostprocessProperty::on_HDRLuminanceMin_valueChanged( double i_value )
{
	GE_PostProcess::getSingleton( ).setHDRLuminanceMin( GEW_Postprocess::getSingleton( )->m_hDR.LuminanceMin = ( float )i_value );
}

void GEW_PostprocessProperty::on_HDRGamma_valueChanged( double i_value )
{
	GE_PostProcess::getSingleton( ).setHDRGamma( GEW_Postprocess::getSingleton( )->m_hDR.Gamma = ( float )i_value );
}

void GEW_PostprocessProperty::on_HDRAdaptationRate_valueChanged( double i_value )
{
	GE_PostProcess::getSingleton( ).setHDRAdaptationRate( GEW_Postprocess::getSingleton( )->m_hDR.AdaptationRate = ( float )i_value );
}

void GEW_PostprocessProperty::on_HDRGradientMap_signalImageChanged( QString i_fileName )
{
	if ( i_fileName.isEmpty() )
	{
		GEW_Postprocess::getSingleton()->m_hDR.GradientMap[ 0 ] = 0;
		GE_PostProcess::getSingleton().deleteHDRGradientMap();
	}

	strcpy_s( GEW_Postprocess::getSingleton()->m_hDR.GradientMap, i_fileName.toStdString().c_str() );
	GE_PostProcess::getSingleton().loadHDRGradientMap( GEW_Postprocess::getSingleton()->m_hDR.GradientMap );
}

void GEW_PostprocessProperty::on_HDRReset_clicked()
{
	if ( QMessageBox::question( this, tr( "Reset" ), tr( "Are you sure?" ) ) == QMessageBox::No )
		return;
	GEW_Postprocess::getSingleton()->m_hDR.reset();
	updateHDRUIValues();
}

//AO

void GEW_PostprocessProperty::updateAOUIValues()
{
	m_ui->AOEnable->blockSignals( true );
	m_ui->AOEnable->setChecked( GEW_Postprocess::getSingleton()->m_aO.Enable );
	m_ui->AOEnable->blockSignals( false );

	m_ui->AORadius->blockSignals( true );
	m_ui->AORadius->setValue( GE_ConvertToMeter( GEW_Postprocess::getSingleton( )->m_aO.Radius ) );
	m_ui->AORadius->blockSignals( false );

	m_ui->AODepth->blockSignals( true );
	m_ui->AODepth->setValue( GEW_Postprocess::getSingleton()->m_aO.Depth );
	m_ui->AODepth->blockSignals( false );

	m_ui->AOIntensity->blockSignals( true );
	m_ui->AOIntensity->setValue( GEW_Postprocess::getSingleton()->m_aO.Intensity );
	m_ui->AOIntensity->blockSignals( false );

	m_ui->AODistanceStart->blockSignals( true );
	m_ui->AODistanceStart->setValue( GE_ConvertToMeter( GEW_Postprocess::getSingleton()->m_aO.DistanceStart ) );
	m_ui->AODistanceStart->blockSignals( false );

	m_ui->AODistanceEnd->blockSignals( true );
	m_ui->AODistanceEnd->setValue( GE_ConvertToMeter( GEW_Postprocess::getSingleton()->m_aO.DistanceEnd ) );
	m_ui->AODistanceEnd->blockSignals( false );

	m_ui->AONormal->blockSignals( true );
	m_ui->AONormal->setValue( GEW_Postprocess::getSingleton()->m_aO.Normal );
	m_ui->AONormal->blockSignals( false );
}

void GEW_PostprocessProperty::on_AOEnable_toggled( bool i_checked )
{
	GEW_Postprocess::getSingleton()->m_aO.Enable = i_checked;
}

void GEW_PostprocessProperty::on_AORadius_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton( )->m_aO.Radius = GE_ConvertToUnit( ( float )i_value );
}

void GEW_PostprocessProperty::on_AODepth_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_aO.Depth = ( float )i_value;
}

void GEW_PostprocessProperty::on_AODistanceStart_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_aO.DistanceStart = GE_ConvertToUnit( ( float )i_value );
}

void GEW_PostprocessProperty::on_AODistanceEnd_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_aO.DistanceEnd = GE_ConvertToUnit( ( float )i_value );
}

void GEW_PostprocessProperty::on_AOIntensity_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_aO.Intensity = ( float )i_value;
}

void GEW_PostprocessProperty::on_AONormal_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_aO.Normal = ( float )i_value;
}

void GEW_PostprocessProperty::on_AOReset_clicked()
{
	if ( QMessageBox::question( this, tr( "Reset" ), tr( "Are you sure?" ) ) == QMessageBox::No )
		return;
	GEW_Postprocess::getSingleton()->m_aO.reset();
	updateAOUIValues();
}

//Lens dirt

void GEW_PostprocessProperty::updateLensDirtUIValues()
{
	m_ui->LensDirtEnable->blockSignals( true );
	m_ui->LensDirtEnable->setChecked( GEW_Postprocess::getSingleton()->m_lensDirt.Enable );
	m_ui->LensDirtEnable->blockSignals( false );

	m_ui->LensDirtLuminance->blockSignals( true );
	m_ui->LensDirtLuminance->setValue( GEW_Postprocess::getSingleton()->m_lensDirt.Luminance );
	m_ui->LensDirtLuminance->blockSignals( false );

	m_ui->LensDirtCutoff->blockSignals( true );
	m_ui->LensDirtCutoff->setValue( GEW_Postprocess::getSingleton()->m_lensDirt.Cutoff );
	m_ui->LensDirtCutoff->blockSignals( false );

	m_ui->LensDirtExp->blockSignals( true );
	m_ui->LensDirtExp->setValue( GEW_Postprocess::getSingleton( )->m_lensDirt.Exp );
	m_ui->LensDirtExp->blockSignals( false );

	m_ui->LensDirtDirtExp->blockSignals( true );
	m_ui->LensDirtDirtExp->setValue( GEW_Postprocess::getSingleton( )->m_lensDirt.DirtExp );
	m_ui->LensDirtDirtExp->blockSignals( false );

	m_ui->LensDirtCoreExp->blockSignals( true );
	m_ui->LensDirtCoreExp->setValue( GEW_Postprocess::getSingleton( )->m_lensDirt.CoreExp );
	m_ui->LensDirtCoreExp->blockSignals( false );

	if( GEW_Postprocess::getSingleton()->m_lensDirt.MapFileName[ 0 ] )
		m_ui->LensDirtMap->loadFromFile( GEW_Postprocess::getSingleton()->m_lensDirt.MapFileName );
	else
		m_ui->LensDirtMap->slotClear();
}

void GEW_PostprocessProperty::on_LensDirtEnable_toggled( bool i_checked )
{
	GEW_Postprocess::getSingleton()->m_lensDirt.Enable = i_checked;
}

void GEW_PostprocessProperty::on_LensDirtLuminance_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_lensDirt.Luminance = ( float )i_value;
}

void GEW_PostprocessProperty::on_LensDirtCutoff_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_lensDirt.Cutoff = ( float )i_value;
}

void GEW_PostprocessProperty::on_LensDirtExp_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton( )->m_lensDirt.Exp = ( float )i_value;
}

void GEW_PostprocessProperty::on_LensDirtDirtExp_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton( )->m_lensDirt.DirtExp = ( float )i_value;
}

void GEW_PostprocessProperty::on_LensDirtCoreExp_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton( )->m_lensDirt.CoreExp = ( float )i_value;
}

void GEW_PostprocessProperty::on_LensDirtMap_signalImageChanged( QString i_fileName )
{
	if ( GEW_Postprocess::getSingleton()->m_dirtMap )
		GE_TextureManager::getSingleton().remove( &GEW_Postprocess::getSingleton()->m_dirtMap );

	if ( i_fileName.isEmpty() )
	{
		GEW_Postprocess::getSingleton()->m_lensDirt.MapFileName[ 0 ] = 0;
	}
	else
	{
		strcpy_s( GEW_Postprocess::getSingleton()->m_lensDirt.MapFileName, i_fileName.toStdString().c_str() );
		GEW_Postprocess::getSingleton()->m_dirtMap = GE_TextureManager::getSingleton().createTexture( GEW_Postprocess::getSingleton()->m_lensDirt.MapFileName );
	}
}

void GEW_PostprocessProperty::on_LensDirtReset_clicked()
{
	if ( QMessageBox::question( this, tr( "Reset" ), tr( "Are you sure?" ) ) == QMessageBox::No )
		return;
	GEW_Postprocess::getSingleton()->m_lensDirt.reset();
	updateLensDirtUIValues();
}

//Glow

void GEW_PostprocessProperty::updateGlowUIValues()
{
	m_ui->GlowEnable->blockSignals( true );
	m_ui->GlowEnable->setChecked( GEW_Postprocess::getSingleton()->m_glow.Enable );
	m_ui->GlowEnable->blockSignals( false );

	m_ui->GlowVScale->blockSignals( true );
	m_ui->GlowVScale->setValue( GEW_Postprocess::getSingleton()->m_glow.VScale );
	m_ui->GlowVScale->blockSignals( false );

	m_ui->GlowHScale->blockSignals( true );
	m_ui->GlowHScale->setValue( GEW_Postprocess::getSingleton()->m_glow.HScale );
	m_ui->GlowHScale->blockSignals( false );

	m_ui->GlowLuminance->blockSignals( true );
	m_ui->GlowLuminance->setValue( GEW_Postprocess::getSingleton()->m_glow.Luminance );
	m_ui->GlowLuminance->blockSignals( false );

	m_ui->GlowCutoff->blockSignals( true );
	m_ui->GlowCutoff->setValue( GEW_Postprocess::getSingleton()->m_glow.Cutoff );
	m_ui->GlowCutoff->blockSignals( false );
}

void GEW_PostprocessProperty::on_GlowEnable_toggled( bool i_checked )
{
	GEW_Postprocess::getSingleton()->m_glow.Enable = i_checked;
}

void GEW_PostprocessProperty::on_GlowVScale_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_glow.VScale = ( float )i_value;
}

void GEW_PostprocessProperty::on_GlowHScale_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_glow.HScale = ( float )i_value;
}

void GEW_PostprocessProperty::on_GlowLuminance_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_glow.Luminance = ( float )i_value;
}

void GEW_PostprocessProperty::on_GlowCutoff_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_glow.Cutoff = ( float )i_value;
}

void GEW_PostprocessProperty::on_GlowReset_clicked()
{
	if ( QMessageBox::question( this, tr( "Reset" ), tr( "Are you sure?" ) ) == QMessageBox::No )
		return;
	GEW_Postprocess::getSingleton()->m_glow.reset();
	updateGlowUIValues();
}

//Glow Mapped

void GEW_PostprocessProperty::updateGlowMappedUIValues()
{
	m_ui->GlowMappedEnable->blockSignals( true );
	m_ui->GlowMappedEnable->setChecked( GEW_Postprocess::getSingleton()->m_glowMapped.Enable );
	m_ui->GlowMappedEnable->blockSignals( false );

	m_ui->GlowMappedVScale->blockSignals( true );
	m_ui->GlowMappedVScale->setValue( GEW_Postprocess::getSingleton()->m_glowMapped.VScale );
	m_ui->GlowMappedVScale->blockSignals( false );

	m_ui->GlowMappedHScale->blockSignals( true );
	m_ui->GlowMappedHScale->setValue( GEW_Postprocess::getSingleton()->m_glowMapped.HScale );
	m_ui->GlowMappedHScale->blockSignals( false );

	m_ui->GlowMappedLuminance->blockSignals( true );
	m_ui->GlowMappedLuminance->setValue( GEW_Postprocess::getSingleton()->m_glowMapped.Luminance );
	m_ui->GlowMappedLuminance->blockSignals( false );

	m_ui->GlowMappedCutoff->blockSignals( true );
	m_ui->GlowMappedCutoff->setValue( GEW_Postprocess::getSingleton()->m_glowMapped.Cutoff );
	m_ui->GlowMappedCutoff->blockSignals( false );
}

void GEW_PostprocessProperty::on_GlowMappedEnable_toggled( bool i_checked )
{
	GEW_Postprocess::getSingleton()->m_glowMapped.Enable = i_checked;
}

void GEW_PostprocessProperty::on_GlowMappedVScale_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_glowMapped.VScale = ( float )i_value;
}

void GEW_PostprocessProperty::on_GlowMappedHScale_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_glowMapped.HScale = ( float )i_value;
}

void GEW_PostprocessProperty::on_GlowMappedLuminance_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_glowMapped.Luminance = ( float )i_value;
}

void GEW_PostprocessProperty::on_GlowMappedCutoff_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton()->m_glowMapped.Cutoff = ( float )i_value;
}

void GEW_PostprocessProperty::on_GlowMappedReset_clicked()
{
	if ( QMessageBox::question( this, tr( "Reset" ), tr( "Are you sure?" ) ) == QMessageBox::No )
		return;
	GEW_Postprocess::getSingleton()->m_glowMapped.reset();
	updateGlowMappedUIValues();
}

void GEW_PostprocessProperty::init()
{
	GEW_Postprocess::getSingleton()->m_sky.reset();
	GEW_Postprocess::getSingleton()->m_lightScatter.reset();
	GEW_Postprocess::getSingleton()->m_motionBlur.reset();
	GEW_Postprocess::getSingleton()->m_hDR.reset();
	GEW_Postprocess::getSingleton()->m_aO.reset();
	GEW_Postprocess::getSingleton()->m_lensDirt.reset();
	GEW_Postprocess::getSingleton()->m_glow.reset();
	GEW_Postprocess::getSingleton()->m_glowMapped.reset();
	GEW_Postprocess::getSingleton( )->m_atmosphereFog.reset( );
	updateSkyUIValues();
	updateLightScatterUIValues();
	updateMotionBlurUIValues();
	updateHDRUIValues();
	updateAOUIValues();
	updateLensDirtUIValues();
	updateGlowUIValues();
	updateGlowMappedUIValues();
	updateAtmosphereFogUIValues( );
}

void GEW_PostprocessProperty::updateUi()
{
	updateSkyUIValues();
	updateLightScatterUIValues();
	updateMotionBlurUIValues();
	updateHDRUIValues();
	updateAOUIValues();
	updateLensDirtUIValues();
	updateGlowUIValues();
	updateGlowMappedUIValues();
	updateAtmosphereFogUIValues();
}

void GEW_PostprocessProperty::updateAtmosphereFogUIValues()
{
	QColor cl;
	cl.setRedF( GEW_Postprocess::getSingleton()->m_atmosphereFog.Color.x );
	cl.setGreenF( GEW_Postprocess::getSingleton()->m_atmosphereFog.Color.y );
	cl.setBlueF( GEW_Postprocess::getSingleton()->m_atmosphereFog.Color.z );
	m_ui->AtmosphereFogColor->slotSetColor( cl );

	m_ui->AtmosphereFogEnable->blockSignals( true );
	m_ui->AtmosphereFogEnable->setChecked( GEW_Postprocess::getSingleton( )->m_atmosphereFog.Enable );
	m_ui->AtmosphereFogEnable->blockSignals( false );

	m_ui->AtmosphereFogIntensity->blockSignals( true );
	m_ui->AtmosphereFogIntensity->setValue( GEW_Postprocess::getSingleton( )->m_atmosphereFog.Intensity );
	m_ui->AtmosphereFogIntensity->blockSignals( false );

	m_ui->AtmosphereFogDepthPosition->blockSignals( true );
	m_ui->AtmosphereFogDepthPosition->setValue( GE_ConvertToMeter(GEW_Postprocess::getSingleton( )->m_atmosphereFog.Depth_PositionFieldFalloffHardness.x) );
	m_ui->AtmosphereFogDepthPosition->blockSignals( false );

	m_ui->AtmosphereFogDepthField->blockSignals( true );
	m_ui->AtmosphereFogDepthField->setValue( GE_ConvertToMeter(GEW_Postprocess::getSingleton()->m_atmosphereFog.Depth_PositionFieldFalloffHardness.y) );
	m_ui->AtmosphereFogDepthField->blockSignals( false );

	m_ui->AtmosphereFogDepthFalloff->blockSignals( true );
	m_ui->AtmosphereFogDepthFalloff->setValue( GEW_Postprocess::getSingleton( )->m_atmosphereFog.Depth_PositionFieldFalloffHardness.z );
	m_ui->AtmosphereFogDepthFalloff->blockSignals( false );

	m_ui->AtmosphereFogDepthHardness->blockSignals( true );
	m_ui->AtmosphereFogDepthHardness->setValue( GEW_Postprocess::getSingleton( )->m_atmosphereFog.Depth_PositionFieldFalloffHardness.w );
	m_ui->AtmosphereFogDepthHardness->blockSignals( false );

	m_ui->AtmosphereFogHeightPosition->blockSignals( true );
	m_ui->AtmosphereFogHeightPosition->setValue( GE_ConvertToMeter( GEW_Postprocess::getSingleton( )->m_atmosphereFog.Height_PositionFieldFalloffHardness.x) );
	m_ui->AtmosphereFogHeightPosition->blockSignals( false );

	m_ui->AtmosphereFogHeightField->blockSignals( true );
	m_ui->AtmosphereFogHeightField->setValue( GE_ConvertToMeter( GEW_Postprocess::getSingleton( )->m_atmosphereFog.Height_PositionFieldFalloffHardness.y) );
	m_ui->AtmosphereFogHeightField->blockSignals( false );

	m_ui->AtmosphereFogHeightFalloff->blockSignals( true );
	m_ui->AtmosphereFogHeightFalloff->setValue( GEW_Postprocess::getSingleton()->m_atmosphereFog.Height_PositionFieldFalloffHardness.z );
	m_ui->AtmosphereFogHeightFalloff->blockSignals( false );

	m_ui->AtmosphereFogHeightHardness->blockSignals( true );
	m_ui->AtmosphereFogHeightHardness->setValue( GEW_Postprocess::getSingleton()->m_atmosphereFog.Height_PositionFieldFalloffHardness.w );
	m_ui->AtmosphereFogHeightHardness->blockSignals( false );
}

void GEW_PostprocessProperty::on_AtmosphereFogEnable_toggled( bool i_checked )
{
	GEW_Postprocess::getSingleton()->m_atmosphereFog.Enable = i_checked;
}

void GEW_PostprocessProperty::on_AtmosphereFogIntensity_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton( )->m_atmosphereFog.Intensity = (float)i_value;
}

void GEW_PostprocessProperty::on_AtmosphereFogDepthPosition_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton( )->m_atmosphereFog.Depth_PositionFieldFalloffHardness.x = GE_ConvertToUnit( ( float )i_value);
}

void GEW_PostprocessProperty::on_AtmosphereFogDepthField_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton( )->m_atmosphereFog.Depth_PositionFieldFalloffHardness.y = GE_ConvertToUnit( ( float )i_value);
}

void GEW_PostprocessProperty::on_AtmosphereFogDepthFalloff_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton( )->m_atmosphereFog.Depth_PositionFieldFalloffHardness.z =  (float )i_value;
}

void GEW_PostprocessProperty::on_AtmosphereFogDepthHardness_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton( )->m_atmosphereFog.Depth_PositionFieldFalloffHardness.w = ( float )i_value;
}

void GEW_PostprocessProperty::on_AtmosphereFogHeightPosition_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton( )->m_atmosphereFog.Height_PositionFieldFalloffHardness.x = GE_ConvertToUnit( ( float )i_value );
}

void GEW_PostprocessProperty::on_AtmosphereFogHeightField_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton( )->m_atmosphereFog.Height_PositionFieldFalloffHardness.y = GE_ConvertToUnit( ( float )i_value );
}

void GEW_PostprocessProperty::on_AtmosphereFogHeightFalloff_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton( )->m_atmosphereFog.Height_PositionFieldFalloffHardness.z = ( float )i_value ;
}

void GEW_PostprocessProperty::on_AtmosphereFogHeightHardness_valueChanged( double i_value )
{
	GEW_Postprocess::getSingleton( )->m_atmosphereFog.Height_PositionFieldFalloffHardness.w = ( float )i_value;
}

void GEW_PostprocessProperty::on_AtmosphereFogColor_signalColorChanged( QColor i_color )
{
	GEW_Postprocess::getSingleton( )->m_atmosphereFog.Color.x =( float )i_color.redF( );
	GEW_Postprocess::getSingleton( )->m_atmosphereFog.Color.y =( float )i_color.greenF( );
	GEW_Postprocess::getSingleton( )->m_atmosphereFog.Color.z =( float )i_color.blueF( );
}

void GEW_PostprocessProperty::on_AtmosphereFogReset_clicked()
{
	if ( QMessageBox::question( this, tr( "Reset" ), tr( "Are you sure?" ) ) == QMessageBox::No )
		return;
	GEW_Postprocess::getSingleton()->m_atmosphereFog.reset();
	updateAtmosphereFogUIValues();
}
