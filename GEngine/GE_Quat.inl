#include "GE_Quat.h"
#include "GE_Math.h"

inline GE_Quat::GE_Quat(float i_x, float i_y, float i_z, float i_w) : 
	x(i_x), y(i_y), z(i_z), w(i_w) 
{
}

inline GE_Quat::GE_Quat(const float *i_array) :
	x(i_array[0]), y(i_array[1]), z(i_array[2]), w(i_array[3])
{
}

inline GE_Quat::GE_Quat(const GE_Quat &i_q)
{
	*this = i_q;
}

inline bool GE_Quat::operator==(const GE_Quat &i_q)
{
	if(x != i_q.x)
		return false;
	if(y != i_q.y)
		return false;
	if(z != i_q.z)
		return false;
	if(w != i_q.w)
		return false;
	return true;
}

inline bool GE_Quat::operator!=(const GE_Quat &i_q)
{
	return !(*this == i_q);
}


inline GE_Quat& GE_Quat::operator*=( const GE_Quat &i_q )
{
	DirectX::XMStoreFloat4( ( DirectX::XMFLOAT4* )this, DirectX::XMQuaternionMultiply( DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )this ),
		DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )&i_q ) ) );
	return *this;
}

inline GE_Quat GE_Quat::operator*( const GE_Quat &i_q )
{
	GE_Quat q = *this;
	q *= i_q;
	return q;
}

inline GE_Quat& GE_Quat::identity()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 1.0f;
	return *this;
}

inline GE_Quat& GE_Quat::multiply( const GE_Quat *i_q )
{
	DirectX::XMStoreFloat4( ( DirectX::XMFLOAT4* )this, DirectX::XMQuaternionMultiply( DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )this ),
		DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )&i_q ) ) );
	return *this;
}

inline void GE_Quat::multiplyOut(GE_Quat *i_out, const GE_Quat *i_q)
{
	DirectX::XMStoreFloat4( ( DirectX::XMFLOAT4* )i_out, DirectX::XMQuaternionMultiply( DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )this ),
		DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )&i_q ) ) );
}

inline GE_Quat& GE_Quat::rotationAxis(const GE_Vec3 *i_axis, float i_angle)
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)this, DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)i_axis), i_angle));
	return *this;
}

inline void GE_Quat::rotationAxisOut(GE_Quat *i_out, const GE_Vec3 *i_axis, float i_angle)
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)i_out, DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)i_axis), i_angle));
}

inline GE_Quat& GE_Quat::rotationYPR( const GE_Vec3 *i_ypr )
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)this, DirectX::XMQuaternionRotationRollPitchYaw(i_ypr->y, i_ypr->x, i_ypr->z));
	return *this;
}

inline void GE_Quat::rotationYPROut( GE_Quat *i_out, const GE_Vec3 *i_ypr )
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)i_out, DirectX::XMQuaternionRotationRollPitchYaw(i_ypr->y, i_ypr->x, i_ypr->z));
}

inline GE_Quat& GE_Quat::normalize()
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)this, DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)this)));
	return *this;
}

inline void GE_Quat::normalizeOut(GE_Quat *i_out)
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)i_out, DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)this)));
}

inline void GE_Quat::toYPR( GE_Vec3 *i_out )
{
	double sqw = this->w*this->w;
	double sqx = this->x*this->x;
	double sqy = this->y*this->y;
	double sqz = this->z*this->z;
	double unit = sqx + sqy + sqz + sqw;
	double test = this->x*this->y + this->z*this->w;
	if (test > 0.499*unit) 
	{
		i_out->x = 2 * atan2(this->x,this->w);
		i_out->z = (float)GE_PI / 2;
		i_out->y = 0;
		return;
	}
	if (test < -0.499*unit) 
	{
		i_out->x = -2 * atan2(this->x,this->w);
		i_out->z = (float)-GE_PI / 2;
		i_out->y = 0;
		return;
	}
	i_out->x = (float)atan2(2*this->y*this->w-2*this->x*this->z , sqx - sqy - sqz + sqw);
	i_out->z = (float)asin(2*test / unit);
	i_out->y = (float)atan2(2*this->x*this->w-2*this->y*this->z , -sqx + sqy - sqz + sqw);
}

inline void GE_Quat::lerp( const GE_Quat *i_second, float i_amount )
{
	DirectX::XMStoreFloat4( ( DirectX::XMFLOAT4* )this, DirectX::XMQuaternionSlerp( DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )this ),
		DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )i_second ), i_amount ) );
}

inline void GE_Quat::lerpOut( GE_Quat *i_out, const GE_Quat *i_second, float i_amount )
{
	DirectX::XMStoreFloat4( ( DirectX::XMFLOAT4* )i_out, DirectX::XMQuaternionSlerp( DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )this ),
		DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )i_second ), i_amount ) );
}

//global functions-----------------------------------------------------
inline void GE_QuatRotationAxis(GE_Quat *i_out, const GE_Vec3 *i_axis, float i_angle)
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)i_out, DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)i_axis), i_angle));
}

inline void GE_QuatNormalize(GE_Quat *i_out, const GE_Quat *i_q)
{
	DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)i_out, DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4((DirectX::XMFLOAT4*)i_q)));
}

inline void GE_QuatLerp( GE_Quat *i_out, const GE_Quat *i_first, const GE_Quat *i_second, float i_amount )
{
	DirectX::XMStoreFloat4( ( DirectX::XMFLOAT4* )i_out, DirectX::XMQuaternionSlerp( DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )i_first ),
		DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )i_second ), i_amount ) );
}

inline void GE_QuatMultiply( GE_Quat *i_out, const GE_Quat *i_q1, const GE_Quat *i_q2 )
{
	DirectX::XMStoreFloat4( ( DirectX::XMFLOAT4* )i_out, DirectX::XMQuaternionMultiply( DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )i_q1 ),
		DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )&i_q2 ) ) );
}
