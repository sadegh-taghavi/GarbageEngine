#pragma once

#define GE_PI						3.141592654f
#define GE_2PI						6.283185307f
#define GE_1DIVPI					0.318309886f
#define GE_1DIV2PI					0.159154943f
#define GE_PIDIV2					1.570796327f
#define GE_PIDIV4					0.785398163f
#define GE_WATTHP					0.001341022f
#define GE_PoundFootKilogramMeter	7.233013850f
#define GE_NewtonMeterPoundFoot		0.737562149f
#define GE_KilogramMeterPoundFoot	0.138254954f

#define GE_FLOAT32_MAX	( 3.402823466e+38f )
#define GE_FLOAT32_MIN	( -3.402823466e+38f )

#define GE_ConvertToRadians( fDegrees ) ( ( fDegrees ) * ( GE_PI / 180.0f ) )
#define GE_ConvertToDegrees( fRadians ) ( ( fRadians ) * ( 180.0f / GE_PI ) )
#define GE_ConvertToMeter( fUnit ) ( ( fUnit ) * 0.05f )
#define GE_ConvertToUnit( fMeter ) ( ( fMeter ) * 20.0f )
#define GE_ConvertToMeter( fUnit ) ( ( fUnit ) * 0.05f )
#define GE_ConvertToSQ( fUnit ) ( ( fUnit ) * 0.0025f )
#define GE_ConvertToUnitSQ( fMeter ) ( ( fMeter ) * 400.0f )
#define GE_ConvertWattToHP( fUnit )( ( fUnit ) * GE_WATTHP )
#define GE_ConvertRadianPerSecondToRPM( fUnit ) ( ( fUnit ) / GE_2PI * 60.0f )
#define GE_ConvertRPMToRadianPerSecond( fUnit ) ( ( fUnit ) * GE_2PI * 0.016666666f )
#define GE_Lerp( fv1, fv2, fAmount ) ( ( fv1 ) + ( fAmount ) * ( ( fv2 ) - ( fv1 ) ) ) //a + f * (b - a)

#define GE_MIN( a, b ) ( (a) < (b) ? (a) : (b) )
#define GE_MAX( a, b ) ( (a) > (b) ? (a) : (b) )

#include "GE_Vec2.h"
#include "GE_Vec3.h"
#include "GE_Vec4.h"
#include "GE_Quat.h"
#include "GE_Mat4x4.h"

