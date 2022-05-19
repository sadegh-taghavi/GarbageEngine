#pragma once
#include <DirectXMath.h>
#include <memory.h>
#include "GE_Vec3.h"
#include "GE_Quat.h"

struct GE_Mat4x4
{
	union
	{
		struct
		{
			float _00, _01, _02, _03;
			float _10, _11, _12, _13;
			float _20, _21, _22, _23;
			float _30, _31, _32, _33;
		};
		float _m[4][4];
		float m[16];
	};
	GE_Mat4x4(){}
	GE_Mat4x4(	float i_00, float i_01, float i_02, float i_03,
				float i_10, float i_11, float i_12, float i_13,
				float i_20, float i_21, float i_22, float i_23,
				float i_30, float i_31, float i_32, float i_33);
	GE_Mat4x4(const float *i_array);
	GE_Mat4x4(const GE_Mat4x4 &i_mat44);

	float		operator()(size_t i_row, size_t i_column) const;
	float&		operator()(size_t i_row, size_t i_column);
	GE_Mat4x4&	operator= (const GE_Mat4x4& i_mat44);
	GE_Mat4x4	operator*(const GE_Mat4x4 &i_mat44);
	GE_Mat4x4&	operator*=(const GE_Mat4x4 &i_mat44);
	GE_Mat4x4&	multiply(const GE_Mat4x4 *i_mat44);
	void		multiplyOut(GE_Mat4x4 *i_out, const GE_Mat4x4 *i_mat44);
	GE_Mat4x4&	srp(const GE_Vec3 &i_position, const GE_Quat &i_rotation, const GE_Vec3 &i_scale);
	GE_Mat4x4&	spr( const GE_Vec3 &i_position, const GE_Quat &i_rotation, const GE_Vec3 &i_scale );
	GE_Mat4x4&	transform( const GE_Vec3 &i_p );
	void		transformOut( GE_Mat4x4 *i_out, const GE_Vec3 &i_p );
	GE_Mat4x4&	lookAtRH(const GE_Vec3 *i_position, const GE_Vec3 *i_target, const GE_Vec3 *i_up);
	GE_Mat4x4&	inverse();
	GE_Mat4x4&	inverseBy(const GE_Mat4x4 *i_mat);
	void		inverseOut(GE_Mat4x4 *i_out);
	GE_Mat4x4&	orthoRH(float i_w, float i_h, float i_near, float i_far);
	GE_Mat4x4&	orthoCenterRH( float i_wLeft, float i_wRight, float i_hBottom, float i_hTop, float i_near, float i_far );
	GE_Mat4x4&	perspectiveFovRH(float i_fov, float i_aspect, float i_near, float i_far);
	GE_Mat4x4&	transpose();
	GE_Mat4x4&	transposeBy(const GE_Mat4x4 *i_mat);
	void		transposeOut(GE_Mat4x4 *i_out);
	void		decompose( GE_Vec3 *i_outPosition, GE_Quat *i_outRotation, GE_Vec3 *i_outScale );
	GE_Mat4x4&	identity();
	GE_Mat4x4&	RotationQuaternion(const GE_Quat *i_q);
	GE_Mat4x4&	transformation2D(const GE_Vec2 *i_scaleCenter, float i_scalingRotation, const GE_Vec2 *i_scale, 
		const GE_Vec2 *i_rotationCenter, float i_rotation, const GE_Vec2 *i_position);
};

void GE_Mat4x4Decompose(const GE_Mat4x4 *i_m, GE_Vec3 *i_outPosition, GE_Quat *i_outRotation, GE_Vec3 *i_outScale );
void GE_Mat4x4Multiply(GE_Mat4x4 *i_out, const GE_Mat4x4 *i_m1, const GE_Mat4x4 *i_m2);
void GE_Mat4x4SRP(GE_Mat4x4 *i_out, const GE_Vec3 &i_position, const GE_Quat &i_rotation, const GE_Vec3 &i_scale);
void GE_Mat4x4Transform( GE_Mat4x4 *i_out, const GE_Vec3 &i_p );
void GE_Mat4x4LookAtRH(GE_Mat4x4 *i_out, const GE_Vec3 *i_position, const GE_Vec3 *i_target, const GE_Vec3 *i_up);
void GE_Mat4x4Inverse(GE_Mat4x4 *i_out, const GE_Mat4x4 *i_mat);
void GE_Mat4x4OrthoRH(GE_Mat4x4 *i_out, float i_w, float i_h, float i_near, float i_far);
void GE_Mat4x4OrthoCenterRH( GE_Mat4x4 *i_out, float i_wLeft, float i_wRight, float i_hBottom, float i_hTop, float i_near, float i_far );
void GE_Mat4x4PerspectiveFovRH(GE_Mat4x4 *i_out, float i_fov, float i_aspect, float i_near, float i_far);
void GE_Mat4x4Transpose(GE_Mat4x4 *i_out, const GE_Mat4x4 *i_mat);
void GE_Mat4x4Identity(GE_Mat4x4 *i_mat);
void GE_Mat4x4RotationQuaternion(GE_Mat4x4 *i_out, const GE_Quat *i_q);
void GE_Mat4x4Transformation2D(GE_Mat4x4 *i_out, const GE_Vec2 *i_scaleCenter, float i_scalingRotation, const GE_Vec2 *i_scale, 
							const GE_Vec2 *i_rotationCenter, float i_rotation, const GE_Vec2 *i_position);


#include "GE_Mat4x4.inl"