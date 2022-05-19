#pragma once
#include "GE_Vec3.h"
#include "GE_Mat4x4.h"

struct GE_Bound
{
	GE_Vec3		RawBoundBoxMin, AABoundBoxMin;
	GE_Vec3		RawBoundBoxMax, AABoundBoxMax;
	GE_Vec3		RawBoundCenter;
	float		RawBoundRadius;
	float		BoundRadius;
	GE_Vec3		BoundCenter;
	float		BoundRadiusMultiply;
	GE_Mat4x4   Transform;
	void transform( const GE_Mat4x4 *i_matrix );
};