#include "GEW_LightProperty.h"
#include "ui_GEW_LightProperty.h"
#include <GE_Light.h>
#include <GE_Texture.h>
#include "GEW_Light.h"
#include "GEW_Scene.h"
#include "GEW_Camera.h"

GEW_LightProperty::GEW_LightProperty( QWidget *i_parent /*= 0 */ )
: QWidget( i_parent ),
m_ui( new Ui::LightProperty )
{
	m_ui->setupUi( this );
	m_light = NULL;
}

GEW_LightProperty::~GEW_LightProperty()
{
	delete m_ui;
}

GEW_Light * GEW_LightProperty::getLight()
{
	return m_light;
}

void GEW_LightProperty::setLight( GEW_Light *i_light )
{
	m_light = NULL;
	if ( !i_light )
		return;
	
	GE_Light::Parameters *lp = i_light->m_light->getParameters( );

	m_ui->Enable->setChecked( lp->Enable );
	m_ui->Type->setCurrentIndex( i_light->getType() );
	m_ui->Dynamic->setChecked( lp->Dynamic );
	m_ui->Intensity->setValue( ( float )lp->Intensity );

	QColor c;
	c.setRedF( lp->Ambient.x );
	c.setGreenF( lp->Ambient.y );
	c.setBlueF( lp->Ambient.z );
	c.setAlphaF( 1.0f );
	m_ui->Ambient->slotSetColor( c );

	c.setRedF( lp->Diffuse.x );
	c.setGreenF( lp->Diffuse.y );
	c.setBlueF( lp->Diffuse.z );
	m_ui->Diffuse->slotSetColor( c );

	c.setRedF( lp->Specular.x );
	c.setGreenF( lp->Specular.y );
	c.setBlueF( lp->Specular.z );
	m_ui->Specular->slotSetColor( c );

	c.setRedF( i_light->m_probe.BoundColor.x );
	c.setGreenF( i_light->m_probe.BoundColor.y );
	c.setBlueF( i_light->m_probe.BoundColor.z );
	m_ui->BoundColor->slotSetColor( c );

	m_ui->DirectionalHasShadow->setChecked( lp->HasShadow );
	m_ui->SpotHasShadow->setChecked( lp->HasShadow );
	m_ui->PointHasShadow->setChecked( lp->HasShadow );
	m_ui->DirectionalNumberOfCascade->setCurrentIndex( lp->NumberOfCascade - 1 );
	m_ui->DirectionalVisualizeCascade->setChecked( lp->VisualizeCascades );

	m_ui->DirectionalCascade1Offset->setValue( ( float )GE_ConvertToMeter( lp->ShadowOffsetBias[ 0 ].x ) );
	m_ui->DirectionalCascade1Bias->setValue( ( float )lp->ShadowOffsetBias[ 0 ].y );
	m_ui->DirectionalCascade1MapSize->setCurrentText( tr( "%1" ).arg( lp->ShadowMapSize[ 0 ].x ) );
	m_ui->DirectionalCascade1OrthoSize->setValue( ( float )GE_ConvertToMeter( lp->ShadowFrustum[ 0 ].x ) );

	m_ui->DirectionalCascade2Offset->setValue( ( float )GE_ConvertToMeter( lp->ShadowOffsetBias[ 1 ].x ) );
	m_ui->DirectionalCascade2Bias->setValue( ( float )lp->ShadowOffsetBias[ 1 ].y );
	m_ui->DirectionalCascade2MapSize->setCurrentText( tr( "%1" ).arg( lp->ShadowMapSize[ 1 ].x ) );
	m_ui->DirectionalCascade2OrthoSize->setValue( ( float )GE_ConvertToMeter( lp->ShadowFrustum[ 1 ].x ) );

	m_ui->DirectionalCascade3Offset->setValue( ( float )GE_ConvertToMeter( lp->ShadowOffsetBias[ 2 ].x ) );
	m_ui->DirectionalCascade3Bias->setValue( ( float )lp->ShadowOffsetBias[ 2 ].y );
	m_ui->DirectionalCascade3MapSize->setCurrentText( tr( "%1" ).arg( lp->ShadowMapSize[ 2 ].x ) );
	m_ui->DirectionalCascade3OrthoSize->setValue( ( float )GE_ConvertToMeter( lp->ShadowFrustum[ 2 ].x ) );

	m_ui->DirectionalCascade4Offset->setValue( ( float )GE_ConvertToMeter( lp->ShadowOffsetBias[ 3 ].x ) );
	m_ui->DirectionalCascade4Bias->setValue( ( float )lp->ShadowOffsetBias[ 3 ].y );
	m_ui->DirectionalCascade4MapSize->setCurrentText( tr( "%1" ).arg( lp->ShadowMapSize[ 3 ].x ) );
	m_ui->DirectionalCascade4OrthoSize->setValue( ( float )GE_ConvertToMeter( lp->ShadowFrustum[ 3 ].x ) );

	m_ui->DirectionalTargetX->setValue( ( float )GE_ConvertToMeter( lp->Target.x ) );
	m_ui->DirectionalTargetY->setValue( ( float )GE_ConvertToMeter( lp->Target.y ) );
	m_ui->DirectionalTargetZ->setValue( ( float )GE_ConvertToMeter( lp->Target.z ) );
	m_ui->SpotTargetX->setValue( ( float )GE_ConvertToMeter( lp->Target.x ) );
	m_ui->SpotTargetY->setValue( ( float )GE_ConvertToMeter( lp->Target.y ) );
	m_ui->SpotTargetZ->setValue( ( float )GE_ConvertToMeter( lp->Target.z ) );

	m_ui->DirectionalDirectionYaw->setValue( GE_ConvertToDegrees( ( float )lp->Yaw ) );
	m_ui->DirectionalDirectionPitch->setValue( GE_ConvertToDegrees( ( float )lp->Pitch ) );
	m_ui->SpotDirectionYaw->setValue( GE_ConvertToDegrees( ( float )lp->Yaw ) );
	m_ui->SpotDirectionPitch->setValue( GE_ConvertToDegrees( ( float )lp->Pitch ) );

	m_ui->SpotAttenuation->setValue( ( float )lp->Attenuation );
	m_ui->SpotRange->setValue( ( float )GE_ConvertToMeter( lp->Range ) );
	m_ui->SpotSperead->setValue( ( float )GE_ConvertToDegrees( lp->SpotSpread ) );
	m_ui->SpotSharpness->setValue( ( float )GE_ConvertToDegrees( lp->SpotSharpness ) );

	m_ui->SpotOffset->setValue( ( float )GE_ConvertToMeter( lp->ShadowOffsetBias[ 0 ].x ) );
	m_ui->SpotBias->setValue( ( float )lp->ShadowOffsetBias[ 0 ].y );
	m_ui->SpotMapSize->setCurrentText( tr( "%1" ).arg( lp->ShadowMapSize[ 0 ].x ) );
	m_ui->PointMapSize->setCurrentText( tr( "%1" ).arg( lp->ShadowMapSize[ 0 ].x ) );

	if ( i_light->m_light->getFilterTexture( ) )
	{
		m_ui->SpotMap->slotLoad( i_light->m_light->getFilterTexture( )->getTextureName( ) );
		m_ui->PointCubeMap->slotLoad( i_light->m_light->getFilterTexture( )->getTextureName( ) );
	}
	else
	{
		m_ui->SpotMap->slotClear();
		m_ui->PointCubeMap->slotClear( );
	}
	
	m_ui->PointAttenuation->setValue( ( float )lp->Attenuation );
	m_ui->PointRange->setValue( ( float )GE_ConvertToMeter( lp->Range ) );
	
	GE_Vec3 rot;
	lp->RotateCubeFilter.toYPR( &rot );
	m_ui->PointYaw->setValue( GE_ConvertToDegrees( rot.x ) );
	m_ui->PointPitch->setValue( GE_ConvertToDegrees( rot.y ) );
	m_ui->PointRoll->setValue( GE_ConvertToDegrees( rot.z ) );

	m_ui->PointOffset->setValue( ( float )GE_ConvertToMeter( lp->ShadowOffsetBias[ 0 ].x ) );
	m_ui->PointBias->setValue( ( float )lp->ShadowOffsetBias[ 0 ].y );

	m_ui->SpotOffDistance->setValue( GE_ConvertToMeter( i_light->m_offDistance ) );
	m_ui->PointOffDistance->setValue( GE_ConvertToMeter( i_light->m_offDistance ) );

	m_ui->SpotShadowDistance->setValue( GE_ConvertToMeter( i_light->m_shadowDistance ) );
	
	m_ui->PointShadowDistance->setValue( GE_ConvertToMeter( i_light->m_shadowDistance ) );


	if ( lp->TargetMode )
	{
		m_ui->SpotTarget->setChecked( true );
		m_ui->DirectionalTarget->setChecked( true );
	}
	else
	{
		m_ui->SpotDirection->setChecked( true );
		m_ui->DirectionalDirection->setChecked( true );
	}
	
	m_light = i_light;
}

void GEW_LightProperty::on_Enable_toggled( bool i_checked )
{
	if( !m_light )
		return;
	m_light->m_light->setEnable( i_checked );
	m_light->setItemCheckState( GEW_Item::Enable, i_checked );
}

void GEW_LightProperty::on_Dynamic_toggled( bool i_checked )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Dynamic = i_checked;
	m_light->setItemCheckState( GEW_Item::Dynamic, i_checked );
}

void GEW_LightProperty::on_Intensity_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->setIntensity( ( float )i_value );
}

void GEW_LightProperty::on_Type_currentIndexChanged( int i_index )
{
	if ( !m_light )
		return;
	m_light->setType( ( GE_Light::Type ) i_index );
}

void GEW_LightProperty::on_Ambient_signalColorChanged( QColor i_color )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Ambient.x = i_color.redF();
	m_light->m_light->getParameters()->Ambient.y = i_color.greenF();
	m_light->m_light->getParameters()->Ambient.z = i_color.blueF();
}

void GEW_LightProperty::on_Diffuse_signalColorChanged( QColor i_color )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Diffuse.x = i_color.redF();
	m_light->m_light->getParameters()->Diffuse.y = i_color.greenF();
	m_light->m_light->getParameters()->Diffuse.z = i_color.blueF();
}

void GEW_LightProperty::on_Specular_signalColorChanged( QColor i_color )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Specular.x = i_color.redF();
	m_light->m_light->getParameters()->Specular.y = i_color.greenF();
	m_light->m_light->getParameters()->Specular.z = i_color.blueF();
}

void GEW_LightProperty::on_DirectionalTarget_toggled( bool i_checked )
{
	if ( !m_light )
		return;

	GE_Light::Parameters *lp = m_light->m_light->getParameters();
	if ( i_checked )
	{
		m_ui->DirectionalTargetX->setValue( ( float )GE_ConvertToMeter( lp->Target.x ) );
		m_ui->DirectionalTargetY->setValue( ( float )GE_ConvertToMeter( lp->Target.y ) );
		m_ui->DirectionalTargetZ->setValue( ( float )GE_ConvertToMeter( lp->Target.z ) );
		m_ui->SpotTargetX->setValue( ( float )GE_ConvertToMeter( lp->Target.x ) );
		m_ui->SpotTargetY->setValue( ( float )GE_ConvertToMeter( lp->Target.y ) );
		m_ui->SpotTargetZ->setValue( ( float )GE_ConvertToMeter( lp->Target.z ) );
	}
	else
	{
		m_ui->DirectionalDirectionYaw->setValue( GE_ConvertToDegrees( ( float )lp->Yaw ) );
		m_ui->DirectionalDirectionPitch->setValue( GE_ConvertToDegrees( ( float )lp->Pitch ) );
		m_ui->SpotDirectionYaw->setValue( GE_ConvertToDegrees( ( float )lp->Yaw ) );
		m_ui->SpotDirectionPitch->setValue( GE_ConvertToDegrees( ( float )lp->Pitch ) );
	}

	lp->TargetMode = i_checked;
}

void GEW_LightProperty::on_DirectionalDirectionYaw_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Yaw = GE_ConvertToRadians( ( float )i_value );
}

void GEW_LightProperty::on_DirectionalDirectionPitch_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Pitch = GE_ConvertToRadians( ( float )i_value );
}

void GEW_LightProperty::on_DirectionalTargetX_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Target.x = GE_ConvertToUnit( ( float )i_value );
}

void GEW_LightProperty::on_DirectionalTargetY_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Target.y = GE_ConvertToUnit( ( float )i_value );
}

void GEW_LightProperty::on_DirectionalTargetZ_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Target.z = GE_ConvertToUnit( ( float )i_value );
}

void GEW_LightProperty::on_DirectionalHasShadow_toggled( bool i_checked )
{
	if ( !m_light )
		return;
	if ( ( ( *m_light->m_light->getPosition( ) == *m_light->m_light->getTarget( ) && m_light->m_light->iSTargetMode( ) ) ||
		( m_light->m_light->getYaw() == 0.0f && m_light->m_light->getPitch() == 0.0f && !m_light->m_light->iSTargetMode() ) )
		&& i_checked )
	{
		m_ui->DirectionalHasShadow->setChecked( false );
		return;
	}
	m_light->m_light->getParameters()->HasShadow = i_checked;
	m_light->m_light->createShadowMap( );
}

void GEW_LightProperty::on_DirectionalVisualizeCascade_toggled( bool i_checked )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->VisualizeCascades = i_checked;
}

void GEW_LightProperty::on_DirectionalNumberOfCascade_currentIndexChanged( int i_index )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->NumberOfCascade = i_index + 1;
	m_light->m_light->createShadowMap();
}

void GEW_LightProperty::on_DirectionalCascade1Offset_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowOffsetBias[ 0 ].x = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_DirectionalCascade1Bias_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowOffsetBias[ 0 ].y = ( float )i_value;
}

void GEW_LightProperty::on_DirectionalCascade1MapSize_currentIndexChanged( int i_index )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowMapSize[ 0 ].x = m_light->m_light->getParameters()->ShadowMapSize[ 0 ].y = 1 << ( i_index + 5 );
	m_light->m_light->createShadowMap();
}

void GEW_LightProperty::on_DirectionalCascade1OrthoSize_valueChanged( int i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowFrustum[ 0 ].x = m_light->m_light->getParameters()->ShadowFrustum[ 0 ].y = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_DirectionalCascade2Offset_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowOffsetBias[ 1 ].x = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_DirectionalCascade2Bias_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowOffsetBias[ 1 ].y = ( float )i_value;
}

void GEW_LightProperty::on_DirectionalCascade2MapSize_currentIndexChanged( int i_index )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowMapSize[ 1 ].x = m_light->m_light->getParameters()->ShadowMapSize[ 1 ].y = 1 << ( i_index + 5 );
	m_light->m_light->createShadowMap();
}

void GEW_LightProperty::on_DirectionalCascade2OrthoSize_valueChanged( int i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowFrustum[ 1 ].x = m_light->m_light->getParameters()->ShadowFrustum[ 1 ].y = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_DirectionalCascade3Offset_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowOffsetBias[ 2 ].x = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_DirectionalCascade3Bias_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowOffsetBias[ 2 ].y = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_DirectionalCascade3MapSize_currentIndexChanged( int i_index )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowMapSize[ 2 ].x = m_light->m_light->getParameters()->ShadowMapSize[ 2 ].y = 1 << ( i_index + 5 );
	m_light->m_light->createShadowMap();
}

void GEW_LightProperty::on_DirectionalCascade3OrthoSize_valueChanged( int i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowFrustum[ 2 ].x = m_light->m_light->getParameters()->ShadowFrustum[ 2 ].y = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_DirectionalCascade4Offset_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowOffsetBias[ 3 ].x = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_DirectionalCascade4Bias_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowOffsetBias[ 3 ].y = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_DirectionalCascade4MapSize_currentIndexChanged( int i_index )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowMapSize[ 3 ].x = m_light->m_light->getParameters()->ShadowMapSize[ 3 ].y = 1 << ( i_index + 5 );
	m_light->m_light->createShadowMap();
}

void GEW_LightProperty::on_DirectionalCascade4OrthoSize_valueChanged( int i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowFrustum[ 3 ].x = m_light->m_light->getParameters()->ShadowFrustum[ 3 ].y = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_SpotAttenuation_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Attenuation = ( float )i_value;
}

void GEW_LightProperty::on_SpotSperead_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->SpotSpread = GE_ConvertToRadians( ( float )i_value );
}

void GEW_LightProperty::on_SpotRange_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Range = GE_ConvertToUnit( ( float )i_value );
}

void GEW_LightProperty::on_SpotSharpness_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->SpotSharpness = GE_ConvertToRadians( ( float )i_value );
}

void GEW_LightProperty::on_SpotHasShadow_toggled( bool i_checked )
{
	if ( !m_light )
		return;
	if ( ( ( *m_light->m_light->getPosition() == *m_light->m_light->getTarget() && m_light->m_light->iSTargetMode() ) ||
		( m_light->m_light->getYaw() == 0.0f && m_light->m_light->getPitch() == 0.0f && !m_light->m_light->iSTargetMode() ) )
		&& i_checked )
	{
		m_ui->SpotHasShadow->setChecked( false );
		return;
	}
	m_light->m_light->getParameters()->HasShadow = i_checked;
	m_light->m_light->createShadowMap();
}

void GEW_LightProperty::on_SpotMapSize_currentIndexChanged( int i_index )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowMapSize[ 0 ].x = m_light->m_light->getParameters()->ShadowMapSize[ 0 ].y = 1 << ( i_index + 5 );

	m_light->m_light->createShadowMap();
}

void GEW_LightProperty::on_SpotOffset_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowOffsetBias[ 0 ].x = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_SpotBias_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->ShadowOffsetBias[ 0 ].y = ( float )i_value;
}

void GEW_LightProperty::on_SpotMap_signalImageChanged( QString i_fileName )
{
	if ( !m_light )
		return;
	if ( i_fileName.isEmpty() )
		m_light->m_light->deleteFilterMap();
	else
		m_light->m_light->setFilterMap( i_fileName.toStdString().c_str() );
}

void GEW_LightProperty::on_SpotOffDistance_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_offDistance = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_SpotOffDistanceFromCamera_clicked()
{
	if ( !m_light )
		return;
	m_ui->SpotOffDistance->setValue( GE_ConvertToMeter( ( GEW_Scene::getSingleton( ).m_camera->m_position - *m_light->m_light->getPosition( ) ).length( ) ) );
}

void GEW_LightProperty::on_SpotShadowDistance_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_shadowDistance = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_SpotShadowDistanceFromCamera_clicked()
{
	if ( !m_light )
		return;
	m_ui->SpotShadowDistance->setValue( GE_ConvertToMeter( ( GEW_Scene::getSingleton( ).m_camera->m_position - *m_light->m_light->getPosition( ) ).length( ) ) );
}

void GEW_LightProperty::on_SpotTarget_toggled( bool i_checked )
{
	on_DirectionalTarget_toggled( i_checked );
}

void GEW_LightProperty::on_SpotDirectionYaw_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Yaw = GE_ConvertToRadians( ( float )i_value );
}

void GEW_LightProperty::on_SpotDirectionPitch_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Pitch = GE_ConvertToRadians( ( float )i_value );
}

void GEW_LightProperty::on_SpotTargetX_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Target.x = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_SpotTargetY_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Target.y = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_SpotTargetZ_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Target.z = ( float )GE_ConvertToUnit( i_value );
}

void GEW_LightProperty::on_PointAttenuation_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Attenuation = ( float )i_value;
}

void GEW_LightProperty::on_PointRange_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	m_light->m_light->getParameters()->Range = GE_ConvertToUnit( ( float )i_value );
}

void GEW_LightProperty::on_PointHasShadow_toggled( bool i_checked )
{
	on_SpotHasShadow_toggled( i_checked );
}

void GEW_LightProperty::on_PointMapSize_currentIndexChanged( int i_index )
{
	on_SpotMapSize_currentIndexChanged( i_index );
}

void GEW_LightProperty::on_PointOffset_valueChanged( double i_value )
{
	on_SpotOffset_valueChanged( i_value );
}

void GEW_LightProperty::on_PointBias_valueChanged( double i_value )
{
	on_SpotBias_valueChanged( i_value );
}

void GEW_LightProperty::on_PointYaw_valueChanged( double i_value )
{
	if ( !m_light )
		return;
	GE_Quat rot;
	rot.rotationYPR( &GE_Vec3( GE_ConvertToRadians( ( float )m_ui->PointYaw->value() ),
		GE_ConvertToRadians( ( float )m_ui->PointPitch->value( ) ),
		GE_ConvertToRadians( ( float )m_ui->PointRoll->value( ) ) ) );
	m_light->m_light->setRotateCubeFilter( rot );
}

void GEW_LightProperty::on_PointPitch_valueChanged( double i_value )
{
	on_PointYaw_valueChanged( i_value );
}

void GEW_LightProperty::on_PointRoll_valueChanged( double i_value )
{
	on_PointYaw_valueChanged( i_value );
}

void GEW_LightProperty::on_PointCubeMap_signalImageChanged( QString i_fileName )
{
	on_SpotMap_signalImageChanged( i_fileName );
}

void GEW_LightProperty::on_PointOffDistance_valueChanged( double i_value )
{
	on_SpotOffDistance_valueChanged( i_value );
}

void GEW_LightProperty::on_PointOffDistanceFromCamera_clicked()
{
	if ( !m_light )
		return;
	m_ui->PointOffDistance->setValue( GE_ConvertToMeter( (GEW_Scene::getSingleton().m_camera->m_position - *m_light->m_light->getPosition()).length() ) );
}

void GEW_LightProperty::on_PointShadowDistance_valueChanged( double i_value )
{
	on_SpotShadowDistance_valueChanged( i_value );
}

void GEW_LightProperty::on_PointShadowDistanceFromCamera_clicked()
{
	if ( !m_light )
		return;
	m_ui->PointShadowDistance->setValue( GE_ConvertToMeter( ( GEW_Scene::getSingleton( ).m_camera->m_position - *m_light->m_light->getPosition( ) ).length( ) ) );
}

void GEW_LightProperty::on_BoundColor_signalColorChanged( QColor i_color )
{
	if ( !m_light )
		return;
	m_light->m_probe.BoundColor.x = i_color.redF();
	m_light->m_probe.BoundColor.y = i_color.greenF( );
	m_light->m_probe.BoundColor.z = i_color.blueF( );
}