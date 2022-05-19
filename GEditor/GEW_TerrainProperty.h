#pragma once

#include <QWidget>
#include <QListWidget>
#include <QFile>
#include <GE_Terrain.h>
#define MAX_FILE_NAME 512
namespace Ui { class TerrainProperty; }
class GEW_TerrainProperty : public QWidget
{
	Q_OBJECT
	Ui::TerrainProperty *m_ui;
	struct PaintMaps
	{
		char DiffuseMap[MAX_FILE_NAME];
		char NormalMap[MAX_FILE_NAME];
		char GSRMap[MAX_FILE_NAME];
		PaintMaps();
		~PaintMaps();
	};
	struct ProceduralMaps
	{
		char DiffuseMap[MAX_FILE_NAME];
		char NormalMap[MAX_FILE_NAME];
		char GSRMap[MAX_FILE_NAME];
		char MaskMap[MAX_FILE_NAME];
		ProceduralMaps();
		~ProceduralMaps();
	};
	struct SaveData
	{
		char PresetsFileName[MAX_FILE_NAME];
		char SplattingNames[GE_MAX_TERRAIN_PAINT_LAYER + GE_MAX_TERRAIN_PROCEDURAL_LAYER][MAX_FILE_NAME];
		char ReflectMap[MAX_FILE_NAME];
		char NormalNoiseMap[ MAX_FILE_NAME ];
		PaintMaps Painting[GE_MAX_TERRAIN_PAINT_LAYER];
		ProceduralMaps Procedural[GE_MAX_TERRAIN_PROCEDURAL_LAYER];
		SaveData();
		~SaveData();
	};
public:
	void baseApply( );
	static GEW_TerrainProperty *m_singleton;
	static GEW_TerrainProperty &getSingleton() { return *m_singleton; }
	explicit GEW_TerrainProperty(QWidget *i_parent = 0);
	~GEW_TerrainProperty();
	void updatePresets();
	void updateSculptCurrentPreset();
	void updateSculptModeAndSplattingNames();
	void updateUI();
	void resetAll();
	void saveToFile(QFile &i_file, const QString &i_resourcePath);
	void loadFromFile(QFile &i_file, const QString &i_resourcePath);
private slots:
	void on_TerrainEnable_toggled(bool i_checked);
	void on_baseVisualizeHorizonColor_signalColorChanged(QColor i_color);
	void on_BaseVisualizeHorizonEnable_toggled(bool i_checked);
	void on_BaseSize_valueChanged(double i_value);
	void on_BaseNumberOfChunks_currentIndexChanged(int i_index);
	void on_BaseQuadDensity_currentIndexChanged(int i_index);
	void on_BaseMaxHeight_valueChanged(double i_value);
	void on_BaseHeightBias_valueChanged(double i_value);
	void on_BaseNormalAspect_valueChanged(double i_value);
	void on_BaseLODMultiplier_valueChanged(double i_value);
	void on_NewHeightMap_clicked();
	void on_NewMaskMap_clicked();
	void on_SaveHeightMap_clicked();
	void on_SaveMaskMap_clicked();
	void on_BaseHeightMap_signalImageChanged(QString i_filepath);
	void on_BaseMaskMap_signalImageChanged(QString i_filepath);

	void on_SculptUndo_clicked();
	void on_SculptRedo_clicked();

	void on_SculptEditMode_toggled(bool i_checked);
	void on_SculptTypeSculptPositive_toggled(bool i_checked);
	void on_SculptTypeSculptNegative_toggled(bool i_checked);
	void on_SculptTypeSmooth_toggled(bool i_checked);
	void on_SculptTypeFlat_toggled(bool i_checked);
	void on_SculptModeHeight_toggled(bool i_checked);
	void on_SculptModeMaskR_toggled(bool i_checked);
	void on_SculptModeMaskG_toggled(bool i_checked);
	void on_SculptModeMaskB_toggled(bool i_checked);
	void on_SculptModeMaskA_toggled(bool i_checked);
	void on_SculptPresetAdd_clicked();
	void on_SculptPresetRemove_clicked();
	void on_SculptPresetSave_clicked();
	void on_SculptPresetLoad_clicked();
	void on_SculptPresets_currentIndexChanged(int i_index);
	void on_SculptSizeValue_valueChanged(double i_value);
	void on_SculptVolumeValue_valueChanged(double i_value);
	void on_SculptIntensity_valueChanged(double i_value);
	void on_SculptRepeatDistanceValue_valueChanged(double i_value);
	void on_SculptFalloffValue_valueChanged(int i_value);
	void on_SculptHardnessValue_valueChanged(int i_value);
	void on_SculptMaskPositionU_valueChanged(double i_value);
	void on_SculptMaskPositionV_valueChanged(double i_value);
	void on_SculptMaskTileU_valueChanged(double i_value);
	void on_SculptMaskTileV_valueChanged(double i_value);
//	void on_SculptMaskArrayIndex_valueChanged(double i_value);
	void on_SculptUseMask_toggled(bool i_checked);
	void on_SculptMaskRotationRandomize_toggled(bool i_checked);
	void on_SculptMaskRotation_valueChanged(double i_value);
	void on_SculptMask_signalImageChanged(QString i_filepath);
	
	void on_MaterialDiffuseColor_signalColorChanged(QColor i_color);
	void on_MaterialDiffuseColorIntensity_valueChanged(double i_value);
	void on_MaterialAmbientColor_signalColorChanged(QColor i_color);
	void on_MaterialAmbientColorIntensity_valueChanged(double i_value);
	void on_MaterialSpecularColor_signalColorChanged(QColor i_color);
	void on_MaterialSpecularColorIntensity_valueChanged(double i_value);
	void on_MaterialEmissiveColor_signalColorChanged(QColor i_color);
	void on_MaterialEmissiveColorIntensity_valueChanged(double i_value);
	void on_MaterialGlowColor_signalColorChanged(QColor i_color);
	void on_MaterialGlowColorIntensity_valueChanged(double i_value);

	void on_MaterialSpecularPower_valueChanged(double i_value);
	void on_MaterialRoughness_valueChanged(double i_value);
	void on_MaterialReflectmask_valueChanged(double i_value);
	void on_MaterialBumpiness_valueChanged(double i_value);
	void on_MaterialReflectMap_signalImageChanged(QString i_filepath);
	void on_NormalNoiseMap_signalImageChanged( QString i_filepath );
	void on_NormalNoiseTileU_valueChanged( double i_value );
	void on_NormalNoiseTileV_valueChanged( double i_value );

	void on_SplattingOpacity_valueChanged(double i_value);
	void on_SplattingTileU_valueChanged(double i_value);
	void on_SplattingTileV_valueChanged(double i_value);
	void on_SplattingPositionU_valueChanged(double i_value);
	void on_SplattingPositionV_valueChanged(double i_value);
	void on_SplattingHeight_valueChanged(double i_value);
	void on_SplattingHeightField_valueChanged(double i_value);
	void on_SplattingHeightFalloff_valueChanged(double i_value);
	void on_SplattingHeightHardness_valueChanged(double i_value);
	void on_SplattingAngle_valueChanged(double i_value);
	void on_SplattingAngleField_valueChanged(double i_value);
	void on_SplattingAngleFalloff_valueChanged(double i_value);
	void on_SplattingAngleHardness_valueChanged(double i_value);

	void on_SplattingMaskPositionX_valueChanged(double i_value);
	void on_SplattingMaskPositionY_valueChanged(double i_value);
	void on_SplattingMaskField_valueChanged(double i_value);
	void on_SplattingMaskFalloff_valueChanged(double i_value);
	void on_SplattingMaskHardness_valueChanged(double i_value);
	void on_SplattingMaskTileU_valueChanged(double i_value);
	void on_SplattingMaskTileV_valueChanged(double i_value);
	void on_SplattingMaskPositionU_valueChanged(double i_value);
	void on_SplattingMaskPositionV_valueChanged(double i_value);
	
	void on_SplattingDiffuseMap_signalImageChanged(QString i_filepath);
	void on_SplattingNormalMap_signalImageChanged(QString i_filepath);
	void on_SplattingGSRMap_signalImageChanged(QString i_filepath);
	void on_SplattingMaskMap_signalImageChanged(QString i_filepath);
	void on_SplattingReloadMaps_clicked();
	void on_SplattingLayerList_currentRowChanged( int i_index );
	void on_SplattingLayerList_doubleClicked(QModelIndex i_modelIndex);
	void on_SplattingLayerList_itemChanged(QListWidgetItem *i_item);

	void on_PhysxQuadDensity_valueChanged( int i_value );
	void on_PhysxQuadDensityVisualize_toggled( bool i_checked );
};

