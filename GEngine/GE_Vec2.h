#pragma once
#include <DirectXMath.h>

struct GE_Mat4x4;

struct GE_Vec2
{
	float x;
	float y;

	GE_Vec2(){}
	GE_Vec2(float i_x, float i_y);
	GE_Vec2(const float *i_array);
	GE_Vec2(const GE_Vec2 &i_vec);

	GE_Vec2 operator+(const GE_Vec2 &i_v);
	GE_Vec2& operator+=(const GE_Vec2 &i_v);
	GE_Vec2 operator-();
	GE_Vec2 operator-(const GE_Vec2 &i_v);
	GE_Vec2& operator-=(const GE_Vec2 &i_v);
	GE_Vec2 operator*(float i_scaler);
	GE_Vec2& operator*=(float i_scaler);
	GE_Vec2 operator/(float i_scaler);
	GE_Vec2& operator/=(float i_scaler);
	bool operator==(const GE_Vec2 &i_v);
	bool operator!=(const GE_Vec2 &i_v);
	float length();
	GE_Vec2& transform(const GE_Mat4x4 *i_mat);
	void transformOut(GE_Vec2 *i_out, const GE_Mat4x4 *i_mat);
	GE_Vec2& normalize();
	void normalizeOut(GE_Vec2* i_out);
	float dot(const GE_Vec2 *i_vec);
	GE_Vec2& cross(const GE_Vec2 *i_vec);
	void crossOut(GE_Vec2 *i_out, const GE_Vec2 *i_vec);
};

void GE_Vec2Transform(GE_Vec2 *i_out, const GE_Vec2 *i_vec, const GE_Mat4x4 *i_mat);
float GE_Vec2Length(const GE_Vec2 *i_vec);
void GE_Vec2Normalize(GE_Vec2 *i_out, const GE_Vec2 *i_vec);
float GE_Vec2Dot(const GE_Vec2 *i_v1, const GE_Vec2 *i_v2);
void GE_Vec2Cross(GE_Vec2 *i_out, const GE_Vec2 *i_v1, const GE_Vec2 *i_v2);

#include "GE_Vec2.inl"