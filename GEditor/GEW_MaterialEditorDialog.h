#pragma once

#include <QDialog>

namespace Ui { class GEW_MaterialEditorDialog; }

class GEW_MaterialEditorDialog : public QDialog
{
	Q_OBJECT
public:
	GEW_MaterialEditorDialog( QWidget *i_parent = 0 );
	~GEW_MaterialEditorDialog();

	void setMaterial( struct GE_Material *i_material );
	struct GE_Material * getMaterial();
	
signals:
	void signalMaterialChanged( struct GE_Material * );
	
public slots:
	void on_diffuseAlphaMap1_signalImageChanged( QString i_filepath );
	void on_diffuseAlphaMap2_signalImageChanged( QString i_filepath );
	void on_normalHeightMap1_signalImageChanged( QString i_filepath );
	void on_normalHeightMap2_signalImageChanged( QString i_filepath );
	void on_glowSpecularReflectMaskMap1_signalImageChanged( QString i_filepath );
	void on_glowSpecularReflectMaskMap2_signalImageChanged( QString i_filepath );
	void on_reflectMap_signalImageChanged( QString i_filepath );
	void on_tintMap_signalImageChanged(QString i_filepath);

	void on_diffuseColorIntensity_valueChanged( double i_value );
	void on_ambientColorIntensity_valueChanged( double i_value );
	void on_specularColorIntensity_valueChanged( double i_value );
	void on_emissiveColorIntensity_valueChanged( double i_value );
	void on_glowColorIntensity_valueChanged( double i_value );

	void on_specularPower_valueChanged( double i_value );
	void on_roughness_valueChanged( double i_value );
	void on_reflectmask_valueChanged( double i_value );
	void on_bumpiness_valueChanged( double i_value );
	void on_alphaTest_valueChanged( double i_value );
	void on_refractness_valueChanged(double i_value);
	void on_refract_valueChanged(double i_value);
	void on_tint_valueChanged(double i_value);
	void on_tintSpread_valueChanged(double i_value);

	void on_diffuseColor_signalColorChanged( QColor i_color );
	void on_ambientColor_signalColorChanged( QColor i_color );
	void on_specularColor_signalColorChanged( QColor i_color );
	void on_emissiveColor_signalColorChanged( QColor i_color );
	void on_glowColor_signalColorChanged( QColor i_color );

	void on_reloadMaps_clicked();

private:
	Ui::GEW_MaterialEditorDialog *m_ui;
	struct GE_Material *m_material;
};
