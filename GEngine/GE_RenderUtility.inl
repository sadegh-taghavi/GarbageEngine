#include "GE_RenderUtility.h"

inline void GE_RenderUtilityObject::setColor( const GE_Vec3 &i_color )
{
	m_color = i_color;
}

inline GE_Vec3 & GE_RenderUtilityObject::getColor()
{
	return m_color;
}

inline void GE_RenderUtilityObject::setScale( GE_Vec3 &i_scale )
{
	m_scale = i_scale; 
	m_transformChange = true;
}

inline void GE_RenderUtilityObject::setRotation( GE_Quat &i_rotation )
{
	m_rotation = i_rotation; 
	m_transformChange = true;
}

inline void GE_RenderUtilityObject::setPosition( GE_Vec3 &i_position )
{
	m_position = i_position; 
	m_transformChange = true;
}

inline GE_Vec3 & GE_RenderUtilityObject::getScale()
{
	return m_scale;
}

inline GE_Quat & GE_RenderUtilityObject::getRotation()
{
	return m_rotation;
}

inline GE_Vec3 & GE_RenderUtilityObject::getPosition()
{
	return m_position;
}

inline GE_Mat4x4 * GE_RenderUtilityObject::getWorld()
{
	if(m_transformForce)
		return m_transformForce;
	return &m_world;
}

inline GE_Mat4x4 *GE_RenderUtilityObject::getTransformBy()
{
	return m_transformBy;
}

inline GE_Mat4x4 *GE_RenderUtilityObject::getTransformForce()
{
	return m_transformForce;
}

inline void GE_RenderUtilityObject::setTransformBy( const GE_Mat4x4 *i_transform )
{
	m_transformBy = (GE_Mat4x4*)i_transform;
}

inline void GE_RenderUtilityObject::setTransformForce( const GE_Mat4x4 *i_transform )
{
	m_transformForce = (GE_Mat4x4*)i_transform;
}

inline void GE_RenderUtilityObject::setDepthDisable( bool i_depthDisable )
{
	m_depthDisable = i_depthDisable;
}

inline bool & GE_RenderUtilityObject::isDepthDisable()
{
	return m_depthDisable;
}

inline void GE_RenderUtilityObject::setVisible( bool i_visible )
{
	m_visible = i_visible;
}

inline bool & GE_RenderUtilityObject::isVisible()
{
	return m_visible;
}

inline void GE_RenderUtilityObject::setAlphaTest( float i_alphaTest )
{
	m_alphaTest=i_alphaTest;
}

inline float & GE_RenderUtilityObject::getAlphaTest()
{
	return m_alphaTest;
}

inline void GE_RenderUtilityObject::setBoundRadiusMultiply( float i_multiply )
{
	m_bound.BoundRadiusMultiply = i_multiply;
	if( m_transformForce )
		m_bound.transform( m_transformForce );
	else
		m_bound.transform( &m_world );
}

inline GE_Bound & GE_RenderUtilityObject::getBound()
{
	return m_bound;
}

inline void GE_RenderUtilityObject::indicesDataEndEdit()
{
	m_indicesChange = true;
}

inline std::vector<int> * GE_RenderUtilityObject::indicesDataBeginEdit()
{
	return &m_indicesData;
}

inline void GE_RenderUtilityObject::colorsDataEndEdit()
{
	m_colorChange = true;
}

inline std::vector<GE_Vec3> * GE_RenderUtilityObject::colorsDataBeginEdit()
{
	return &m_colorsData;
}

inline void GE_RenderUtilityObject::verticesDataEndEdit()
{
	m_verticesChange = true;
}

inline std::vector<GE_Vec3> * GE_RenderUtilityObject::verticesDataBeginEdit()
{
	return &m_verticesData;
}

inline void GE_RenderUtilityObject::texcoordDataEndEdit()
{
	m_texcoordChange = true;
}

inline std::vector<GE_Vec2> * GE_RenderUtilityObject::texcoordDataBeginEdit()
{
	return &m_texcoordData;
}

inline void GE_RenderUtilityObject::setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY i_topology )
{
	m_topology = i_topology;
}

inline void GE_RenderUtilityObject::setSRV( ID3D11ShaderResourceView *i_sRV/*=NULL*/ )
{
	m_sRV = i_sRV;
}

inline void GE_RenderUtilityObject::setBillboard( bool i_billboard )
{
	m_billboard = i_billboard;
}

inline bool & GE_RenderUtilityObject::isBillboard()
{
	return m_billboard;
}