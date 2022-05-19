#pragma once
#include <list>
#include <stdint.h>
#include <GEW_Input.h>

class GE_Model;
class GE_RenderUtilityObject;
class GE_Light;
class GE_Sprite;
class GE_Decal;
struct GE_Occluder;
class GE_EnvironmentProbe;
class GEW_FreeCamera;
class GEW_Item;
class GEW_Text;
class GEW_Viewport;

class GEW_Scene
{
public:
	struct ShadowModels
	{
		GE_Light *Light;
		std::list< std::list< GE_Model * > * > ModelLists;
		std::list< std::list< GE_Model * > * > TwoSideModelLists;
		ShadowModels()
		{
			Light = NULL;
		}
		~ShadowModels()
		{
			auto itTwo = TwoSideModelLists.begin();
			for ( auto it = ModelLists.begin(); it != ModelLists.end(); it++ )
			{
				delete ( *it );
				delete( *itTwo );
				itTwo++;
			}

			ModelLists.clear();
			TwoSideModelLists.clear();
		}
	};

	static GEW_Scene &getSingleton() { return m_singleton; }
	void update( GEW_Input::Data &i_inputData );
	void render();
	void init();
	void resize( int i_width, int i_height );
	void setViewPort( GEW_Viewport *i_viewport );
	GEW_FreeCamera	*m_camera;
	GEW_Viewport *m_viewport;
private:
	GEW_Scene();
	~GEW_Scene();

	static GEW_Scene m_singleton;

	float		m_frameRate;
	float		m_lastRate;
	float		m_time;
	uint32_t	m_counter;

	class GE_StreamMap *m_streamMap;

	GEW_Text								*m_text;
	std::list< GE_Model* >					m_renderModelList;
	std::list< GE_Model* >					m_renderTwoSideModelList;
	std::list< GE_Model* >					m_renderTransparentModelList;
	std::list< GE_Model* >					m_renderTransparentTwoSideModelList;
	std::list< GE_RenderUtilityObject* >	m_renderRouList;
	std::list< GE_Light* >					m_renderLightList;
	std::list< ShadowModels >				m_shadowModelsList;
	std::list< GE_Sprite* >					m_renderSpriteList;
	std::list< GE_Occluder* >				m_renderOccluderList;
	std::list< GE_EnvironmentProbe* >		m_renderEnvironmentProbList;
	std::list< GE_Decal *>					m_renderDecalList;
	GE_Light								*m_sunLight;
};