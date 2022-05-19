#include "GE_EnvironmentProbe.h"


inline bool GE_EnvironmentProbe::isEnable()
{
	return m_enable;
}

inline void GE_EnvironmentProbe::setEnable( bool i_enable )
{
	m_enable = i_enable;
}

inline GE_Vec3 &GE_EnvironmentProbe::getPosition()
{
	return m_position;
}

inline GE_Vec3 & GE_EnvironmentProbe::getScale( )
{
	return m_scale;
}

inline void GE_EnvironmentProbe::setScale( GE_Vec3 &i_scale )
{
	m_scale = i_scale;
}

inline GE_Vec2 &GE_EnvironmentProbe::getMapSize()
{
	return m_mapSize;
}
