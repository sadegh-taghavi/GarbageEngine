#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"


class GE_TransformController
{
public:
	friend class GE_Model;
	struct Transform
	{
		GE_Vec3				Position;
		GE_Quat				Rotation;
		GE_Vec3				Scale;
		GE_Mat4x4			Matrix;
		Transform(){ memset( this, 0 , sizeof( Transform ) ); }

	};
private:
	struct TransformData
	{
		char			GMT[ 3 ];
		uint32_t		NumberOfFrames;
	};
	float		 m_lastFrame;
	std::vector< Transform > m_frames;
	Transform m_transform;
	void loadFromMem( const uint8_t *i_memory );
	GE_Model *m_parent;
public:
	void unload();
	uint32_t getNumberOfFrames();
	GE_TransformController( GE_Model *i_parent );
	void loadAnimation( const std::string i_fileName );
	std::vector< Transform > *getFrames();
	void setFrame( float i_frame );
	Transform *getTransform();
	~GE_TransformController(void);
};

