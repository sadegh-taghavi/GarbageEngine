#pragma once
#include <QWidget>

namespace Ui { class DefaultCameraProperty; }
class GEW_DefaultCameraProperty : public QWidget
{
	Q_OBJECT

	Ui::DefaultCameraProperty *m_ui;
	static GEW_DefaultCameraProperty *m_singleton;
public:
	explicit GEW_DefaultCameraProperty( QWidget *i_parent = NULL );
	~GEW_DefaultCameraProperty( );
	static GEW_DefaultCameraProperty *getSingleton( ){ return m_singleton; }
public slots:
	void on_Near_valueChanged( double i_value );
	void on_Far_valueChanged( double i_value );
	void on_FOV_valueChanged( double i_value );
	void on_Reset_clicked();
	void on_Set_clicked( );
};

