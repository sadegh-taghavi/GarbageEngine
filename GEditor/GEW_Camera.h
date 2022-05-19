#pragma once
#include <GE_View.h>
#include "GEW_Input.h"

class GEW_FreeCamera : public GE_View
{
public:
	GE_Vec3		LookVector;
	GE_Mat4x4	View;
	float		Pitch;
	float		Yaw;
	float		ForwardUnits;
	float		SidewardUnits;
	float		UpwardUnits;
		
	GEW_FreeCamera();
	void lookAt( GE_Vec3 &i_target );
	void lookAt( GE_Vec3 &i_target, float i_distance );
	void update( GEW_Input::Data &i_inputData );
};
