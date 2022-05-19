#include "GEW_Screenshot.h"
#include "GEW_Viewport.h"
#include "GEW_Scene.h"
#include <GE_Core.h>
#include <GE_RenderTarget.h>
GEW_Screenshot GEW_Screenshot::m_singleton;

GEW_Screenshot::GEW_Screenshot()
{
	m_capture = 0;
	m_viewport = NULL;
}

GEW_Screenshot::~GEW_Screenshot()
{
}

void GEW_Screenshot::update(GEW_Viewport *i_viewport, GEW_Scene *i_scene)
{
	m_viewport = i_viewport;
	if (m_capture == 0)
		return;
	if (m_capture == 5)
	{
		m_width = (int32_t)GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Width;
		m_height = (int32_t)GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Height;

		GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Width = m_captureParameters.Width;
		GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Height = m_captureParameters.Height;
		GE_Core::getSingleton().getCurrentSwapchain()->resize();
		i_scene->resize((int32_t)m_captureParameters.Width, (int32_t)m_captureParameters.Height);
	}
	else if ( m_capture == 1 )
	{
		GE_RenderTarget::Parameters par(1);
		par.Width[0] = (uint32_t)m_captureParameters.Width;
		par.Height[0] = (uint32_t)m_captureParameters.Height;
		GE_RenderTarget *rt = new GE_RenderTarget(par);
		GE_Core::getSingleton().getImmediateContext()->CopyResource(rt->getRTTexture(0),
			GE_Core::getSingleton().getCurrentSwapchain()->BackBuffer);
		rt->saveRTToFile(m_fileName.toStdString().c_str(), 0);
		delete rt;

		GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Width = m_width;
		GE_Core::getSingleton().getCurrentSwapchain()->ViewPort.Height = m_height;
		GE_Core::getSingleton().getCurrentSwapchain()->resize();
		i_scene->resize(m_width, m_height);
	}
	m_capture--;
}

void GEW_Screenshot::capture()
{
	m_capture = 5;
}

GEW_Screenshot::captureParameters::captureParameters()
{
	Width = 4096;
	Height = 2304;
}

GEW_Screenshot::captureParameters::~captureParameters()
{

}
