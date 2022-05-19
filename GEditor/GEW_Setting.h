#pragma once

#include <QWidget>
#include <QFile>
#include <QString>
#include <stdint.h>
namespace Ui { class GEW_Setting; }

class GEW_Setting : public QWidget
{
	Q_OBJECT
	Ui::GEW_Setting *m_ui;
	static GEW_Setting *m_singleton;
public:
	explicit GEW_Setting( QWidget *i_parent = 0 );
	~GEW_Setting();
	static GEW_Setting &getSingleton( ) { return *m_singleton; }
	struct Parameters
	{
		bool		Render;
		bool		TextVisible;
		bool		GameMode;
		bool		WireFrameShadeMode;
		bool		PhysicsSimulation;
		bool		PhysicsSingleStep;
		bool		PhysicsReset;
		uint32_t	AnisotropicLevel;
		uint32_t	FXAAQuality;
		uint32_t	FPSLimit;
		uint32_t	SyncLevel;
	}m_parameters;

	void saveToFile( QFile &i_file, const QString &i_resourcePath );
	void loadFromFile( QFile &i_file, const QString &i_resourcePath );

public slots:
	void showBar();

private slots:
	void on_Info_toggled( bool i_checked );
	void on_GameMode_toggled( bool i_checked );
	void on_ShadeMode_toggled( bool i_checked );
	void on_PhysicsSimulation_toggled( bool i_checked );
	void on_PhysicsSingleStep_clicked();
	void on_PhysicsReset_clicked();
	void on_AnisotropicLevel_currentIndexChanged( int i_index );
	void on_FXAAQuality_currentIndexChanged( int i_index );
	void on_Render_toggled( bool i_checked );
	void on_FPSLimit_valueChanged( int i_value );
	void on_SyncLevel_currentIndexChanged( int i_index );
};