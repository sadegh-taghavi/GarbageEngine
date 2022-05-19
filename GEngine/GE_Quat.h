#pragma once
#include <DirectXMath.h>
#include "GE_Vec3.h"

struct GE_Quat
{
	float x;
	float y;
	float z;
	float w;

	GE_Quat(){}
	GE_Quat(float i_x, float i_y, float i_z, float i_w);
	GE_Quat(const float *i_array);
	GE_Quat(const GE_Quat &i_q);

	bool operator==(const GE_Quat &i_q);
	GE_Quat& operator*=(const GE_Quat &i_q);
	GE_Quat operator*(const GE_Quat &i_q);
	bool operator!=(const GE_Quat &i_q);
	GE_Quat& multiply(const GE_Quat *i_mat44);
	void	 multiplyOut(GE_Quat *i_out, const GE_Quat *i_q);
	GE_Quat& identity();
	GE_Quat& rotationAxis(const GE_Vec3 *i_axis, float i_angle);
	void rotationAxisOut(GE_Quat *i_out, const GE_Vec3 *i_axis, float i_angle);
	GE_Quat& rotationYPR(const GE_Vec3 *i_ypr);
	void rotationYPROut(GE_Quat *i_out, const GE_Vec3 *i_ypr);
	GE_Quat& normalize();
	void normalizeOut(GE_Quat *i_out);
	void toYPR(GE_Vec3 *i_out);
	void lerp( const GE_Quat *i_second, float i_amount );
	void lerpOut( GE_Quat *i_out, const GE_Quat *i_second, float i_amount );
};
void GE_QuatRotationAxis(GE_Quat *i_out, const GE_Vec3 *i_axis, float i_angle);
void GE_QuatNormalize(GE_Quat *i_out, const GE_Quat *i_q);
void GE_QuatLerp( GE_Quat *i_out, const GE_Quat *i_first, const GE_Quat *i_second, float i_amount );
void GE_QuatMultiply(GE_Quat *i_out, const GE_Quat *i_q1, const GE_Quat *i_q2);

#include "GE_Quat.inl"