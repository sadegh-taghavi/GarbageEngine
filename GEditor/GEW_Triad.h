#pragma once
#include <list>
#include <GE_Math.h>
#include <GE_View.h>
#include <GE_Math.h>
#include "GEW_Input.h"

class GEW_Triad
{
	class GE_RenderUtilityObject	*m_xRUO[ 2 ];
	class GE_RenderUtilityObject	*m_yRUO[ 2 ];
	class GE_RenderUtilityObject	*m_zRUO[ 2 ];
	class GE_Model					*m_boxModel;
	struct GE_Material				*m_boxMaterial;
	GEW_Triad();
	~GEW_Triad();
	std::list<class GE_RenderUtilityObject * > m_listRUO;
	std::list<class GE_Model * > m_listModel;
	static GEW_Triad m_singleton;
public:
	class GE_View					*m_view;
	static GEW_Triad *getSingleton(){ return &m_singleton; }
	void init();
	void update( GEW_Input::Data &i_inputData, GE_View &i_view, const GE_Vec2 &i_cursorPos, const GE_Vec2 &i_widthHeight );
	void render();
};

