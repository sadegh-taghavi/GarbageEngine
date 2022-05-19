#include "GE_Bound.h"
#include "GE_Math.h"

void GE_Bound::transform(const GE_Mat4x4 *i_matrix)
{
	Transform = *i_matrix;
	RawBoundBoxMin.transformOut(&AABoundBoxMin, i_matrix);
	RawBoundBoxMax.transformOut(&AABoundBoxMax, i_matrix);
	BoundCenter = (AABoundBoxMax + AABoundBoxMin) * 0.5f;
	BoundRadius = (AABoundBoxMax - AABoundBoxMin).length() * 0.5f * BoundRadiusMultiply;
}
