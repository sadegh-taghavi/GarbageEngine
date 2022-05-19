#include "MaterialEditorDialog.h"
#include "ui_MaterialEditorDialog.h"
#include <GE_Prerequisites.h>
#include <GE_Material.h>
#include <GE_Texture.h>
#include <GE_Vec3.h>

MaterialEditorDialog::MaterialEditorDialog( QWidget *i_parent /*= 0 */ )
: QDialog( i_parent ),
m_ui( new Ui::MaterialEditorDialog ),
m_material( NULL )
{
	m_ui->setupUi( this );
}

MaterialEditorDialog::~MaterialEditorDialog()
{
	delete m_ui;
}

inline QColor colorFromVector( GE_Vec3 &i_color )
{
	return QColor( i_color.x, i_color.y, i_color.z );
}

void MaterialEditorDialog::setMaterial( GE_Material *i_material )
{
	m_material = i_material;
	blockSignals( true );
	if( m_material )
	{
		m_ui->diffuseColor->slotSetColor( colorFromVector( m_material->getBaseDiffuse() ) );
		m_ui->ambientColor->slotSetColor( colorFromVector( m_material->getBaseAmbient() ) );
		m_ui->specularColor->slotSetColor( colorFromVector( m_material->getBaseSpecular() ) );
		m_ui->emissiveColor->slotSetColor( colorFromVector( m_material->getBaseEmissive() ) );
		m_ui->glowColor->slotSetColor( colorFromVector( m_material->getBaseGlow() ) );

		m_ui->diffuseColorIntensity->setValue( m_material->getDiffuseIntensity() );
		m_ui->ambientColorIntensity->setValue( m_material->getAmbientIntensity() );
		m_ui->specularColorIntensity->setValue( m_material->getSpecularIntensity() );
		m_ui->emissiveColorIntensity->setValue( m_material->getEmissiveIntensity() );
		m_ui->glowColorIntensity->setValue( m_material->getGlowIntensity() );

		m_ui->specularPower->setValue( m_material->SpecularPower );
		m_ui->roughness->setValue( m_material->Roughness);
		m_ui->reflectmask->setValue( m_material->Reflectmask );
		m_ui->bumpiness->setValue( m_material->Bumpiness );
		m_ui->alphaTest->setValue( m_material->AlphaTest );

		if( m_material->DiffuseAlphaMap[ 0 ] )
			m_ui->diffuseAlphaMap1->loadFromFile( m_material->DiffuseAlphaMap[ 0 ]->getTextureName() );
		else
			m_ui->diffuseAlphaMap1->loadFromFile( QString() );
		if( m_material->DiffuseAlphaMap[ 1 ] )
			m_ui->diffuseAlphaMap2->loadFromFile( m_material->DiffuseAlphaMap[ 1 ]->getTextureName() );
		else
			m_ui->diffuseAlphaMap2->loadFromFile( QString() );

		if( m_material->NormalHeightMap[ 0 ] )
			m_ui->normalHeightMap1->loadFromFile( m_material->NormalHeightMap[ 0 ]->getTextureName() );
		else
			m_ui->normalHeightMap1->loadFromFile( QString() );
		if( m_material->NormalHeightMap[ 1 ] )
			m_ui->normalHeightMap2->loadFromFile( m_material->NormalHeightMap[ 1 ]->getTextureName() );
		else
			m_ui->normalHeightMap2->loadFromFile( QString() );

		if( m_material->GlowSpecularReflectMaskMap[ 0 ] )
			m_ui->glowSpecularReflectMaskMap1->loadFromFile( m_material->GlowSpecularReflectMaskMap[ 0 ]->getTextureName() );
		else
			m_ui->glowSpecularReflectMaskMap1->loadFromFile( QString() );
		if( m_material->GlowSpecularReflectMaskMap[ 1 ] )
			m_ui->glowSpecularReflectMaskMap2->loadFromFile( m_material->GlowSpecularReflectMaskMap[ 1 ]->getTextureName() );
		else
			m_ui->glowSpecularReflectMaskMap2->loadFromFile( QString() );

		if( m_material->ReflectMap )
			m_ui->reflectMap->loadFromFile( m_material->ReflectMap->getTextureName() );
		else
			m_ui->reflectMap->loadFromFile( QString() );

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
		m_ui->reflectance->setValue( 1.0 );
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
	blockSignals( false );
	emit signalMaterialChanged( m_material );
}

GE_Material *MaterialEditorDialog::getMaterial()
{
	return m_material;
}

void MaterialEditorDialog::on_diffuseAlphaMap1_signalImageChanged( QString i_filepath )
{
	if ( !m_material )
		return;

	if( m_material->DiffuseAlphaMap[ 0 ] )
		GE_TextureManager::getSingleton().remove( &m_material->DiffuseAlphaMap[ 0 ] );
	m_material->DiffuseAlphaMap[ 0 ] = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_diffuseAlphaMap2_signalImageChanged( QString i_filepath )
{
	if ( !m_material )
		return;

	if ( m_material->DiffuseAlphaMap[ 1 ] )
		GE_TextureManager::getSingleton().remove( &m_material->DiffuseAlphaMap[ 1 ] );
	m_material->DiffuseAlphaMap[ 1 ] = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_normalHeightMap1_signalImageChanged( QString i_filepath )
{
	if ( !m_material )
		return;

	if ( m_material->NormalHeightMap[ 0 ] )
		GE_TextureManager::getSingleton().remove( &m_material->NormalHeightMap[ 0 ] );
	m_material->NormalHeightMap[ 0 ] = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_normalHeightMap2_signalImageChanged( QString i_filepath )
{
	if ( !m_material )
		return;

	if ( m_material->NormalHeightMap[ 1 ] )
		GE_TextureManager::getSingleton().remove( &m_material->NormalHeightMap[ 1 ] );
	m_material->NormalHeightMap[ 1 ] = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_glowSpecularReflectMaskMap1_signalImageChanged( QString i_filepath )
{
	if ( !m_material )
		return;

	if ( m_material->GlowSpecularReflectMaskMap[ 0 ] )
		GE_TextureManager::getSingleton().remove( &m_material->GlowSpecularReflectMaskMap[ 0 ] );
	m_material->GlowSpecularReflectMaskMap[ 0 ] = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_glowSpecularReflectMaskMap2_signalImageChanged( QString i_filepath )
{
	if ( !m_material )
		return;

	if ( m_material->GlowSpecularReflectMaskMap[ 1 ] )
		GE_TextureManager::getSingleton().remove( &m_material->GlowSpecularReflectMaskMap[ 1 ] );
	m_material->GlowSpecularReflectMaskMap[ 1 ] = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_reflectMap_signalImageChanged( QString i_filepath )
{
	if ( !m_material )
		return;

	if ( m_material->ReflectMap )
		GE_TextureManager::getSingleton().remove( &m_material->ReflectMap );
	m_material->ReflectMap = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_ambientColorIntensity_valueChanged( double i_value )
{
	if ( !m_material )
		return;

	m_material->setAmbientIntensity( ( float ) i_value );
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_specularColorIntensity_valueChanged( double i_value )
{
	if ( !m_material )
		return;

	m_material->setSpecularIntensity( ( float ) i_value );
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_emissiveColorIntensity_valueChanged( double i_value )
{
	if ( !m_material )
		return;

	m_material->setEmissiveIntensity( ( float ) i_value );
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_glowColorIntensity_valueChanged( double i_value )
{
	if ( !m_material )
		return;

	m_material->setGlowIntensity( ( float ) i_value );
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_diffuseColorIntensity_valueChanged( double i_value )
{
	if ( !m_material )
		return;

	m_material->setDiffuseIntensity( ( float ) i_value );
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_diffuseColor_signalColorChanged( QColor i_color )
{
	if ( !m_material )
		return;

	GE_Vec3 color;
	color.x = i_color.redF();
	color.y = i_color.greenF();
	color.z = i_color.blueF();
	m_material->setBaseDiffuse( color );
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_ambientColor_signalColorChanged( QColor i_color )
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

void MaterialEditorDialog::on_specularColor_signalColorChanged( QColor i_color )
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

void MaterialEditorDialog::on_emissiveColor_signalColorChanged( QColor i_color )
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

void MaterialEditorDialog::on_glowColor_signalColorChanged( QColor i_color )
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

void MaterialEditorDialog::on_specularPower_valueChanged( double i_value )
{
	if ( !m_material )
		return;
	m_material->SpecularPower = ( float )i_value;
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_roughness_valueChanged( double i_value )
{
	if ( !m_material )
		return;
	m_material->Roughness = ( float )i_value;
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_reflectmask_valueChanged( double i_value )
{
	if ( !m_material )
		return;
	m_material->Reflectmask = ( float )i_value;
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_bumpiness_valueChanged( double i_value )
{
	if ( !m_material )
		return;
	m_material->Bumpiness = ( float )i_value;
	emit signalMaterialChanged( m_material );
}

void MaterialEditorDialog::on_alphaTest_valueChanged( double i_value )
{
	if ( !m_material )
		return;
	m_material->AlphaTest = ( float )i_value;
	emit signalMaterialChanged( m_material );
}
