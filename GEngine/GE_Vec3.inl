#include "GE_Vec3.h"
#include "GE_Mat4x4.h"

inline GE_Vec3::GE_Vec3( float i_x, float i_y, float i_z ) :
x( i_x ), y( i_y ), z( i_z )
{
}

inline GE_Vec3::GE_Vec3( const float *i_array ) :
x( i_array[ 0 ] ), y( i_array[ 1 ] ), z( i_array[ 2 ] )
{
}

inline GE_Vec3::GE_Vec3( const GE_Vec3 &i_vec )
{
	*this = i_vec;
}

inline GE_Vec3 GE_Vec3::operator+( const GE_Vec3 &i_v )
{

	return GE_Vec3( x + i_v.x, y + i_v.y, z + i_v.z );
}

inline GE_Vec3& GE_Vec3::operator+=( const GE_Vec3 &i_v )
{
	x += i_v.x;
	y += i_v.y;
	z += i_v.z;
	return *this;
}

inline GE_Vec3 GE_Vec3::operator-( )
{
	return GE_Vec3( -x, -y, -z );
}

inline GE_Vec3 GE_Vec3::operator-( const GE_Vec3 &i_v )
{
	return GE_Vec3( x - i_v.x, y - i_v.y, z - i_v.z );
}

inline GE_Vec3& GE_Vec3::operator-=( const GE_Vec3 &i_v )
{
	x -= i_v.x;
	y -= i_v.y;
	z -= i_v.z;
	return *this;
}

inline GE_Vec3 GE_Vec3::operator*( float i_scaler )
{
	return GE_Vec3( x * i_scaler, y * i_scaler, z * i_scaler );
}

inline GE_Vec3& GE_Vec3::operator*=( float i_scaler )
{
	x *= i_scaler;
	y *= i_scaler;
	z *= i_scaler;
	return *this;
}

inline GE_Vec3 GE_Vec3::operator/( float i_scaler )
{
	return GE_Vec3( x / i_scaler, y / i_scaler, z / i_scaler );
}

inline GE_Vec3& GE_Vec3::operator/=( float i_scaler )
{
	x /= i_scaler;
	y /= i_scaler;
	z /= i_scaler;
	return *this;
}

inline bool GE_Vec3::operator==( const GE_Vec3 &i_v )
{
	if ( x != i_v.x )
		return false;
	if ( y != i_v.y )
		return false;
	if ( z != i_v.z )
		return false;
	return true;
}

inline bool GE_Vec3::operator!=( const GE_Vec3 &i_v )
{
	return !( *this == i_v );
}

inline float GE_Vec3::length()
{
	float l;
	DirectX::XMStoreFloat( &l, DirectX::XMVector3Length( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )this ) ) );
	return l;
}

inline GE_Vec3& GE_Vec3::transform( const GE_Mat4x4 *i_mat )
{
	DirectX::XMStoreFloat3( ( DirectX::XMFLOAT3* )this, DirectX::XMVector3Transform( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )this ),
		DirectX::XMLoadFloat4x4( ( DirectX::XMFLOAT4X4* )i_mat ) ) );
	return *this;
}

inline void GE_Vec3::transformOut( GE_Vec3 *i_out, const GE_Mat4x4 *i_mat )
{
	DirectX::XMStoreFloat3( ( DirectX::XMFLOAT3* )i_out, DirectX::XMVector3Transform( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )this ),
		DirectX::XMLoadFloat4x4( ( DirectX::XMFLOAT4X4* )i_mat ) ) );
}

inline GE_Vec3& GE_Vec3::normalize()
{
	DirectX::XMStoreFloat3( ( DirectX::XMFLOAT3* )this, DirectX::XMVector3Normalize( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )this ) ) );
	return *this;
}

inline GE_Vec3& GE_Vec3::normalizeBy( const GE_Vec3* i_vec )
{
	DirectX::XMStoreFloat3( ( DirectX::XMFLOAT3* )this, DirectX::XMVector3Normalize( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_vec ) ) );
	return *this;
}

inline void GE_Vec3::normalizeOut( GE_Vec3* i_out )
{
	DirectX::XMStoreFloat3( ( DirectX::XMFLOAT3* )i_out, DirectX::XMVector3Normalize( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )this ) ) );
}

inline float GE_Vec3::dot( const GE_Vec3 *i_vec )
{
	float d;
	DirectX::XMStoreFloat( &d, DirectX::XMVector3Dot( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )this ), DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_vec ) ) );
	return d;
}

inline GE_Vec3& GE_Vec3::cross( const GE_Vec3 *i_vec )
{
	DirectX::XMStoreFloat3( ( DirectX::XMFLOAT3* )this, DirectX::XMVector3Cross( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )this ), DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_vec ) ) );
	return *this;
}

inline void GE_Vec3::crossOut( GE_Vec3 *i_out, const GE_Vec3 *i_vec )
{
	DirectX::XMStoreFloat3( ( DirectX::XMFLOAT3* )i_out, DirectX::XMVector3Cross( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )this ), DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_vec ) ) );
}

inline void GE_Vec3::lerp( const GE_Vec3 *i_second, float i_amount )
{
	DirectX::XMStoreFloat3( ( DirectX::XMFLOAT3* )this, DirectX::XMVectorLerp( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )this ),
		DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_second ), i_amount ) );
}

inline void GE_Vec3::lerpOut( GE_Vec3 *i_out, const GE_Vec3 *i_second, float i_amount )
{
	DirectX::XMStoreFloat3( ( DirectX::XMFLOAT3* )i_out, DirectX::XMVectorLerp( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )this ),
		DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_second ), i_amount ) );
}

//global functions-----------------------------------------------------------------------------------
inline void GE_Vec3Transform( GE_Vec3 *i_out, const GE_Vec3 *i_vec, const GE_Mat4x4 *i_mat )
{
	DirectX::XMStoreFloat3( ( DirectX::XMFLOAT3* )i_out, DirectX::XMVector3Transform( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_vec ), DirectX::XMLoadFloat4x4( ( DirectX::XMFLOAT4X4* )i_mat ) ) );
}

inline float GE_Vec3Length( const GE_Vec3 *i_vec )
{
	float l;
	DirectX::XMStoreFloat( &l, DirectX::XMVector3Length( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_vec ) ) );
	return l;
}

inline void GE_Vec3Normalize( GE_Vec3 *i_out, const GE_Vec3 *i_vec )
{
	DirectX::XMStoreFloat3( ( DirectX::XMFLOAT3* )i_out, DirectX::XMVector3Normalize( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_vec ) ) );
}

inline float GE_Vec3Dot( const GE_Vec3 *i_v1, const GE_Vec3 *i_v2 )
{
	float d;
	DirectX::XMStoreFloat( &d, DirectX::XMVector3Dot( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_v1 ), DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_v2 ) ) );
	return d;
}

inline void GE_Vec3Cross( GE_Vec3 *i_out, const GE_Vec3 *i_v1, const GE_Vec3 *i_v2 )
{
	DirectX::XMStoreFloat3( ( DirectX::XMFLOAT3* )i_out, DirectX::XMVector3Cross( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_v1 ), DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_v2 ) ) );
}

inline void GE_Vec3Lerp( GE_Vec3 *i_out, const GE_Vec3 *i_first, const GE_Vec3 *i_second, float i_amount )
{
	DirectX::XMStoreFloat3( ( DirectX::XMFLOAT3* )i_out, DirectX::XMVectorLerp( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_first ),
		DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_second ), i_amount ) );
}

inline bool GE_Vec3RayIntersectToPlane( const GE_Vec3 *i_org, const GE_Vec3 *i_dir, const GE_Vec3 *i_verts, float &i_distance )
{
	DirectX::XMVECTOR v[ 4 ];
	v[ 0 ] = DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3 * )&i_verts[ 0 ] );
	v[ 1 ] = DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3 * )&i_verts[ 1 ] );
	v[ 2 ] = DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3 * )&i_verts[ 2 ] );
	v[ 3 ] = DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3 * )&i_verts[ 3 ] );

	if ( DirectX::TriangleTests::Intersects( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3 * )i_org ),
		DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3 * )i_dir ), v[ 0 ], v[ 1 ], v[ 3 ], i_distance ) )
		return true;

	if ( DirectX::TriangleTests::Intersects( DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3 * )i_org ),
		DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3 * )i_dir ), v[ 0 ], v[ 2 ], v[ 3 ], i_distance ) )
		return true;

	return false;
}

inline bool GE_Vec3RayIntersectToBox( const GE_Vec3 *i_org, const GE_Vec3 *i_dir,
	const GE_Vec3 *i_min, const GE_Vec3 *i_max, const GE_Mat4x4 *i_transform, float &i_distance )
{
	DirectX::BoundingBox bb;
	DirectX::BoundingBox::CreateFromPoints( bb, DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3 * )i_min ),
		DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3 * )i_max ) );
	bb.Transform( bb, DirectX::XMLoadFloat4x4( ( DirectX::XMFLOAT4X4 * )i_transform ) );
	return bb.Intersects( XMLoadFloat3( ( DirectX::XMFLOAT3 * )i_org ), XMLoadFloat3( ( DirectX::XMFLOAT3 * )i_dir ), i_distance );
}