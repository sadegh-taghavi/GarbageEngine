#include "GE_Vec2.h"

inline GE_Vec2::GE_Vec2(float i_x, float i_y) : 
	x(i_x), y(i_y) 
{
}

inline GE_Vec2::GE_Vec2(const float *i_array) :
	x(i_array[0]), y(i_array[1]) 
{
}

inline GE_Vec2::GE_Vec2(const GE_Vec2 &i_vec)
{
	*this = i_vec;
}

inline GE_Vec2 GE_Vec2::operator+(const GE_Vec2 &i_v)
{
	return GE_Vec2(x + i_v.x, y + i_v.y);
}

inline GE_Vec2& GE_Vec2::operator+=(const GE_Vec2 &i_v)
{
	x += i_v.x;
	y += i_v.y;
	return *this;
}

inline GE_Vec2 GE_Vec2::operator-()
{
	return GE_Vec2(-x, -y);
}

inline GE_Vec2 GE_Vec2::operator-(const GE_Vec2 &i_v)
{
	return GE_Vec2(x - i_v.x, y - i_v.y);
}

inline GE_Vec2& GE_Vec2::operator-=(const GE_Vec2 &i_v)
{
	x -= i_v.x;
	y -= i_v.y;
	return *this;
}

inline GE_Vec2 GE_Vec2::operator*(float i_scaler)
{
	return GE_Vec2(x * i_scaler, y * i_scaler);
}

inline GE_Vec2& GE_Vec2::operator*=(float i_scaler)
{
	x *= i_scaler;
	y *= i_scaler;
	return *this;
}

inline GE_Vec2 GE_Vec2::operator/(float i_scaler)
{
	return GE_Vec2(x / i_scaler, y / i_scaler);
}

inline GE_Vec2& GE_Vec2::operator/=(float i_scaler)
{
	x /= i_scaler;
	y /= i_scaler;
	return *this;
}

inline bool GE_Vec2::operator==(const GE_Vec2 &i_v)
{
	if(x != i_v.x)
		return false;
	if(y != i_v.y)
		return false;
	return true;
}

inline bool GE_Vec2::operator!=(const GE_Vec2 &i_v)
{
	return !(*this == i_v);
}

inline float GE_Vec2::length()
{
	float l;
	DirectX::XMStoreFloat(&l, DirectX::XMVector2Length(DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)this)));
	return l;
}

inline GE_Vec2& GE_Vec2::transform(const GE_Mat4x4 *i_mat)
{
	DirectX::XMStoreFloat2((DirectX::XMFLOAT2*)this, DirectX::XMVector2Transform(DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)this), 
		DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)i_mat)));
	return *this;
}

inline void GE_Vec2::transformOut(GE_Vec2 *i_out, const GE_Mat4x4 *i_mat)
{
	DirectX::XMStoreFloat2((DirectX::XMFLOAT2*)i_out, DirectX::XMVector2Transform(DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)this), 
		DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)i_mat)));
}

inline GE_Vec2& GE_Vec2::normalize()
{
	DirectX::XMStoreFloat2((DirectX::XMFLOAT2*)this, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)this)));
	return *this;
}

inline void GE_Vec2::normalizeOut(GE_Vec2* i_out)
{
	DirectX::XMStoreFloat2((DirectX::XMFLOAT2*)i_out, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)this)));
}

inline float GE_Vec2::dot(const GE_Vec2 *i_vec)
{
	float d;
	DirectX::XMStoreFloat(&d, DirectX::XMVector2Dot(DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)this), DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)i_vec)));
	return d;
}

inline GE_Vec2& GE_Vec2::cross(const GE_Vec2 *i_vec)
{
	DirectX::XMStoreFloat2((DirectX::XMFLOAT2*)this, DirectX::XMVector2Cross(DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)this), DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)i_vec)));
	return *this;
}

inline void GE_Vec2::crossOut(GE_Vec2 *i_out, const GE_Vec2 *i_vec)
{
	DirectX::XMStoreFloat2((DirectX::XMFLOAT2*)i_out, DirectX::XMVector2Cross(DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)this), DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)i_vec)));
}

//global functions-------------------------------------------------------------------
inline void GE_Vec2Transform(GE_Vec2 *i_out, const GE_Vec2 *i_vec, const GE_Mat4x4 *i_mat)
{
	DirectX::XMStoreFloat2((DirectX::XMFLOAT2*)i_out, DirectX::XMVector2Transform(DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)i_vec), DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)i_mat)));
}

inline float GE_Vec2Length(const GE_Vec2 *i_vec)
{
	float l;
	DirectX::XMStoreFloat(&l, DirectX::XMVector2Length(DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)i_vec)));
	return l;
}

inline void GE_Vec2Normalize(GE_Vec2 *i_out, const GE_Vec2 *i_vec)
{
	DirectX::XMStoreFloat2((DirectX::XMFLOAT2*)i_out, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)i_vec)));
}

inline float GE_Vec2Dot(const GE_Vec2 *i_v1, const GE_Vec2 *i_v2)
{
	float d;
	DirectX::XMStoreFloat(&d, DirectX::XMVector2Dot(DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)i_v1), DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)i_v2)));
	return d;
}

inline void GE_Vec2Cross(GE_Vec2 *i_out, const GE_Vec2 *i_v1, const GE_Vec2 *i_v2)
{
	DirectX::XMStoreFloat2((DirectX::XMFLOAT2*)i_out, DirectX::XMVector2Cross(DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)i_v1), DirectX::XMLoadFloat2((DirectX::XMFLOAT2*)i_v2)));
}