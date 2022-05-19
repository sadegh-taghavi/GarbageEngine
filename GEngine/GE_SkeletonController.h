#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"
#include "GE_Model.h"

class GE_SkeletonController
{
public:
	friend class GE_Model;
	
private:
	
	float					m_lastFrame;
	GE_Model				*m_parent;
	struct BoneData
	{
		char										GMB[ 3 ];
		uint32_t									NumberOfBones;
		uint32_t									NumberOfFrames;
		int32_t										BoundBoneID;
	};

	struct BonePC
	{
		char		Name[ MAX_NAME_LENGTH ];
		GE_Mat4x4	InitializeMatrix;
		int32_t		ParentID;
	};

	struct BoneHierarchy
	{
		int32_t							ID;
		GE_Mat4x4						Matrix;
		BoneHierarchy					*Parent;
		std::vector< BoneHierarchy * >	Childs;
		BoneHierarchy()
		{
			Matrix.identity();
			Parent = NULL;
			ID = -1;
		}
	};

	struct Bone
	{
		std::string				Name;
		GE_Mat4x4				InitializeMatrix;
		int32_t					ParentID;
	};

	BoneHierarchy			*m_bonesHierarchy;
	std::vector<Bone>		m_bones;
	std::vector<GE_Mat4x4>	m_boneFrames;
	std::vector<GE_Mat4x4>	m_frameValues;
	std::vector<GE_Mat4x4>	m_frameValuesT1;
	std::vector<GE_Mat4x4>	m_frameValuesT2;
	GE_Mat4x4				m_BoundTransform;
	uint32_t m_numberOfFrames;
	int32_t	 m_boundBoneID;
	void loadFromMem( const uint8_t *i_memory );
	void addToHierarchy( BoneHierarchy *i_boneHierarchy, BonePC *i_bones );
	void removeHierarchy( BoneHierarchy *i_hierarchy );
	void calculateFrame( BoneHierarchy *i_hierarchy, uint32_t i_frame, std::vector< GE_Mat4x4 > *i_bones );
public:
	uint32_t getNumberOfFrames();
	GE_SkeletonController( GE_Model *i_parent );
	void unload();
	void loadAnimation( const std::string i_fileName );
	std::vector< GE_Mat4x4 > *getFrames();
	std::vector< GE_Mat4x4 > *getFrameValues();
	std::vector< Bone > *getBones();
	void setFrame( float i_frame );
	int32_t getBoundBoneID(){return m_boundBoneID;}
	~GE_SkeletonController(void);
};

