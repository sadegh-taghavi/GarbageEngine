#pragma once
#include <DirectXMath.h>

struct GE_Vec4
{
	float x;
	float y;
	float z;
	float w;

	GE_Vec4(){}
	GE_Vec4(float i_x, float i_y, float i_z, float i_w);
	GE_Vec4(const float *i_array);
	GE_Vec4(const GE_Vec4 &i_vec);

	GE_Vec4 operator+(const GE_Vec4 &i_v);
	GE_Vec4& operator+=(const GE_Vec4 &i_v);
	GE_Vec4 operator-();
	GE_Vec4 operator-(const GE_Vec4 &i_v);
	GE_Vec4& operator-=(const GE_Vec4 &i_v);
	GE_Vec4 operator*(float i_scaler);
	GE_Vec4& operator*=(float i_scaler);
	GE_Vec4 operator/(float i_scaler);
	GE_Vec4& operator/=(float i_scaler);
	bool operator==(const GE_Vec4 &i_v);
	bool operator!=(const GE_Vec4 &i_v);
	float operator[](int i_index);
	float length();
	GE_Vec4& transform(const GE_Mat4x4 *i_mat);
	void transform(GE_Vec4 *i_out, const GE_Mat4x4 *i_mat);
	GE_Vec4& normalize();
	void normalizeOut(GE_Vec4* i_out);
	float dot(const GE_Vec4 *i_vec);
	void To2DLeftUpPosition( const GE_Vec3 &i_position, const GE_Mat4x4 &i_viewProjection );
	void To2DLeftUpPositionOut( GE_Vec4 &i_out, const GE_Vec3 &i_position, const GE_Mat4x4 &i_viewProjection );

	/*GE_Vec4& cross(const GE_Vec4 *i_vec)
	{
		DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)this, DirectX::XMVector4Cross(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)this), DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)i_vec)));
		return *this;
	}*/

	/*void cross(GE_Vec4 *i_out, const GE_Vec4 *i_vec)
	{
		DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)i_out, DirectX::XMVector4Cross(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)this), DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)i_vec)));
	}*/
};

void GE_Vec4Transform(GE_Vec4 *i_out, const GE_Vec4 *i_vec, const GE_Mat4x4 *i_mat);
float GE_Vec4Length(const GE_Vec4 *i_vec);
void GE_Vec4Normalize(GE_Vec4 *i_out, const GE_Vec4 *i_vec);
float GE_Vec4Dot(const GE_Vec4 *i_v1, const GE_Vec4 *i_v2);
//void GE_Vec4Cross(GE_Vec4 *i_out, const GE_Vec4 *i_v1, const GE_Vec4 *i_v2);

#include "GE_Vec4.inl"