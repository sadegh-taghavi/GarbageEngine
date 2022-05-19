#pragma once
#include <DirectXMath.h>
#include <DirectXCollision.h>

struct GE_Vec3
{
	float x;
	float y;
	float z;

	GE_Vec3(){}
	GE_Vec3(float i_x, float i_y, float i_z);
	GE_Vec3(const float *i_array);
	GE_Vec3(const GE_Vec3 &i_vec);

	GE_Vec3 operator+(const GE_Vec3 &i_v);
	GE_Vec3& operator+=(const GE_Vec3 &i_v);
	GE_Vec3 operator-();
	GE_Vec3 operator-(const GE_Vec3 &i_v);
	GE_Vec3& operator-=(const GE_Vec3 &i_v);
	GE_Vec3 operator*(float i_scaler);
	GE_Vec3& operator*=(float i_scaler);
	GE_Vec3 operator/(float i_scaler);
	GE_Vec3& operator/=(float i_scaler);
	bool operator==(const GE_Vec3 &i_v);
	bool operator!=(const GE_Vec3 &i_v);
	float length();
	GE_Vec3& transform(const struct GE_Mat4x4 *i_mat);
	void transformOut( GE_Vec3 *i_out, const struct GE_Mat4x4 *i_mat );
	GE_Vec3& normalize();
	GE_Vec3& normalizeBy(const GE_Vec3* i_vec);
	void normalizeOut(GE_Vec3* i_out);
	float dot(const GE_Vec3 *i_vec);
	GE_Vec3& cross(const GE_Vec3 *i_vec);
	void crossOut(GE_Vec3 *i_out, const GE_Vec3 *i_vec);
	void lerp( const GE_Vec3 *i_second, float i_amount );
	void lerpOut( GE_Vec3 *i_out, const GE_Vec3 *i_second, float i_amount );
};

void GE_Vec3Transform( GE_Vec3 *i_out, const GE_Vec3 *i_vec, const struct GE_Mat4x4 *i_mat );
float GE_Vec3Length(const GE_Vec3 *i_vec);
void GE_Vec3Normalize(GE_Vec3 *i_out, const GE_Vec3 *i_vec);
float GE_Vec3Dot(const GE_Vec3 *i_v1, const GE_Vec3 *i_v2);
void GE_Vec3Cross(GE_Vec3 *i_out, const GE_Vec3 *i_v1, const GE_Vec3 *i_v2);
void GE_Vec3Lerp( GE_Vec3 *i_out, const GE_Vec3 *i_first, const GE_Vec3 *i_second, float i_amount );
bool GE_Vec3RayIntersectToPlane(const GE_Vec3 *i_org, const GE_Vec3 *i_dir, const GE_Vec3 *i_verts, float &i_distance);
bool GE_Vec3RayIntersectToBox( const GE_Vec3 *i_org, const GE_Vec3 *i_dir,
	const GE_Vec3 *i_min, const GE_Vec3 *i_max, const GE_Mat4x4 *i_transform, float &i_distance );

#include "GE_Vec3.inl"