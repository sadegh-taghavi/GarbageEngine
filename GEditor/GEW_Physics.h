#pragma once

#include <PxPhysicsAPI.h>
#include <QIcon>
#include <QList>
#include <QFile>
#include <stdint.h>
#include <GE_Math.h>

#define TO_PxVec3( geVec3 ) ( *( (physx::PxVec3*)&geVec3 ) )
#define TO_PxQuat( geQuat ) ( *( (physx::PxQuat*)&geQuat ) )

#define TO_GEVec3( pxVec3 ) ( *( (GE_Vec3*)&pxVec3 ) )
#define TO_GEQuat( pxQuat ) ( *( (GE_Quat*)&pxQuat ) )

static inline physx::PxFilterFlags FilterShader(	physx::PxFilterObjectAttributes attributes0, 
													physx::PxFilterData filterData0, 
													physx::PxFilterObjectAttributes attributes1, 
													physx::PxFilterData filterData1,
													physx::PxPairFlags& pairFlags, 
													const void* constantBlock, 
													physx::PxU32 constantBlockSize );

class GEW_PhysicsErrorCallback : public physx::PxErrorCallback
{
public:
	//GEW_PhysicsErrorCallback();
	//~GEW_PhysicsErrorCallback();
	virtual void reportError( physx::PxErrorCode::Enum code, const char* message, const char* file, int line );
};

class GEW_Physics
{
public:
	static GEW_Physics m_singleton;

	physx::PxDefaultAllocator			m_allocator;
	GEW_PhysicsErrorCallback			m_errorCallback;
	physx::PxFoundation					*m_foundation;
	physx::PxPhysics					*m_physics;
	physx::PxCooking					*m_cooking;
	physx::PxDefaultCpuDispatcher		*m_cpuDispatcher;
	physx::PxScene						*m_scene;
	physx::PxScene						*m_scene2;
	physx::PxScene						*m_scene3;
	physx::PxVisualDebuggerConnection	*m_connection;
	bool								m_enable;
	float								m_timeStep;
	float								m_elapsedTime;

	GEW_Physics(void);
	~GEW_Physics(void);
//public:
	static	QIcon m_iconMaterial;
	static	QIcon m_iconMaterialNull;
	static	inline GEW_Physics &getSingleton();
	inline physx::PxScene *getScene2();
	bool	init();
	void	release();
	void	setEnable( bool i_enable );
	bool	isEnable();
	void	setTimeStep( float i_timeStep );
	float	getTimeStep();
	bool	singleStep( float i_elapsedTime );

	static inline void getColumnMajor( physx::PxTransform &i_transform, float* i_mat );

	enum
	{
		DRIVABLE_SURFACE = 0xffff0000,
		UNDRIVABLE_SURFACE = 0x0000ffff
	};

	enum
	{
		COLLISION_FLAG_GROUND = 1 << 0,
		COLLISION_FLAG_WHEEL = 1 << 1,
		COLLISION_FLAG_CHASSIS = 1 << 2,
		COLLISION_FLAG_OBSTACLE = 1 << 3,
		COLLISION_FLAG_DRIVABLE_OBSTACLE = 1 << 4,

		COLLISION_FLAG_GROUND_AGAINST = COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_WHEEL_AGAINST = COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE,
		COLLISION_FLAG_CHASSIS_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	};

	enum
	{
		TIRE_TYPE_WETS = 0,
		TIRE_TYPE_SLICKS,
		TIRE_TYPE_ICE,
		TIRE_TYPE_MUD,
		MAX_NUM_TIRE_TYPES
	};

	struct Material
	{
		physx::PxMaterial	*Mat;
		QString				Name;
		bool				HasTireFrictionPair;
		float				TireFrictionWet;
		float				TireFrictionSlick;
		float				TireFrictionIce;
		float				TireFrictionMud;
		int32_t				_IndexInTireFrictionPairList;
		Material( const QString  &i_name )
		{
			Mat = NULL;
			Name = i_name;
			HasTireFrictionPair = false;
			TireFrictionWet = 1.0f;
			TireFrictionSlick = 1.0f;
			TireFrictionIce = 1.0f;
			TireFrictionMud = 1.0f;
			_IndexInTireFrictionPairList = -1;
		}
	};

	struct MaterialSaveData
	{
		char	Name[ 64 ];
		bool	DisableFriction;
		bool	DisableStrongFriction;
		float	StaticFriction;
		float	DynamicFriction;
		float	Restitution;
		int32_t	FrictionCombMode;
		int32_t	RestitutionCombMode;

		bool	HasTireFrictionPair;
		float	TireFrictionWet;
		float	TireFrictionSlick;
		float	TireFrictionIce;
		float	TireFrictionMud;
	};

	uint32_t				m_materialNewID;
	std::vector<Material*>	m_materialList;
	GEW_Physics::Material*	materialAdd( const QString &i_name );
	void					materialRemove( int32_t i_index );
	GEW_Physics::Material*	getMaterial( int32_t i_index );
	physx::PxMaterial*		getPxMaterial( int32_t i_index );
	QString					getMaterialName( int32_t i_index );
	size_t getNbMaterial();
	int32_t					materialFind( const QString &i_name );
	void					materialSetHasFrictionPair( int32_t i_index, bool i_hasFrictionPair );
	void					materialSetFrictionPairWet( int32_t i_index, float i_value );
	void					materialSetFrictionPairSlick( int32_t i_index, float i_value );
	void					materialSetFrictionPairIce( int32_t i_index, float i_value );
	void					materialSetFrictionPairMud( int32_t i_index, float i_value );
	void					materialSave( QString i_path );
	void					materialLoad( QString i_path );

	void saveToFile( QFile &i_file, const QString &i_resourcePath );
	void loadFromFile( QFile &i_file, const QString &i_resourcePath );
	//Mesh creation
	physx::PxTriangleMesh* createTriangleMesh32( const GE_Vec3* i_verts, uint32_t i_vertCount, const uint32_t* i_indices32, uint32_t i_triCount );
	physx::PxConvexMesh* createConvexMesh( const GE_Vec3* i_verts, uint32_t i_vertCount );
	//this function is for MultiMaterial and MultiMaterialTerrain
	physx::PxTriangleMesh* createTriangleMesh32( physx::PxTriangleMeshDesc &i_meshDesc );

	//Vehicle-------------------------------------------------------------------------------------------------------
	physx::PxVehicleDrivableSurfaceToTireFrictionPairs*	m_frictionPairs;
	void reCreateFrictionPairs();
	physx::PxRigidDynamic* createVehicleActor(	
		const physx::PxVehicleChassisData& i_chassisData,
		physx::PxMaterial** i_wheelMaterials, 
		physx::PxConvexMesh** i_wheelConvexMeshes, 
		const uint32_t i_numWheels, 
		physx::PxMaterial** i_chassisMaterials, 
		physx::PxConvexMesh** i_chassisConvexMeshes, 
		const uint32_t i_numChassisMeshes );
};

#include "GEW_Physics.inl"