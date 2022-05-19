#include "GEW_Triad.h"
#include <GE_View.h>
#include <GE_RenderUtility.h>
#include <GE_Model.h>
#include <GE_Texture.h>
#include <GE_Material.h>
#include <GE_BasicRenderer.h>
#include "GEW_Path.h"
GEW_Triad GEW_Triad::m_singleton;

GEW_Triad::GEW_Triad()
{
	m_xRUO[0] = NULL;
	m_xRUO[1] = NULL;

	m_yRUO[0] = NULL;
	m_yRUO[1] = NULL;

	m_zRUO[0] = NULL;
	m_zRUO[1] = NULL;

	m_boxModel = NULL;

	m_boxMaterial = NULL;

	m_view = NULL;
}


GEW_Triad::~GEW_Triad()
{
	SAFE_DELETE(m_xRUO[0]);
	SAFE_DELETE(m_xRUO[1]);
	SAFE_DELETE(m_yRUO[0]);
	SAFE_DELETE(m_yRUO[1]);
	SAFE_DELETE(m_zRUO[0]);
	SAFE_DELETE(m_zRUO[1]);
	SAFE_DELETE(m_boxModel);
	SAFE_DELETE(m_boxMaterial);
	SAFE_DELETE(m_view);
}

void GEW_Triad::update(GEW_Input::Data &i_inputData, GE_View &i_view, const GE_Vec2 &i_cursorPos, const GE_Vec2 &i_widthHeight)
{
	m_view->m_position = (i_view.m_position - i_view.m_target).normalize() * 100.0f;
	m_view->m_orthoRight = i_widthHeight.x;
	m_view->m_orthoTop = i_widthHeight.y;

	m_view->updateDepth();
	m_view->update();
	/*GE_Vec3 org = m_view->m_position;
	GE_Vec3 dir, right, up;
	dir = (m_view->m_target - m_view->m_position).normalize();
	dir.crossOut(&right, &m_view->m_uP);
	right.crossOut(&up, &dir);
	right *= i_cursorPos.x + m_view->m_orthoLeft;
	up *= (i_widthHeight.y + m_view->m_orthoBottom) - i_cursorPos.y;
	org = m_view->m_position + right + up;*/

	m_boxModel->update();
	m_xRUO[0]->update();
	m_xRUO[1]->update();
	m_yRUO[0]->update();
	m_yRUO[1]->update();
	m_zRUO[0]->update();
	m_zRUO[1]->update();
	/*float dis;
	if (GE_Vec3RayIntersectToBox(&org, &dir, &m_boxModel->getBound().AABoundBoxMin,
		&m_boxModel->getBound().AABoundBoxMax, &GE_Mat4x4().identity(), dis))
	{
		m_boxMaterial->AlphaTest = 2.0f;
	}
	else
	{
		m_boxMaterial->AlphaTest = 0.0f;
	}*/

}

void GEW_Triad::render()
{
	list<GE_Model *>emptylist;
	GE_BasicRenderer::getSingleton().render(&m_listModel, &emptylist, &m_view->m_matViewProjection);
	GE_RenderUtility::getSingleton().render(&m_listRUO, m_view, NULL);
}

void GEW_Triad::init()
{
	if (m_view)
		return;

	m_view = new GE_View();
	m_view->m_isOrtho = true;
	m_view->m_near = GE_ConvertToUnit(0.1f);
	m_view->m_far = GE_ConvertToUnit(100.0f);
	m_view->m_isCenterOrtho = true;
	m_view->m_target = GE_Vec3(0.0f, 0.0f, 0.0f);
	m_view->m_orthoLeft = -40;
	m_view->m_orthoBottom = -40;

	float size = 25.0f;
	float space = 25.0f;
	GE_Vec3 ruoSize = GE_Vec3(20.0f, 20.0f, 20.0f);
	m_xRUO[0] = new GE_RenderUtilityObject();
	m_xRUO[0]->createPlane(GE_Vec3(0.0f, 0.0f, 0.0f), GE_Quat().identity(), ruoSize);
	m_xRUO[0]->setTexture((string(GEW_TEXTURE_PATH) + "TriadX.dds").c_str());
	m_xRUO[0]->setBillboard(true);
	m_xRUO[0]->setDepthDisable(false);
	m_xRUO[0]->setPosition(GE_Vec3(space, 0.0f, 0.0f));
	m_xRUO[0]->setAlphaTest(0.5f);
	m_listRUO.push_back(m_xRUO[0]);

	m_xRUO[1] = new GE_RenderUtilityObject();
	m_xRUO[1]->createPlane(GE_Vec3(0.0f, 0.0f, 0.0f), GE_Quat().identity(), ruoSize);
	m_xRUO[1]->setTexture((string(GEW_TEXTURE_PATH) + "Triad-X.dds").c_str());
	m_xRUO[1]->setBillboard(true);
	m_xRUO[1]->setDepthDisable(false);
	m_xRUO[1]->setPosition(GE_Vec3(-space, 0.0f, 0.0f));
	m_xRUO[1]->setAlphaTest(0.5f);
	m_listRUO.push_back(m_xRUO[1]);

	m_yRUO[0] = new GE_RenderUtilityObject();
	m_yRUO[0]->createPlane(GE_Vec3(0.0f, 0.0f, 0.0f), GE_Quat().identity(), ruoSize);
	m_yRUO[0]->setTexture((string(GEW_TEXTURE_PATH) + "TriadY.dds").c_str());
	m_yRUO[0]->setBillboard(true);
	m_yRUO[0]->setDepthDisable(false);
	m_yRUO[0]->setPosition(GE_Vec3(0.0f, space, 0.0f));
	m_yRUO[0]->setAlphaTest(0.5f);
	m_listRUO.push_back(m_yRUO[0]);

	m_yRUO[1] = new GE_RenderUtilityObject();
	m_yRUO[1]->createPlane(GE_Vec3(0.0f, 0.0f, 0.0f), GE_Quat().identity(), ruoSize);
	m_yRUO[1]->setTexture((string(GEW_TEXTURE_PATH) + "Triad-Y.dds").c_str());
	m_yRUO[1]->setBillboard(true);
	m_yRUO[1]->setDepthDisable(false);
	m_yRUO[1]->setPosition(GE_Vec3(0.0f, -space, 0.0f));
	m_yRUO[1]->setAlphaTest(0.5f);
	m_listRUO.push_back(m_yRUO[1]);

	m_zRUO[0] = new GE_RenderUtilityObject();
	m_zRUO[0]->createPlane(GE_Vec3(0.0f, 0.0f, 0.0f), GE_Quat().identity(), ruoSize);
	m_zRUO[0]->setTexture((string(GEW_TEXTURE_PATH) + "TriadZ.dds").c_str());
	m_zRUO[0]->setBillboard(true);
	m_zRUO[0]->setDepthDisable(false);
	m_zRUO[0]->setPosition(GE_Vec3(0.0f, 0.0f, space));
	m_zRUO[0]->setAlphaTest(0.5f);
	m_listRUO.push_back(m_zRUO[0]);

	m_zRUO[1] = new GE_RenderUtilityObject();
	m_zRUO[1]->createPlane(GE_Vec3(0.0f, 0.0f, 0.0f), GE_Quat().identity(), ruoSize);
	m_zRUO[1]->setTexture((string(GEW_TEXTURE_PATH) + "Triad-Z.dds").c_str());
	m_zRUO[1]->setBillboard(true);
	m_zRUO[1]->setDepthDisable(false);
	m_zRUO[1]->setPosition(GE_Vec3(0.0f, 0.0f, -space));
	m_zRUO[1]->setAlphaTest(0.5f);
	m_listRUO.push_back(m_zRUO[1]);

	m_boxModel = new GE_Model((string(GEW_MODEL_PATH) + "TriadBox.GModel").c_str());
	m_boxMaterial = new GE_Material();
	m_boxMaterial->DiffuseAlphaMap[0] = GE_TextureManager::getSingleton().createTexture((string(GEW_TEXTURE_PATH) + "TriadBox.dds").c_str());
	m_boxModel->setMaterial(0, m_boxMaterial);
	m_boxModel->setScale(GE_Vec3(size, size, size));
	m_listModel.push_back(m_boxModel);

}
