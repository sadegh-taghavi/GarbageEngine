#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"
#include "GE_Frustum.h"

class GE_ViewAnimation
{
	friend class GE_View;
	struct Frame
	{
		GE_Vec3 Position;
		GE_Quat Rotation;
		GE_Vec3 Target;
		float	Fovy;
		float	ZNear;
		float	ZFar;
		float	FocusDistance;
	};

	struct FData
	{
		char		GC[ 2 ];
		uint32_t	NumberOfFrames;
		char		Name[ MAX_NAME_LENGTH ];
		bool		IsTargetMode;
		bool		IsOrtho;
		GE_Vec3		Position;
		GE_Quat		Rotation;
		GE_Vec3		Target;
		float		ZNear;
		float		ZFar;
		float		Fovy;
		float		FocusDistance;
	};
	struct Data
	{
		string		Name;
		bool		IsTargetMode;
		bool		IsOrtho;
		GE_Vec3		Position;
		GE_Quat		Rotation;
		GE_Vec3		Target;
		GE_Vec3		Up;
		float		ZNear;
		float		ZFar;
		float		Fovy;
		float		FocusDistance;
	};
	Data m_data;
	std::vector< Frame > m_frames;
	Frame m_frame;
	float m_lastFrame;
	GE_View *m_parent;
	void loadFromMem( const uint8_t *i_memory );
public:
	GE_ViewAnimation( GE_View *i_parent );
	~GE_ViewAnimation();
	void loadAnimation( std::string i_fileName );
	void unload();
	void setFrame( float i_frame );
	Frame &getFrame();
	std::vector< Frame > &getFrames();
	Data &getData();
};

class GE_View
{
public:
	
	friend class GE_ViewAnimation;
	enum  Type
	{
		Free,
		Target,
	};

	GE_Mat4x4	m_matView;
	GE_Mat4x4	m_matViewInv;
	GE_Mat4x4	m_matProjection;
	GE_Mat4x4	m_matProjectionInv;
	GE_Mat4x4	m_matViewProjection;
	GE_Mat4x4	m_matViewProjectionInv;
	GE_Mat4x4	m_matViewProjectionLast;
	GE_Vec3		m_position;
	GE_Vec3		m_target;
	GE_Vec3		m_uP;
	GE_Vec2		m_orthoWH;
	float		m_orthoLeft;
	float		m_orthoRight;
	float		m_orthoBottom;
	float		m_orthoTop;
	float		m_pitch;
	float		m_yaw;
	float		m_aspect;
	float		m_fovy;
	float		m_near;
	float		m_far;
	Type		m_viewType;
	bool		m_isOrtho;
	bool		m_isCenterOrtho;
	float		m_focusDistance;
	GE_Frustum	m_frustum;
	GE_ViewAnimation *m_animation;
	GE_View();
	virtual ~GE_View();
	virtual void update();
	virtual void updateDepth();
	void getRayFromScreen( const GE_Vec2 &i_cursorPos, const GE_Vec2 &i_widthHeight, GE_Vec3 *i_outOrigin, GE_Vec3 *i_outDirection );
};
