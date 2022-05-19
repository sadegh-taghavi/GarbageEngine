#pragma once
#include <GE_Math.h>
#include <QString>

class GEW_EnvironmentProbe
{
public:
	class GE_EnvironmentProbe *m_probe;
	float	m_radius;
	QString m_Path;
	class GE_Texture *m_map;
	GE_Vec3 m_boundColor;
	class GE_RenderUtilityObject *m_bound;
	void update();
	GEW_EnvironmentProbe();
	~GEW_EnvironmentProbe();
};

