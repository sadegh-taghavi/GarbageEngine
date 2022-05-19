#include "GEW_Setting.h"
#include <GE_Core.h>
#include <GE_DeferredGeometry.h>
#include "GEW_Viewport.h"
#include "GEW_Scene.h"
#include "ui_GEW_Setting.h"

GEW_Setting * GEW_Setting::m_singleton = NULL;

GEW_Setting::GEW_Setting( QWidget *i_parent /*= 0 */ )
: QWidget( i_parent ),
m_ui( new Ui::GEW_Setting )
{
	m_ui->setupUi( this );
	m_singleton = this;
	m_parameters.Render = true;
	m_parameters.TextVisible = true;
	m_parameters.GameMode = false;
	m_parameters.WireFrameShadeMode = false;
	m_parameters.PhysicsSimulation = false;
	m_parameters.AnisotropicLevel = 0;
	m_ui->AnisotropicLevel->setCurrentIndex( m_parameters.AnisotropicLevel );
	m_parameters.FXAAQuality = 0;
	m_ui->FXAAQuality->setCurrentIndex( m_parameters.FXAAQuality );
	m_parameters.FPSLimit = 1000000;
	m_ui->FPSLimit->setValue( m_parameters.FPSLimit );
	m_parameters.SyncLevel = 0; 
	m_ui->SyncLevel->setCurrentIndex( m_parameters.SyncLevel );
}

GEW_Setting::~GEW_Setting()
{
	delete m_ui;
}

void GEW_Setting::saveToFile( QFile &i_file, const QString &i_resourcePath )
{
	i_file.write( ( const char* ) &m_parameters, sizeof( GEW_Setting::Parameters ) );
}

void GEW_Setting::loadFromFile( QFile &i_file, const QString &i_resourcePath )
{
	i_file.read( ( char* ) &m_parameters, sizeof( GEW_Setting::Parameters ) );
	
	m_ui->Render->blockSignals( true );
	m_ui->Render->setChecked( !m_parameters.Render );
	m_ui->Render->blockSignals( false );

	m_ui->Info->blockSignals( true );
	m_ui->Info->setChecked( m_parameters.TextVisible );
	m_ui->Info->blockSignals( false );

	m_ui->GameMode->blockSignals( true );
	m_ui->GameMode->setChecked( m_parameters.GameMode );
	m_ui->GameMode->blockSignals( false );

	m_ui->ShadeMode->blockSignals( true );
	m_ui->ShadeMode->setChecked( m_parameters.WireFrameShadeMode );
	GE_DeferredGeometry::getSingleton().setWireframeMode( m_parameters.WireFrameShadeMode );
	m_ui->ShadeMode->blockSignals( false );

	m_ui->PhysicsSimulation->blockSignals( true );
	m_ui->PhysicsSimulation->setChecked( m_parameters.PhysicsSimulation );
	m_ui->PhysicsSimulation->blockSignals( false );

	m_ui->AnisotropicLevel->blockSignals( true );
	m_ui->AnisotropicLevel->setCurrentIndex( m_parameters.AnisotropicLevel);
	GE_Core::getSingleton().createRenderStates( m_parameters.AnisotropicLevel );
	m_ui->AnisotropicLevel->blockSignals( false );

	m_ui->FXAAQuality->blockSignals( true );
	m_ui->FXAAQuality->setCurrentIndex( m_parameters.FXAAQuality );
	m_ui->FXAAQuality->blockSignals( false );

	m_ui->SyncLevel->blockSignals( true );
	m_ui->SyncLevel->setCurrentIndex( m_parameters.SyncLevel );
	m_ui->SyncLevel->blockSignals( false );

	m_ui->FPSLimit->blockSignals( true );
	m_ui->FPSLimit->setValue( m_parameters.FPSLimit );
	GEW_Scene::getSingleton().m_viewport->setFps( ( float ) m_parameters.FPSLimit );
	m_ui->FPSLimit->blockSignals( false );

}

void GEW_Setting::showBar()
{
	m_ui->ViewportBarPin->blockSignals( true );
	m_ui->ViewportBarPin->setChecked( true );
	m_ui->ViewportBarPin->blockSignals( false );
	setVisible( true );
}

void GEW_Setting::on_Info_toggled( bool i_checked )
{
	m_parameters.TextVisible = i_checked;
}

void GEW_Setting::on_GameMode_toggled( bool i_checked )
{
	m_parameters.GameMode = i_checked;
}

void GEW_Setting::on_ShadeMode_toggled( bool i_checked )
{
	m_parameters.WireFrameShadeMode = i_checked;
	GE_DeferredGeometry::getSingleton().setWireframeMode( i_checked );
}

void GEW_Setting::on_PhysicsSimulation_toggled( bool i_checked )
{
	m_parameters.PhysicsSimulation = i_checked;
}

void GEW_Setting::on_PhysicsSingleStep_clicked()
{
	m_ui->PhysicsSimulation->setChecked( false );
	m_parameters.PhysicsSingleStep = true;
}

void GEW_Setting::on_PhysicsReset_clicked()
{
	m_parameters.PhysicsReset = true;
}

void GEW_Setting::on_AnisotropicLevel_currentIndexChanged( int i_index )
{
	m_parameters.AnisotropicLevel = i_index;
	GE_Core::getSingleton().createRenderStates( i_index );
}

void GEW_Setting::on_FXAAQuality_currentIndexChanged( int i_index )
{
	m_parameters.FXAAQuality = i_index;
}

void GEW_Setting::on_Render_toggled( bool i_checked )
{
	m_parameters.Render = !i_checked;
	if( m_parameters.Render )
		GEW_Scene::getSingleton().m_viewport->setFps( ( float ) m_parameters.FPSLimit );
	else
		GEW_Scene::getSingleton().m_viewport->setFps( -1.0f );
}

void GEW_Setting::on_FPSLimit_valueChanged( int i_value )
{
	m_parameters.FPSLimit = ( uint32_t )i_value;
	GEW_Scene::getSingleton( ).m_viewport->setFps( ( float )m_parameters.FPSLimit );
}

void GEW_Setting::on_SyncLevel_currentIndexChanged( int i_index )
{
	m_parameters.SyncLevel = i_index;
}


