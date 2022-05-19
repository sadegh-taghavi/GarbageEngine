#include "GEW_TerrainProperty.h"
#include "ui_GEW_TerrainProperty.h"
#include <GE_Texture.h>
#include <GE_RenderTarget.h>
#include <GE_RenderUtility.h>
#include <QInputDialog>
#include <QMenu>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <GE_Terrain.h>
#include <GE_RenderUtility.h>
#include <QFile>
#include "GEW_Utility.h"
#include "GEW_Terrain.h"

inline QColor colorFromVector( GE_Vec3 &i_color )
{
	QColor color;
	color.setRedF( i_color.x );
	color.setGreenF( i_color.y );
	color.setBlueF( i_color.z );
	color.setAlphaF( 1.0f );
	return color;
}

GEW_TerrainProperty::GEW_TerrainProperty( QWidget *i_parent /*= 0*/ ) : QWidget( i_parent ),
m_ui( new Ui::TerrainProperty )
{
	m_singleton = this;
	m_ui->setupUi( this );
	GEW_Terrain::getSingleton()->resetAll();
	updateUI();
}

GEW_TerrainProperty::~GEW_TerrainProperty()
{
	delete m_ui;
}

void GEW_TerrainProperty::on_TerrainEnable_toggled( bool i_checked )
{
	GEW_Terrain::getSingleton()->m_baseData.Enable = i_checked;	
	GEW_Terrain::getSingleton()->updateBrushMap();
	GEW_Terrain::getSingleton()->initHeightMapUndoMemory();
	GEW_Terrain::getSingleton()->initMaskMapUndoMemory();
	if ( i_checked )
		GEW_Terrain::getSingleton()->m_physxData.set();
}

void GEW_TerrainProperty::on_BaseSize_valueChanged( double i_value )
{
	baseApply();
}

void GEW_TerrainProperty::on_BaseNumberOfChunks_currentIndexChanged( int i_index )
{
	baseApply();
}

void GEW_TerrainProperty::on_BaseQuadDensity_currentIndexChanged( int i_index )
{
	baseApply();
}

void GEW_TerrainProperty::on_BaseMaxHeight_valueChanged( double i_value )
{
	m_ui->SculptVolumeValue->setMaximum( i_value );
	GEW_Terrain::getSingleton()->m_baseParam.MaxHeight = GE_ConvertToUnit( i_value );
	GEW_Terrain::getSingleton()->m_baseParam.set();
}

void GEW_TerrainProperty::on_BaseNormalAspect_valueChanged( double i_value )
{
	GEW_Terrain::getSingleton()->m_baseParam.NormalAspect = ( float )i_value;
	GEW_Terrain::getSingleton()->m_baseParam.set();
}

void GEW_TerrainProperty::on_BaseLODMultiplier_valueChanged( double i_value )
{
	GEW_Terrain::getSingleton()->m_baseParam.LodMultiplier = i_value;
	GEW_Terrain::getSingleton()->m_baseParam.set();
}

void GEW_TerrainProperty::baseApply()
{
	GEW_Terrain::getSingleton()->m_baseData.Size = GE_ConvertToUnit( ( float )m_ui->BaseSize->value() );
	GEW_Terrain::getSingleton()->m_baseData.NumberOfChunks = 1 << ( m_ui->BaseNumberOfChunks->currentIndex() + 1 );
	GEW_Terrain::getSingleton()->m_baseData.QuadDensity = 1 << ( m_ui->BaseQuadDensity->currentIndex() + 1 );
	GEW_Terrain::getSingleton()->m_baseData.set();
}

void GEW_TerrainProperty::on_BaseHeightBias_valueChanged( double i_value )
{
	GEW_Terrain::getSingleton()->m_baseParam.HeightBias = GE_ConvertToUnit( i_value );
	GEW_Terrain::getSingleton()->m_baseParam.set();
}

void GEW_TerrainProperty::on_BaseHeightMap_signalImageChanged( QString i_filepath )
{
	if ( i_filepath.isEmpty() )
	{
		SAFE_DELETE( GEW_Terrain::getSingleton()->m_heightMapRT[ 0 ] );
	}
	else
	{
		GEW_Terrain::getSingleton()->createHeightMap( i_filepath, 0, false, true );
	}
	if ( GEW_Terrain::getSingleton()->m_heightMapRT[ 0 ] )
	{
		m_ui->BaseHeightMapSize->blockSignals( true );
		m_ui->BaseHeightMapSize->setCurrentIndex( log( ( uint32_t )GEW_Terrain::getSingleton()->m_heightMapRT[ 0 ]->getViewPort()->Width ) / log( 2 ) - 2 );
		m_ui->BaseHeightMapSize->blockSignals( false );
	}
	GEW_Terrain::getSingleton()->setHeightMap();
	GEW_Terrain::getSingleton()->setSculptEnableChange();
	GEW_Terrain::getSingleton()->m_undoParams.HeightMapMemory.clear();
	GEW_Terrain::getSingleton()->initHeightMapUndoMemory();
}

void GEW_TerrainProperty::on_BaseMaskMap_signalImageChanged( QString i_filepath )
{
	if ( i_filepath.isEmpty() )
	{
		SAFE_DELETE( GEW_Terrain::getSingleton()->m_maskMapRT[ 0 ] );
	}
	else
	{
		GEW_Terrain::getSingleton()->createMaskMap( i_filepath, 0, false, true );
	}
	if ( GEW_Terrain::getSingleton()->m_maskMapRT[ 0 ] )
	{
		m_ui->BaseMaskMapSize->blockSignals( true );
		m_ui->BaseMaskMapSize->setCurrentIndex( log( ( uint32_t )GEW_Terrain::getSingleton()->m_maskMapRT[ 0 ]->getViewPort()->Width ) / log( 2 ) - 2 );
		m_ui->BaseMaskMapSize->blockSignals( false );
	}
	GEW_Terrain::getSingleton()->setMaskMap();
	GEW_Terrain::getSingleton()->setSculptEnableChange();
	GEW_Terrain::getSingleton()->m_undoParams.MaskMapMemory.clear();
	GEW_Terrain::getSingleton()->initMaskMapUndoMemory();
}

void GEW_TerrainProperty::on_NewHeightMap_clicked()
{
	int ret = QMessageBox::No;
	if ( GEW_Terrain::getSingleton()->m_heightMapRT[ 0 ] )
	{
		ret = QMessageBox::warning(
			this,
			tr( "HeightMap" ),
			tr( "Map already exist!\nDo you want to copy height map data to the new one?" ),
			QMessageBox::Yes,
			QMessageBox::No, QMessageBox::Cancel );
		if ( ret == QMessageBox::Cancel )
			return;
	}
	QString fileName;
	fileName = QFileDialog::getSaveFileName(
		this,
		QString(),
		QString(),
		tr( "Height texture (*.dds)" ) );
	if ( fileName.isEmpty() )
		return;
	GEW_Terrain::getSingleton()->createHeightMap( fileName, 1 << ( m_ui->BaseHeightMapSize->currentIndex() + 2 ), ( ret == QMessageBox::Yes ) );
	m_ui->BaseHeightMap->loadFromFile( fileName );
}

void GEW_TerrainProperty::on_NewMaskMap_clicked()
{
	int ret = QMessageBox::No;
	if ( GEW_Terrain::getSingleton()->m_maskMapRT )
	{
		ret = QMessageBox::warning(
			this,
			tr( "MaskMap" ),
			tr( "Map already exist!\nDo you want to copy mask map data to the new one?" ),
			QMessageBox::Yes,
			QMessageBox::No, QMessageBox::Cancel );
		if ( ret == QMessageBox::Cancel )
			return;
	}
	QString fileName;
	fileName = QFileDialog::getSaveFileName(
		this,
		QString(),
		QString(),
		tr( "Mask texture (*.dds)" ) );
	if ( fileName.isEmpty() )
		return;
	GEW_Terrain::getSingleton()->createMaskMap( fileName, 1 << ( m_ui->BaseMaskMapSize->currentIndex() + 2 ), ( ret == QMessageBox::Yes ) );
	m_ui->BaseMaskMap->loadFromFile( fileName );
}

void GEW_TerrainProperty::on_BaseVisualizeHorizonEnable_toggled( bool i_checked )
{
	GEW_Terrain::getSingleton()->m_horizonRUO->setVisible( i_checked );
}

void GEW_TerrainProperty::on_baseVisualizeHorizonColor_signalColorChanged( QColor i_color )
{
	GE_Vec3 color;
	color.x = i_color.redF();
	color.y = i_color.greenF();
	color.z = i_color.blueF();
	GEW_Terrain::getSingleton()->m_horizonRUO->setColor( color );
	GEW_Terrain::getSingleton()->m_brushRUO->setColor( color );
}

void GEW_TerrainProperty::on_SculptEditMode_toggled( bool i_checked )
{
	GEW_Terrain::getSingleton()->m_sculptData.Enable = i_checked;
	GEW_Terrain::getSingleton()->updateBrushMap();
	GEW_Terrain::getSingleton()->setSculptEnableChange();
	GEW_Terrain::getSingleton()->initHeightMapUndoMemory();
	GEW_Terrain::getSingleton()->initMaskMapUndoMemory();
}

void GEW_TerrainProperty::on_SculptTypeSculptPositive_toggled( bool i_checked )
{
	if ( i_checked )
		GEW_Terrain::getSingleton()->m_sculptCurrentParam.Type = GEW_Terrain::POSITIVE;
}

void GEW_TerrainProperty::on_SculptTypeSculptNegative_toggled( bool i_checked )
{
	if ( i_checked )
		GEW_Terrain::getSingleton()->m_sculptCurrentParam.Type = GEW_Terrain::NEGATIVE;
}

void GEW_TerrainProperty::on_SculptTypeSmooth_toggled( bool i_checked )
{
	if ( i_checked )
		GEW_Terrain::getSingleton()->m_sculptCurrentParam.Type = GEW_Terrain::SMOOTH;
}

void GEW_TerrainProperty::on_SculptTypeFlat_toggled( bool i_checked )
{
	if ( i_checked )
		GEW_Terrain::getSingleton()->m_sculptCurrentParam.Type = GEW_Terrain::FLAT;
}

void GEW_TerrainProperty::on_SculptModeHeight_toggled( bool i_checked )
{
	if ( i_checked )
		GEW_Terrain::getSingleton()->m_sculptCurrentParam.Mode = 0;
	GEW_Terrain::getSingleton()->initHeightMapUndoMemory();
	GEW_Terrain::getSingleton()->initMaskMapUndoMemory();
}

void GEW_TerrainProperty::on_SculptModeMaskR_toggled( bool i_checked )
{
	if ( i_checked )
		GEW_Terrain::getSingleton()->m_sculptCurrentParam.Mode = 1;
	GEW_Terrain::getSingleton()->initHeightMapUndoMemory();
	GEW_Terrain::getSingleton()->initMaskMapUndoMemory();
}

void GEW_TerrainProperty::on_SculptModeMaskG_toggled( bool i_checked )
{
	if ( i_checked )
		GEW_Terrain::getSingleton()->m_sculptCurrentParam.Mode = 2;
	GEW_Terrain::getSingleton()->initHeightMapUndoMemory();
	GEW_Terrain::getSingleton()->initMaskMapUndoMemory();
}

void GEW_TerrainProperty::on_SculptModeMaskB_toggled( bool i_checked )
{
	if ( i_checked )
		GEW_Terrain::getSingleton()->m_sculptCurrentParam.Mode = 3;
	GEW_Terrain::getSingleton()->initHeightMapUndoMemory();
	GEW_Terrain::getSingleton()->initMaskMapUndoMemory();
}

void GEW_TerrainProperty::on_SculptModeMaskA_toggled( bool i_checked )
{
	if ( i_checked )
		GEW_Terrain::getSingleton()->m_sculptCurrentParam.Mode = 4;
	GEW_Terrain::getSingleton()->initHeightMapUndoMemory();
	GEW_Terrain::getSingleton()->initMaskMapUndoMemory();
}

void GEW_TerrainProperty::on_SculptPresetSave_clicked()
{
	if ( m_ui->SculptPresets->count() < 1 )
		return;
	QString fileName = QFileDialog::getSaveFileName(
		this,
		QString(),
		QString(),
		tr( "Brushe presets file (*.Geb)" ) );
	if ( fileName.isEmpty() )
		return;

	GEW_Terrain::getSingleton()->saveBrushPresets( fileName );
}

void GEW_TerrainProperty::on_SculptPresetLoad_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(
		this,
		QString(),
		QString(),
		tr( "Brushe presets file (*.Geb)" ) );
	if ( fileName.isEmpty() )
		return;

	if ( !GEW_Terrain::getSingleton()->loadBrushPresets( fileName ) )
		return;
	m_ui->SculptPresets->clear();
	for ( uint32_t i = 0; i < ( uint32_t )GEW_Terrain::getSingleton()->m_sculptPresets.size(); ++i )
		m_ui->SculptPresets->addItem( GEW_Terrain::getSingleton()->m_sculptPresets[ i ].Name );
	on_SculptPresets_currentIndexChanged( 0 );

}
void GEW_TerrainProperty::on_SculptPresetAdd_clicked()
{
	QString presetName = QInputDialog::getText(
		this,
		tr( "Add preset" ),
		tr( "Preset name:" ),
		QLineEdit::Normal,
		tr( "SculptPreset_%1" ).arg( GEW_Terrain::getSingleton()->m_sculptPresets.size() ) );
	if ( presetName.isEmpty() )
		return;
	GEW_Terrain::getSingleton()->m_sculptPresets.push_back( GEW_Terrain::getSingleton()->m_sculptCurrentParam );
	strcpy_s( GEW_Terrain::getSingleton()->m_sculptPresets[ GEW_Terrain::getSingleton()->m_sculptPresets.size() - 1 ].Name,
		presetName.toStdString().c_str() );
	updatePresets();
	m_ui->SculptPresets->setCurrentIndex( m_ui->SculptPresets->count() - 1 );

}

void GEW_TerrainProperty::on_SculptPresetRemove_clicked()
{
	if ( GEW_Terrain::getSingleton()->m_sculptPresets.size() <= 0 || m_ui->SculptPresets->currentIndex() < 0 )
		return;
	GEW_Terrain::getSingleton()->m_sculptPresets.remove( m_ui->SculptPresets->currentIndex() );
	updatePresets();
}

void GEW_TerrainProperty::on_SculptPresets_currentIndexChanged( int i_index )
{
	if ( i_index < 0 )
		return;
	GEW_Terrain::getSingleton()->m_sculptCurrentParam = GEW_Terrain::getSingleton()->m_sculptPresets[ i_index ];
	updateSculptCurrentPreset();
}

void GEW_TerrainProperty::on_SculptSizeValue_valueChanged( double i_value )
{
	GEW_Terrain::getSingleton()->m_sculptCurrentParam.Size = GE_ConvertToUnit( ( float )i_value );
	GEW_Terrain::getSingleton()->updateBrushMap();
}

void GEW_TerrainProperty::on_SculptVolumeValue_valueChanged( double i_value )
{
	GEW_Terrain::getSingleton()->m_sculptCurrentParam.Volume = GE_ConvertToUnit( ( float )i_value );
	m_ui->SculptIntensity->blockSignals( true );
	m_ui->SculptIntensity->setValue( GEW_Terrain::getSingleton()->m_sculptCurrentParam.Volume /
		GEW_Terrain::getSingleton()->m_baseParam.MaxHeight );
	m_ui->SculptIntensity->blockSignals( false );
}

void GEW_TerrainProperty::on_SculptIntensity_valueChanged( double i_value )
{
	GEW_Terrain::getSingleton()->m_sculptCurrentParam.Volume = ( float )i_value *
		GEW_Terrain::getSingleton()->m_baseParam.MaxHeight;
	m_ui->SculptVolumeValue->blockSignals( true );
	m_ui->SculptVolumeValue->setValue( GE_ConvertToMeter( GEW_Terrain::getSingleton()->m_sculptCurrentParam.Volume ) );
	m_ui->SculptVolumeValue->blockSignals( false );
}


void GEW_TerrainProperty::on_SculptRepeatDistanceValue_valueChanged( double i_value )
{
	GEW_Terrain::getSingleton()->m_sculptCurrentParam.ReapeatDistance = GE_ConvertToUnit( ( float )i_value );
}

void GEW_TerrainProperty::on_SculptFalloffValue_valueChanged( int i_value )
{
	GEW_Terrain::getSingleton()->m_sculptCurrentParam.Falloff = ( float )i_value / 100.0f;
	GEW_Terrain::getSingleton()->updateBrushMap();
}

void GEW_TerrainProperty::on_SculptHardnessValue_valueChanged( int i_value )
{
	GEW_Terrain::getSingleton()->m_sculptCurrentParam.Hardness = ( float )i_value / 100.0f;
	GEW_Terrain::getSingleton()->updateBrushMap();
}

void GEW_TerrainProperty::on_SculptMaskPositionU_valueChanged( double i_value )
{
	GEW_Terrain::getSingleton()->m_sculptCurrentParam.MaskPositionUV.x = ( float )i_value;
}

void GEW_TerrainProperty::on_SculptMaskPositionV_valueChanged( double i_value )
{
	GEW_Terrain::getSingleton()->m_sculptCurrentParam.MaskPositionUV.y = ( float )i_value;
}

void GEW_TerrainProperty::on_SculptMaskTileU_valueChanged( double i_value )
{
	GEW_Terrain::getSingleton()->m_sculptCurrentParam.MaskTileUV.x = ( float )i_value;
}

void GEW_TerrainProperty::on_SculptMaskTileV_valueChanged( double i_value )
{
	GEW_Terrain::getSingleton()->m_sculptCurrentParam.MaskTileUV.y = ( float )i_value;
}

/*void GEW_TerrainProperty::on_SculptMaskArrayIndex_valueChanged(double i_value)
{
GEW_Terrain::getSingleton()->m_sculptCurrentParam.ArrayIndex = (uint32_t)i_value;
}*/

void GEW_TerrainProperty::on_SculptUseMask_toggled( bool i_checked )
{
	GEW_Terrain::getSingleton()->m_sculptCurrentParam.MaskEnable = i_checked;
}

void GEW_TerrainProperty::on_SculptMaskRotation_valueChanged( double i_value )
{
	GEW_Terrain::getSingleton()->m_sculptCurrentParam.Angle = GE_ConvertToRadians( ( float )i_value );
}

void GEW_TerrainProperty::on_SculptMask_signalImageChanged( QString i_filepath )
{
	GEW_Terrain::getSingleton()->m_sculptCurrentParam.MaskFileName[ 0 ] = NULL;
	strcpy_s( GEW_Terrain::getSingleton()->m_sculptCurrentParam.MaskFileName, i_filepath.toStdString().c_str() );
	GE_TextureManager::getSingleton().remove( &GEW_Terrain::getSingleton()->m_heightMaskMap );
	GEW_Terrain::getSingleton()->m_heightMaskMap = GE_TextureManager::getSingleton().createTexture( GEW_Terrain::getSingleton()->m_sculptCurrentParam.MaskFileName );
}

void GEW_TerrainProperty::on_SculptMaskRotationRandomize_toggled( bool i_checked )
{
	GEW_Terrain::getSingleton()->m_sculptCurrentParam.Randomize = i_checked;
}


void GEW_TerrainProperty::updatePresets()
{
	m_ui->SculptPresets->clear();
	for ( int i = 0; i < GEW_Terrain::getSingleton()->m_sculptPresets.size(); ++i )
		m_ui->SculptPresets->addItem( GEW_Terrain::getSingleton()->m_sculptPresets[ i ].Name );
}

void GEW_TerrainProperty::updateSculptCurrentPreset()
{
	GEW_Terrain::SculptParam *spr = &GEW_Terrain::getSingleton()->m_sculptCurrentParam;
	if ( spr->Type == GEW_Terrain::SculptType::POSITIVE )
		m_ui->SculptTypeSculptPositive->setChecked( true );
	else if ( spr->Type == GEW_Terrain::SculptType::NEGATIVE )
		m_ui->SculptTypeSculptNegative->setChecked( true );
	else if ( spr->Type == GEW_Terrain::SculptType::FLAT )
		m_ui->SculptTypeFlat->setChecked( true );
	else if ( spr->Type == GEW_Terrain::SculptType::SMOOTH )
		m_ui->SculptTypeSmooth->setChecked( true );

	if ( spr->Mode == 0 )
		m_ui->SculptModeHeight->setChecked( true );
	else if ( spr->Mode == 1 )
		m_ui->SculptModeMaskR->setChecked( true );
	else if ( spr->Mode == 2 )
		m_ui->SculptModeMaskG->setChecked( true );
	else if ( spr->Mode == 3 )
		m_ui->SculptModeMaskB->setChecked( true );
	else if ( spr->Mode == 4 )
		m_ui->SculptModeMaskA->setChecked( true );

	m_ui->SculptSizeValue->setValue( GE_ConvertToMeter( spr->Size ) );
	m_ui->SculptVolumeValue->setValue( GE_ConvertToMeter( spr->Volume ) );
	m_ui->SculptRepeatDistanceValue->setValue( GE_ConvertToMeter( spr->ReapeatDistance ) );
	m_ui->SculptFalloffValue->setValue( ( int )( spr->Falloff * 100.0f ) );
	m_ui->SculptHardnessValue->setValue( ( int )( spr->Hardness * 100.0f ) );
	m_ui->SculptUseMask->setChecked( spr->MaskEnable );
	m_ui->SculptMaskPositionU->setValue( spr->MaskPositionUV.x );
	m_ui->SculptMaskPositionV->setValue( spr->MaskPositionUV.y );
	m_ui->SculptMaskTileU->setValue( spr->MaskTileUV.x );
	m_ui->SculptMaskTileV->setValue( spr->MaskTileUV.y );
	m_ui->SculptMaskRotation->setValue( GE_ConvertToDegrees( spr->Angle ) );
	m_ui->SculptMaskRotationRandomize->setChecked( spr->Randomize );
	m_ui->SculptMask->slotLoad( spr->MaskFileName );
}

void GEW_TerrainProperty::on_SaveHeightMap_clicked()
{
	if ( GEW_Terrain::getSingleton()->m_heightMapRT[ 0 ] )
		GEW_Terrain::getSingleton()->m_heightMapRT[ 0 ]->saveRTToFile( GEW_Terrain::getSingleton()->m_baseParam.HeightMapFileName, 0 );
}

void GEW_TerrainProperty::on_SaveMaskMap_clicked()
{
	if ( GEW_Terrain::getSingleton()->m_maskMapRT[ 0 ] )
		GEW_Terrain::getSingleton()->m_maskMapRT[ 0 ]->saveRTToFile( GEW_Terrain::getSingleton()->m_baseParam.MaskMapFileName, 0 );
}

void GEW_TerrainProperty::on_SculptUndo_clicked()
{
	if ( GEW_Terrain::getSingleton()->m_sculptCurrentParam.Mode > 0 )
		GEW_Terrain::getSingleton()->undoMaskMap();
	else
		GEW_Terrain::getSingleton()->undoHeightMap();
}

void GEW_TerrainProperty::on_SculptRedo_clicked()
{
	if ( GEW_Terrain::getSingleton()->m_sculptCurrentParam.Mode > 0 )
		GEW_Terrain::getSingleton()->redoMaskMap();
	else
		GEW_Terrain::getSingleton()->redoHeightMap();
}

void GEW_TerrainProperty::on_MaterialDiffuseColor_signalColorChanged( QColor i_color )
{
	GE_Terrain::getSingleton().m_materialParameters.setBaseDiffuse( GE_Vec3( i_color.redF(), i_color.greenF(), i_color.blueF() ) );
}

void GEW_TerrainProperty::on_MaterialDiffuseColorIntensity_valueChanged( double i_value )
{
	GE_Terrain::getSingleton().m_materialParameters.setDiffuseIntensity( ( float )i_value );
}

void GEW_TerrainProperty::on_MaterialAmbientColor_signalColorChanged( QColor i_color )
{
	GE_Terrain::getSingleton().m_materialParameters.setBaseAmbient( GE_Vec3( i_color.redF(), i_color.greenF(), i_color.blueF() ) );
}

void GEW_TerrainProperty::on_MaterialAmbientColorIntensity_valueChanged( double i_value )
{
	GE_Terrain::getSingleton().m_materialParameters.setAmbientIntensity( ( float )i_value );
}

void GEW_TerrainProperty::on_MaterialSpecularColor_signalColorChanged( QColor i_color )
{
	GE_Terrain::getSingleton().m_materialParameters.setBaseSpecular( GE_Vec3( i_color.redF(), i_color.greenF(), i_color.blueF() ) );
}

void GEW_TerrainProperty::on_MaterialSpecularColorIntensity_valueChanged( double i_value )
{
	GE_Terrain::getSingleton().m_materialParameters.setSpecularIntensity( ( float )i_value );
}

void GEW_TerrainProperty::on_MaterialEmissiveColor_signalColorChanged( QColor i_color )
{
	GE_Terrain::getSingleton().m_materialParameters.setBaseEmissive( GE_Vec3( i_color.redF(), i_color.greenF(), i_color.blueF() ) );
}

void GEW_TerrainProperty::on_MaterialEmissiveColorIntensity_valueChanged( double i_value )
{
	GE_Terrain::getSingleton().m_materialParameters.setEmissiveIntensity( ( float )i_value );
}

void GEW_TerrainProperty::on_MaterialGlowColor_signalColorChanged( QColor i_color )
{
	GE_Terrain::getSingleton().m_materialParameters.setBaseGlow( GE_Vec3( i_color.redF(), i_color.greenF(), i_color.blueF() ) );
}

void GEW_TerrainProperty::on_MaterialGlowColorIntensity_valueChanged( double i_value )
{
	GE_Terrain::getSingleton().m_materialParameters.setGlowIntensity( ( float )i_value );
}

void GEW_TerrainProperty::on_MaterialSpecularPower_valueChanged( double i_value )
{
	GE_Terrain::getSingleton().m_materialParameters.SpecularPower = ( float )i_value;
}

void GEW_TerrainProperty::on_MaterialRoughness_valueChanged( double i_value )
{
	GE_Terrain::getSingleton().m_materialParameters.Roughness = ( float )i_value;
}

void GEW_TerrainProperty::on_MaterialReflectmask_valueChanged( double i_value )
{
	GE_Terrain::getSingleton().m_materialParameters.Reflectmask = ( float )i_value;
}

void GEW_TerrainProperty::on_MaterialBumpiness_valueChanged( double i_value )
{
	GE_Terrain::getSingleton().m_materialParameters.Bumpiness = ( float )i_value;
}

void GEW_TerrainProperty::on_MaterialReflectMap_signalImageChanged( QString i_filepath )
{
	if ( i_filepath.isEmpty() )
		GE_TextureManager::getSingleton().remove( &GE_Terrain::getSingleton().m_maps.ReflectMap );
	else
		GE_Terrain::getSingleton().m_maps.ReflectMap = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
}

void GEW_TerrainProperty::on_NormalNoiseMap_signalImageChanged( QString i_filepath )
{
	if ( i_filepath.isEmpty() )
		GE_TextureManager::getSingleton().remove( &GE_Terrain::getSingleton().m_maps.NormalNoiseMap );
	else
		GE_Terrain::getSingleton().m_maps.NormalNoiseMap = GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
}


void GEW_TerrainProperty::on_SplattingOpacity_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
		GE_Terrain::getSingleton().m_materialParameters.Painting[ index ].Opacity = ( float )i_value;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].Opacity = ( float )i_value;
	}
}

void GEW_TerrainProperty::on_SplattingTileU_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
		GE_Terrain::getSingleton().m_materialParameters.Painting[ index ].UVTile.x = ( float )i_value;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].UVTile.x = ( float )i_value;
	}
}


void GEW_TerrainProperty::on_NormalNoiseTileU_valueChanged( double i_value )
{
	GEW_Terrain::getSingleton( )->m_baseParam.NormalNoiseUVTile.x = ( float )i_value;
	GEW_Terrain::getSingleton()->m_baseParam.set();
}

void GEW_TerrainProperty::on_NormalNoiseTileV_valueChanged( double i_value )
{
	GEW_Terrain::getSingleton()->m_baseParam.NormalNoiseUVTile.y = ( float )i_value;
	GEW_Terrain::getSingleton( )->m_baseParam.set( );
}

void GEW_TerrainProperty::on_SplattingTileV_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
		GE_Terrain::getSingleton().m_materialParameters.Painting[ index ].UVTile.y = ( float )i_value;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].UVTile.y = ( float )i_value;
	}
}

void GEW_TerrainProperty::on_SplattingPositionU_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
		GE_Terrain::getSingleton().m_materialParameters.Painting[ index ].UVPosition.x = ( float )i_value;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].UVPosition.x = ( float )i_value;
	}
}

void GEW_TerrainProperty::on_SplattingPositionV_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
		GE_Terrain::getSingleton().m_materialParameters.Painting[ index ].UVPosition.y = ( float )i_value;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].UVPosition.y = ( float )i_value;
	}
}

void GEW_TerrainProperty::on_SplattingHeight_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].Height = GE_ConvertToUnit( ( float )i_value );
	}
}

void GEW_TerrainProperty::on_SplattingHeightField_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].HeightField = GE_ConvertToUnit( ( float )i_value );
	}
}

void GEW_TerrainProperty::on_SplattingHeightFalloff_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].HeightFalloff = ( float )i_value * 0.01f;
	}
}

void GEW_TerrainProperty::on_SplattingHeightHardness_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].HeightHardness = ( float )i_value * 0.01f;
	}
}

void GEW_TerrainProperty::on_SplattingAngle_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].Angle = GE_ConvertToRadians( ( float )i_value );
	}
}

void GEW_TerrainProperty::on_SplattingAngleField_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].AngleField = GE_ConvertToRadians( ( float )i_value );
	}
}

void GEW_TerrainProperty::on_SplattingAngleFalloff_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].AngleFalloff = ( float )i_value * 0.01f;
	}
}

void GEW_TerrainProperty::on_SplattingAngleHardness_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].AngleHardness = ( float )i_value * 0.01f;
	}
}

void GEW_TerrainProperty::on_SplattingMaskPositionX_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].MaskPosition.x = GE_ConvertToUnit( ( float )i_value );
	}
}

void GEW_TerrainProperty::on_SplattingMaskPositionY_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].MaskPosition.y = GE_ConvertToUnit( ( float )i_value );
	}
}

void GEW_TerrainProperty::on_SplattingMaskField_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].MaskField = GE_ConvertToUnit( ( float )i_value );
	}
}

void GEW_TerrainProperty::on_SplattingMaskFalloff_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].MaskFalloff = ( float )i_value * 0.01f;
	}
}

void GEW_TerrainProperty::on_SplattingMaskHardness_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].MaskHardness = ( float )i_value * 0.01f;
	}
}

void GEW_TerrainProperty::on_SplattingMaskTileU_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].MaskUVTile.x = ( float )i_value;
	}
}

void GEW_TerrainProperty::on_SplattingMaskTileV_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].MaskUVTile.y = ( float )i_value;
	}
}

void GEW_TerrainProperty::on_SplattingMaskPositionU_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].MaskUVPosition.x = ( float )i_value;
	}
}

void GEW_TerrainProperty::on_SplattingMaskPositionV_valueChanged( double i_value )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].MaskUVPosition.y = ( float )i_value;
	}
}

void GEW_TerrainProperty::on_SplattingDiffuseMap_signalImageChanged( QString i_filepath )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
		if ( i_filepath.isEmpty() )
			GE_TextureManager::getSingleton().remove( &GE_Terrain::getSingleton().m_maps.Painting[ index ].DiffuseMap );
		else
			GE_Terrain::getSingleton().m_maps.Painting[ index ].DiffuseMap =
			GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	}
	else //Procedural layers
	{
		if ( i_filepath.isEmpty() )
			GE_TextureManager::getSingleton().remove( &GE_Terrain::getSingleton().m_maps.Procedural[ index ].DiffuseMap );
		else
			GE_Terrain::getSingleton().m_maps.Procedural[ index ].DiffuseMap =
			GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	}
}

void GEW_TerrainProperty::on_SplattingNormalMap_signalImageChanged( QString i_filepath )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
		if ( i_filepath.isEmpty() )
			GE_TextureManager::getSingleton().remove( &GE_Terrain::getSingleton().m_maps.Painting[ index ].NormalMap );
		else
			GE_Terrain::getSingleton().m_maps.Painting[ index ].NormalMap =
			GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	}
	else //Procedural layers
	{
		if ( i_filepath.isEmpty() )
			GE_TextureManager::getSingleton().remove( &GE_Terrain::getSingleton().m_maps.Procedural[ index ].NormalMap );
		else
			GE_Terrain::getSingleton().m_maps.Procedural[ index ].NormalMap =
			GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	}
}

void GEW_TerrainProperty::on_SplattingGSRMap_signalImageChanged( QString i_filepath )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
		if ( i_filepath.isEmpty() )
			GE_TextureManager::getSingleton().remove( &GE_Terrain::getSingleton().m_maps.Painting[ index ].GSRMap );
		else
			GE_Terrain::getSingleton().m_maps.Painting[ index ].GSRMap =
			GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	}
	else //Procedural layers
	{
		if ( i_filepath.isEmpty() )
			GE_TextureManager::getSingleton().remove( &GE_Terrain::getSingleton().m_maps.Procedural[ index ].GSRMap );
		else
			GE_Terrain::getSingleton().m_maps.Procedural[ index ].GSRMap =
			GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	}
}


void GEW_TerrainProperty::on_SplattingMaskMap_signalImageChanged( QString i_filepath )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{

	}
	else //Procedural layers
	{
		if ( i_filepath.isEmpty() )
			GE_TextureManager::getSingleton().remove( &GE_Terrain::getSingleton().m_maps.Procedural[ index ].MaskMap );
		else
			GE_Terrain::getSingleton().m_maps.Procedural[ index ].MaskMap =
			GE_TextureManager::getSingleton().createTexture( i_filepath.toStdString().c_str() );
	}
}


void GEW_TerrainProperty::on_SplattingReloadMaps_clicked()
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
		if ( GE_Terrain::getSingleton().m_maps.Painting[ index ].DiffuseMap )
			GE_Terrain::getSingleton().m_maps.Painting[ index ].DiffuseMap->reload();
		if ( GE_Terrain::getSingleton().m_maps.Painting[ index ].NormalMap )
			GE_Terrain::getSingleton().m_maps.Painting[ index ].NormalMap->reload();
		if ( GE_Terrain::getSingleton().m_maps.Painting[ index ].GSRMap )
			GE_Terrain::getSingleton().m_maps.Painting[ index ].GSRMap->reload();
	}
	else //Procedural layers
	{
		if ( GE_Terrain::getSingleton().m_maps.Procedural[ index ].DiffuseMap )
			GE_Terrain::getSingleton().m_maps.Procedural[ index ].DiffuseMap->reload();
		if ( GE_Terrain::getSingleton().m_maps.Procedural[ index ].NormalMap )
			GE_Terrain::getSingleton().m_maps.Procedural[ index ].NormalMap->reload();
		if ( GE_Terrain::getSingleton().m_maps.Procedural[ index ].GSRMap )
			GE_Terrain::getSingleton().m_maps.Procedural[ index ].GSRMap->reload();
		if ( GE_Terrain::getSingleton().m_maps.Procedural[ index ].MaskMap )
			GE_Terrain::getSingleton().m_maps.Procedural[ index ].MaskMap->reload();
	}
	on_SplattingLayerList_currentRowChanged( m_ui->SplattingLayerList->currentRow() );
}

void GEW_TerrainProperty::on_SplattingLayerList_currentRowChanged( int i_index )
{
	if ( i_index < 0 )
		return;

	QString DiffuseMap;
	QString NormalMap;
	QString GSRMap;
	QString MaskMap;
	if ( i_index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		i_index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;

		if ( GE_Terrain::getSingleton().m_maps.Painting[ i_index ].DiffuseMap )
			DiffuseMap = GE_Terrain::getSingleton().m_maps.Painting[ i_index ].DiffuseMap->getTextureName();

		if ( GE_Terrain::getSingleton().m_maps.Painting[ i_index ].NormalMap )
			NormalMap = GE_Terrain::getSingleton().m_maps.Painting[ i_index ].NormalMap->getTextureName();

		if ( GE_Terrain::getSingleton().m_maps.Painting[ i_index ].GSRMap )
			GSRMap = GE_Terrain::getSingleton().m_maps.Painting[ i_index ].GSRMap->getTextureName();

		m_ui->SplattingOpacity->blockSignals( true );
		m_ui->SplattingOpacity->setValue( GE_Terrain::getSingleton().m_materialParameters.Painting[ i_index ].Opacity );
		m_ui->SplattingOpacity->blockSignals( false );

		m_ui->SplattingTileU->blockSignals( true );
		m_ui->SplattingTileU->setValue( GE_Terrain::getSingleton().m_materialParameters.Painting[ i_index ].UVTile.x );
		m_ui->SplattingTileU->blockSignals( false );

		m_ui->SplattingTileV->blockSignals( true );
		m_ui->SplattingTileV->setValue( GE_Terrain::getSingleton().m_materialParameters.Painting[ i_index ].UVTile.y );
		m_ui->SplattingTileV->blockSignals( false );

		m_ui->SplattingPositionU->blockSignals( true );
		m_ui->SplattingPositionU->setValue( GE_Terrain::getSingleton().m_materialParameters.Painting[ i_index ].UVPosition.x );
		m_ui->SplattingPositionU->blockSignals( false );

		m_ui->SplattingPositionV->blockSignals( true );
		m_ui->SplattingPositionV->setValue( GE_Terrain::getSingleton().m_materialParameters.Painting[ i_index ].UVPosition.y );
		m_ui->SplattingPositionV->blockSignals( false );

		m_ui->SplattingHeightGroup->setEnabled( false );
		m_ui->SplattingAngleGroup->setEnabled( false );
		m_ui->SplattingMaskGroup->setEnabled( false );
		m_ui->MaskMapGroup->setEnabled( false );

	}
	else //Procedural layers
	{

		if ( GE_Terrain::getSingleton().m_maps.Procedural[ i_index ].DiffuseMap )
			DiffuseMap = GE_Terrain::getSingleton().m_maps.Procedural[ i_index ].DiffuseMap->getTextureName();

		if ( GE_Terrain::getSingleton().m_maps.Procedural[ i_index ].NormalMap )
			NormalMap = GE_Terrain::getSingleton().m_maps.Procedural[ i_index ].NormalMap->getTextureName();

		if ( GE_Terrain::getSingleton().m_maps.Procedural[ i_index ].GSRMap )
			GSRMap = GE_Terrain::getSingleton().m_maps.Procedural[ i_index ].GSRMap->getTextureName();

		if ( GE_Terrain::getSingleton().m_maps.Procedural[ i_index ].MaskMap )
			MaskMap = GE_Terrain::getSingleton().m_maps.Procedural[ i_index ].MaskMap->getTextureName();

		m_ui->SplattingHeightGroup->setEnabled( true );
		m_ui->SplattingAngleGroup->setEnabled( true );
		m_ui->SplattingMaskGroup->setEnabled( true );
		m_ui->MaskMapGroup->setEnabled( true );

		m_ui->SplattingOpacity->blockSignals( true );
		m_ui->SplattingOpacity->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].Opacity );
		m_ui->SplattingOpacity->blockSignals( false );

		m_ui->SplattingTileU->blockSignals( true );
		m_ui->SplattingTileU->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].UVTile.x );
		m_ui->SplattingTileU->blockSignals( false );

		m_ui->SplattingTileV->blockSignals( true );
		m_ui->SplattingTileV->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].UVTile.y );
		m_ui->SplattingTileV->blockSignals( false );

		m_ui->SplattingPositionU->blockSignals( true );
		m_ui->SplattingPositionU->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].UVPosition.x );
		m_ui->SplattingPositionU->blockSignals( false );

		m_ui->SplattingPositionV->blockSignals( true );
		m_ui->SplattingPositionV->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].UVPosition.y );
		m_ui->SplattingPositionV->blockSignals( false );

		m_ui->SplattingHeight->blockSignals( true );
		m_ui->SplattingHeight->setValue( GE_ConvertToMeter( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].Height ) );
		m_ui->SplattingHeight->blockSignals( false );

		m_ui->SplattingHeightField->blockSignals( true );
		m_ui->SplattingHeightField->setValue( GE_ConvertToMeter( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].HeightField ) );
		m_ui->SplattingHeightField->blockSignals( false );

		m_ui->SplattingHeightFalloff->blockSignals( true );
		m_ui->SplattingHeightFalloff->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].HeightFalloff * 100.0f );
		m_ui->SplattingHeightFalloff->blockSignals( false );

		m_ui->SplattingHeightHardness->blockSignals( true );
		m_ui->SplattingHeightHardness->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].HeightHardness  * 100.0f );
		m_ui->SplattingHeightHardness->blockSignals( false );

		m_ui->SplattingAngle->blockSignals( true );
		m_ui->SplattingAngle->setValue( GE_ConvertToDegrees( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].Angle ) );
		m_ui->SplattingAngle->blockSignals( false );

		m_ui->SplattingAngleField->blockSignals( true );
		m_ui->SplattingAngleField->setValue( GE_ConvertToDegrees( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].AngleField ) );
		m_ui->SplattingAngleField->blockSignals( false );

		m_ui->SplattingAngleFalloff->blockSignals( true );
		m_ui->SplattingAngleFalloff->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].AngleFalloff * 100.0f );
		m_ui->SplattingAngleFalloff->blockSignals( false );

		m_ui->SplattingAngleHardness->blockSignals( true );
		m_ui->SplattingAngleHardness->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].AngleHardness * 100.0f );
		m_ui->SplattingAngleHardness->blockSignals( false );

		m_ui->SplattingMaskPositionX->blockSignals( true );
		m_ui->SplattingMaskPositionX->setValue( GE_ConvertToMeter( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].MaskPosition.x ) );
		m_ui->SplattingMaskPositionX->blockSignals( false );

		m_ui->SplattingMaskPositionY->blockSignals( true );
		m_ui->SplattingMaskPositionY->setValue( GE_ConvertToMeter( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].MaskPosition.y ) );
		m_ui->SplattingMaskPositionY->blockSignals( false );

		m_ui->SplattingMaskField->blockSignals( true );
		m_ui->SplattingMaskField->setValue( GE_ConvertToMeter( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].MaskField ) );
		m_ui->SplattingMaskField->blockSignals( false );

		m_ui->SplattingMaskFalloff->blockSignals( true );
		m_ui->SplattingMaskFalloff->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].MaskFalloff * 100.0f );
		m_ui->SplattingMaskFalloff->blockSignals( false );

		m_ui->SplattingMaskHardness->blockSignals( true );
		m_ui->SplattingMaskHardness->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].MaskHardness * 100.0f );
		m_ui->SplattingMaskHardness->blockSignals( false );

		m_ui->SplattingMaskTileU->blockSignals( true );
		m_ui->SplattingMaskTileU->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].MaskUVTile.x );
		m_ui->SplattingMaskTileU->blockSignals( false );

		m_ui->SplattingMaskTileV->blockSignals( true );
		m_ui->SplattingMaskTileV->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].MaskUVTile.y );
		m_ui->SplattingMaskTileV->blockSignals( false );

		m_ui->SplattingMaskPositionU->blockSignals( true );
		m_ui->SplattingMaskPositionU->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].MaskUVPosition.x );
		m_ui->SplattingMaskPositionU->blockSignals( false );

		m_ui->SplattingMaskPositionV->blockSignals( true );
		m_ui->SplattingMaskPositionV->setValue( GE_Terrain::getSingleton().m_materialParameters.Procedural[ i_index ].MaskUVPosition.y );
		m_ui->SplattingMaskPositionV->blockSignals( false );

	}

	m_ui->SplattingDiffuseMap->blockSignals( true );
	m_ui->SplattingDiffuseMap->loadFromFile( DiffuseMap );
	m_ui->SplattingDiffuseMap->blockSignals( false );

	m_ui->SplattingNormalMap->blockSignals( true );
	m_ui->SplattingNormalMap->loadFromFile( NormalMap );
	m_ui->SplattingNormalMap->blockSignals( false );

	m_ui->SplattingGSRMap->blockSignals( true );
	m_ui->SplattingGSRMap->loadFromFile( GSRMap );
	m_ui->SplattingGSRMap->blockSignals( false );

	m_ui->SplattingMaskMap->blockSignals( true );
	m_ui->SplattingMaskMap->loadFromFile( MaskMap );
	m_ui->SplattingMaskMap->blockSignals( false );

}

void GEW_TerrainProperty::on_SplattingLayerList_doubleClicked( QModelIndex i_modelIndex )
{
	int index = m_ui->SplattingLayerList->currentRow();
	if ( index < 0 )
		return;

	QString Name = QInputDialog::getText(
		this,
		tr( "Rename layer" ),
		tr( "Layer name:" ),
		QLineEdit::Normal,
		GEW_Terrain::getSingleton()->m_splattingListNames[ index ] );
	if ( Name.isEmpty() )
		return;
	GEW_Terrain::getSingleton()->m_splattingListNames[ index ] = Name;
	updateSculptModeAndSplattingNames();
}

void GEW_TerrainProperty::on_SplattingLayerList_itemChanged( QListWidgetItem *i_item )
{
	int index = m_ui->SplattingLayerList->row( i_item );
	if ( index < 0 )
		return;

	if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
	{
		index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
		GE_Terrain::getSingleton().m_materialParameters.Painting[ index ].Enable = ( i_item->checkState() == Qt::Checked );
	}
	else //Procedural layers
	{
		GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].Enable = ( i_item->checkState() == Qt::Checked );
	}
}

void GEW_TerrainProperty::updateSculptModeAndSplattingNames()
{
	for ( int i = 0; i < ( GE_MAX_TERRAIN_PROCEDURAL_LAYER + GE_MAX_TERRAIN_PAINT_LAYER ); ++i )
	{
		m_ui->SplattingLayerList->blockSignals( true );
		m_ui->SplattingLayerList->item( i )->setText( GEW_Terrain::getSingleton()->m_splattingListNames[ i ] );
		int index = i;
		if ( index >= GE_MAX_TERRAIN_PROCEDURAL_LAYER ) //Paint layers
		{
			index %= GE_MAX_TERRAIN_PROCEDURAL_LAYER;
			if ( index == 0 )
				m_ui->SculptModeMaskR->setToolTip( m_ui->SplattingLayerList->item( i )->text() );
			else if ( index == 1 )
				m_ui->SculptModeMaskG->setToolTip( m_ui->SplattingLayerList->item( i )->text() );
			else if ( index == 2 )
				m_ui->SculptModeMaskB->setToolTip( m_ui->SplattingLayerList->item( i )->text() );
			else if ( index == 3 )
				m_ui->SculptModeMaskA->setToolTip( m_ui->SplattingLayerList->item( i )->text() );
		}
		else
		{

			m_ui->SplattingLayerList->item( i )->setCheckState( ( GE_Terrain::getSingleton().m_materialParameters.Procedural[ index ].Enable
				? Qt::Checked : Qt::Unchecked ) );
		}
		m_ui->SplattingLayerList->blockSignals( false );
	}
}

void GEW_TerrainProperty::resetAll()
{
	GEW_Terrain::getSingleton()->resetAll();
	m_ui->MaterialReflectMap->blockSignals( true );
	m_ui->MaterialReflectMap->slotLoad( "" );
	m_ui->MaterialReflectMap->blockSignals( false );

	m_ui->NormalNoiseMap->blockSignals( true );
	m_ui->NormalNoiseMap->slotLoad( "" );
	m_ui->NormalNoiseMap->blockSignals( false );

	m_ui->BaseHeightMap->blockSignals( true );
	m_ui->BaseHeightMap->slotLoad( "" );
	m_ui->BaseHeightMap->blockSignals( false );

	m_ui->BaseMaskMap->blockSignals( true );
	m_ui->BaseMaskMap->slotLoad( "" );
	m_ui->BaseMaskMap->blockSignals( false );

	m_ui->SculptMask->blockSignals( true );
	m_ui->SculptMask->slotLoad( "" );
	m_ui->SculptMask->blockSignals( false );

	m_ui->SplattingDiffuseMap->blockSignals( true );
	m_ui->SplattingDiffuseMap->slotLoad( "" );
	m_ui->SplattingDiffuseMap->blockSignals( false );

	m_ui->SplattingNormalMap->blockSignals( true );
	m_ui->SplattingNormalMap->slotLoad( "" );
	m_ui->SplattingNormalMap->blockSignals( false );

	m_ui->SplattingGSRMap->blockSignals( true );
	m_ui->SplattingGSRMap->slotLoad( "" );
	m_ui->SplattingGSRMap->blockSignals( false );

	m_ui->SplattingMaskMap->blockSignals( true );
	m_ui->SplattingMaskMap->slotLoad( "" );
	m_ui->SplattingMaskMap->blockSignals( false );

	updateUI();
}

void GEW_TerrainProperty::updateUI()
{
	m_ui->BaseHeightMap->setSizeLimit( 2048 );
	m_ui->BaseMaskMap->setSizeLimit( 2048 );

	m_ui->TerrainEnable->blockSignals( true );
	m_ui->TerrainEnable->setChecked( GEW_Terrain::getSingleton()->m_baseData.Enable );
	m_ui->TerrainEnable->blockSignals( false );

	m_ui->BaseSize->blockSignals( true );
	m_ui->BaseSize->setValue( GE_ConvertToMeter( GEW_Terrain::getSingleton()->m_baseData.Size ) );
	m_ui->BaseSize->blockSignals( false );

	m_ui->BaseNumberOfChunks->blockSignals( true );
	m_ui->BaseNumberOfChunks->setCurrentIndex( log( GEW_Terrain::getSingleton()->m_baseData.NumberOfChunks ) / log( 2 ) - 1 );
	m_ui->BaseNumberOfChunks->blockSignals( false );

	m_ui->BaseQuadDensity->blockSignals( true );
	m_ui->BaseQuadDensity->setCurrentIndex( log( GEW_Terrain::getSingleton()->m_baseData.QuadDensity ) / log( 2 ) - 1 );
	m_ui->BaseQuadDensity->blockSignals( false );

	m_ui->BaseMaxHeight->blockSignals( true );
	m_ui->BaseMaxHeight->setValue( GE_ConvertToMeter( GEW_Terrain::getSingleton()->m_baseParam.MaxHeight ) );
	m_ui->BaseMaxHeight->blockSignals( false );
	m_ui->SculptVolumeValue->setMaximum( m_ui->BaseMaxHeight->value() );

	m_ui->BaseHeightBias->blockSignals( true );
	m_ui->BaseHeightBias->setValue( GE_ConvertToMeter( GEW_Terrain::getSingleton()->m_baseParam.HeightBias ) );
	m_ui->BaseHeightBias->blockSignals( false );

	m_ui->PhysxQuadDensity->blockSignals( true );
	m_ui->PhysxQuadDensity->setValue( GEW_Terrain::getSingleton( )->m_physxData.QuadDensity );
	m_ui->PhysxQuadDensity->blockSignals( false );

	m_ui->PhysxQuadDensityVisualize->blockSignals( true );
	m_ui->PhysxQuadDensityVisualize->setChecked( GEW_Terrain::getSingleton( )->m_physxData.VisualizeHeightfield );
	m_ui->PhysxQuadDensityVisualize->blockSignals( false );

	m_ui->BaseLODMultiplier->blockSignals( true );
	m_ui->BaseLODMultiplier->setValue( GEW_Terrain::getSingleton()->m_baseParam.LodMultiplier );
	m_ui->BaseLODMultiplier->blockSignals( false );

	m_ui->BaseNormalAspect->blockSignals( true );
	m_ui->BaseNormalAspect->setValue( GEW_Terrain::getSingleton()->m_baseParam.NormalAspect );
	m_ui->BaseNormalAspect->blockSignals( false );

	m_ui->NormalNoiseTileU->blockSignals( true );
	m_ui->NormalNoiseTileU->setValue( GEW_Terrain::getSingleton( )->m_baseParam.NormalNoiseUVTile.x );
	m_ui->NormalNoiseTileU->blockSignals( false );

	m_ui->NormalNoiseTileV->blockSignals( true );
	m_ui->NormalNoiseTileV->setValue( GEW_Terrain::getSingleton()->m_baseParam.NormalNoiseUVTile.y );
	m_ui->NormalNoiseTileV->blockSignals( false );

	m_ui->SculptEditMode->blockSignals( true );
	m_ui->SculptEditMode->setChecked( GEW_Terrain::getSingleton()->m_sculptData.Enable );
	m_ui->SculptEditMode->blockSignals( false );

	m_ui->MaterialAmbientColor->blockSignals( true );
	m_ui->MaterialAmbientColor->slotSetColor( colorFromVector( GE_Terrain::getSingleton().m_materialParameters.getBaseAmbient() ) );
	m_ui->MaterialAmbientColor->blockSignals( false );

	m_ui->MaterialDiffuseColor->blockSignals( true );
	m_ui->MaterialDiffuseColor->slotSetColor( colorFromVector( GE_Terrain::getSingleton().m_materialParameters.getBaseDiffuse() ) );
	m_ui->MaterialDiffuseColor->blockSignals( false );

	m_ui->MaterialSpecularColor->blockSignals( true );
	m_ui->MaterialSpecularColor->slotSetColor( colorFromVector( GE_Terrain::getSingleton().m_materialParameters.getBaseSpecular() ) );
	m_ui->MaterialSpecularColor->blockSignals( false );

	m_ui->MaterialEmissiveColor->blockSignals( true );
	m_ui->MaterialEmissiveColor->slotSetColor( colorFromVector( GE_Terrain::getSingleton().m_materialParameters.getBaseEmissive() ) );
	m_ui->MaterialEmissiveColor->blockSignals( false );

	m_ui->MaterialGlowColor->blockSignals( true );
	m_ui->MaterialGlowColor->slotSetColor( colorFromVector( GE_Terrain::getSingleton().m_materialParameters.getBaseGlow() ) );
	m_ui->MaterialGlowColor->blockSignals( false );

	m_ui->MaterialAmbientColorIntensity->blockSignals( true );
	m_ui->MaterialAmbientColorIntensity->setValue( GE_Terrain::getSingleton().m_materialParameters.getAmbientIntensity() );
	m_ui->MaterialAmbientColorIntensity->blockSignals( false );

	m_ui->MaterialDiffuseColorIntensity->blockSignals( true );
	m_ui->MaterialDiffuseColorIntensity->setValue( GE_Terrain::getSingleton().m_materialParameters.getDiffuseIntensity() );
	m_ui->MaterialDiffuseColorIntensity->blockSignals( false );

	m_ui->MaterialSpecularColorIntensity->blockSignals( true );
	m_ui->MaterialSpecularColorIntensity->setValue( GE_Terrain::getSingleton().m_materialParameters.getSpecularIntensity() );
	m_ui->MaterialSpecularColorIntensity->blockSignals( false );

	m_ui->MaterialEmissiveColorIntensity->blockSignals( true );
	m_ui->MaterialEmissiveColorIntensity->setValue( GE_Terrain::getSingleton().m_materialParameters.getEmissiveIntensity() );
	m_ui->MaterialEmissiveColorIntensity->blockSignals( false );

	m_ui->MaterialGlowColorIntensity->blockSignals( true );
	m_ui->MaterialGlowColorIntensity->setValue( GE_Terrain::getSingleton().m_materialParameters.getGlowIntensity() );
	m_ui->MaterialGlowColorIntensity->blockSignals( false );

	m_ui->MaterialSpecularPower->blockSignals( true );
	m_ui->MaterialSpecularPower->setValue( GE_Terrain::getSingleton().m_materialParameters.SpecularPower );
	m_ui->MaterialSpecularPower->blockSignals( false );

	m_ui->MaterialReflectmask->blockSignals( true );
	m_ui->MaterialReflectmask->setValue( GE_Terrain::getSingleton().m_materialParameters.Reflectmask );
	m_ui->MaterialReflectmask->blockSignals( false );

	m_ui->MaterialBumpiness->blockSignals( true );
	m_ui->MaterialBumpiness->setValue( GE_Terrain::getSingleton().m_materialParameters.Bumpiness );
	m_ui->MaterialBumpiness->blockSignals( false );

	m_ui->MaterialRoughness->blockSignals( true );
	m_ui->MaterialRoughness->setValue( GE_Terrain::getSingleton().m_materialParameters.Roughness );
	m_ui->MaterialRoughness->blockSignals( false );

	if ( GE_Terrain::getSingleton().m_maps.ReflectMap )
		m_ui->MaterialReflectMap->slotLoad( GE_Terrain::getSingleton().m_maps.ReflectMap->getTextureName() );

	if ( GE_Terrain::getSingleton().m_maps.NormalNoiseMap )
		m_ui->NormalNoiseMap->slotLoad( GE_Terrain::getSingleton().m_maps.NormalNoiseMap->getTextureName() );

	if ( GEW_Terrain::getSingleton()->m_baseParam.HeightMapFileName[ 0 ] )
		m_ui->BaseHeightMap->slotLoad( GEW_Terrain::getSingleton()->m_baseParam.HeightMapFileName );

	if ( GEW_Terrain::getSingleton()->m_baseParam.MaskMapFileName[ 0 ] )
		m_ui->BaseMaskMap->slotLoad( GEW_Terrain::getSingleton()->m_baseParam.MaskMapFileName );

	switch ( GEW_Terrain::getSingleton()->m_sculptCurrentParam.Type )
	{
	case GEW_Terrain::POSITIVE:
		m_ui->SculptTypeSculptPositive->setChecked( true );
		break;
	case GEW_Terrain::NEGATIVE:
		m_ui->SculptTypeSculptNegative->setChecked( true );
		break;
	case GEW_Terrain::SMOOTH:
		m_ui->SculptTypeSmooth->setChecked( true );
		break;
	case GEW_Terrain::FLAT:
		m_ui->SculptTypeFlat->setChecked( true );
		break;
	};
	updateSculptCurrentPreset();
	updateSculptModeAndSplattingNames();
}

void GEW_TerrainProperty::saveToFile( QFile &i_file, const QString &i_resourcePath )
{
	SaveData saveData;
	if ( m_ui->SculptPresets->count() > 0 )
	{
		strcpy_s( saveData.PresetsFileName, "Presets.Geb" );
		GEW_Terrain::getSingleton()->saveBrushPresets( i_resourcePath + saveData.PresetsFileName );
	}
	else
		saveData.PresetsFileName[ 0 ] = NULL;

	for ( uint32_t i = 0; i < ( uint32_t )GEW_Terrain::getSingleton()->m_splattingListNames.size(); ++i )
		strcpy_s( saveData.SplattingNames[ i ], GEW_Terrain::getSingleton()->m_splattingListNames[ i ].toStdString().c_str() );

	if ( GE_Terrain::getSingleton().m_maps.ReflectMap )
	{
		strcpy_s( saveData.ReflectMap,
			GEW_Utility::copyFile( GE_Terrain::getSingleton().m_maps.ReflectMap->getTextureName(), i_resourcePath ).c_str() );
	}

	if ( GE_Terrain::getSingleton().m_maps.NormalNoiseMap )
	{
		strcpy_s( saveData.NormalNoiseMap,
			GEW_Utility::copyFile( GE_Terrain::getSingleton().m_maps.NormalNoiseMap->getTextureName(), i_resourcePath ).c_str() );
	}

	for ( uint32_t i = 0; i < GE_MAX_TERRAIN_PAINT_LAYER; ++i )
	{
		if ( GE_Terrain::getSingleton().m_maps.Painting[ i ].DiffuseMap )
			strcpy_s( saveData.Painting[ i ].DiffuseMap,
			GEW_Utility::copyFile( GE_Terrain::getSingleton().m_maps.Painting[ i ].DiffuseMap->getTextureName(), i_resourcePath ).c_str() );

		if ( GE_Terrain::getSingleton().m_maps.Painting[ i ].NormalMap )
			strcpy_s( saveData.Painting[ i ].NormalMap,
			GEW_Utility::copyFile( GE_Terrain::getSingleton().m_maps.Painting[ i ].NormalMap->getTextureName(), i_resourcePath ).c_str() );

		if ( GE_Terrain::getSingleton().m_maps.Painting[ i ].GSRMap )
			strcpy_s( saveData.Painting[ i ].GSRMap,
			GEW_Utility::copyFile( GE_Terrain::getSingleton().m_maps.Painting[ i ].GSRMap->getTextureName(), i_resourcePath ).c_str() );
	}

	for ( uint32_t i = 0; i < GE_MAX_TERRAIN_PROCEDURAL_LAYER; ++i )
	{
		if ( GE_Terrain::getSingleton().m_maps.Procedural[ i ].DiffuseMap )
			strcpy_s( saveData.Procedural[ i ].DiffuseMap,
			GEW_Utility::copyFile( GE_Terrain::getSingleton().m_maps.Procedural[ i ].DiffuseMap->getTextureName(), i_resourcePath ).c_str() );

		if ( GE_Terrain::getSingleton().m_maps.Procedural[ i ].NormalMap )
			strcpy_s( saveData.Procedural[ i ].NormalMap,
			GEW_Utility::copyFile( GE_Terrain::getSingleton().m_maps.Procedural[ i ].NormalMap->getTextureName(), i_resourcePath ).c_str() );

		if ( GE_Terrain::getSingleton().m_maps.Procedural[ i ].GSRMap )
			strcpy_s( saveData.Procedural[ i ].GSRMap,
			GEW_Utility::copyFile( GE_Terrain::getSingleton().m_maps.Procedural[ i ].GSRMap->getTextureName(), i_resourcePath ).c_str() );

		if ( GE_Terrain::getSingleton().m_maps.Procedural[ i ].MaskMap )
			strcpy_s( saveData.Procedural[ i ].MaskMap,
			GEW_Utility::copyFile( GE_Terrain::getSingleton().m_maps.Procedural[ i ].MaskMap->getTextureName(), i_resourcePath ).c_str() );
	}

	i_file.write( ( char * )&saveData, sizeof( SaveData ) );

	on_SaveHeightMap_clicked();
	on_SaveMaskMap_clicked();

	strcpy_s( GEW_Terrain::getSingleton()->m_baseParam.HeightMapFileName,
		GEW_Utility::copyFile( GEW_Terrain::getSingleton()->m_baseParam.HeightMapFileName, i_resourcePath ).c_str() );

	strcpy_s( GEW_Terrain::getSingleton()->m_baseParam.MaskMapFileName,
		GEW_Utility::copyFile( GEW_Terrain::getSingleton()->m_baseParam.MaskMapFileName, i_resourcePath ).c_str() );

	i_file.write( ( char * )&GEW_Terrain::getSingleton()->m_baseData, sizeof( GEW_Terrain::BaseData ) );
	i_file.write( ( char * )&GEW_Terrain::getSingleton()->m_baseParam, sizeof( GEW_Terrain::BaseParam ) );
	i_file.write( ( char * )&GEW_Terrain::getSingleton()->m_sculptData, sizeof( GEW_Terrain::SculptData ) );
	i_file.write( ( char * )&GEW_Terrain::getSingleton( )->m_physxData, sizeof( GEW_Terrain::PhysxData ) );
	i_file.write( ( char * )&GE_Terrain::getSingleton().m_materialParameters, sizeof( GE_Terrain::MaterialParameters ) );

	GE_Terrain::getSingleton().m_maps.~Maps();
	for ( uint32_t i = 0; i < GE_MAX_TERRAIN_PAINT_LAYER; ++i )
	{
		if ( saveData.Painting[ i ].DiffuseMap[ 0 ] )
			GE_Terrain::getSingleton().m_maps.Painting[ i ].DiffuseMap =
			GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.Painting[ i ].DiffuseMap, i_resourcePath ).c_str() );

		if ( saveData.Painting[ i ].NormalMap[ 0 ] )
			GE_Terrain::getSingleton().m_maps.Painting[ i ].NormalMap =
			GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.Painting[ i ].NormalMap, i_resourcePath ).c_str() );

		if ( saveData.Painting[ i ].GSRMap[ 0 ] )
			GE_Terrain::getSingleton().m_maps.Painting[ i ].GSRMap =
			GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.Painting[ i ].GSRMap, i_resourcePath ).c_str() );
	}

	for ( uint32_t i = 0; i < GE_MAX_TERRAIN_PROCEDURAL_LAYER; ++i )
	{
		if ( saveData.Procedural[ i ].DiffuseMap[ 0 ] )
			GE_Terrain::getSingleton().m_maps.Procedural[ i ].DiffuseMap =
			GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.Procedural[ i ].DiffuseMap, i_resourcePath ).c_str() );

		if ( saveData.Procedural[ i ].NormalMap[ 0 ] )
			GE_Terrain::getSingleton().m_maps.Procedural[ i ].NormalMap =
			GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.Procedural[ i ].NormalMap, i_resourcePath ).c_str() );

		if ( saveData.Procedural[ i ].GSRMap[ 0 ] )
			GE_Terrain::getSingleton().m_maps.Procedural[ i ].GSRMap =
			GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.Procedural[ i ].GSRMap, i_resourcePath ).c_str() );

		if ( saveData.Procedural[ i ].MaskMap[ 0 ] )
			GE_Terrain::getSingleton().m_maps.Procedural[ i ].MaskMap =
			GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.Procedural[ i ].MaskMap, i_resourcePath ).c_str() );
	}

	m_ui->BaseHeightMap->slotLoad( GEW_Utility::absolutePath( GEW_Terrain::getSingleton()->m_baseParam.HeightMapFileName, i_resourcePath ).c_str() );
	m_ui->BaseMaskMap->slotLoad( GEW_Utility::absolutePath( GEW_Terrain::getSingleton()->m_baseParam.MaskMapFileName, i_resourcePath ).c_str() );
	m_ui->MaterialReflectMap->slotLoad( GEW_Utility::absolutePath( saveData.ReflectMap, i_resourcePath ).c_str() );
}

void GEW_TerrainProperty::loadFromFile( QFile &i_file, const QString &i_resourcePath )
{
	SaveData saveData;
	resetAll();
	i_file.read( ( char * )&saveData, sizeof( SaveData ) );
	GEW_Terrain::getSingleton()->m_splattingListNames.clear();
	for ( uint32_t i = 0; i < ( GE_MAX_TERRAIN_PAINT_LAYER + GE_MAX_TERRAIN_PROCEDURAL_LAYER ); ++i )
		GEW_Terrain::getSingleton()->m_splattingListNames.push_back( saveData.SplattingNames[ i ] );
	GE_Terrain::getSingleton().m_maps.~Maps();
	if ( saveData.ReflectMap[ 0 ] )
		GE_Terrain::getSingleton().m_maps.ReflectMap =
		GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.ReflectMap, i_resourcePath ).c_str() );

	if ( saveData.NormalNoiseMap[ 0 ] )
		GE_Terrain::getSingleton().m_maps.NormalNoiseMap =
		GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.NormalNoiseMap, i_resourcePath ).c_str() );

	for ( uint32_t i = 0; i < GE_MAX_TERRAIN_PAINT_LAYER; ++i )
	{
		if ( saveData.Painting[ i ].DiffuseMap[ 0 ] )
			GE_Terrain::getSingleton().m_maps.Painting[ i ].DiffuseMap =
			GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.Painting[ i ].DiffuseMap, i_resourcePath ).c_str() );

		if ( saveData.Painting[ i ].NormalMap[ 0 ] )
			GE_Terrain::getSingleton().m_maps.Painting[ i ].NormalMap =
			GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.Painting[ i ].NormalMap, i_resourcePath ).c_str() );

		if ( saveData.Painting[ i ].GSRMap[ 0 ] )
			GE_Terrain::getSingleton().m_maps.Painting[ i ].GSRMap =
			GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.Painting[ i ].GSRMap, i_resourcePath ).c_str() );
	}

	for ( uint32_t i = 0; i < GE_MAX_TERRAIN_PROCEDURAL_LAYER; ++i )
	{
		if ( saveData.Procedural[ i ].DiffuseMap[ 0 ] )
			GE_Terrain::getSingleton().m_maps.Procedural[ i ].DiffuseMap =
			GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.Procedural[ i ].DiffuseMap, i_resourcePath ).c_str() );

		if ( saveData.Procedural[ i ].NormalMap[ 0 ] )
			GE_Terrain::getSingleton().m_maps.Procedural[ i ].NormalMap =
			GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.Procedural[ i ].NormalMap, i_resourcePath ).c_str() );

		if ( saveData.Procedural[ i ].GSRMap[ 0 ] )
			GE_Terrain::getSingleton().m_maps.Procedural[ i ].GSRMap =
			GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.Procedural[ i ].GSRMap, i_resourcePath ).c_str() );

		if ( saveData.Procedural[ i ].MaskMap[ 0 ] )
			GE_Terrain::getSingleton().m_maps.Procedural[ i ].MaskMap =
			GE_TextureManager::getSingleton().createTexture( GEW_Utility::absolutePath( saveData.Procedural[ i ].MaskMap, i_resourcePath ).c_str() );
	}

	i_file.read( ( char * )&GEW_Terrain::getSingleton()->m_baseData, sizeof( GEW_Terrain::BaseData ) );
	i_file.read( ( char * )&GEW_Terrain::getSingleton()->m_baseParam, sizeof( GEW_Terrain::BaseParam ) );
	i_file.read( ( char * )&GEW_Terrain::getSingleton()->m_sculptData, sizeof( GEW_Terrain::SculptData ) );
	i_file.read( ( char * )&GEW_Terrain::getSingleton( )->m_physxData, sizeof( GEW_Terrain::PhysxData ) );
	i_file.read( ( char * )&GE_Terrain::getSingleton().m_materialParameters, sizeof( GE_Terrain::MaterialParameters ) );
	if ( GEW_Terrain::getSingleton()->m_baseParam.HeightMapFileName[ 0 ] )
		strcpy_s( GEW_Terrain::getSingleton()->m_baseParam.HeightMapFileName,
		GEW_Utility::absolutePath( GEW_Terrain::getSingleton()->m_baseParam.HeightMapFileName, i_resourcePath ).c_str() );

	if ( GEW_Terrain::getSingleton()->m_baseParam.MaskMapFileName[ 0 ] )
		strcpy_s( GEW_Terrain::getSingleton()->m_baseParam.MaskMapFileName,
		GEW_Utility::absolutePath( GEW_Terrain::getSingleton()->m_baseParam.MaskMapFileName, i_resourcePath ).c_str() );

	updateUI();
	if ( saveData.PresetsFileName[ 0 ] )
		GEW_Terrain::getSingleton()->loadBrushPresets( GEW_Utility::absolutePath( saveData.PresetsFileName, i_resourcePath ).c_str() );
	m_ui->SculptPresets->clear();
	for ( uint32_t i = 0; i < ( uint32_t )GEW_Terrain::getSingleton()->m_sculptPresets.size(); ++i )
		m_ui->SculptPresets->addItem( GEW_Terrain::getSingleton()->m_sculptPresets[ i ].Name );
	if ( ( uint32_t )GEW_Terrain::getSingleton()->m_sculptPresets.size() > 0 )
		on_SculptPresets_currentIndexChanged( 0 );
	GEW_Terrain::getSingleton()->m_baseData.set();
	GEW_Terrain::getSingleton()->m_baseParam.set();
	GEW_Terrain::getSingleton()->m_physxData.set();
	GEW_Terrain::getSingleton( )->updateBrushMap( );
}

GEW_TerrainProperty * GEW_TerrainProperty::m_singleton = NULL;


GEW_TerrainProperty::PaintMaps::PaintMaps()
{
	memset( this, 0, sizeof( PaintMaps ) );
}

GEW_TerrainProperty::PaintMaps::~PaintMaps()
{

}

GEW_TerrainProperty::ProceduralMaps::ProceduralMaps()
{
	memset( this, 0, sizeof( ProceduralMaps ) );
}

GEW_TerrainProperty::ProceduralMaps::~ProceduralMaps()
{

}

GEW_TerrainProperty::SaveData::SaveData()
{
	memset( this, 0, sizeof( SaveData ) );
}

GEW_TerrainProperty::SaveData::~SaveData()
{

}

void GEW_TerrainProperty::on_PhysxQuadDensity_valueChanged( int i_value )
{
	uint32_t quad = m_ui->PhysxQuadDensity->value();
	quad = ( ( quad % 2 ) ? quad + 1 : quad );
	GEW_Terrain::getSingleton()->m_physxData.QuadDensity = quad;
	GEW_Terrain::getSingleton( )->m_physxData.set( );
}

void GEW_TerrainProperty::on_PhysxQuadDensityVisualize_toggled( bool i_checked )
{
	GEW_Terrain::getSingleton()->m_physxData.VisualizeHeightfield = i_checked;
}
