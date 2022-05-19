#pragma once
#include <QWidget>

namespace Ui { class ScreenshotProperty; }
class GEW_ScreenshotProperty : public QWidget
{
	Q_OBJECT

	Ui::ScreenshotProperty *m_ui;
public:
	explicit GEW_ScreenshotProperty(QWidget *i_parent= NULL);
	~GEW_ScreenshotProperty();
	float getAspect();
private slots:
	void on_Preset_currentIndexChanged(int i_index);
	void on_Aspect_currentIndexChanged(int i_index);
	void on_Width_valueChanged(int i_value);
	void on_Height_valueChanged(int i_value);
	void on_AspectFromScene_clicked();
	void on_PresetSave_clicked();
	void on_PresetDelete_clicked();
	void on_Capture_clicked();

};

