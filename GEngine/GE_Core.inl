#pragma once
#include "GE_Core.h"

inline const GE_Core::RenderState * GE_Core::getRenderStates( )
{
	return &m_renderStates;
}

inline const float * GE_Core::getFrameRate( )
{
	return &m_frameRate;
}

inline const float * GE_Core::getElapsedTime( )
{
	return &m_elapsedTime;
}

inline GE_Core* GE_Core::getSingleton()
{
	return &m_singleton;
}

inline D3D_FEATURE_LEVEL GE_Core::getFeautureLevel()
{
	return m_featureLevel;
}

inline ID3D11Device* GE_Core::getDevice()
{
	return m_pd3dDevice;
}

inline ID3D11DeviceContext* GE_Core::getImmediateContext()
{
	return m_pImmediateContext;
}
