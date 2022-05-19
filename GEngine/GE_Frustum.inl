#include "GE_Frustum.h"

inline GE_Frustum::GE_Frustum()
{
	memset( this, 0, sizeof( GE_Frustum ) );
}

inline void GE_Frustum::calculatePlanes( const GE_Mat4x4 *i_viewProjection )
{
	// Left plane
	Planes[ 0 ].x = i_viewProjection->_03 + i_viewProjection->_00;
	Planes[ 0 ].y = i_viewProjection->_13 + i_viewProjection->_10;
	Planes[ 0 ].z = i_viewProjection->_23 + i_viewProjection->_20;
	Planes[ 0 ].w = i_viewProjection->_33 + i_viewProjection->_30;

	// Right plane
	Planes[ 1 ].x = i_viewProjection->_03 - i_viewProjection->_00;
	Planes[ 1 ].y = i_viewProjection->_13 - i_viewProjection->_10;
	Planes[ 1 ].z = i_viewProjection->_23 - i_viewProjection->_20;
	Planes[ 1 ].w = i_viewProjection->_33 - i_viewProjection->_30;

	// Top plane
	Planes[ 2 ].x = i_viewProjection->_03 - i_viewProjection->_01;
	Planes[ 2 ].y = i_viewProjection->_13 - i_viewProjection->_11;
	Planes[ 2 ].z = i_viewProjection->_23 - i_viewProjection->_21;
	Planes[ 2 ].w = i_viewProjection->_33 - i_viewProjection->_31;

	// Bottom plane
	Planes[ 3 ].x = i_viewProjection->_03 + i_viewProjection->_01;
	Planes[ 3 ].y = i_viewProjection->_13 + i_viewProjection->_11;
	Planes[ 3 ].z = i_viewProjection->_23 + i_viewProjection->_21;
	Planes[ 3 ].w = i_viewProjection->_33 + i_viewProjection->_31;

	// Near plane
	Planes[ 4 ].x = i_viewProjection->_02;
	Planes[ 4 ].y = i_viewProjection->_12;
	Planes[ 4 ].z = i_viewProjection->_22;
	Planes[ 4 ].w = i_viewProjection->_32;

	// Far plane
	Planes[ 5 ].x = i_viewProjection->_03 - i_viewProjection->_02;
	Planes[ 5 ].y = i_viewProjection->_13 - i_viewProjection->_12;
	Planes[ 5 ].z = i_viewProjection->_23 - i_viewProjection->_22;
	Planes[ 5 ].w = i_viewProjection->_33 - i_viewProjection->_32;

	// Normalize planes
	for ( int i = 0; i < 6; i++ )
	{
		DirectX::XMStoreFloat4( ( DirectX::XMFLOAT4 * )&Planes[ i ], DirectX::XMPlaneNormalize( DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4 * )&Planes[ i ] ) ) );
	}
}

inline GE_Frustum::Result GE_Frustum::sphereInFrustum( const GE_Vec3* i_position, float i_radius )
{
	int c = 0;
	float d;
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )i_position );
	for ( int p = 0; p < 6; p++ )
	{
		DirectX::XMStoreFloat( &d, DirectX::XMPlaneDotCoord( DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )&Planes[ p ] ), pos ) );
		if ( d <= -i_radius )
			return OutSide;
		if ( d > i_radius )
			c++;
	}
	return ( c == 6 ) ? Inside : Colled;
}

inline GE_Frustum::Result GE_Frustum::cubeInFrustum( const GE_Vec3 *i_position, const GE_Vec3 *i_halfSize )
{
	int c;
	int c2 = 0;

	for ( int p = 0; p < 6; p++ )
	{
		c = 0;
		if ( Planes[ p ][ 0 ] * ( i_position->x - i_halfSize->x ) + Planes[ p ][ 1 ] * ( i_position->y - i_halfSize->y ) + Planes[ p ][ 2 ] * ( i_position->z - i_halfSize->z ) + Planes[ p ][ 3 ] > 0 )
			c++;
		if ( Planes[ p ][ 0 ] * ( i_position->x + i_halfSize->x ) + Planes[ p ][ 1 ] * ( i_position->y - i_halfSize->y ) + Planes[ p ][ 2 ] * ( i_position->z - i_halfSize->z ) + Planes[ p ][ 3 ] > 0 )
			c++;
		if ( Planes[ p ][ 0 ] * ( i_position->x - i_halfSize->x ) + Planes[ p ][ 1 ] * ( i_position->y + i_halfSize->y ) + Planes[ p ][ 2 ] * ( i_position->z - i_halfSize->z ) + Planes[ p ][ 3 ] > 0 )
			c++;
		if ( Planes[ p ][ 0 ] * ( i_position->x + i_halfSize->x ) + Planes[ p ][ 1 ] * ( i_position->y + i_halfSize->y ) + Planes[ p ][ 2 ] * ( i_position->z - i_halfSize->z ) + Planes[ p ][ 3 ] > 0 )
			c++;
		if ( Planes[ p ][ 0 ] * ( i_position->x - i_halfSize->x ) + Planes[ p ][ 1 ] * ( i_position->y - i_halfSize->y ) + Planes[ p ][ 2 ] * ( i_position->z + i_halfSize->z ) + Planes[ p ][ 3 ] > 0 )
			c++;
		if ( Planes[ p ][ 0 ] * ( i_position->x + i_halfSize->x ) + Planes[ p ][ 1 ] * ( i_position->y - i_halfSize->y ) + Planes[ p ][ 2 ] * ( i_position->z + i_halfSize->z ) + Planes[ p ][ 3 ] > 0 )
			c++;
		if ( Planes[ p ][ 0 ] * ( i_position->x - i_halfSize->x ) + Planes[ p ][ 1 ] * ( i_position->y + i_halfSize->y ) + Planes[ p ][ 2 ] * ( i_position->z + i_halfSize->z ) + Planes[ p ][ 3 ] > 0 )
			c++;
		if ( Planes[ p ][ 0 ] * ( i_position->x + i_halfSize->x ) + Planes[ p ][ 1 ] * ( i_position->y + i_halfSize->y ) + Planes[ p ][ 2 ] * ( i_position->z + i_halfSize->z ) + Planes[ p ][ 3 ] > 0 )
			c++;
		if ( c == 0 )
			return OutSide;
		if ( c == 8 )
			c2++;
	}
	return ( c2 == 6 ) ? Inside : Colled;
}

inline bool GE_Frustum::polygonListInFrustum( const GE_Vec3* i_polylist, uint32_t i_numPolys )
{
	uint32_t p;
	float d;
	for ( uint32_t f = 0; f < 6; f++ )
	{
		for ( p = 0; p < i_numPolys; p++ )
		{
			DirectX::XMStoreFloat( &d, DirectX::XMPlaneDotCoord( DirectX::XMLoadFloat4( ( DirectX::XMFLOAT4* )&Planes[ f ] ),
				DirectX::XMLoadFloat3( ( DirectX::XMFLOAT3* )&i_polylist[ p ] ) ) );
			if ( d )
				break;
		}
		if ( p == i_numPolys )
			return false;
	}
	return true;
}