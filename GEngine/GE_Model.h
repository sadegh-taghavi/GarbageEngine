#pragma once
#include "GE_Prerequisites.h"
#include "GE_BufferResource.h"
#include "GE_Math.h"
#include "GE_view.h"
#include "GE_Bound.h"
#include "GE_MorphController.h"
#include "GE_SkeletonController.h"
#include "GE_TransformController.h"
#include "GE_SkeletonController.h"
#include "GE_MorphController.h"

class _GE_BufferResource;
struct GE_Material;
class GE_Texture;

class GE_Model
{
public:
	struct VertexBuffer
	{
		GE_Vec3		Position;
		GE_Vec3		Normal;
		GE_Vec4		Tangent;
		uint32_t	VertexID;
	};
	struct VertexDataColor
	{
		GE_Vec4		VertexColor;
	};

	struct VertexDataBoneIndex
	{
		int32_t Indices[ BONE_MAX_IW ];
	};

	struct VertexDataBoneWeight
	{
		float Weights[ BONE_MAX_IW ];
	};
	struct MorphTargetVertex
	{
		GE_Vec3		Position;
		GE_Vec3		Normal;
		GE_Vec4		Tangent;
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
	};
	struct ModelSubsetLoader
	{
		uint32_t									NumberOfVertices;
		uint32_t									NumberOfIndices;
	};

	struct CBBoxData
	{
		char		Name[ MAX_NAME_LENGTH ];
		GE_Vec3		HalfDimension;
		GE_Vec3		Position;
		GE_Quat		Rotation;
	};
	struct CBBox
	{
		std::string Name;
		GE_Vec3		HalfDimension;
		GE_Vec3		Position;
		GE_Quat		Rotation;
	};

	struct CBSphereData
	{
		char		Name[ MAX_NAME_LENGTH ];
		float		Radius;
		GE_Vec3		Position;
		GE_Quat		Rotation;
	};

	struct CBSphere
	{
		std::string Name;
		float		Radius;
		GE_Vec3		Position;
		GE_Quat		Rotation;
	};

	struct CBCapsuleData
	{
		char		Name[ MAX_NAME_LENGTH ];
		float		Radius;
		float		HalfHeight;
		GE_Vec3		Position;
		GE_Quat		Rotation;
	};

	struct CBCapsule
	{
		std::string Name;
		float		Radius;
		float		HalfHeight;
		GE_Vec3		Position;
		GE_Quat		Rotation;
	};

	struct CBConvexData
	{
		char		Name[ MAX_NAME_LENGTH ];
		uint32_t	NumberOfVertices;
		uint32_t	NumberOfIndices;
		GE_Vec3		Position;
		GE_Quat		Rotation;
	};

	struct CBConvex
	{
		std::string				Name;
		std::vector< GE_Vec3 >	Vertices;
		std::vector< uint32_t > Indices;
		GE_Vec3					Position;
		GE_Quat					Rotation;
	};

	struct CBTriangleMeshData
	{
		char		Name[ MAX_NAME_LENGTH ];
		uint32_t	NumberOfVertices;
		uint32_t	NumberOfIndices;
		GE_Vec3		Position;
		GE_Quat		Rotation;
	};

	struct CBTriangleMesh
	{
		std::string				Name;
		std::vector< GE_Vec3 >	Vertices;
		std::vector< uint32_t > Indices;
		GE_Vec3					Position;
		GE_Quat					Rotation;
	};

	struct ComplexBoundData
	{
		uint32_t NumberOfBoxes;
		uint32_t NumberOfSpheres;
		uint32_t NumberOfCapsules;
		uint32_t NumberOfConvexes;
		uint32_t NumberOfTriangleMeshes;
	};

	struct ComplexBound
	{
		std::vector< CBBox >			Boxes;
		std::vector< CBSphere >			Spheres;
		std::vector< CBCapsule >		Capsules;
		std::vector< CBConvex >			Convexes;
		std::vector< CBTriangleMesh >	TriangleMeshes;
	};

	struct LodData
	{
		ID3D11Buffer		*VB;
		ID3D11Buffer		*IB;
		ID3D11Buffer		*StreamOutBuffer;
		_GE_BufferResource	TexcoordBuffer;
		_GE_BufferResource	ColorBuffer;
		_GE_BufferResource	BoneIndexBuffer;
		_GE_BufferResource	BoneWeightBuffer;
		uint32_t			NumberOfIndices;
		uint32_t			NumberOfVertices;
		LodData();
		~LodData();
	};
	//--------------------------------------- Subset --------------------------------------------------
	struct Subset
	{
		std::vector<LodData*>	Lods;
		_GE_BufferResource		MorphTargetsBuffer;
		GE_Material				*Material;
		GE_Texture				*LightMap;
		bool					UseVertexColor;
		float					LayerMultiply;
		float					LayerOffset;
		Subset();
		~Subset();
	};

	//--------------------------------------- Instance --------------------------------------------------
	struct Instance
	{
		friend		class GE_Model;
		friend		struct Subset;

		bool		Visible;
		bool		NeedUpdate;
		GE_Vec3		Position;
		GE_Vec3		Scale;
		GE_Quat		Rotation;
		GE_Bound	Bound;
		GE_Mat4x4	World;
		uint32_t	MyIndex;
		GE_Model	*ParentModel;
		void		_updateBound();
		void		_update();

		void		setBoundRadiusMultiply( float i_boundRadiusMultiply );
		void		setPosition( GE_Vec3 &i_position );
		void		setRotation( GE_Quat &i_rotation );
		void		setScale( GE_Vec3 &i_scale );
	};

private:
	friend	struct			Instance;
	friend	struct			Subset;
	friend	class			GE_DeferredGeometry;
	friend	class			GE_BasicRenderer;
	friend	class			GE_MorphController;
	friend	class			GE_SkeletonController;
	friend	class			GE_TransformController;

	bool					m_instanceNeedFill;
	bool					m_worldNeedCalculate;
	bool					m_visible;
	bool					m_bufferDynamic;
	bool					m_hasAdditionTexcoord;
	bool					m_isOk;

	uint32_t				m_numberOfRenderInstances;

	uint32_t				m_lod;
	uint32_t				m_numberOfLod;
	uint32_t				m_numberOfRenderInstance;

	std::list<std::string>	m_lodFilePath;
	std::vector< float >	m_lodDistance;
	float					m_visibleDistance;
	ComplexBound			m_complexBound;
	std::string				m_name;

	GE_Bound				m_bound;
	GE_Vec3					m_position;
	GE_Vec3					m_scale;
	GE_Quat					m_rotation;
	GE_Mat4x4				m_world;
	GE_Mat4x4				*m_transformBy;
	GE_Mat4x4				*m_transformForce;
	_GE_BufferResource		m_bonesBuffer;
	_GE_BufferResource		m_morphValuesBuffer;
	std::list<Instance *>	m_instancesList;
	std::vector<GE_Mat4x4>	m_instanceData;
	_GE_BufferResource		m_instanceBuffer;
	std::vector<Subset*>	m_subsets;
	void		loadFromMem( const uint8_t *i_memory );
	bool		loadLodFromMem( const uint8_t *i_memory );
	GE_MorphController		*m_morphController;
	GE_SkeletonController	*m_skeletonController;
	GE_TransformController	*m_transformController;

	struct subsetsData
	{
		std::vector<GE_Vec3> Vertices;
		std::vector<GE_Vec3> Normals;
		std::vector<GE_Vec4> Tangents;
		std::vector<GE_Vec4> Texcoords;
		std::vector<GE_Vec4> Colors;
		std::vector<VertexDataBoneIndex> BoneIndices;
		std::vector<VertexDataBoneWeight> BoneWeights;
		std::vector<uint32_t> Indices;
	};
	std::vector< subsetsData > m_subsetData;

public:
	GE_MorphController *getMorphController();
	GE_SkeletonController *getSkeletonController();
	GE_TransformController *getTransformController();
	GE_Model( const std::string i_fileName );
	~GE_Model();
	ComplexBound *getComplexBound();
	std::string	&getName();
	void		setName( std::string i_name );
	void		cleanUp();
	void		setLod( uint32_t i_lod );
	void		setLodDistance( uint32_t i_lod, float i_distance );
	void		setVisibleDistance( float i_distance );
	void		setVisible( bool i_visible );
	void		setPosition( GE_Vec3 &i_position );
	void		setRotation( GE_Quat &i_rotation );
	void		setScale( GE_Vec3 &i_scale );
	void		setTransformBy( const GE_Mat4x4 *i_transform );
	void		setTransformForce( const GE_Mat4x4 *i_transform );
	void		setNumberOfRenderInstances( uint32_t i_numberOfRenderInstances );
	void		setBufferDynamic( bool i_bufferDynamic );
	void		setMaterial( uint32_t i_subset, const GE_Material *i_material );
	void		setLightMap( uint32_t i_subset, const GE_Texture *i_lightMap );
	bool		isVisible();
	bool		isOk();
	bool		hasAdditionTexcoord();
	bool		isBufferDynamic();
	GE_Vec3		&getPosition();
	GE_Quat		&getRotation();
	GE_Vec3		&getScale();
	GE_Mat4x4	*getTransformBy();
	GE_Mat4x4	*getTransformForce();
	uint32_t	getNumberOfRenderInstances();
	GE_Mat4x4	*getWorld();
	GE_Bound    &getBound();
	uint32_t	getLod();
	float		getLodDistance( uint32_t i_lod );
	float		getVisibleDistance();
	uint32_t	getNumberOfLod();
	std::string &getLodFilePath( uint32_t i_lod );
	Subset		*getSubset( uint32_t i_subset );
	std::vector<Subset *> &getSubsets();
	uint32_t	getNumberOfSubsets();
	GE_Material *getMaterial( uint32_t i_subset );
	GE_Texture	*getLightMap( uint32_t i_subset );
	uint32_t	getNumberOfVertices( uint32_t i_lod );
	uint32_t	getNumberOfIndices( uint32_t i_lod );
	uint32_t	getNumberOfSubsetVertices( uint32_t i_subset, uint32_t i_lod );
	uint32_t	getNumberOfSubsetIndices( uint32_t i_subset, uint32_t i_lod );
	void		getSubsetVerticesData( uint32_t i_subset, std::vector<GE_Vec3> *i_vertices, std::vector<GE_Vec3> *i_normals,
		std::vector<GE_Vec4> *i_tangents, std::vector<GE_Vec4> *i_texcoords, std::vector<GE_Vec4> *i_colors,
		std::vector<VertexDataBoneIndex> *i_boneIndices, std::vector<VertexDataBoneWeight> *i_boneWeights,
		std::vector<uint32_t> *i_indices );

	bool		addLod( std::string &i_filePath );
	void		removeLod( uint32_t i_lod );
	void		update();
	void		render( uint32_t i_subset, uint32_t i_lod );
	void		renderInstance( uint32_t i_subset, uint32_t i_lod );
	void		renderInstanceToStreamOut( uint32_t i_subset, uint32_t i_lod );
	void		renderInstanceByStreamOut( uint32_t i_subset, uint32_t i_lod );
	void		SetUpStreamOut( uint32_t i_subset, uint32_t i_lod );
	void		SetDownStreamOut();
	void		CreateStreamOutBuffer( uint32_t i_subset, uint32_t i_lod );
	void		fillInstances();
	void		updateInstances();
	void		removeAllInstances();
	void		removeInstance( Instance **i_instance );
	void		addInstance();
	list<Instance *>::iterator createInstance( uint32_t i_count );
};

#include "GE_Model.inl"