#pragma once
class GEW_DefaultCamera
{
	GEW_DefaultCamera();
	~GEW_DefaultCamera();
	static GEW_DefaultCamera m_singleton;
public:
	struct Settings
	{
		float Near;
		float Far;
		float FOV;
		Settings();
		~Settings( );
		void reset();
	}m_settings;
	static GEW_DefaultCamera *getSingleton(){ return &m_singleton; }

};

