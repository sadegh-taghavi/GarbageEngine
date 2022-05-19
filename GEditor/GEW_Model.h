#pragma once

#include "GEW_Item.h"
#include "GE_Vec3.h"
#include "GE_RenderUtility.h"

namespace physx
{
	class PxRigidStatic;
	class PxRigidDynamic;
	class PxShape;
	class PxConvexMesh;
	class PxTriangleMesh;
};

class GE_Model;
class GE_RenderUtilityObject;
struct GE_Material;

class GEW_Model : public GEW_Item
{
public:
	static QString					m_lastFileName;
	static QString					m_lastLightMapName;
	static QIcon					m_iconModel;
	static QIcon					m_iconSubset;
	static QIcon					m_iconSubsetNoMaterial;
	static QList< GEW_Model* >		m_list;
	static GEW_Model				*find( QString &i_name );
	static GEW_Model				*find( uint32_t i_id );
	static GEW_Model				*CreateFromFile( GEW_Item *i_parent, QFile &i_file, const QString &i_resourcePath );

	bool							m_cullingEnable;
	bool							m_dynamic;
	bool							m_shadowCaster;
	bool							m_transparent;
	bool							m_twoSideDepth;
	bool							m_twoSideShade;
	QStringList						m_subsetName;
	GE_Model						*m_model;
	GE_RenderUtilityObject			*m_renderUtilityBB;
	GE_RenderUtilityObject			*m_pxRenderUtilityCenterOfMass;
	QVector<GE_RenderUtilityObject*> m_pxRenderUtilityObjects;

	struct ActorData
	{
		bool		Enable;
		enum Type
		{
			Static = 0,
			Dynamic
		}			MyType;
		bool		UseMass;
		float		Mass;
		bool		UseCenterOfMass;
		GE_Vec3		CenterOfMass;
		bool		ShowCenterOfMass;
		float		LinearDamping;
		float		AngularDampin;
		struct
		{
			bool	eKINEMATIC;
			bool	eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES;
			bool	eENABLE_CCD;
			bool	eENABLE_CCD_FRICTION;
		}RigidBodyFlags;
		struct
		{
			bool	eVISUALIZATION;
			bool	eDISABLE_GRAVITY;
			bool	eSEND_SLEEP_NOTIFIES;
			bool	eDISABLE_SIMULATION;
		}ActorFlags;
		ActorData();
	};
	struct ShapeData
	{
		enum Type
		{
			Unknown = 0,
			Box,
			Sphere,
			Capsule,
			Convex,
			TriangleMesh
		}			MyType;
		bool		Use;
		uint32_t	MaterialID;
		struct
		{
			bool	eVISUALIZATION;
			bool	eTRIGGER_SHAPE;
			bool	eSIMULATION_SHAPE;
			bool	eSCENE_QUERY_SHAPE;
			bool	ePARTICLE_DRAIN;
		}			ShapeFlags;
		enum Surface
		{
			Groung = 0,
			Obstacle,
			DrivableObstacle
		}			MySurface;
		float		Mass;
		bool		BoundVisible;
		GE_Vec3		BoundColor;
		ShapeData();
	};
	struct SaveData
	{
		char		Name[ 64 ];
		char		FileName[ 512 ];
		GE_Vec3		Position;
		GE_Vec3		Scale;
		GE_Quat		Rotation;
		bool		BufferDynamic;
		bool		Visible;
		bool		Dynamic;
		bool		Culling;
		bool		TwoSideDepth;
		bool		TwoSideShade;
		bool		Transparent;
		bool		ShadowCaster;
		uint32_t	NumberOfSubset;
		uint32_t	NumberOfLod;
		uint32_t	CurrentLod;
		GE_Vec3		BoundColor;
		uint32_t	NbShape;
		float		VisibleDistance;
		SaveData();
	};
	struct SubsetSaveData
	{
		char	Name[ 64 ];
		float	LayerMultiply;
		float	LayerOffset;
		bool	UseVertexColor;
		char	LightMap[ 512 ];
		int		MaterialGroupId;
		int		MaterialId;
		SubsetSaveData();
	};
	struct LodSaveData
	{
		char		LodFileName[ 512 ];
		float		LodDistance;
		LodSaveData();
	};

	GE_Mat4x4					m_pxMatrix;
	physx::PxRigidStatic		*m_pxRigidStatic;
	physx::PxRigidDynamic		*m_pxRigidDynamic;
	QVector<physx::PxShape*>	m_pxShapeList;
	QVector<physx::PxConvexMesh*> m_pxConvexMeshList;
	QVector<physx::PxTriangleMesh*> m_pxTriangleMeshList;

	ActorData					m_pxActorData;
	QVector<ShapeData>			m_pxShapeData;
	QVector<void*>				m_pxShapePointer;
	QStringList					m_pxShapeName;

	GEW_Model( const QString &i_fileName, GEW_Item *i_parent );
	GEW_Model( GE_Model *i_model, GEW_Item *i_parent );
	~GEW_Model();

	void	replaceMaterial( GE_Material *i_material, GE_Material *i_replaceWith );

	void	initPx();
	void	setCullingEnable( bool i_enable );
	bool	isCullingEnable();
	void	setDynamic( bool i_dynamic );
	bool	isDynamic();
	void	setTransparent(bool i_transparent);
	bool	isTransparent();
	void	setTwoSideDepth(bool i_twoSide);
	bool	isTwoSideDepth();
	void	setTwoSideShade(bool i_twoSide);
	bool	isTwoSideShade();
	void	setShadowCaster( bool i_enable );
	bool	isShadowCaster();
	void	changeModel( GE_Model *i_model );
	QString &getSubsetName( uint32_t i_index );
	void	setSubsetName( uint32_t i_index, const QString &i_name );
	int		getSubsetIndex( const QString &i_name );
	uint32_t getPxShapeCount();
	QString &getPxShapeName( uint32_t i_index );
	bool	createPhysics();
	void	destroyPhysics();
	void	setLinearVelocity( GE_Vec3 &i_velocity );
	void	setAngularVelocity( GE_Vec3 &i_velocity );
	void	setSurfaceType( uint32_t i_shapeIndex, GEW_Model::ShapeData::Surface i_surface );
	void	setShowCenterOfMass( bool i_show );
	bool	isPhysicsEnable();

	GEW_Item	*duplicate();
	void		update();
	void		select();
	void		deSelect();
	void		checkStateChanged( GEW_Item::CheckIndex i_column );
	void		parentCheckStateChanged( GEW_Item::CheckIndex i_column, Qt::CheckState i_checkState );
	void		saveToFile( QFile &i_file, const QString &i_resourcePath );
	void		setPosition( const GE_Vec3 &i_position );
	void		setPosition2( const GE_Vec3 &i_position );
	void		setRotation( const GE_Vec3 &i_rotation );
	void		setRotation( GE_Mat4x4 &i_rot, GE_Vec3 & i_pivot );
	void		setScale( const GE_Vec3 &i_scale );
	void		setTransform( GE_Mat4x4 *i_matrix );
	GE_Vec3		getPosition();
	GE_Vec3		getRotation();
	GE_Vec3		getScale();
	void		getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList );

};
