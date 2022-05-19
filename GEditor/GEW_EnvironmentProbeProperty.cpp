#include "GEW_EnvironmentProbeProperty.h"
#include "GEW_EnvironmentProbe.h"
#include "GE_RenderUtility.h"
#include <GE_Texture.h>
#include <GE_EnvironmentProbe.h>
#include "ui_GEW_EnvironmentProbeProperty.h"
#include <QFileDialog>

GEW_EnvironmentProbeProperty::GEW_EnvironmentProbeProperty( QWidget *i_parent ) : QWidget( i_parent ), m_ui( new Ui::EnvironmentProbeProperty )
{
	m_ui->setupUi( this );
	m_probe = NULL;
}

GEW_EnvironmentProbeProperty::~GEW_EnvironmentProbeProperty()
{
	delete m_ui;
}

void GEW_EnvironmentProbeProperty::on_Enable_toggled( bool i_checked )
{
	if ( !m_probe )
		return;
	m_probe->m_probe->setEnable( i_checked );
}

void GEW_EnvironmentProbeProperty::on_BoundVisible_toggled( bool i_checked )
{
	if ( !m_probe )
		return;
	m_probe->m_bound->setVisible( i_checked );
}

void GEW_EnvironmentProbeProperty::on_Radius_valueChanged( double i_value )
{
	if ( !m_probe )
		return;
	m_probe->m_radius = GE_ConvertToUnit( ( float )i_value );
	m_probe->m_bound->setScale( GE_Vec3( m_probe->m_radius, m_probe->m_radius, m_probe->m_radius ) );
}

void GEW_EnvironmentProbeProperty::on_BoundColor_signalColorChanged( QColor i_color )
{
	if ( !m_probe )
		return;
	m_probe->m_bound->setColor( GE_Vec3( i_color.redF(), i_color.greenF(), i_color.blueF() ) );
}

void GEW_EnvironmentProbeProperty::on_Map_signalImageChanged( QString i_fileName )
{
	if ( !m_probe )
		return;
	if ( m_probe->m_map )
		GE_TextureManager::getSingleton().remove( &m_probe->m_map );
	if ( !i_fileName.isEmpty() )
		m_probe->m_map = GE_TextureManager::getSingleton().createTexture( i_fileName.toStdString().c_str() );
}

void GEW_EnvironmentProbeProperty::on_SelectOutputPath_clicked()
{
	if ( !m_probe )
		return;
	QString path = QFileDialog::getExistingDirectory(
		this,
		tr( "Environment Map Path" ),
		m_ui->OutputPath->text(),
		QFileDialog::Option::ShowDirsOnly );
	if ( path.isEmpty() )
		return;

	m_ui->OutputPath->setText( path );
	m_probe->m_Path = path;
}

void GEW_EnvironmentProbeProperty::on_MapSize_currentIndexChanged( int i_index )
{
	if ( !m_probe )
		return;
	GE_Vec2 size;
	size.x = size.y = ( 1 << i_index );
	m_probe->m_probe->setMapSize( size );
}

void GEW_EnvironmentProbeProperty::on_Bake_clicked()
{
	if ( !m_probe )
		return;	
}

void GEW_EnvironmentProbeProperty::setProbe( GEW_EnvironmentProbe *i_probe )
{
	m_probe = NULL;

	QColor cl;
	cl.setRedF( i_probe->m_boundColor.x );
	cl.setGreenF( i_probe->m_boundColor.y );
	cl.setBlueF( i_probe->m_boundColor.z );
	m_ui->BoundColor->slotSetColor( cl );
	m_ui->BoundVisible->setChecked( i_probe->m_bound->isVisible() );
	m_ui->Radius->setValue( i_probe->m_radius );
	m_ui->OutputPath->setText( i_probe->m_Path );
	if ( m_ui->Map )
		m_ui->Map->slotLoad( i_probe->m_map->getTextureName() );
	else
		m_ui->Map->slotClear();
	m_ui->OutputPath->setText(i_probe->m_Path );
	m_ui->MapSize->setCurrentIndex( log( i_probe->m_probe->getMapSize().x ) / log( 2 ) );

	m_probe = i_probe;
}
