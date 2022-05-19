#include "GEW_DefaultCameraProperty.h"
#include "ui_GEW_DefaultCameraProperty.h"
#include <QFileDialog>
#include <QStringList>
#include <QSettings>
#include "GEW_DefaultCamera.h"
#include <GE_Math.h>
#include "GEW_Camera.h"
#include "GEW_Scene.h"

GEW_DefaultCameraProperty::GEW_DefaultCameraProperty( QWidget *i_parent ) : QWidget( i_parent ),
m_ui( new Ui::DefaultCameraProperty )
{
	m_ui->setupUi( this );
	m_ui->Near->setValue( GE_ConvertToMeter( GEW_DefaultCamera::getSingleton()->m_settings.Near ) );
	m_ui->Far->setValue( GE_ConvertToMeter( GEW_DefaultCamera::getSingleton()->m_settings.Far ) );
	m_ui->FOV->setValue( GE_ConvertToDegrees( GEW_DefaultCamera::getSingleton()->m_settings.FOV ) );
	m_singleton = this;
}


GEW_DefaultCameraProperty::~GEW_DefaultCameraProperty()
{
	delete m_ui;
}

void GEW_DefaultCameraProperty::on_Near_valueChanged( double i_value )
{
	if ( GE_ConvertToUnit( ( float )i_value ) == GEW_DefaultCamera::getSingleton()->m_settings.Far )
	{
		GEW_DefaultCamera::getSingleton()->m_settings.Near = GE_ConvertToUnit( 0.3f );
		GEW_DefaultCamera::getSingleton()->m_settings.Far = GE_ConvertToUnit( 4000.0f );
		m_ui->Near->blockSignals( true );
		m_ui->Far->blockSignals( true );
		m_ui->Near->setValue( GE_ConvertToMeter( GEW_DefaultCamera::getSingleton()->m_settings.Near ) );
		m_ui->Far->setValue( GE_ConvertToMeter( GEW_DefaultCamera::getSingleton()->m_settings.Far ) );
		m_ui->Near->blockSignals( false );
		m_ui->Far->blockSignals( false );
	}
	else
		GEW_DefaultCamera::getSingleton()->m_settings.Near = GE_ConvertToUnit( ( float )i_value );
	GEW_Scene::getSingleton().m_camera->m_fovy = GEW_DefaultCamera::getSingleton()->m_settings.FOV;
	GEW_Scene::getSingleton().m_camera->m_near = GEW_DefaultCamera::getSingleton()->m_settings.Near;
	GEW_Scene::getSingleton().m_camera->m_far = GEW_DefaultCamera::getSingleton()->m_settings.Far;
}

void GEW_DefaultCameraProperty::on_Far_valueChanged( double i_value )
{
	if ( GE_ConvertToUnit( ( float )i_value ) == GEW_DefaultCamera::getSingleton()->m_settings.Near )
	{
		GEW_DefaultCamera::getSingleton()->m_settings.Near = GE_ConvertToUnit( 0.3f );
		GEW_DefaultCamera::getSingleton()->m_settings.Far = GE_ConvertToUnit( 4000.0f );
		m_ui->Near->blockSignals( true );
		m_ui->Far->blockSignals( true );
		m_ui->Near->setValue( GE_ConvertToMeter( GEW_DefaultCamera::getSingleton()->m_settings.Near ) );
		m_ui->Far->setValue( GE_ConvertToMeter( GEW_DefaultCamera::getSingleton()->m_settings.Far ) );
		m_ui->Near->blockSignals( false );
		m_ui->Far->blockSignals( false );
	}
	else
		GEW_DefaultCamera::getSingleton()->m_settings.Far = GE_ConvertToUnit( ( float )i_value );
	GEW_Scene::getSingleton().m_camera->m_fovy = GEW_DefaultCamera::getSingleton()->m_settings.FOV;
	GEW_Scene::getSingleton().m_camera->m_near = GEW_DefaultCamera::getSingleton()->m_settings.Near;
	GEW_Scene::getSingleton().m_camera->m_far = GEW_DefaultCamera::getSingleton()->m_settings.Far;
}

void GEW_DefaultCameraProperty::on_FOV_valueChanged( double i_value )
{
	GEW_DefaultCamera::getSingleton()->m_settings.FOV = GE_ConvertToRadians( ( float )i_value );
	GEW_Scene::getSingleton().m_camera->m_fovy = GEW_DefaultCamera::getSingleton()->m_settings.FOV;
	GEW_Scene::getSingleton().m_camera->m_near = GEW_DefaultCamera::getSingleton()->m_settings.Near;
	GEW_Scene::getSingleton().m_camera->m_far = GEW_DefaultCamera::getSingleton()->m_settings.Far;
}

void GEW_DefaultCameraProperty::on_Reset_clicked()
{
	GEW_DefaultCamera::getSingleton()->m_settings.reset();
	m_ui->Near->setValue( GE_ConvertToMeter( GEW_DefaultCamera::getSingleton()->m_settings.Near ) );
	m_ui->Far->setValue( GE_ConvertToMeter( GEW_DefaultCamera::getSingleton()->m_settings.Far ) );
	m_ui->FOV->setValue( GE_ConvertToDegrees( GEW_DefaultCamera::getSingleton()->m_settings.FOV ) );
}

void GEW_DefaultCameraProperty::on_Set_clicked()
{
	GEW_Scene::getSingleton().m_camera->m_position.x = GE_ConvertToUnit( ( float )m_ui->PosX->value() );
	GEW_Scene::getSingleton().m_camera->m_position.y = GE_ConvertToUnit( ( float )m_ui->PosY->value() );
	GEW_Scene::getSingleton().m_camera->m_position.z = GE_ConvertToUnit( ( float )m_ui->PosZ->value() );
	m_ui->PosX->setValue( 0.0 );
	m_ui->PosY->setValue( 0.0 );
	m_ui->PosZ->setValue( 0.0 );
}

GEW_DefaultCameraProperty * GEW_DefaultCameraProperty::m_singleton = NULL;
