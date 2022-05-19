#include "GEW_Physics.h"
#include <../Source/foundation/include/PsThread.h>
#include <iostream>
#include "GE_Math.h"
#include "GEditor.h"
#include "PxTkStream.h"
#include <QString>
//#include "GEW_PhysicsVehicle.h"

using namespace physx;

#define TOPXVEC3(x) ( * ( (PxVec3*)&( x ) ) )
#define TOPXQUAT(x) ( * ( (PxQuat*)&( x ) ) )

#define PVD_HOST "127.0.0.1"

#define SAFE_DELETE(p) {if(p) delete p; p = NULL;}

//GEW_PhysicsErrorCallback--------------------------------------------------------------------------------------------
void GEW_PhysicsErrorCallback::reportError( physx::PxErrorCode::Enum code, const char* message, const char* file, int line )
{
	//GEditor::getSingleton().logError( message );
}

//GEW_Physics--------------------------------------------------------------------------------------------
GEW_Physics GEW_Physics::m_singleton;
QIcon GEW_Physics::m_iconMaterial;
QIcon GEW_Physics::m_iconMaterialNull;

GEW_Physics::GEW_Physics( void )
{
	m_foundation = NULL;
	m_physics = NULL;
	m_cpuDispatcher = NULL;
	m_scene = NULL;
	m_scene2 = NULL;
	m_scene3 = NULL;
	m_connection = NULL;
	m_enable = false;
	m_timeStep = 1.0f / 60.0f;
	m_frictionPairs = NULL;
}

GEW_Physics::~GEW_Physics( void )
{
	release();
}

bool GEW_Physics::init()
{
	if( m_foundation )
		return true;

	GEW_Physics::m_iconMaterial = QIcon( ":/Resources/MetroStyle/poo.png" );
	GEW_Physics::m_iconMaterialNull = QIcon( ":/Resources/Alphaback.png" );

	m_foundation = PxCreateFoundation( PX_PHYSICS_VERSION, m_allocator, m_errorCallback );
	if( !m_foundation )
	{
		//GEditor::getSingleton().logError( "PxCreateFoundation failed!" );
		return false;
	}

	PxTolerancesScale scale;
	scale.length = GE_ConvertToUnit( 1.0f );
	scale.speed = GE_ConvertToUnit( 9.8f );
	m_physics = PxCreatePhysics( PX_PHYSICS_VERSION, *m_foundation, scale, false, NULL );
	if( !m_physics )
	{
		//GEditor::getSingleton().logError(  "PxCreatePhysics failed!" );
		return false;
	}

	/*if( !PxInitExtensions( *m_physics ) )
	{
	GEditor::getSingleton().logError( "PxInitExtensions failed!" );
	return false;
	}*/

	if( m_physics->getPvdConnectionManager() )
	{
		m_physics->getVisualDebugger()->setVisualizeConstraints( true );
		m_physics->getVisualDebugger()->setVisualDebuggerFlag( PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true );
		m_physics->getVisualDebugger()->setVisualDebuggerFlag( PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true );
		m_connection = PxVisualDebuggerExt::createConnection( m_physics->getPvdConnectionManager(), PVD_HOST, 5425, 100 );
	}

	PxU32 numWorkers = PxMax( PxI32( shdfnd::Thread::getNbPhysicalCores() ) - 1, 0 );
	m_cpuDispatcher = PxDefaultCpuDispatcherCreate( numWorkers );
	if( !m_cpuDispatcher )
	{
		//GEditor::getSingleton().logError( "PxDefaultCpuDispatcherCreate failed!" );
		return false;
	}
	PxSceneDesc sceneDesc( m_physics->getTolerancesScale() );
	sceneDesc.gravity = PxVec3( 0.0f, 0.0f, GE_ConvertToUnit( -9.8f ) );
	sceneDesc.cpuDispatcher = m_cpuDispatcher;
	sceneDesc.filterShader = FilterShader;
	m_scene = m_physics->createScene( sceneDesc );
	m_scene2 = m_physics->createScene( sceneDesc );
	m_scene3 = m_physics->createScene( sceneDesc );
	if( !m_scene || !m_scene2 || !m_scene3 )
	{
		//GEditor::getSingleton().logError( "createScene failed!" );
		return false;
	}

	m_cooking = PxCreateCooking( PX_PHYSICS_VERSION, *m_foundation, PxCookingParams( m_physics->getTolerancesScale() ) );
	if( !m_cooking )
	{
		//GEditor::getSingleton().logError( "PxCreateCooking failed!" );
		return false;
	}

	m_materialNewID = 0;
	materialAdd( "PxMaterial" );

	//GEditor::getSingleton().logInfo( "Physics initialized." );

	PxInitVehicleSDK( *m_physics );
	PxVehicleSetBasisVectors( PxVec3( 0, 0, 1.0f ), PxVec3( 1.0, 0.0f, 0 ) );
	PxVehicleSetUpdateMode( PxVehicleUpdateMode::eVELOCITY_CHANGE );
	reCreateFrictionPairs();
	return true;
}

void GEW_Physics::release()
{
	foreach( Material *material, m_materialList )
		delete material;
	m_materialList.clear();

	if( m_frictionPairs )
		m_frictionPairs->release();
	PxCloseVehicleSDK();
	m_scene3->release();
	m_scene2->release();
	m_scene->release();
	m_cpuDispatcher->release();
	if( m_connection )
		m_connection->release();
	m_physics->release();
	m_cooking->release();
	//PxCloseExtensions();
	m_foundation->release();
}

GEW_Physics::Material* GEW_Physics::materialAdd( const QString &i_name )
{
	m_materialNewID++;

	Material *material = new Material( i_name );
	material->Mat = m_physics->createMaterial( 0.5f, 0.5f, 0.5f );
	m_materialList.push_back( material );
	return material;
}

void GEW_Physics::materialRemove( int32_t i_index )
{

}

GEW_Physics::Material* GEW_Physics::getMaterial( int32_t i_index )
{
	return m_materialList[ i_index ];
}

physx::PxMaterial *GEW_Physics::getPxMaterial( int32_t i_index )
{
	return m_materialList[ i_index ]->Mat;
}

QString GEW_Physics::getMaterialName( int32_t i_index )
{
	return m_materialList[ i_index ]->Name;
}

size_t GEW_Physics::getNbMaterial()
{
	return m_materialList.size();
}

int32_t GEW_Physics::materialFind( const QString &i_name )
{
	int32_t index = 0;
	foreach( Material *material, m_materialList )
	{
		if( material->Name.toLower() == i_name )
			return index;
		index++;
	}
	return -1;
}

void GEW_Physics::materialSetHasFrictionPair( int32_t i_index, bool i_hasFrictionPair )
{
	m_materialList[ i_index ]->HasTireFrictionPair = i_hasFrictionPair;
	reCreateFrictionPairs();
	//recreate friction pair
}

void GEW_Physics::materialSetFrictionPairWet( int32_t i_index, float i_value )
{
	m_materialList[ i_index ]->TireFrictionWet = i_value;
	m_frictionPairs->setTypePairFriction( m_materialList[ i_index ]->_IndexInTireFrictionPairList, TIRE_TYPE_WETS, i_value );
	//update friction pair
}

void GEW_Physics::materialSetFrictionPairSlick( int32_t i_index, float i_value )
{
	m_materialList[ i_index ]->TireFrictionSlick = i_value;
	m_frictionPairs->setTypePairFriction( m_materialList[ i_index ]->_IndexInTireFrictionPairList, TIRE_TYPE_SLICKS, i_value );
	//update friction pair
}

void GEW_Physics::materialSetFrictionPairIce( int32_t i_index, float i_value )
{
	m_materialList[ i_index ]->TireFrictionIce = i_value;
	m_frictionPairs->setTypePairFriction( m_materialList[ i_index ]->_IndexInTireFrictionPairList, TIRE_TYPE_ICE, i_value );
	//update friction pair
}

void GEW_Physics::materialSetFrictionPairMud( int32_t i_index, float i_value )
{
	m_materialList[ i_index ]->TireFrictionMud = i_value;
	m_frictionPairs->setTypePairFriction( m_materialList[ i_index ]->_IndexInTireFrictionPairList, TIRE_TYPE_MUD, i_value );
	//update friction pair
}

void GEW_Physics::materialSave( QString i_path )
{
	/*FILE *fp;
	fopen_s( &fp, ( i_path + "PxMaterial.ges" ).toStdString().c_str(), "wb" );
	if( !fp )
	return;

	std::vector<_PxMaterialData> list;
	list.resize( m_materialList.size() );

	for( uint32_t i=0; i<m_materialList.size(); ++i )
	{
	strcpy_s( list[ i ].Name, m_materialList[ i ]->Name.toStdString().c_str() );
	list[ i ].DisableFriction = m_materialList[ i ]->Mat->getFlags().isSet( PxMaterialFlag::eDISABLE_FRICTION );
	list[ i ].DisableStrongFriction = m_materialList[ i ]->Mat->getFlags().isSet( PxMaterialFlag::eDISABLE_STRONG_FRICTION );
	list[ i ].StaticFriction = m_materialList[ i ]->Mat->getStaticFriction();
	list[ i ].DynamicFriction = m_materialList[ i ]->Mat->getDynamicFriction();
	list[ i ].Restitution = m_materialList[ i ]->Mat->getRestitution();
	list[ i ].FrictionCombMode = (int32_t)m_materialList[ i ]->Mat->getFrictionCombineMode();
	list[ i ].RestitutionCombMode = (int32_t)m_materialList[ i ]->Mat->getRestitutionCombineMode();
	list[ i ].HasTireFrictionPair = m_materialList[ i ]->HasTireFrictionPair;
	list[ i ].TireFrictionWet = m_materialList[ i ]->TireFrictionWet;
	list[ i ].TireFrictionSlick = m_materialList[ i ]->TireFrictionSlick;
	list[ i ].TireFrictionIce = m_materialList[ i ]->TireFrictionIce;
	list[ i ].TireFrictionMud = m_materialList[ i ]->TireFrictionMud;
	}

	char *header = "GE";
	uint32_t count = m_materialList.size();
	uint32_t id = m_materialNewID;
	fwrite(header, sizeof(char), 2, fp);
	fwrite(&count, sizeof(uint32_t), 1, fp);
	fwrite(&id, sizeof(uint32_t), 1, fp);

	fwrite(list.data(), sizeof(_PxMaterialData), list.size(), fp);
	fclose(fp);*/
}

void GEW_Physics::materialLoad( QString i_path )
{
	/*FILE *fp;
	fopen_s( &fp, ( i_path + "PxMaterial.ges" ).toStdString().c_str(), "rb" );
	if(!fp)
	return;
	char header[ 2 ];
	uint32_t count;
	uint32_t id;
	fread( header, sizeof(char), 2, fp );
	fread( &count, sizeof(uint32_t), 1, fp );
	fread( &id, sizeof(uint32_t), 1, fp );
	if( count )
	{
	//clear all PxMaterials
	for (uint32_t i = 0; i<m_materialList.size(); ++i)
	{
	m_materialList[ i ]->Mat->release();
	SAFE_DELETE( m_materialList[ i ] );
	}
	m_materialList.clear();
	m_materialNewID = id;
	std::vector<_PxMaterialData> list;
	list.resize(count);
	fread( list.data(), sizeof(_PxMaterialData), count, fp );

	for (uint32_t i = 0; i < count; ++i)
	{
	Material *material = materialAdd( list[ i ].Name  );
	material->Mat->setFlag( PxMaterialFlag::eDISABLE_FRICTION, list[ i ].DisableFriction );
	material->Mat->setFlag( PxMaterialFlag::eDISABLE_STRONG_FRICTION, list[ i ].DisableStrongFriction );
	material->Mat->setStaticFriction( list[ i ].StaticFriction );
	material->Mat->setDynamicFriction( list[ i ].DynamicFriction );
	material->Mat->setRestitution( list[ i ].Restitution );
	material->Mat->setFrictionCombineMode( (physx::PxCombineMode::Enum)list[ i ].FrictionCombMode );
	material->Mat->setRestitutionCombineMode( (physx::PxCombineMode::Enum)list[ i ].RestitutionCombMode );

	material->HasTireFrictionPair = list[ i ].HasTireFrictionPair;
	material->TireFrictionWet = list[ i ].TireFrictionWet;
	material->TireFrictionSlick = list[ i ].TireFrictionSlick;
	material->TireFrictionIce = list[ i ].TireFrictionIce;
	material->TireFrictionMud = list[ i ].TireFrictionMud;
	}
	reCreateFrictionPairs();
	}

	fclose( fp );*/
}

void GEW_Physics::saveToFile( QFile &i_file, const QString &i_resourcePath )
{
	std::vector<MaterialSaveData> list;
	list.resize( m_materialList.size() );
	for( int i = 0; i < ( int ) m_materialList.size(); ++i )
	{
		strcpy_s( list[ i ].Name, m_materialList[ i ]->Name.toStdString().c_str() );
		list[ i ].DisableFriction = m_materialList[ i ]->Mat->getFlags().isSet( PxMaterialFlag::eDISABLE_FRICTION );
		list[ i ].DisableStrongFriction = m_materialList[ i ]->Mat->getFlags().isSet( PxMaterialFlag::eDISABLE_STRONG_FRICTION );
		list[ i ].StaticFriction = m_materialList[ i ]->Mat->getStaticFriction();
		list[ i ].DynamicFriction = m_materialList[ i ]->Mat->getDynamicFriction();
		list[ i ].Restitution = m_materialList[ i ]->Mat->getRestitution();
		list[ i ].FrictionCombMode = ( int32_t ) m_materialList[ i ]->Mat->getFrictionCombineMode();
		list[ i ].RestitutionCombMode = ( int32_t ) m_materialList[ i ]->Mat->getRestitutionCombineMode();
		list[ i ].HasTireFrictionPair = m_materialList[ i ]->HasTireFrictionPair;
		list[ i ].TireFrictionWet = m_materialList[ i ]->TireFrictionWet;
		list[ i ].TireFrictionSlick = m_materialList[ i ]->TireFrictionSlick;
		list[ i ].TireFrictionIce = m_materialList[ i ]->TireFrictionIce;
		list[ i ].TireFrictionMud = m_materialList[ i ]->TireFrictionMud;

	}
}

void GEW_Physics::loadFromFile( QFile &i_file, const QString &i_resourcePath )
{

}

//Mesh creation
physx::PxTriangleMesh* GEW_Physics::createTriangleMesh32( const GE_Vec3* i_verts, uint32_t i_vertCount, const uint32_t* i_indices32, uint32_t i_triCount )
{
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = i_vertCount;
	meshDesc.points.stride = sizeof( GE_Vec3 );
	meshDesc.points.data = i_verts;

	meshDesc.triangles.count = i_triCount;
	meshDesc.triangles.stride = 3 * sizeof( uint32_t );
	meshDesc.triangles.data = i_indices32;

	PxTriangleMesh *triangleMesh = NULL;

	PxToolkit::MemoryOutputStream writeBuffer;
	if( m_cooking->cookTriangleMesh( meshDesc, writeBuffer ) )
	{
		PxToolkit::MemoryInputData readBuffer( writeBuffer.getData(), writeBuffer.getSize() );
		triangleMesh = m_physics->createTriangleMesh( readBuffer );
	}

	return triangleMesh;
}
physx::PxConvexMesh* GEW_Physics::createConvexMesh( const GE_Vec3* i_verts, uint32_t i_vertCount )
{
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = i_vertCount;
	convexDesc.points.stride = sizeof( GE_Vec3 );
	convexDesc.points.data = i_verts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eINFLATE_CONVEX;

	PxConvexMesh* convexMesh = NULL;

	PxToolkit::MemoryOutputStream writeBuffer;
	if( m_cooking->cookConvexMesh( convexDesc, writeBuffer ) )
	{
		PxToolkit::MemoryInputData readBuffer( writeBuffer.getData(), writeBuffer.getSize() );
		convexMesh = m_physics->createConvexMesh( readBuffer );
	}

	return convexMesh;
}

physx::PxTriangleMesh* GEW_Physics::createTriangleMesh32( physx::PxTriangleMeshDesc &i_meshDesc )
{
	PxTriangleMesh *triangleMesh = NULL;

	PxToolkit::MemoryOutputStream writeBuffer;
	if( m_cooking->cookTriangleMesh( i_meshDesc, writeBuffer ) )
	{
		PxToolkit::MemoryInputData readBuffer( writeBuffer.getData(), writeBuffer.getSize() );
		triangleMesh = m_physics->createTriangleMesh( readBuffer );
	}

	return triangleMesh;
}

void GEW_Physics::reCreateFrictionPairs()
{
	if( m_frictionPairs )
		m_frictionPairs->release();
	m_frictionPairs = NULL;

	std::vector<GEW_Physics::Material*> materials;
	std::vector<physx::PxMaterial*> pxMaterials;
	std::vector<physx::PxVehicleDrivableSurfaceType> surfaceTypes;

	for( uint32_t i = 0; i < m_materialList.size(); ++i )
	{
		if( m_materialList[ i ]->HasTireFrictionPair )
		{
			m_materialList[ i ]->_IndexInTireFrictionPairList = ( int32_t ) materials.size(); // Surface type index in m_frictionpair
			materials.push_back( m_materialList[ i ] );
		}
	}

	if( materials.size() == 0 )//no material has friction pair. so use default material to avoid error!
	{
		m_materialList[ 0 ]->_IndexInTireFrictionPairList = 0;
		materials.push_back( m_materialList[ 0 ] );
	}

	pxMaterials.resize( materials.size() );
	surfaceTypes.resize( materials.size() );

	for( uint32_t i = 0; i < materials.size(); ++i )
	{
		pxMaterials[ i ] = materials[ i ]->Mat;
		surfaceTypes[ i ].mType = i; // Surface type
	}

	m_frictionPairs = PxVehicleDrivableSurfaceToTireFrictionPairs::allocate( MAX_NUM_TIRE_TYPES, ( uint32_t ) pxMaterials.size() );
	m_frictionPairs->setup( MAX_NUM_TIRE_TYPES, ( uint32_t ) pxMaterials.size(), ( const physx::PxMaterial** )pxMaterials.data(), surfaceTypes.data() );
	for( uint32_t i = 0; i < materials.size(); ++i )
	{
		m_frictionPairs->setTypePairFriction( i, TIRE_TYPE_WETS, materials[ i ]->TireFrictionWet );
		m_frictionPairs->setTypePairFriction( i, TIRE_TYPE_SLICKS, materials[ i ]->TireFrictionSlick );
		m_frictionPairs->setTypePairFriction( i, TIRE_TYPE_ICE, materials[ i ]->TireFrictionIce );
		m_frictionPairs->setTypePairFriction( i, TIRE_TYPE_MUD, materials[ i ]->TireFrictionMud );
	}
}

physx::PxRigidDynamic* GEW_Physics::createVehicleActor(
	const physx::PxVehicleChassisData& i_chassisData,
	physx::PxMaterial** i_wheelMaterials,
	physx::PxConvexMesh** i_wheelConvexMeshes,
	const uint32_t i_numWheels,
	physx::PxMaterial** i_chassisMaterials,
	physx::PxConvexMesh** i_chassisConvexMeshes,
	const uint32_t i_numChassisMeshes )
{
	//We need a rigid body actor for the vehicle.
	//Don't forget to add the actor to the scene after setting up the associated vehicle.
	PxRigidDynamic* vehActor = m_physics->createRigidDynamic( PxTransform( PxIdentity ) );

	//Wheel and chassis simulation filter data.
	PxFilterData wheelSimFilterData;
	wheelSimFilterData.word0 = COLLISION_FLAG_WHEEL;
	wheelSimFilterData.word1 = COLLISION_FLAG_WHEEL_AGAINST;
	PxFilterData chassisSimFilterData;
	chassisSimFilterData.word0 = COLLISION_FLAG_CHASSIS;
	chassisSimFilterData.word1 = COLLISION_FLAG_CHASSIS_AGAINST;

	//Wheel and chassis query filter data.
	//Optional: cars don't drive on other cars.
	PxFilterData wheelQryFilterData;
	wheelQryFilterData.word3 = UNDRIVABLE_SURFACE; // setupNonDrivableSurface
	PxFilterData chassisQryFilterData;
	chassisQryFilterData.word3 = UNDRIVABLE_SURFACE;// setupNonDrivableSurface

	//Add all the wheel shapes to the actor.
	for( uint32_t i = 0; i < i_numWheels; ++i )
	{
		PxConvexMeshGeometry geom( i_wheelConvexMeshes[ i ] );
		//geom.scale.scale = GE_ConvertToUnit( geom.scale.scale ) ;
		PxShape* wheelShape = vehActor->createShape( geom, *i_wheelMaterials[ i ] );
		wheelShape->setQueryFilterData( wheelQryFilterData );
		wheelShape->setSimulationFilterData( wheelSimFilterData );
		wheelShape->setLocalPose( PxTransform( PxIdentity ) );
	}

	//Add the chassis shapes to the actor.
	for( uint32_t i = 0; i < i_numChassisMeshes; ++i )
	{
		PxConvexMeshGeometry geom( i_chassisConvexMeshes[ i ] );
		PxShape* chassisShape = vehActor->createShape( geom, *i_chassisMaterials[ i ] );
		chassisShape->setQueryFilterData( chassisQryFilterData );
		chassisShape->setSimulationFilterData( chassisSimFilterData );
		chassisShape->setLocalPose( PxTransform( PxIdentity ) );
	}

	vehActor->setMass( i_chassisData.mMass );
	vehActor->setMassSpaceInertiaTensor( i_chassisData.mMOI );
	vehActor->setCMassLocalPose( PxTransform( i_chassisData.mCMOffset, PxQuat( PxIdentity ) ) );

	return vehActor;
}
