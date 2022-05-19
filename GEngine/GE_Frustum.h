#pragma once
#include <DirectXMath.h>
#include "GE_Vec4.h"
#include "GE_Vec3.h"
#include "GE_Mat4x4.h"

struct GE_Frustum
{
	enum Result
	{
		OutSide	=0,
		Colled	=1,
		Inside	=2
	};

	GE_Vec4		Planes[6];

	GE_Frustum();
	~GE_Frustum(){}
	void calculatePlanes( const GE_Mat4x4 *i_viewProjection );
	Result sphereInFrustum( const GE_Vec3* i_position, float i_radius );
	Result cubeInFrustum( const GE_Vec3* i_position, const GE_Vec3 *i_halfSize );
	bool polygonListInFrustum( const GE_Vec3* i_polylist, uint32_t i_numPolys );
};

#include "GE_Frustum.inl"