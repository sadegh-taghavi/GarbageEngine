#pragma once
#include "GE_BasicRenderer.h"

inline GE_BasicRenderer* GE_BasicRenderer::getSingleton( )
{
	return &m_singleton;
}

inline ID3D11InputLayout * GE_BasicRenderer::getInputLayout( )
{
	return m_vertexDeclaration;
}
