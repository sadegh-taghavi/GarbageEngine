#include "ui_GEW_ScreenshotProperty.h"
#include "GEW_ScreenshotProperty.h"
#include "GEW_Screenshot.h"
#include <QFileDialog>
#include <QStringList>
#include <QSettings>
#include "GEW_Path.h"
#include "GEW_Viewport.h"

GEW_ScreenshotProperty::GEW_ScreenshotProperty(QWidget *i_parent) : QWidget(i_parent),
m_ui(new Ui::ScreenshotProperty)
{
	m_ui->setupUi(this);
	QSettings settings( GEW_SETTINGS_FILE, QSettings::IniFormat );
	int count = settings.beginReadArray("screenshot_presets");
	for (int i = 0; i < count; ++i)
	{
		settings.setArrayIndex(i);
		m_ui->Preset->addItem( settings.value("preset").toString() );
	}
	settings.endArray();
}


GEW_ScreenshotProperty::~GEW_ScreenshotProperty()
{
	delete m_ui;
}

void GEW_ScreenshotProperty::on_Preset_currentIndexChanged(int i_index)
{
	QString str = m_ui->Preset->currentText();
	if (str.isEmpty())
		return;
	m_ui->Width->blockSignals(true);
	m_ui->Height->blockSignals(true);
	m_ui->Aspect->blockSignals(true);
	QStringList sl = str.split("x");
	m_ui->Width->setValue(sl[0].toInt());
	sl = sl[1].split("<>");
	m_ui->Height->setValue(sl[0].toInt());
	m_ui->Aspect->setCurrentText( sl[1] );
	m_ui->Width->blockSignals(false);
	m_ui->Height->blockSignals(false);
	m_ui->Aspect->blockSignals(false);
}

void GEW_ScreenshotProperty::on_Width_valueChanged(int i_value)
{
	float aspect = getAspect();
	if (!aspect)
		return;
	m_ui->Height->blockSignals(true);
	m_ui->Height->setValue((int)((float)m_ui->Width->value() / aspect));
	m_ui->Height->blockSignals(false);
}

void GEW_ScreenshotProperty::on_Height_valueChanged(int i_value)
{
	float aspect = getAspect();
	if (!aspect)
		return;
	m_ui->Width->blockSignals(true);
	m_ui->Width->setValue((int)((float)m_ui->Height->value() * aspect));
	m_ui->Width->blockSignals(false);
}

void GEW_ScreenshotProperty::on_PresetSave_clicked()
{
	QString pre = m_ui->Width->text() + "x" + m_ui->Height->text() + "<>" + m_ui->Aspect->currentText();
	if (m_ui->Preset->findText(pre) >= 0)
		return;
	m_ui->Preset->addItem(pre);
	m_ui->Preset->setCurrentIndex(m_ui->Preset->count() - 1);
	QSettings settings( GEW_SETTINGS_FILE, QSettings::IniFormat );
	settings.beginWriteArray("screenshot_presets");
	for (int i = 0; i < m_ui->Preset->count(); ++i)
	{
		settings.setArrayIndex(i);
		settings.setValue("preset",m_ui->Preset->itemText(i));
	}
	settings.endArray();
}

void GEW_ScreenshotProperty::on_PresetDelete_clicked()
{
	m_ui->Preset->removeItem(m_ui->Preset->currentIndex());
	QSettings settings("geditor.ini", QSettings::IniFormat);
	settings.beginWriteArray("screenshot_presets");
	for (int i = 0; i < m_ui->Preset->count(); ++i)
	{
		settings.setArrayIndex(i);
		settings.setValue("preset", m_ui->Preset->itemText(i));
	}
	settings.endArray();
}

void GEW_ScreenshotProperty::on_Aspect_currentIndexChanged(int i_index)
{
	float aspect = getAspect();
	if (!aspect)
		return;
	m_ui->Height->blockSignals(true);
	m_ui->Height->setValue((int)((float)m_ui->Width->value() / aspect));
	m_ui->Height->blockSignals(false);
}

void GEW_ScreenshotProperty::on_Capture_clicked()
{
	GEW_Screenshot::getSingleton()->m_fileName = QFileDialog::getSaveFileName(
		this,
		QString("Screenshot output file"),
		QString("Screenshot"),
		tr("TARGA file (*.tga);;DDS file (*.dds)"));
	if (GEW_Screenshot::getSingleton()->m_fileName.isEmpty())
		return;

	GEW_Screenshot::getSingleton()->m_captureParameters.Width = m_ui->Width->value();
	GEW_Screenshot::getSingleton()->m_captureParameters.Height = m_ui->Height->value();
	GEW_Screenshot::getSingleton()->capture();
}

float GEW_ScreenshotProperty::getAspect()
{
	if (m_ui->Aspect->currentIndex() == 0)
		return 0;
	QStringList sl = m_ui->Aspect->itemText(m_ui->Aspect->currentIndex()).split(":");
	return sl[0].toFloat() / sl[1].toFloat();
}

void GEW_ScreenshotProperty::on_AspectFromScene_clicked()
{
	if (!GEW_Screenshot::getSingleton()->m_viewport)
		return;
	float aspect = (float)GEW_Screenshot::getSingleton()->m_viewport->width() / 
		(float)GEW_Screenshot::getSingleton()->m_viewport->height();
	if (!aspect)
		return;
	m_ui->Aspect->setCurrentIndex(0);
	m_ui->Height->blockSignals(true);
	m_ui->Height->setValue((int)((float)m_ui->Width->value() / aspect));
	m_ui->Height->blockSignals(false);
}
