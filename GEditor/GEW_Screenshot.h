#pragma once
#include <stdint.h>
#include <GE_Math.h>
#include <QFile>

class GEW_Screenshot
{
	static GEW_Screenshot m_singleton;
	GEW_Screenshot();
	~GEW_Screenshot();
	int32_t m_capture;
	int32_t m_width;
	int32_t m_height;
public:
	class GEW_Viewport *m_viewport;
	struct captureParameters
	{
		uint32_t Width;
		uint32_t Height;
		captureParameters();
		~captureParameters();
	}m_captureParameters;
	QString m_fileName;
	static GEW_Screenshot *getSingleton(){ return &m_singleton; }
	void capture();
	void update(class GEW_Viewport *i_viewport, class GEW_Scene *i_scene);
};

