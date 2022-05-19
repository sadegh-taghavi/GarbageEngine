#pragma once
#include "GE_Prerequisites.h"
#include <fstream>
#include <atlstr.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fbxsdk.h>
#include <list>
#include <vector>
#include "Hash/hashlibpp.h"
#include <boost/foreach.hpp>
#pragma comment( lib, "libfbxsdk-md.lib" )

using namespace std;

#define MAX_NAME_LENGTH 64

#include "GE_Math.h"
#include "GE_Bound.h"

struct VertexBuffer
{
	GE_Vec3				Position;
	GE_Vec3				Normal;
	GE_Vec4				Tangent;
	uint32_t			VertexID;
	VertexBuffer()
	{
		RtlZeroMemory( this, sizeof( VertexBuffer ) ); 
	};
};

struct MorphData
{
	char										GMM[ 3 ];
	uint32_t									NumberOfMorphTargets;
	uint32_t									NumberOfFrames;
	MorphData()
	{
		RtlZeroMemory( this, sizeof( MorphData ) );
		GMM[ 0 ] = 'G';
		GMM[ 1 ] = 'M';
		GMM[ 2 ] = 'M';
	}
};

struct MorphTargetVertex
{
	GE_Vec3		Position;
	GE_Vec3		Normal;
	GE_Vec3		Tangent;
	MorphTargetVertex()
	{
		RtlZeroMemory( this, sizeof(MorphTargetVertex) ); 
	}
};

struct MorphTarget
{
	char		Name[ MAX_NAME_LENGTH ];
};

struct VertexDataColor
{
	GE_Vec4				VertexColor;
	VertexDataColor()
	{
		RtlZeroMemory( this, sizeof(VertexDataColor) ); 
	}
};

#define BONE_MAX_IW 8

struct VertexDataBoneIndex
{
	int32_t Indices[ BONE_MAX_IW ];
	VertexDataBoneIndex()
	{	
		for( uint32_t i = 0; i < BONE_MAX_IW; ++i )
			Indices[ i ] = -1;
	}
};

struct VertexDataBoneWeight
{
	float Weights[ BONE_MAX_IW ];
	VertexDataBoneWeight()
	{
		RtlZeroMemory( this, sizeof(VertexDataBoneWeight) ); 
	}
};

struct BoneData
{
	char										GMB[ 3 ];
	uint32_t									NumberOfBones;
	uint32_t									NumberOfFrames;
	int32_t										BoundBoneID;
	BoneData()
	{
		BoundBoneID = -1;
		NumberOfBones = 0;
		NumberOfFrames = 0;
		GMB[ 0 ] = 'G';
		GMB[ 1 ] = 'M';
		GMB[ 2 ] = 'B';
	}
};

struct Bone
{
	char				Name[ MAX_NAME_LENGTH ];
	GE_Mat4x4			InitializeMatrix;
	int32_t				ParentID;
};

struct materialIndexAt
{
	string Name;
	vector<int32_t> Vertices;
};

struct TransformData
{
	char										GMT[ 3 ];
	uint32_t									NumberOfFrames;
	TransformData()
	{
		RtlZeroMemory( this, sizeof( TransformData ) );
		GMT[ 0 ] = 'G';
		GMT[ 1 ] = 'M';
		GMT[ 2 ] = 'T';
	}
};

struct Transform
{
	GE_Vec3				Position;
	GE_Quat				Rotation;
	GE_Vec3				Scale;
	GE_Mat4x4			Matrix;
};

struct ModelData
{
	char										GM[ 2 ];
	char										Name[ MAX_NAME_LENGTH ];
	GE_Bound									Bound;
	GE_Vec3										Position;
	GE_Vec3										Scale;
	GE_Quat										Rotation;
	uint32_t									NumberOfSubsets;
	bool										HasVerticesDataTexcoord;
	bool										HasVerticesDataAdditionTexcoord;
	bool										HasVerticesDataColor;
	bool										HasVerticesDataBoneIW;
	ModelData()
	{
		RtlZeroMemory( this, sizeof( ModelData ) );
		GM[ 0 ] = 'G';
		GM[ 1 ] = 'M';
		Bound.BoundRadiusMultiply = 1.0f;
	}
};

struct ModelSubset
{
	uint32_t									NumberOfVertices;
	uint32_t									NumberOfIndices;
	ModelSubset()
	{		
		RtlZeroMemory( this, sizeof( ModelSubset ) );
		
	}
};

struct BigNum
{
	BigNum( const char * i_str )
	{
		memcpy_s( Value, sizeof( BigNum ), i_str, sizeof( BigNum ) );
	}
	unsigned int Value[4];
	void operator=( const char * i_str)
	{
		memcpy_s( Value, sizeof( BigNum ), i_str, sizeof( BigNum ) );
	}
	bool operator==( BigNum &i_other )
	{
		return Value[0] == i_other.Value[0] && Value[1] == i_other.Value[1] && Value[2] == i_other.Value[2] && Value[3] == i_other.Value[3];
	}
};

struct VertexCombination
{
	VertexBuffer			VB;
	GE_Vec4					Texcoord;
	VertexDataColor			Color;
	VertexDataBoneIndex		BoneIndex;
	VertexDataBoneWeight	BoneWeight;

	VertexCombination()
	{
		Texcoord = GE_Vec4( 0.0f, 0.0f, 0.0f, 0.0f );
	}
	BigNum getHash();
};

struct CBBox
{
	char	Name[ MAX_NAME_LENGTH ];
	GE_Vec3 HalfDimension;
	GE_Vec3 Position;
	GE_Quat Rotation;
};

struct CBSphere
{
	char	Name[ MAX_NAME_LENGTH ];
	float	Radius;
	GE_Vec3 Position;
	GE_Quat Rotation;
};

struct CBCapsule
{
	char	Name[ MAX_NAME_LENGTH ];
	float	Radius;
	float	HalfHeight;
	GE_Vec3 Position;
	GE_Quat Rotation;
};

struct CBConvex
{
	char		Name[ MAX_NAME_LENGTH ];
	uint32_t	NumberOfVertices;
	uint32_t	NumberOfIndices;
	GE_Vec3		Position;
	GE_Quat		Rotation;
};

struct CBTriangleMesh
{
	char		Name[ MAX_NAME_LENGTH ];
	uint32_t	NumberOfVertices;
	uint32_t	NumberOfIndices;
	GE_Vec3		Position;
	GE_Quat		Rotation;
};

struct TriangleMeshData
{
	CBTriangleMesh		TriangleMesh;
	vector< GE_Vec3 >	Vertices;
	vector< uint32_t >	Indices;
};

struct ConvexData
{
	CBConvex			Convex;
	vector< GE_Vec3 >	Vertices;
	vector< uint32_t >	Indices;
};

struct ComplexBound
{
	uint32_t NumberOfBoxes;
	uint32_t NumberOfSpheres;
	uint32_t NumberOfCapsules;
	uint32_t NumberOfConvexes;
	uint32_t NumberOfTriangleMeshes;
	ComplexBound()
	{
		RtlZeroMemory( this, sizeof( ComplexBound ) );
	}
};

class GS_Model;
struct ModelSubsetData
{
	vector<VertexBuffer>				VertexBufferData;
	vector<GE_Vec4>						VerticesDataTexcoord;
	vector<VertexDataColor>				VerticesDataColor;
	vector<VertexDataBoneIndex>			VerticesDataBoneIndex;
	vector<VertexDataBoneWeight>		VerticesDataBoneWeight;
	vector<uint32_t>					Indices;
	vector<MorphTargetVertex>			VertexDataMorphTarget;
	vector<BigNum>						Combination;
	void optimize( GS_Model * i_model );
};

#define BOX				"gbox_"
#define SPHERE			"gsph_"
#define CAPSULE			"gcap_"
#define CONVEX			"gcon_"
#define TRIANGLE_MESH	"gtri_"
#define BOUND			"gbound_"

class GS_Model
{
public:

	ComplexBound							m_complexBound;

	vector<CBBox>							m_cBBoxes;
	vector<CBSphere>						m_cBSpheres;
	vector<CBCapsule>						m_cBCapsules;
	vector<ConvexData>						m_cBConvexesData;
	vector<TriangleMeshData>				m_cBTriangleMeshesData;
	
	ModelData								m_data;

	MorphData								m_morphData;
	vector<float>							m_morphFrames;

	TransformData							m_transformData;
	vector<Transform>						m_transformFrames;

	BoneData								m_boneData;
	vector<Bone>							m_bones;
	vector<GE_Mat4x4>						m_boneFrames;

	vector<ModelSubsetData>					m_subsetsData;
		
	vector<vector<int32_t>>					m_indexAt;
	vector<int32_t>							m_posAt;
	vector<int32_t>							m_normalAt;
	vector<int32_t>							m_tangentAt;

	vector<GE_Vec3>							m_vPositions;
	vector<GE_Vec3>							m_vNormals;
	vector<GE_Vec4>							m_vTangents;
	vector<GE_Vec2>							m_vTexcoords;
	vector<VertexDataBoneIndex>				m_vBoneIndices;
	vector<VertexDataBoneWeight>			m_vBoneWeights;
	vector<GE_Vec2>							m_vAdditionalTexcoords;
	vector<GE_Vec4>							m_vColors;

	vector<GE_Vec3>							m_vMorphPositions;
	vector<GE_Vec3>							m_vMorphNormals;
	vector<GE_Vec3>							m_vMorphTangents;

	vector<string>							m_materialIndices;
	vector<materialIndexAt>					m_materialindexAt;

	vector<FbxNode *>						m_boneNodes;
	vector<FbxBlendShapeChannel *>			m_morphChannels;

	int32_t									m_boneStartFrame;
	int32_t									m_boneEndFrame;
	
	
	void fillSubsets();

	GS_Model()
	{		

	}

	~GS_Model()
	{

	}

};

struct Camera
{
	GE_Vec3 Position;
	GE_Quat Rotation;
	GE_Vec3 Target;
	float	Fovy;
	float	ZNear;
	float	ZFar;
	float	FocusDistance;
};

struct CameraData
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
	CameraData()
	{
		RtlZeroMemory( this, sizeof( CameraData ) );
		GC[ 0 ] = 'G';
		GC[ 1 ] = 'C';
	}
};

class GE_Camera
{
public:

	CameraData			m_data;
	vector< Camera >	m_frames;

	GE_Camera()
	{		

	}

	~GE_Camera()
	{

	}
};

class GE_SceneManager
{
	GS_Model *m_currentModel;
	GE_Camera *m_currentCamera;
	static GE_SceneManager m_singleton;
	std::string m_path;

	GE_SceneManager();
	~GE_SceneManager(){ SAFE_DELETE( HashWrapper ); }
	FbxManager*			m_sdkManager;
	FbxIOSettings*		m_ios;
	FbxScene*			m_scene;
	FbxAnimStack*		m_animStack;
	
public:
	bool m_ctrlKeyDown, m_shiftKeyDown;
	hashwrapper *HashWrapper;
	static GE_SceneManager &getSingleton(){return m_singleton;}

	void GetGeometryOffset( FbxNode* i_node, FbxAMatrix &i_matrix )
	{
		const FbxVector4 lT = i_node->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = i_node->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = i_node->GetGeometricScaling(FbxNode::eSourcePivot);

		i_matrix.SetTRS( lT, lR, lS );
	}

	void ConvertFBXMatToGEMat( const FbxAMatrix &i_fbxMat, GE_Mat4x4 &i_geMat );
	void ConvertGEMatToFBXMat( const GE_Mat4x4 &i_geMat, FbxAMatrix &i_fbxMat );

	void dumpNode( FbxNode *i_node );
	void dumpMesh( FbxNode *i_node );
	void dumpPolygon( FbxMesh *i_mesh );
	void dumpShape( FbxGeometry *i_geometry );
	void dumpSkeleton( FbxNode *i_node );
	void dumpLink( FbxGeometry *i_geometry );
	void dumpPose( FbxNode *i_node, bool i_child = true );
	void dumpCamera( FbxNode *i_node );

	void unpackScene( const string &i_sceneName );
};
