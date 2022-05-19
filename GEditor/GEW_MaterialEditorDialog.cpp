#include "GEW_MaterialEditorDialog.h"
#include "ui_GEW_MaterialEditorDialog.h"
#include <GE_Material.h>
#include <GE_Texture.h>
#include <GE_Vec3.h>

GEW_MaterialEditorDialog::GEW_MaterialEditorDialog( QWidget *i_parent /*= 0 */ )
: QDialog( i_parent ),
m_ui( new Ui::GEW_MaterialEditorDialog ),
m_material( NULL )
{
	m_ui->setupUi( this );
}

GEW_MaterialEditorDialog::~GEW_MaterialEditorDialog()
{
	delete m_ui;
}

inline QColor colorFromVector( GE_Vec3 &i_color )
{
	QColor color;
	color.setRedF( i_color.x );
	color.setGreenF( i_color.y );
	color.setBlueF( i_color.z );
	color.setAlphaF( 1.0f );
	return color;
}

void GEW_MaterialEditorDialog::setMaterial( GE_Material *i_material )
{
	m_material = i_material;
	if( m_material )
	{
		QColor color = colorFromVector(m_material->getBaseDiffuse());
		color.setAlphaF(m_material->Alpha);
		m_ui->diffuseColor->blockSignals( true );
		m_ui->diffuseColor->slotSetColor( color );
		m_ui->diffuseColor->blockSignals( false );

		m_ui->ambientColor->blockSignals( true );
		m_ui->ambientColor->slotSetColor( colorFromVector( m_material->getBaseAmbient() ) );
		m_ui->ambientColor->blockSignals( false );

		m_ui->specularColor->blockSignals( true );
		m_ui->specularColor->slotSetColor( colorFromVector( m_material->getBaseSpecular() ) );
		m_ui->specularColor->blockSignals( false );
		
		m_ui->emissiveColor->blockSignals( true );
		m_ui->emissiveColor->slotSetColor( colorFromVector( m_material->getBaseEmissive() ) );
		m_ui->emissiveColor->blockSignals( false );

		m_ui->glowColor->blockSignals( true );
		m_ui->glowColor->slotSetColor( colorFromVector( m_material->getBaseGlow() ) );
		m_ui->glowColor->blockSignals( false );

		m_ui->diffuseColorIntensity->blockSignals( true );
		m_ui->diffuseColorIntensity->setValue( m_material->getDiffuseIntensity() );
		m_ui->diffuseColorIntensity->blockSignals( false );

		m_ui->ambientColorIntensity->blockSignals( true );
		m_ui->ambientColorIntensity->setValue( m_material->getAmbientIntensity() );
		m_ui->ambientColorIntensity->blockSignals( false );

		m_ui->specularColorIntensity->blockSignals( true );
		m_ui->specularColorIntensity->setValue( m_material->getSpecularIntensity() );
		m_ui->specularColorIntensity->blockSignals( false );

		m_ui->emissiveColorIntensity->blockSignals( true );
		m_ui->emissiveColorIntensity->setValue( m_material->getEmissiveIntensity() );
		m_ui->emissiveColorIntensity->blockSignals( false );

		m_ui->glowColorIntensity->blockSignals( true );
		m_ui->glowColorIntensity->setValue( m_material->getGlowIntensity() );
		m_ui->glowColorIntensity->blockSignals( false );

		m_ui->specularPower->blockSignals( true );
		m_ui->specularPower->setValue( m_material->SpecularPower );
		m_ui->specularPower->blockSignals( false );

		m_ui->roughness->blockSignals( true );
		m_ui->roughness->setValue( m_material->Roughness);
		m_ui->roughness->blockSignals( false );

		m_ui->reflectmask->blockSignals( true );
		m_ui->reflectmask->setValue( m_material->Reflectmask );
		m_ui->reflectmask->blockSignals( false );

		m_ui->bumpiness->blockSignals( true );
		m_ui->bumpiness->setValue( m_material->Bumpiness );
		m_ui->bumpiness->blockSignals( false );

		m_ui->alphaTest->blockSignals( true );
		m_ui->alphaTest->setValue( m_material->AlphaTest );
		m_ui->alphaTest->blockSignals( false );

		m_ui->refract->blockSignals(true);
		m_ui->refract->setValue(m_material->Refract);
		m_ui->refract->blockSignals(false);

		m_ui->refractness->blockSignals(true);
		m_ui->refractness->setValue(m_material->Refractness);
		m_ui->refractness->blockSignals(false);

		m_ui->tint->blockSignals(true);
		m_ui->tint->setValue( m_material->Tint );
		m_ui->tint->blockSignals( false );

		m_ui->tintSpread->blockSignals( true );
		m_ui->tintSpread->setValue( m_material->TintSpread );
		m_ui->tintSpread->blockSignals( false );

		m_ui->diffuseAlphaMap1->blockSignals( true );
		if( m_material->DiffuseAlphaMap[ 0 ] )
			m_ui->diffuseAlphaMap1->loadFromFile( m_material->DiffuseAlphaMap[ 0 ]->getTextureName() );
		else
			m_ui->diffuseAlphaMap1->loadFromFile( QString() );
		m_ui->diffuseAlphaMap1->blockSignals( false );

		m_ui->diffuseAlphaMap2->blockSignals( true );
		if( m_material->DiffuseAlphaMap[ 1 ] )
			m_ui->diffuseAlphaMap2->loadFromFile( m_material->DiffuseAlphaMap[ 1 ]->getTextureName() );
		else
			m_ui->diffuseAlphaMap2->loadFromFile( QString() );
		m_ui->diffuseAlphaMap2->blockSignals( false );

		m_ui->normalHeightMap1->blockSignals( true );
		if( m_material->NormalHeightMap[ 0 ] )
			m_ui->normalHeightMap1->loadFromFile( m_material->NormalHeightMap[ 0 ]->getTextureName() );
		else
			m_ui->normalHeightMap1->loadFromFile( QString() );
		m_ui->normalHeightMap1->blockSignals( false );

		m_ui->normalHeightMap2->blockSignals( true );
		if( m_material->NormalHeightMap[ 1 ] )
			m_ui->normalHeightMap2->loadFromFile( m_material->NormalHeightMap[ 1 ]->getTextureName() );
		else
			m_ui->normalHeightMap2->loadFromFile( QString() );
		m_ui->normalHeightMap2->blockSignals( false );

		m_ui->glowSpecularReflectMaskMap1->blockSignals( true );
		if( m_material->GlowSpecularReflectMaskMap[ 0 ] )
			m_ui->glowSpecularReflectMaskMap1->loadFromFile( m_material->GlowSpecularReflectMaskMap[ 0 ]->getTextureName() );
		else
			m_ui->glowSpecularReflectMaskMap1->loadFromFile( QString() );
		m_ui->glowSpecularReflectMaskMap1->blockSignals( false );

		m_ui->glowSpecularReflectMaskMap2->blockSignals( true );
		if( m_material->GlowSpecularReflectMaskMap[ 1 ] )
			m_ui->glowSpecularReflectMaskMap2->loadFromFile( m_material->GlowSpecularReflectMaskMap[ 1 ]->getTextureName() );
		else
			m_ui->glowSpecularReflectMaskMap2->loadFromFile( QString() );
		m_ui->glowSpecularReflectMaskMap2->blockSignals( false );

		m_ui->reflectMap->blockSignals( true );
		if( m_material->ReflectMap )
			m_ui->reflectMap->loadFromFile( m_material->ReflectMap->getTextureName() );
		else
			m_ui->reflectMap->loadFromFile( QString() );
		m_ui->reflectMap->blockSignals( false );

		m_ui->tintMap->blockSignals( true );
		if (m_material->TintMap)
			m_ui->tintMap->loadFromFile( m_material->TintMap->getTextureName( ) );
		else
			m_ui->tintMap->loadFromFile( QString( ) );
		m_ui->tintMap->blockSignals( false );

		setWindowTitle( m_material->Name.c_str() );
	} else
	{
		m_ui->diffuseColor->slotSetColor( Qt::red );
		m_ui->ambientColor->slotSetColor( Qt::red );
		m_ui->specularColor->slotSetColor( Qt::red );
		m_ui->emissiveColor->slotSetColor( Qt::red );
		m_ui->glowColor->slotSetColor( Qt::red );
		m_ui->diffuseColorIntensity->setValue( 1.0 );
		m_ui->ambientColorIntensity->setValue( 1.0 );
		m_ui->specularColorIntensity->setValue( 1.0 );
		m_ui->emissiveColorIntensity->setValue( 1.0 );
		m_ui->glowColorIntensity->setValue( 1.0 );
		m_ui->specularPower->setValue( 10.0 );
		m_ui->roughness->setValue( 1.0 );
		m_ui->reflectmask->setValue( 1.0 );
		m_ui->bumpiness->setValue( 1.0 );
		m_ui->alphaTest->setValue( 0.0 );
		m_ui->diffuseAlphaMap1->loadFromFile( QString() );
		m_ui->diffuseAlphaMap2->loadFromFile( QString() );
		m_ui->normalHeightMap1->loadFromFile( QString() );
		m_ui->normalHeightMap2->loadFromFile( QString() );
		m_ui->glowSpecularReflectMaskMap1->loadFromFile( QString() );
		m_ui->glowSpecularReflectMaskMap2->loadFromFile( QString() );
		m_ui->reflectMap->loadFromFile( QString() );
		setWindowTitle( tr( "Material Editor" ) );
	}

	emit signalMaterialChanged( m_material );
}

GE_Material *GEW_MaterialEditorDialog::getMaterial()
{
	return m_material;
}

void GEW_MaterialEditorDialog::on_diffuseAlphaMap1_signalImageChanged( QString i_filepath )
{
	if ( !m_material )
		return;

	if( m_material->DiffuseAlphaMap[ 0 ] )
		GE_TextureManager::getSingleton().remove( &m_material->DiffuseAlphaMap[ 0 ] );
	m_material->DiffuseAlphaMap[ 0 ] = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_diffuseAlphaMap2_signalImageChanged( QString i_filepath )
{
	if ( !m_material )
		return;

	if ( m_material->DiffuseAlphaMap[ 1 ] )
		GE_TextureManager::getSingleton().remove( &m_material->DiffuseAlphaMap[ 1 ] );
	m_material->DiffuseAlphaMap[ 1 ] = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_normalHeightMap1_signalImageChanged( QString i_filepath )
{
	if ( !m_material )
		return;

	if ( m_material->NormalHeightMap[ 0 ] )
		GE_TextureManager::getSingleton().remove( &m_material->NormalHeightMap[ 0 ] );
	m_material->NormalHeightMap[ 0 ] = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_normalHeightMap2_signalImageChanged( QString i_filepath )
{
	if ( !m_material )
		return;

	if ( m_material->NormalHeightMap[ 1 ] )
		GE_TextureManager::getSingleton().remove( &m_material->NormalHeightMap[ 1 ] );
	m_material->NormalHeightMap[ 1 ] = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_glowSpecularReflectMaskMap1_signalImageChanged( QString i_filepath )
{
	if ( !m_material )
		return;

	if ( m_material->GlowSpecularReflectMaskMap[ 0 ] )
		GE_TextureManager::getSingleton().remove( &m_material->GlowSpecularReflectMaskMap[ 0 ] );
	m_material->GlowSpecularReflectMaskMap[ 0 ] = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_glowSpecularReflectMaskMap2_signalImageChanged( QString i_filepath )
{
	if ( !m_material )
		return;

	if ( m_material->GlowSpecularReflectMaskMap[ 1 ] )
		GE_TextureManager::getSingleton().remove( &m_material->GlowSpecularReflectMaskMap[ 1 ] );
	m_material->GlowSpecularReflectMaskMap[ 1 ] = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_reflectMap_signalImageChanged( QString i_filepath )
{
	if ( !m_material )
		return;

	if ( m_material->ReflectMap )
		GE_TextureManager::getSingleton().remove( &m_material->ReflectMap );
	m_material->ReflectMap = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_tintMap_signalImageChanged(QString i_filepath)
{
	if (!m_material)
		return;

	if (m_material->TintMap)
		GE_TextureManager::getSingleton().remove(&m_material->TintMap);
	m_material->TintMap = GE_TextureManager::getSingleton().createTexture(i_filepath.toStdString().c_str());
	emit signalMaterialChanged(m_material);
}

void GEW_MaterialEditorDialog::on_ambientColorIntensity_valueChanged( double i_value )
{
	if ( !m_material )
		return;

	m_material->setAmbientIntensity( ( float ) i_value );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_specularColorIntensity_valueChanged( double i_value )
{
	if ( !m_material )
		return;

	m_material->setSpecularIntensity( ( float ) i_value );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_emissiveColorIntensity_valueChanged( double i_value )
{
	if ( !m_material )
		return;

	m_material->setEmissiveIntensity( ( float ) i_value );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_glowColorIntensity_valueChanged( double i_value )
{
	if ( !m_material )
		return;

	m_material->setGlowIntensity( ( float ) i_value );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_diffuseColorIntensity_valueChanged( double i_value )
{
	if ( !m_material )
		return;

	m_material->setDiffuseIntensity( ( float ) i_value );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_diffuseColor_signalColorChanged( QColor i_color )
{
	if ( !m_material )
		return;

	GE_Vec3 color;
	color.x = i_color.redF();
	color.y = i_color.greenF();
	color.z = i_color.blueF();
	m_material->setBaseDiffuse( color );
	m_material->Alpha = i_color.alphaF();
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_ambientColor_signalColorChanged( QColor i_color )
{
	if ( !m_material )
		return;

	GE_Vec3 color;
	color.x = i_color.redF();
	color.y = i_color.greenF();
	color.z = i_color.blueF();
	m_material->setBaseAmbient( color );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_specularColor_signalColorChanged( QColor i_color )
{
	if ( !m_material )
		return;

	GE_Vec3 color;
	color.x = i_color.redF();
	color.y = i_color.greenF();
	color.z = i_color.blueF();
	m_material->setBaseSpecular( color );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_emissiveColor_signalColorChanged( QColor i_color )
{
	if ( !m_material )
		return;

	GE_Vec3 color;
	color.x = i_color.redF();
	color.y = i_color.greenF();
	color.z = i_color.blueF();
	m_material->setBaseEmissive( color );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_glowColor_signalColorChanged( QColor i_color )
{
	if ( !m_material )
		return;

	GE_Vec3 color;
	color.x = i_color.redF();
	color.y = i_color.greenF();
	color.z = i_color.blueF();
	m_material->setBaseGlow( color );
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_specularPower_valueChanged( double i_value )
{
	if ( !m_material )
		return;
	m_material->SpecularPower = ( float )i_value;
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_roughness_valueChanged( double i_value )
{
	if ( !m_material )
		return;
	m_material->Roughness = ( float )i_value;
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_reflectmask_valueChanged( double i_value )
{
	if ( !m_material )
		return;
	m_material->Reflectmask = ( float )i_value;
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_bumpiness_valueChanged( double i_value )
{
	if ( !m_material )
		return;
	m_material->Bumpiness = ( float )i_value;
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_alphaTest_valueChanged( double i_value )
{
	if ( !m_material )
		return;
	m_material->AlphaTest = ( float )i_value;
	emit signalMaterialChanged( m_material );
}

void GEW_MaterialEditorDialog::on_refract_valueChanged(double i_value)
{
	if (!m_material)
		return;
	m_material->Refract = (float)i_value;
	emit signalMaterialChanged(m_material);
}

void GEW_MaterialEditorDialog::on_refractness_valueChanged(double i_value)
{
	if (!m_material)
		return;
	m_material->Refractness = (float)i_value;
	emit signalMaterialChanged(m_material);
}

void GEW_MaterialEditorDialog::on_tint_valueChanged(double i_value)
{
	if (!m_material)
		return;
	m_material->Tint = (float)i_value;
	emit signalMaterialChanged(m_material);
}

void GEW_MaterialEditorDialog::on_tintSpread_valueChanged(double i_value)
{
	if (!m_material)
		return;
	m_material->TintSpread = (float)i_value;
	emit signalMaterialChanged(m_material);
}


void GEW_MaterialEditorDialog::on_reloadMaps_clicked()
{
	if( !m_material )
		return;

	m_ui->diffuseAlphaMap1->slotReload();
	m_ui->diffuseAlphaMap2->slotReload();
	m_ui->normalHeightMap1->slotReload();
	m_ui->normalHeightMap2->slotReload();
	m_ui->glowSpecularReflectMaskMap1->slotReload();
	m_ui->glowSpecularReflectMaskMap2->slotReload();
	m_ui->reflectMap->slotReload();
	m_ui->tintMap->slotReload();
}
