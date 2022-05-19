#pragma once

#include <QWidget>
#include <GE_Math.h>

namespace Ui { class EnvironmentProbeProperty; }

class GEW_EnvironmentProbeProperty : public QWidget
{
	Q_OBJECT

	Ui::EnvironmentProbeProperty *m_ui;
public:
	explicit GEW_EnvironmentProbeProperty( QWidget *i_parent );
	~GEW_EnvironmentProbeProperty();
	class GEW_EnvironmentProbe *m_probe;
	void setProbe( class GEW_EnvironmentProbe *i_probe );
private slots:
	void on_Enable_toggled( bool i_checked );
	void on_BoundVisible_toggled( bool i_checked );
	void on_Radius_valueChanged( double i_value );
	void on_BoundColor_signalColorChanged( QColor i_color );
	void on_Map_signalImageChanged( QString i_fileName );
	void on_SelectOutputPath_clicked( );
	void on_MapSize_currentIndexChanged( int i_index );
	void on_Bake_clicked( );
};