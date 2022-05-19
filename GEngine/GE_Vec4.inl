#include "GE_Vec4.h"
#include "GE_Vec3.h"

inline GE_Vec4::GE_Vec4(float i_x, float i_y, float i_z, float i_w) : 
	x(i_x), y(i_y), z(i_z), w(i_w) 
{
}

inline GE_Vec4::GE_Vec4(const float *i_array) :
	x(i_array[0]), y(i_array[1]), z(i_array[2]), w(i_array[3]) 
{
}

inline GE_Vec4::GE_Vec4(const GE_Vec4 &i_vec)
{
	*this = i_vec;
}

inline GE_Vec4 GE_Vec4::operator+(const GE_Vec4 &i_v)
{
	return GE_Vec4(x + i_v.x, y + i_v.y, z + i_v.z, w + i_v.w);
}

inline GE_Vec4& GE_Vec4::operator+=(const GE_Vec4 &i_v)
{
	x += i_v.x;
	y += i_v.y;
	z += i_v.z;
	w += i_v.w;
	return *this;
}

inline GE_Vec4 GE_Vec4::operator-()
{
	return GE_Vec4(-x, -y, -z, -w);
}

inline GE_Vec4 GE_Vec4::operator-(const GE_Vec4 &i_v)
{
	return GE_Vec4(x - i_v.x, y - i_v.y, z - i_v.z, w - i_v.w);
}

inline GE_Vec4& GE_Vec4::operator-=(const GE_Vec4 &i_v)
{
	x -= i_v.x;
	y -= i_v.y;
	z -= i_v.z;
	w -= i_v.w;
	return *this;
}

inline GE_Vec4 GE_Vec4::operator*(float i_scaler)
{
	return GE_Vec4(x * i_scaler, y * i_scaler, z * i_scaler, w * i_scaler);
}

inline GE_Vec4& GE_Vec4::operator*=(float i_scaler)
{
	x *= i_scaler;
	y *= i_scaler;
	z *= i_scaler;
	w *= i_scaler;
	return *this;
}

inline GE_Vec4 GE_Vec4::operator/(float i_scaler)
{
	return GE_Vec4(x / i_scaler, y / i_scaler, z / i_scaler, w / i_scaler);
}

inline GE_Vec4& GE_Vec4::operator/=(float i_scaler)
{
	x /= i_scaler;
	y /= i_scaler;
	z /= i_scaler;
	w /= i_scaler;
	return *this;
}

inline bool GE_Vec4::operator==(const GE_Vec4 &i_v)
{
	if(x != i_v.x)
		return false;
	if(y != i_v.y)
		return false;
	if(z != i_v.z)
		return false;
	if(w != i_v.w)
		return false;
	return true;
}

inline bool GE_Vec4::operator!=(const GE_Vec4 &i_v)
{
	return !(*this == i_v);
}

inline float GE_Vec4::operator[](int i_index)
{
	return ((float*)this)[i_index];
}

inline float GE_Vec4::length()
{
	float l;
	DirectX::XMStoreFloat(&l, DirectX::XMVector4Length(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)this)));
	return l;
}

inline GE_Vec4& GE_Vec4::transform(const GE_Mat4x4 *i_mat)
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)this, DirectX::XMVector4Transform(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)this), 
		DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)i_mat)));
	return *this;
}

inline void GE_Vec4::transform(GE_Vec4 *i_out, const GE_Mat4x4 *i_mat)
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)i_out, DirectX::XMVector4Transform(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)this), 
		DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)i_mat)));
}

inline GE_Vec4& GE_Vec4::normalize()
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)this, DirectX::XMVector4Normalize(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)this)));
	return *this;
}

inline void GE_Vec4::normalizeOut(GE_Vec4* i_out)
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)i_out, DirectX::XMVector4Normalize(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)this)));
}

inline float GE_Vec4::dot(const GE_Vec4 *i_vec)
{
	float d;
	DirectX::XMStoreFloat(&d, DirectX::XMVector4Dot(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)this), DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)i_vec)));
	return d;
}

inline void GE_Vec4::To2DLeftUpPosition( const GE_Vec3 &i_position, const GE_Mat4x4 &i_viewProjection )
{
	GE_Vec4 pos;
	pos.x = i_position.x;
	pos.y = i_position.y;
	pos.z = i_position.z;
	pos.w = 1.0f;

	GE_Vec4Transform( this, &pos, &i_viewProjection );
	
	x /= w;
	y /= w;
	z /= w;
	x = x * 0.5f + 0.5f;
	y = ( -y * 0.5f + 0.5f );
}

inline void GE_Vec4::To2DLeftUpPositionOut( GE_Vec4 &i_out, const GE_Vec3 &i_position,const GE_Mat4x4 &i_viewProjection )
{
	GE_Vec4 pos;
	pos.x = i_position.x;
	pos.y = i_position.y;
	pos.z = i_position.z;
	pos.w = 1.0f;

	GE_Vec4Transform( &i_out, &pos, &i_viewProjection );

	i_out.x /= i_out.w;
	i_out.y /= i_out.w;
	i_out.z /= i_out.w;
	i_out.x = i_out.x * 0.5f + 0.5f;
	i_out.y = ( -i_out.y * 0.5f + 0.5f );
}

//global functions --------------------------------------------------------------------------
inline void GE_Vec4Transform(GE_Vec4 *i_out, const GE_Vec4 *i_vec, const GE_Mat4x4 *i_mat)
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)i_out, DirectX::XMVector4Transform(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)i_vec), DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)i_mat)));
}

inline float GE_Vec4Length(const GE_Vec4 *i_vec)
{
	float l;
	DirectX::XMStoreFloat(&l, DirectX::XMVector4Length(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)i_vec)));
	return l;
}

inline void GE_Vec4Normalize(GE_Vec4 *i_out, const GE_Vec4 *i_vec)
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)i_out, DirectX::XMVector4Normalize(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)i_vec)));
}

inline float GE_Vec4Dot(const GE_Vec4 *i_v1, const GE_Vec4 *i_v2)
{
	float d;
	DirectX::XMStoreFloat(&d, DirectX::XMVector4Dot(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)i_v1), DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)i_v2)));
	return d;
}

/*inline void GE_Vec4Cross(GE_Vec4 *i_out, const GE_Vec4 *i_v1, const GE_Vec4 *i_v2)
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)i_out, DirectX::XMVector4Cross(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)i_v1), DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)i_v2)));
}*/