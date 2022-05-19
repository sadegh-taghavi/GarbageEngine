#include "GEW_Model.h"
#include "GE_Model.h"
#include "GE_RenderUtility.h"
#include "GE_Material.h"
#include "GEW_Material.h"
#include "GEW_Utility.h"
#include "GEW_Physics.h"

QString				GEW_Model::m_lastFileName;
QString				GEW_Model::m_lastLightMapName;
QIcon				GEW_Model::m_iconModel;
QIcon				GEW_Model::m_iconSubset;
QIcon				GEW_Model::m_iconSubsetNoMaterial;
QList< GEW_Model* > GEW_Model::m_list;

GEW_Model *GEW_Model::find( QString &i_name )
{
	foreach( GEW_Model *model, m_list )
	if ( model->text( 0 ).toLower() == i_name )
		return model;
	return NULL;
}

GEW_Model *GEW_Model::find( uint32_t i_id )
{
	foreach( GEW_Model *model, m_list )
	if ( model->getId() == i_id )
		return model;
	return NULL;
}

GEW_Model *GEW_Model::CreateFromFile( GEW_Item *i_parent, QFile &i_file, const QString &i_resourcePath )
{
	GEW_Model::SaveData modelData;
	i_file.read( ( char* )&modelData, sizeof( GEW_Model::SaveData ) );

	GEW_Model *gewModel = new GEW_Model(
		GEW_Utility::absolutePath( modelData.FileName, i_resourcePath ).c_str(),
		i_parent );
	GE_Model *model = gewModel->m_model;

	gewModel->setText( 0, modelData.Name );
	model->setPosition( modelData.Position );
	model->setRotation( modelData.Rotation );
	model->setScale( modelData.Scale );
	model->setBufferDynamic( modelData.BufferDynamic );
	model->setVisibleDistance( modelData.VisibleDistance );
	model->setVisible( modelData.Visible );

	gewModel->setItemCheckState( GEW_Item::Enable, modelData.Visible );
	gewModel->setCullingEnable( modelData.Culling );
	gewModel->setDynamic( modelData.Dynamic );
	gewModel->setTransparent( modelData.Transparent );
	gewModel->setShadowCaster( modelData.ShadowCaster );
	gewModel->setTwoSideDepth( modelData.TwoSideDepth );
	gewModel->setTwoSideShade( modelData.TwoSideShade );

	for ( uint32_t subsetIndex = 0; subsetIndex < modelData.NumberOfSubset; ++subsetIndex )
	{
		GEW_Model::SubsetSaveData subsetData;
		i_file.read( ( char* )&subsetData, sizeof( GEW_Model::SubsetSaveData ) );
		GE_Model::Subset *subset = model->getSubset( subsetIndex );
		subset->LayerMultiply = subsetData.LayerMultiply;
		subset->LayerOffset = subsetData.LayerOffset;
		subset->UseVertexColor = subsetData.UseVertexColor;
		gewModel->setSubsetName( subsetIndex, subsetData.Name );
		if ( subsetData.LightMap[ 0 ] != 0 )
			subset->LightMap = GE_TextureManager::getSingleton().createTexture(
			GEW_Utility::absolutePath( subsetData.LightMap, i_resourcePath ).c_str() );
		subset->Material = GEW_Material::getSingleton().getMaterial(
			subsetData.MaterialGroupId,
			subsetData.MaterialId );
	}

	for ( uint32_t lodIndex = 0; lodIndex < modelData.NumberOfLod; ++lodIndex )
	{
		GEW_Model::LodSaveData lodData;
		i_file.read( ( char* )&lodData, sizeof( GEW_Model::LodSaveData ) );
		if ( lodIndex > 0 )
			model->addLod( GEW_Utility::absolutePath( lodData.LodFileName, i_resourcePath ) );
		model->setLodDistance( lodIndex, lodData.LodDistance );
	}
	model->setLod( modelData.CurrentLod );
	gewModel->m_renderUtilityBB->setColor( modelData.BoundColor );
	gewModel->update();

	i_file.read( ( char* )&gewModel->m_pxActorData, sizeof( GEW_Model::ActorData ) );
	if ( modelData.NbShape )
		i_file.read( ( char* )&gewModel->m_pxShapeData[ 0 ], sizeof( GEW_Model::ShapeData ) * modelData.NbShape );

	if ( gewModel->m_pxActorData.Enable )
		gewModel->m_pxActorData.Enable = gewModel->createPhysics();

	gewModel->setItemCheckState( GEW_Item::Enable, modelData.Visible );
	gewModel->setItemCheckState( GEW_Item::Dynamic, modelData.Dynamic );
	gewModel->setItemCheckState( GEW_Item::Physics, gewModel->m_pxActorData.Enable );

	return gewModel;
}

GEW_Model::ActorData::ActorData()
{
	Enable = false;
	MyType = ActorData::Static;
	UseMass = false;
	Mass = 1.0f;
	UseCenterOfMass = false;
	CenterOfMass = GE_Vec3( 0.0f, 0.0f, 0.0f );
	ShowCenterOfMass = false;
	LinearDamping = 0.0f;
	AngularDampin = GE_ConvertToRadians( 2.865f );

	RigidBodyFlags.eKINEMATIC = false;
	RigidBodyFlags.eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES = false;
	RigidBodyFlags.eENABLE_CCD = false;
	RigidBodyFlags.eENABLE_CCD_FRICTION = false;

	ActorFlags.eVISUALIZATION = false;
	ActorFlags.eDISABLE_GRAVITY = false;
	ActorFlags.eSEND_SLEEP_NOTIFIES = false;
	ActorFlags.eDISABLE_SIMULATION = false;
}

GEW_Model::ShapeData::ShapeData()
{
	MyType = ShapeData::Unknown;
	Use = true;
	MaterialID = 0;
	ShapeFlags.eVISUALIZATION = false;
	ShapeFlags.eTRIGGER_SHAPE = false;
	ShapeFlags.eSIMULATION_SHAPE = true;
	ShapeFlags.eSCENE_QUERY_SHAPE = true;
	ShapeFlags.ePARTICLE_DRAIN = false;
	MySurface = ShapeData::Obstacle;
	Mass = 1.0f;
	BoundVisible = false;
	BoundColor = GE_Vec3( 1.0f, 1.0f, 1.0f );
}

GEW_Model::SaveData::SaveData()
{
	memset( this, 0, sizeof( SaveData ) );
}

GEW_Model::SubsetSaveData::SubsetSaveData()
{
	memset( this, 0, sizeof( SubsetSaveData ) );
}

GEW_Model::LodSaveData::LodSaveData()
{
	memset( this, 0, sizeof( LodSaveData ) );
}

GEW_Model::GEW_Model( const QString &i_fileName, GEW_Item *i_parent )
: GEW_Item( i_parent, GEW_Item::Model )
{
	m_model = new GE_Model( i_fileName.toStdString() );

	GE_Material *material = GEW_Material::getSingleton().getNullMaterial();
	for ( uint32_t i = 0; i < m_model->getNumberOfSubsets(); ++i )
	{
		m_model->getSubset( i )->Material = material;
		m_subsetName.push_back( QString( "%1" ).arg( i ) );
	}

	GE_Vec3 size = ( m_model->getBound().RawBoundBoxMax - m_model->getBound().RawBoundBoxMin ) * 0.5f;
	GE_Vec3 offset = ( m_model->getBound().RawBoundBoxMax + m_model->getBound().RawBoundBoxMin ) * 0.5f;

	m_renderUtilityBB = new GE_RenderUtilityObject();
	m_renderUtilityBB->createBox( size, offset );
	m_renderUtilityBB->setColor( GE_Vec3( 1.0, 1.0f, 0.0f ) );
	m_renderUtilityBB->setDepthDisable( false );
	m_renderUtilityBB->setVisible( false );
	m_cullingEnable = true;
	m_dynamic = false;
	m_shadowCaster = true;
	m_transparent = false;
	m_twoSideDepth = false;
	m_twoSideShade = false;
	m_pxRigidStatic = NULL;
	m_pxRigidDynamic = NULL;

	m_pxRenderUtilityCenterOfMass = new GE_RenderUtilityObject();
	m_pxRenderUtilityCenterOfMass->createSphere( 0.5f, GE_Vec3( 0, 0, 0 ), 2, 2 );
	m_pxRenderUtilityCenterOfMass->setVisible( false );
	m_pxRenderUtilityCenterOfMass->setDepthDisable( false );

	initPx();
	setText( 0, m_model->getName().c_str() );
	setIcon( 0, m_iconModel );
	setCheckState( GEW_Item::Enable, Qt::Checked );
	setCheckState( GEW_Item::Dynamic, Qt::Unchecked );
	setCheckState( GEW_Item::Physics, Qt::Unchecked );
	setToolTip( GEW_Item::Enable, "Visible" );
	setToolTip( GEW_Item::Dynamic, "Dynamic Model" );
	setToolTip( GEW_Item::Physics, "Physics Enable" );
	setExpanded( true );
	setFlags(
		Qt::ItemFlag::ItemIsEnabled |
		Qt::ItemFlag::ItemIsUserCheckable |
		Qt::ItemFlag::ItemIsSelectable
		);

	m_list.push_back( this );
	m_model->getBound().BoundRadiusMultiply = 100000.f;
	m_model->getBound().transform( &m_model->getBound().Transform );

	/*auto instances = m_model->createInstance( 500 );
	for ( uint32_t i = 0; i < 500; ++i )
	{
		int sc = ( rand() % 3 ) + 3.0f;
		int rot = ( rand() % 360 );
		( *instances )->setRotation( GE_Quat().rotationAxis( &GE_Vec3( 0.0f, 0.0f, 1.0f ), GE_ConvertToRadians( rot )) );
		( *instances )->setScale( GE_Vec3( sc, sc, sc ) );
		( *instances )->setPosition( GE_Vec3( ( rand() % ( int )GE_ConvertToUnit( 100.0f ) ) - ( int )GE_ConvertToUnit( 50.0f ),
			( rand() % ( int )GE_ConvertToUnit( 100.0f ) ) - ( int )GE_ConvertToUnit( 50.0f ),
			0.0f ) );
		instances++;
	}*/
}

GEW_Model::GEW_Model( GE_Model *i_model, GEW_Item *i_parent )
: GEW_Item( i_parent, GEW_Item::Model )
{
	m_model = i_model;

	GE_Material *material = GEW_Material::getSingleton().getNullMaterial();
	for ( uint32_t i = 0; i < m_model->getNumberOfSubsets(); ++i )
		m_model->getSubset( i )->Material = material;

	GE_Vec3 size = ( m_model->getBound().RawBoundBoxMax - m_model->getBound().RawBoundBoxMin ) * 0.5f;
	GE_Vec3 offset = ( m_model->getBound().RawBoundBoxMax + m_model->getBound().RawBoundBoxMin ) * 0.5f;

	m_renderUtilityBB = new GE_RenderUtilityObject();
	m_pxRenderUtilityCenterOfMass = new GE_RenderUtilityObject();

	m_renderUtilityBB->createBox( size, offset );
	m_renderUtilityBB->setColor( GE_Vec3( 1.0, 1.0f, 0.0f ) );
	m_renderUtilityBB->setDepthDisable( false );
	m_renderUtilityBB->setVisible( false );
	m_cullingEnable = true;
	m_dynamic = false;
	m_shadowCaster = true;
	m_transparent = false;
	m_pxRigidDynamic = NULL;
	m_pxRigidStatic = NULL;

	m_pxRenderUtilityCenterOfMass = new GE_RenderUtilityObject();
	m_pxRenderUtilityCenterOfMass->createSphere( 0.5f, GE_Vec3( 0, 0, 0 ), 2, 2 );
	m_pxRenderUtilityCenterOfMass->setVisible( false );
	m_pxRenderUtilityCenterOfMass->setDepthDisable( false );

	initPx();
	setText( 0, m_model->getName().c_str() );
	setIcon( 0, m_iconModel );
	setCheckState( GEW_Item::Enable, Qt::Checked );
	setCheckState( GEW_Item::Dynamic, Qt::Unchecked );
	setCheckState( GEW_Item::Physics, Qt::Unchecked );
	setToolTip( GEW_Item::Enable, "Visible" );
	setToolTip( GEW_Item::Dynamic, "Dynamic Model" );
	setToolTip( GEW_Item::Physics, "Physics Enable" );
	setExpanded( true );
	setFlags(
		Qt::ItemFlag::ItemIsEnabled |
		Qt::ItemFlag::ItemIsUserCheckable |
		Qt::ItemFlag::ItemIsSelectable
		);

	m_list.push_back( this );
}

GEW_Model::~GEW_Model()
{
	destroyPhysics();

	m_list.removeOne( this );

	//delete all childes
	SAFE_DELETE( m_model );
	delete m_renderUtilityBB;
	delete m_pxRenderUtilityCenterOfMass;
	for ( int32_t i = 0; i < m_pxRenderUtilityObjects.size(); ++i )
		delete m_pxRenderUtilityObjects[ i ];
}

void GEW_Model::replaceMaterial( GE_Material *i_material, GE_Material *i_replaceWith )
{
	for ( uint32_t i = 0; i < m_model->getNumberOfSubsets(); ++i )
	{
		GE_Model::Subset *subset = m_model->getSubset( i );
		if ( subset->Material == i_material )
			subset->Material = i_replaceWith;
	}
}

void GEW_Model::initPx()
{
	GE_Model::ComplexBound *cBound = m_model->getComplexBound();
	uint32_t nbObjects =
		( uint32_t )cBound->Boxes.size() +
		( uint32_t )cBound->Spheres.size() +
		( uint32_t )cBound->Capsules.size() +
		( uint32_t )cBound->Convexes.size() +
		( uint32_t )cBound->TriangleMeshes.size();
	if ( nbObjects )
	{
		for ( int32_t i = 0; i < m_pxRenderUtilityObjects.size(); ++i )
			delete m_pxRenderUtilityObjects[ i ];
		m_pxRenderUtilityObjects.resize( nbObjects );
		for ( int32_t i = 0; i < m_pxRenderUtilityObjects.size(); ++i )
			m_pxRenderUtilityObjects[ i ] = new GE_RenderUtilityObject();

		m_pxShapeData.resize( nbObjects );
		m_pxShapePointer.resize( nbObjects );
		int32_t index = 0;

		for ( uint32_t i = 0; i < cBound->Boxes.size(); ++i ) // create boxes
		{
			m_pxRenderUtilityObjects[ index ]->createBox(
				cBound->Boxes[ i ].HalfDimension,
				GE_Vec3( 0.0f, 0.0f, 0.0f ) );
			m_pxRenderUtilityObjects[ index ]->setPosition( cBound->Boxes[ i ].Position );
			m_pxRenderUtilityObjects[ index ]->setRotation( cBound->Boxes[ i ].Rotation );
			m_pxRenderUtilityObjects[ index ]->setTransformBy( m_model->getWorld() );
			m_pxRenderUtilityObjects[ index ]->setVisible( false );
			m_pxRenderUtilityObjects[ index ]->setDepthDisable( false );
			//m_pxRenderUtilityObjects[ index ]->setObjectName(
			//	m_model->getObjectName() +
			//	std::string( "_Px" ) +
			//	cBound->Boxes[ i ].Name );
			m_pxShapeName.push_back( cBound->Boxes[ i ].Name.c_str() );
			m_pxShapeData[ index ].MyType = ShapeData::Box;
			m_pxShapePointer[ index ] = ( void * )&cBound->Boxes[ i ];
			++index;
		}

		for ( uint32_t i = 0; i < cBound->Spheres.size(); ++i ) // create spheres
		{
			m_pxRenderUtilityObjects[ index ]->createSphere(
				cBound->Spheres[ i ].Radius,
				GE_Vec3( 0.0f, 0.0f, 0.0f ),
				6,
				10 );
			m_pxRenderUtilityObjects[ index ]->setPosition( cBound->Spheres[ i ].Position );
			m_pxRenderUtilityObjects[ index ]->setRotation( cBound->Spheres[ i ].Rotation );
			m_pxRenderUtilityObjects[ index ]->setTransformBy( m_model->getWorld() );
			m_pxRenderUtilityObjects[ index ]->setVisible( false );
			m_pxRenderUtilityObjects[ index ]->setDepthDisable( false );
			//m_pxRenderUtilityObjects[ index ]->setObjectName(
			//	m_model->getObjectName() +
			//	std::string( "_Px" ) +
			//	cBound->Spheres[ i ].Name );
			m_pxShapeName.push_back( cBound->Spheres[ i ].Name.c_str() );
			m_pxShapeData[ index ].MyType = ShapeData::Sphere;
			m_pxShapePointer[ index ] = ( void * )&cBound->Spheres[ i ];
			++index;
		}

		for ( uint32_t i = 0; i < cBound->Capsules.size(); ++i ) // create capsules
		{
			m_pxRenderUtilityObjects[ index ]->createCapsule(
				cBound->Capsules[ i ].Radius,
				cBound->Capsules[ i ].HalfHeight,
				GE_Vec3( 0.0f, 0.0f, 0.0f ),
				6,
				6 );
			m_pxRenderUtilityObjects[ index ]->setPosition( cBound->Capsules[ i ].Position );
			m_pxRenderUtilityObjects[ index ]->setRotation( cBound->Capsules[ i ].Rotation );
			m_pxRenderUtilityObjects[ index ]->setTransformBy( m_model->getWorld() );
			m_pxRenderUtilityObjects[ index ]->setVisible( false );
			m_pxRenderUtilityObjects[ index ]->setDepthDisable( false );
			//m_pxRenderUtilityObjects[ index ]->setObjectName(
			//	m_model->getObjectName() +
			//	std::string( "_Px" ) +
			//	cBound->Capsules[ i ].Name );
			m_pxShapeName.push_back( cBound->Capsules[ i ].Name.c_str() );
			m_pxShapeData[ index ].MyType = ShapeData::Capsule;
			m_pxShapePointer[ index ] = ( void * )&cBound->Capsules[ i ];
			++index;
		}

		for ( uint32_t i = 0; i < cBound->Convexes.size(); ++i ) // create Convexes
		{
			m_pxRenderUtilityObjects[ index ]->createTriangleMesh(
				cBound->Convexes[ i ].Vertices.data(),
				( uint32_t )cBound->Convexes[ i ].Vertices.size(),
				cBound->Convexes[ i ].Indices.data(),
				( uint32_t )cBound->Convexes[ i ].Indices.size() );
			m_pxRenderUtilityObjects[ index ]->setPosition( cBound->Convexes[ i ].Position );
			m_pxRenderUtilityObjects[ index ]->setRotation( cBound->Convexes[ i ].Rotation );
			m_pxRenderUtilityObjects[ index ]->setTransformBy( m_model->getWorld() );
			m_pxRenderUtilityObjects[ index ]->setVisible( false );
			m_pxRenderUtilityObjects[ index ]->setDepthDisable( false );
			//m_pxRenderUtilityObjects[ index ]->setObjectName(
			//	m_model->getObjectName() +
			//	std::string( "_Px" ) +
			//	cBound->Convexes[ i ].Name );
			m_pxShapeName.push_back( cBound->Convexes[ i ].Name.c_str() );
			m_pxShapeData[ index ].MyType = ShapeData::Convex;
			m_pxShapePointer[ index ] = ( void * )&cBound->Convexes[ i ];
			++index;
		}

		for ( uint32_t i = 0; i < cBound->TriangleMeshes.size(); ++i ) // create TriangleMeshes
		{
			m_pxRenderUtilityObjects[ index ]->createTriangleMesh(
				cBound->TriangleMeshes[ i ].Vertices.data(),
				( uint32_t )cBound->TriangleMeshes[ i ].Vertices.size(),
				cBound->TriangleMeshes[ i ].Indices.data(),
				( uint32_t )cBound->TriangleMeshes[ i ].Indices.size() );
			m_pxRenderUtilityObjects[ index ]->setPosition( cBound->TriangleMeshes[ i ].Position );
			m_pxRenderUtilityObjects[ index ]->setRotation( cBound->TriangleMeshes[ i ].Rotation );
			m_pxRenderUtilityObjects[ index ]->setTransformBy( m_model->getWorld() );
			m_pxRenderUtilityObjects[ index ]->setVisible( false );
			m_pxRenderUtilityObjects[ index ]->setDepthDisable( false );
			//m_pxRenderUtilityObjects[ index ]->setObjectName(
			//	m_model->getObjectName() +
			//	std::string( "_Px" ) +
			//	cBound->TriangleMeshes[ i ].Name );
			m_pxShapeName.push_back( cBound->TriangleMeshes[ i ].Name.c_str() );
			m_pxShapeData[ index ].MyType = ShapeData::TriangleMesh;
			m_pxShapePointer[ index ] = ( void * )&cBound->TriangleMeshes[ i ];
			++index;
		}
	}
}

void GEW_Model::setCullingEnable( bool i_enable )
{
	m_cullingEnable = i_enable;
}

bool GEW_Model::isCullingEnable()
{
	return m_cullingEnable;
}

void GEW_Model::setDynamic( bool i_dynamic )
{
	m_dynamic = i_dynamic;
}

bool GEW_Model::isDynamic()
{
	return m_dynamic;
}

void GEW_Model::setShadowCaster( bool i_enable )
{
	m_shadowCaster = i_enable;
}

bool GEW_Model::isShadowCaster()
{
	return m_shadowCaster;
}

void GEW_Model::changeModel( GE_Model *i_model )
{
	//TODO: change model stuff
}

QString & GEW_Model::getSubsetName( uint32_t i_index )
{
	return m_subsetName[ i_index ];
}

void GEW_Model::setSubsetName( uint32_t i_index, const QString &i_name )
{
	m_subsetName[ i_index ] = i_name;
}

int GEW_Model::getSubsetIndex( const QString &i_name )
{
	for ( int i = 0; i < m_subsetName.size(); ++i )
	{
		if ( m_subsetName[ i ].toLower() == i_name )
			return i;
	}
	return -1;
}

uint32_t GEW_Model::getPxShapeCount()
{
	return m_pxShapeData.size();
}

bool calcShapes(
	physx::PxRigidActor *i_rigidActor,
	QVector<GEW_Model::ShapeData> &i_shapeData,
	QVector<void*> &i_shapePointer,
	QVector<physx::PxShape*> &i_outShapeList,
	QVector<physx::PxConvexMesh*> &i_outConvexMeshList,
	QVector<physx::PxTriangleMesh*> &i_outTriangleMeshList )
{
	for ( int i = 0; i < i_shapeData.size(); ++i )
	{
		if ( !i_shapeData[ i ].Use )
			continue;

		physx::PxMaterial *pxMaterial = GEW_Physics::getSingleton().getPxMaterial( i_shapeData[ i ].MaterialID );

		uint32_t shapeFlags = 0;
		if ( i_shapeData[ i ].ShapeFlags.eVISUALIZATION )
			shapeFlags |= physx::PxShapeFlag::eVISUALIZATION;
		if ( i_shapeData[ i ].ShapeFlags.eTRIGGER_SHAPE )
			shapeFlags |= physx::PxShapeFlag::eTRIGGER_SHAPE;
		if ( i_shapeData[ i ].ShapeFlags.eSIMULATION_SHAPE )
			shapeFlags |= physx::PxShapeFlag::eSIMULATION_SHAPE;
		if ( i_shapeData[ i ].ShapeFlags.eSCENE_QUERY_SHAPE )
			shapeFlags |= physx::PxShapeFlag::eSCENE_QUERY_SHAPE;
		if ( i_shapeData[ i ].ShapeFlags.ePARTICLE_DRAIN )
			shapeFlags |= physx::PxShapeFlag::ePARTICLE_DRAIN;

		physx::PxFilterData queryFilterData;
		queryFilterData.word3 = GEW_Physics::DRIVABLE_SURFACE;

		physx::PxFilterData simulationFilterData;
		switch ( i_shapeData[ i ].MySurface )
		{
		case GEW_Model::ShapeData::Groung:
		{
											 simulationFilterData.word0 = GEW_Physics::COLLISION_FLAG_GROUND;
											 simulationFilterData.word1 = GEW_Physics::COLLISION_FLAG_GROUND_AGAINST;
		}break;

		case GEW_Model::ShapeData::Obstacle:
		{
											   simulationFilterData.word0 = GEW_Physics::COLLISION_FLAG_OBSTACLE;
											   simulationFilterData.word1 = GEW_Physics::COLLISION_FLAG_OBSTACLE_AGAINST;
		}break;

		case GEW_Model::ShapeData::DrivableObstacle:
		{
													   simulationFilterData.word0 = GEW_Physics::COLLISION_FLAG_DRIVABLE_OBSTACLE;
													   simulationFilterData.word1 = GEW_Physics::COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST;
		}break;
		}

		switch ( i_shapeData[ i ].MyType )
		{
		case GEW_Model::ShapeData::Box:
		{
										  GE_Model::CBBox *box = ( GE_Model::CBBox* )i_shapePointer[ i ];

										  physx::PxBoxGeometry boxGeometry;
										  boxGeometry.halfExtents = TO_PxVec3( box->HalfDimension );

										  physx::PxVec3 position = TO_PxVec3( box->Position );
										  physx::PxQuat rotation = TO_PxQuat( box->Rotation );
										  physx::PxTransform localPos( position, rotation );

										  physx::PxShape *shape = i_rigidActor->createShape(
											  boxGeometry,
											  *pxMaterial,
											  ( physx::PxShapeFlag::Enum )shapeFlags );

										  if ( shape )
										  {
											  shape->setLocalPose( localPos );
											  shape->setSimulationFilterData( simulationFilterData );
											  shape->setQueryFilterData( queryFilterData );
											  i_outShapeList.push_back( shape );
										  }
										  else
										  {
											  //GEditor::getSingleton().logError( QString( "Can not create Shape \"" ) +
											  //	QString( box->Name.c_str() ) +
											  //	QString( "\". Wrong flags maybe!" ) );
										  }

		}break;

		case GEW_Model::ShapeData::Sphere:
		{
											 GE_Model::CBSphere *sphere = ( GE_Model::CBSphere* )i_shapePointer[ i ];

											 physx::PxSphereGeometry SphereGeometry;
											 SphereGeometry.radius = sphere->Radius;

											 physx::PxVec3 position = TO_PxVec3( sphere->Position );
											 physx::PxQuat rotation = TO_PxQuat( sphere->Rotation );
											 physx::PxTransform localPos( position, rotation );

											 physx::PxShape *shape = i_rigidActor->createShape(
												 SphereGeometry,
												 *pxMaterial,
												 ( physx::PxShapeFlag::Enum )shapeFlags );

											 if ( shape )
											 {
												 shape->setLocalPose( localPos );
												 shape->setSimulationFilterData( simulationFilterData );
												 shape->setQueryFilterData( queryFilterData );
												 i_outShapeList.push_back( shape );
											 }
											 else
											 {
												 //GEditor::getSingleton().logError( QString( "Can not create Shape \"" ) +
												 //	QString( sphere->Name.c_str() ) +
												 //	QString( "\". Wrong flags maybe!" ) );
											 }
		}break;

		case GEW_Model::ShapeData::Capsule:
		{
											  GE_Model::CBCapsule *capsule = ( GE_Model::CBCapsule* )i_shapePointer[ i ];

											  physx::PxCapsuleGeometry capsuleGeometry;
											  capsuleGeometry.radius = capsule->Radius;
											  capsuleGeometry.halfHeight = capsule->HalfHeight;

											  physx::PxVec3 position = TO_PxVec3( capsule->Position );
											  physx::PxQuat rotation = TO_PxQuat( capsule->Rotation );
											  physx::PxTransform localPos( position, rotation );

											  physx::PxShape *shape = i_rigidActor->createShape(
												  capsuleGeometry,
												  *pxMaterial,
												  ( physx::PxShapeFlag::Enum )shapeFlags );

											  if ( shape )
											  {
												  shape->setLocalPose( localPos );
												  shape->setSimulationFilterData( simulationFilterData );
												  shape->setQueryFilterData( queryFilterData );
												  i_outShapeList.push_back( shape );
											  }
											  else
											  {
												  //GEditor::getSingleton().logError( QString( "Can not create Shape \"" ) +
												  //	QString( capsule->Name.c_str() ) +
												  //	QString( "\". Wrong flags maybe!" ) );
											  }
		}break;

		case GEW_Model::ShapeData::Convex:
		{
											 GE_Model::CBConvex *convex = ( GE_Model::CBConvex* )i_shapePointer[ i ];

											 physx::PxVec3 position = TO_PxVec3( convex->Position );
											 physx::PxQuat rotation = TO_PxQuat( convex->Rotation );
											 physx::PxTransform localPos( position, rotation );

											 physx::PxConvexMesh *convexMesh = GEW_Physics::getSingleton().createConvexMesh(
												 convex->Vertices.data(),
												 ( uint32_t )convex->Vertices.size() );

											 i_outConvexMeshList.push_back( convexMesh );

											 physx::PxConvexMeshGeometry convexMeshGeometry;
											 convexMeshGeometry.convexMesh = convexMesh;

											 physx::PxShape *shape = i_rigidActor->createShape(
												 convexMeshGeometry,
												 *pxMaterial,
												 ( physx::PxShapeFlag::Enum )shapeFlags );
											 if ( shape )
											 {
												 shape->setLocalPose( localPos );
												 shape->setSimulationFilterData( simulationFilterData );
												 shape->setQueryFilterData( queryFilterData );
												 i_outShapeList.push_back( shape );
											 }
											 else
											 {
												 //GEditor::getSingleton().logError( QString( "Can not create Shape \"" ) +
												 //	QString( convex->Name.c_str() ) +
												 //	QString( "\". Wrong flags maybe!" ) );
											 }
		}break;

		case GEW_Model::ShapeData::TriangleMesh:
		{
												   GE_Model::CBTriangleMesh *triangle = ( GE_Model::CBTriangleMesh* )i_shapePointer[ i ];

												   physx::PxVec3 position = TO_PxVec3( triangle->Position );
												   physx::PxQuat rotation = TO_PxQuat( triangle->Rotation );
												   physx::PxTransform localPos( position, rotation );

												   physx::PxTriangleMesh *triangleMesh = GEW_Physics::getSingleton().createTriangleMesh32(
													   triangle->Vertices.data(),
													   ( uint32_t )triangle->Vertices.size(),
													   triangle->Indices.data(),
													   ( uint32_t )triangle->Indices.size() / 3 );
												   i_outTriangleMeshList.push_back( triangleMesh );

												   physx::PxTriangleMeshGeometry triangleMeshGeometry;
												   triangleMeshGeometry.triangleMesh = triangleMesh;

												   physx::PxShape *shape = i_rigidActor->createShape(
													   triangleMeshGeometry,
													   *pxMaterial,
													   ( physx::PxShapeFlag::Enum )shapeFlags );

												   if ( shape )
												   {
													   shape->setLocalPose( localPos );
													   shape->setSimulationFilterData( simulationFilterData );
													   shape->setQueryFilterData( queryFilterData );
													   i_outShapeList.push_back( shape );
												   }
												   else
												   {
													   //GEditor::getSingleton().logError( QString( "Can not create Shape \"" ) +
													   //	QString( triangle->Name.c_str() ) +
													   //	QString( "\". Wrong flags maybe!" ) );
												   }
		}break;
		}
	}
	return true;
}

QString & GEW_Model::getPxShapeName( uint32_t i_index )
{
	return m_pxShapeName[ i_index ];
}

bool GEW_Model::createPhysics()
{
	bool isAnyPxShapeUsed = false;
	for ( int i = 0; i < m_pxShapeData.size(); ++i ) // is there any used pxShape??
	{
		if ( m_pxShapeData[ i ].Use )
		{
			isAnyPxShapeUsed = true;
			break;
		}
	}
	if ( !isAnyPxShapeUsed )
		return false;

	switch ( m_pxActorData.MyType )
	{
	case ActorData::Static:
	{
							  physx::PxVec3 position = TO_PxVec3( m_model->getPosition() );
							  physx::PxQuat rotation = TO_PxQuat( m_model->getRotation() );
							  physx::PxTransform transform( position, rotation );
							  m_pxRigidStatic = GEW_Physics::getSingleton().m_physics->createRigidStatic( transform );
							  calcShapes(
								  m_pxRigidStatic,
								  m_pxShapeData,
								  m_pxShapePointer,
								  m_pxShapeList,
								  m_pxConvexMeshList,
								  m_pxTriangleMeshList );
							  m_pxRigidStatic->setName( m_model->getName().c_str() );

							  m_pxRigidStatic->setActorFlag( physx::PxActorFlag::eVISUALIZATION, m_pxActorData.ActorFlags.eVISUALIZATION );
							  m_pxRigidStatic->setActorFlag( physx::PxActorFlag::eDISABLE_GRAVITY, m_pxActorData.ActorFlags.eDISABLE_GRAVITY );
							  m_pxRigidStatic->setActorFlag( physx::PxActorFlag::eSEND_SLEEP_NOTIFIES, m_pxActorData.ActorFlags.eSEND_SLEEP_NOTIFIES );
							  m_pxRigidStatic->setActorFlag( physx::PxActorFlag::eDISABLE_SIMULATION, m_pxActorData.ActorFlags.eDISABLE_SIMULATION );

							  GEW_Physics::getSingleton().m_scene->addActor( *m_pxRigidStatic );
	}break;

	case ActorData::Dynamic:
	{
							   physx::PxVec3 position = TO_PxVec3( m_model->getPosition() );
							   physx::PxQuat rotation = TO_PxQuat( m_model->getRotation() );
							   physx::PxTransform transform( position, rotation );
							   m_pxRigidDynamic = GEW_Physics::getSingleton().m_physics->createRigidDynamic( transform );
							   calcShapes(
								   m_pxRigidDynamic,
								   m_pxShapeData,
								   m_pxShapePointer,
								   m_pxShapeList,
								   m_pxConvexMeshList,
								   m_pxTriangleMeshList );
							   m_pxRigidDynamic->setName( m_model->getName().c_str() );

							   physx::PxVec3 *centerOfMass = NULL;
							   if ( m_pxActorData.UseCenterOfMass )
								   centerOfMass = &TO_PxVec3( m_pxActorData.CenterOfMass );

							   if ( m_pxActorData.UseMass )
							   {
								   physx::PxRigidBodyExt::setMassAndUpdateInertia(
									   *m_pxRigidDynamic,
									   m_pxActorData.Mass,
									   centerOfMass );
							   }
							   else
							   {
								   std::vector<float> densityList;
								   for ( int i = 0; i < m_pxShapeList.size(); ++i )
								   if ( m_pxShapeData[ i ].Use && m_pxShapeData[ i ].ShapeFlags.eSIMULATION_SHAPE )
									   densityList.push_back( m_pxShapeData[ i ].Mass );

								   physx::PxRigidBodyExt::setMassAndUpdateInertia(
									   *m_pxRigidDynamic, densityList.data(),
									   ( uint32_t )densityList.size(),
									   centerOfMass );
							   }

							   m_pxRigidDynamic->setLinearDamping( m_pxActorData.LinearDamping );
							   m_pxRigidDynamic->setAngularDamping( m_pxActorData.AngularDampin );

							   m_pxRigidDynamic->setActorFlag( physx::PxActorFlag::eVISUALIZATION, m_pxActorData.ActorFlags.eVISUALIZATION );
							   m_pxRigidDynamic->setActorFlag( physx::PxActorFlag::eDISABLE_GRAVITY, m_pxActorData.ActorFlags.eDISABLE_GRAVITY );
							   m_pxRigidDynamic->setActorFlag( physx::PxActorFlag::eSEND_SLEEP_NOTIFIES, m_pxActorData.ActorFlags.eSEND_SLEEP_NOTIFIES );
							   m_pxRigidDynamic->setActorFlag( physx::PxActorFlag::eDISABLE_SIMULATION, m_pxActorData.ActorFlags.eDISABLE_SIMULATION );

							   m_pxRigidDynamic->setRigidBodyFlag( physx::PxRigidBodyFlag::eKINEMATIC, m_pxActorData.RigidBodyFlags.eKINEMATIC );
							   m_pxRigidDynamic->setRigidBodyFlag( physx::PxRigidBodyFlag::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, m_pxActorData.RigidBodyFlags.eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES );
							   m_pxRigidDynamic->setRigidBodyFlag( physx::PxRigidBodyFlag::eENABLE_CCD, m_pxActorData.RigidBodyFlags.eENABLE_CCD );
							   m_pxRigidDynamic->setRigidBodyFlag( physx::PxRigidBodyFlag::eENABLE_CCD_FRICTION, m_pxActorData.RigidBodyFlags.eENABLE_CCD_FRICTION );

							   GEW_Physics::getSingleton().m_scene->addActor( *m_pxRigidDynamic );
							   GEW_Physics::getColumnMajor( m_pxRigidDynamic->getGlobalPose(), ( float * )&m_pxMatrix );
							   m_model->setTransformForce( &m_pxMatrix );

							   physx::PxVec3 p = m_pxRigidDynamic->getCMassLocalPose().p;
							   physx::PxQuat q = m_pxRigidDynamic->getCMassLocalPose().q;
							   m_pxRenderUtilityCenterOfMass->setPosition( TO_GEVec3( p ) );
							   m_pxRenderUtilityCenterOfMass->setRotation( TO_GEQuat( q ) );
							   m_pxRenderUtilityCenterOfMass->setVisible( m_pxActorData.ShowCenterOfMass );
	}break;
	}

	return true;
}

void GEW_Model::destroyPhysics()
{
	if ( m_pxRigidDynamic )
	{
		m_pxRenderUtilityCenterOfMass->setVisible( false );
		m_model->setTransformForce( NULL );
		GEW_Physics::getSingleton().m_scene->removeActor( *m_pxRigidDynamic );
		m_pxRigidDynamic->release();
		m_pxRigidDynamic = NULL;
	}

	if ( m_pxRigidStatic )
	{
		GEW_Physics::getSingleton().m_scene->removeActor( *m_pxRigidStatic );
		m_pxRigidStatic->release();
		m_pxRigidStatic = NULL;
	}

	//for( int32_t i=0; i<m_pxShapeList.size(); ++i )
	//m_pxShapeList[ i ]->release();
	m_pxShapeList.clear();

	foreach( physx::PxConvexMesh *convexMesh, m_pxConvexMeshList )
		convexMesh->release();
	m_pxConvexMeshList.clear();

	foreach( physx::PxTriangleMesh *triangleMesh, m_pxTriangleMeshList )
		triangleMesh->release();
	m_pxTriangleMeshList.clear();
}

void GEW_Model::setLinearVelocity( GE_Vec3 &i_velocity )
{
	if ( m_pxRigidDynamic )
	{
		m_pxRigidDynamic->setLinearVelocity( TO_PxVec3( i_velocity ) );
		//m_pxRigidDynamic->wakeUp();
	}
}

void GEW_Model::setAngularVelocity( GE_Vec3 &i_velocity )
{
	if ( m_pxRigidDynamic )
	{
		m_pxRigidDynamic->setAngularVelocity( TO_PxVec3( i_velocity ) );
		//m_pxRigidDynamic->wakeUp();
	}
}

void GEW_Model::setSurfaceType( uint32_t i_shapeIndex, ShapeData::Surface i_surface )
{
	m_pxShapeData[ i_shapeIndex ].MySurface = i_surface;
	if ( m_pxShapeList.size() )
	{
		physx::PxFilterData filterData;
		switch ( i_surface )
		{
		case ShapeData::Groung:
		{
								  filterData.word0 = GEW_Physics::COLLISION_FLAG_GROUND;
								  filterData.word1 = GEW_Physics::COLLISION_FLAG_GROUND_AGAINST;
		}
			break;

		case ShapeData::Obstacle:
		{
									filterData.word0 = GEW_Physics::COLLISION_FLAG_OBSTACLE;
									filterData.word1 = GEW_Physics::COLLISION_FLAG_OBSTACLE_AGAINST;
		}
			break;

		case ShapeData::DrivableObstacle:
		{
											filterData.word0 = GEW_Physics::COLLISION_FLAG_DRIVABLE_OBSTACLE;
											filterData.word1 = GEW_Physics::COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST;
		}
			break;
		}

		m_pxShapeList[ i_shapeIndex ]->setSimulationFilterData( filterData );

		if ( m_pxRigidDynamic )
			m_pxRigidDynamic->wakeUp();
	}
}

void GEW_Model::setShowCenterOfMass( bool i_show )
{
	m_pxActorData.ShowCenterOfMass = i_show;
	m_pxRenderUtilityCenterOfMass->setVisible( i_show );
}

bool GEW_Model::isPhysicsEnable()
{
	return m_pxActorData.Enable;
}

GEW_Item* GEW_Model::duplicate()
{
	return NULL;
	/*GE_Model *model = new GE_Model( m_model->getLodFilePath( 0 ) );
	if( !model )
	return NULL;

	uint32_t id = 1;
	QString itemName = text( 0 );
	QStringList splitString = itemName.split( '_' );
	if( splitString.count() > 1 )
	{
	bool ok = false;
	id = splitString.last().toUInt( &ok ) + 1;
	if( ok )
	itemName = itemName.left( itemName.length() - splitString.last().length() );
	} else
	itemName += "_";

	QString newItemName = itemName + QString( "%1" ).arg( id );
	//while( GEditor::getSingleton().m_root->findItem( newItemName.toLower() ) )
	//{
	//	id++;
	//	newItemName = itemName + QString( "%1" ).arg( id );
	//}
	model->setName( newItemName.toStdString() );
	model->setPosition( m_model->getPosition() );
	model->setRotation( m_model->getRotation() );
	model->setScale( m_model->getScale() );
	model->setVisibleDistance( m_model->getVisibleDistance() );
	model->update();

	GEW_Model *gwModel = new GEW_Model( model, ( GEW_Item * ) parent() );
	gwModel->m_dynamic = m_dynamic;
	gwModel->m_cullingEnable = m_cullingEnable;
	gwModel->setTwoSideDepth( m_model->isTwoSideDepth() );
	gwModel->setTwoSideShade( m_model->isTwoSideShade() );
	gwModel->m_renderUtilityBB->setColor( m_renderUtilityBB->getColor() );

	for( uint32_t i = 0; i < m_model->getNumberOfLod(); ++i )
	{
	if( i > 0 )
	model->addLod( m_model->getLodFilePath( i ) );
	model->setLodDistance( i, m_model->getLodDistance( i ) );
	}

	for( uint32_t i = 0; i < m_model->getNumberOfSubsets(); ++i )
	model->setMaterial( i, m_model->getMaterial( i ) );

	gwModel->m_pxActorData = m_pxActorData;
	if( gwModel->m_pxShapeData.size() )
	memcpy_s( &gwModel->m_pxShapeData[ 0 ],
	sizeof( GEW_Model::ShapeData ) * m_pxShapeData.size(),
	&m_pxShapeData[ 0 ],
	sizeof( GEW_Model::ShapeData ) * m_pxShapeData.size() );

	if( m_pxActorData.Enable )
	gwModel->createPhysics();

	return gwModel;*/
}

void GEW_Model::update()
{
	if ( m_pxRigidDynamic )
		GEW_Physics::getColumnMajor( m_pxRigidDynamic->getGlobalPose(), ( float* )&m_pxMatrix );
	m_model->update();

	if ( m_renderUtilityBB->isVisible() )
	{
		m_renderUtilityBB->setTransformForce( m_model->getWorld() );
		m_renderUtilityBB->update();
	}

	if ( m_pxRenderUtilityCenterOfMass->isVisible() )
	{
		m_pxRenderUtilityCenterOfMass->setTransformBy( m_model->getWorld() );
		m_pxRenderUtilityCenterOfMass->update();
	}

	for ( int32_t i = 0; i < m_pxRenderUtilityObjects.size(); ++i )
	{
		if ( m_pxRenderUtilityObjects[ i ]->isVisible() )
		{
			m_pxRenderUtilityObjects[ i ]->setTransformBy( m_model->getWorld() );
			m_pxRenderUtilityObjects[ i ]->update();
		}
	}
}

void GEW_Model::select()
{
	m_selected = true;
	m_renderUtilityBB->setVisible( true );
}

void GEW_Model::deSelect()
{
	m_selected = false;
	m_renderUtilityBB->setVisible( false );
}

void GEW_Model::checkStateChanged( GEW_Item::CheckIndex i_column )
{
	bool checked = checkState( i_column ) == Qt::Checked;
	if ( i_column == GEW_Item::Enable )
	{
		m_model->setVisible( checked );
	}
	else if ( i_column == GEW_Item::Dynamic )
	{
		setDynamic( checked );
	}
	else if ( i_column == GEW_Item::Physics )
	{
		if ( checkState( GEW_Item::Physics ) == Qt::Checked )
		{
			if ( !createPhysics() )
				setCheckState( GEW_Item::Physics, Qt::Unchecked );
			else
				m_pxActorData.Enable = true;
		}
		else
		{
			destroyPhysics();
			m_pxActorData.Enable = false;
		}
	}
}

void GEW_Model::parentCheckStateChanged( GEW_Item::CheckIndex i_column, Qt::CheckState i_checkState )
{
	if (
		i_column == GEW_Item::Enable ||
		i_column == GEW_Item::Dynamic ||
		i_column == GEW_Item::Physics
		)
	{
		if ( checkState( i_column ) == i_checkState )
			return;
		setCheckState( i_column, i_checkState );
		checkStateChanged( i_column );
	}
}

void GEW_Model::saveToFile( QFile &i_file, const QString &i_resourcePath )
{
	GEW_Item::SaveHeader header;
	header.MyType = ( GEW_Item::Type )type();
	header.ParentId = ( ( GEW_Item* )parent() )->getId();
	header.MyId = getId();
	i_file.write( ( const char * )&header, sizeof( GEW_Item::SaveHeader ) );

	SaveData modelData;
	strcpy_s( modelData.Name, text( 0 ).toStdString().c_str() );
	strcpy_s( modelData.FileName, GEW_Utility::copyFile( m_model->getLodFilePath( 0 ).c_str(), i_resourcePath ).c_str() );
	modelData.Position = m_model->getPosition();
	modelData.Rotation = m_model->getRotation();
	modelData.Scale = m_model->getScale();
	modelData.BufferDynamic = m_model->isBufferDynamic();
	modelData.Visible = m_model->isVisible();
	modelData.Culling = isCullingEnable();
	modelData.Dynamic = isDynamic();
	modelData.NumberOfSubset = m_model->getNumberOfSubsets();
	modelData.NumberOfLod = m_model->getNumberOfLod();
	modelData.CurrentLod = m_model->getLod();
	modelData.TwoSideDepth = isTwoSideDepth();
	modelData.TwoSideShade = isTwoSideShade();
	modelData.ShadowCaster = isShadowCaster();
	modelData.Transparent = isTransparent();
	modelData.BoundColor = m_renderUtilityBB->getColor();
	modelData.NbShape = m_pxShapeData.size();
	modelData.VisibleDistance = m_model->getVisibleDistance();
	i_file.write( ( const char * )&modelData, sizeof( SaveData ) );

	for ( uint32_t i = 0; i < modelData.NumberOfSubset; ++i )
	{
		SubsetSaveData subsetData;
		GE_Model::Subset *subset = m_model->getSubset( i );
		subsetData.LayerMultiply = subset->LayerMultiply;
		subsetData.LayerOffset = subset->LayerOffset;
		subsetData.UseVertexColor = subset->UseVertexColor;
		strcpy_s( subsetData.Name, m_subsetName[ i ].toStdString().c_str() );
		GEW_Material::getSingleton().getMaterialId( subsetData.MaterialGroupId, subsetData.MaterialId, subset->Material );
		if ( subset->LightMap )
			strcpy_s( subsetData.LightMap, GEW_Utility::copyFile( subset->LightMap->getTextureName(), i_resourcePath ).c_str() );
		i_file.write( ( const char * )&subsetData, sizeof( SubsetSaveData ) );
	}

	for ( uint32_t i = 0; i < modelData.NumberOfLod; ++i )
	{
		LodSaveData lodData;
		strcpy_s( lodData.LodFileName, GEW_Utility::copyFile( m_model->getLodFilePath( i ).c_str(), i_resourcePath ).c_str() );
		lodData.LodDistance = m_model->getLodDistance( i );
		i_file.write( ( const char * )&lodData, sizeof( LodSaveData ) );
	}

	i_file.write( ( const char * )&m_pxActorData, sizeof( GEW_Model::ActorData ) );
	if ( m_pxShapeData.size() )
		i_file.write( ( const char * )&m_pxShapeData[ 0 ], sizeof( GEW_Model::ShapeData ) * m_pxShapeData.size() );
}

void GEW_Model::setPosition( const GE_Vec3 &i_position )
{
	m_model->setPosition( ( GE_Vec3 )i_position );
}

void GEW_Model::setPosition2( const GE_Vec3 &i_position )
{
	setPosition( i_position );
}

void GEW_Model::setRotation( const GE_Vec3 &i_rotation )
{
	GE_Quat rotation = m_model->getRotation();
	rotation.rotationYPR( &i_rotation );
	m_model->setRotation( rotation );
}

void GEW_Model::setRotation( GE_Mat4x4 &i_rot, GE_Vec3 & i_pivot )
{
	GE_Mat4x4 pvotMat;
	pvotMat.transform( i_pivot );
	GE_Mat4x4 mat;
	mat.srp( m_model->getPosition() - i_pivot, m_model->getRotation(), m_model->getScale() );
	mat *= i_rot;
	mat *= pvotMat;

	GE_Vec3 pos;
	GE_Quat rot;
	GE_Vec3 scale;
	mat.decompose( &pos, &rot, &scale );
	m_model->setPosition( pos );
	m_model->setRotation( rot );
}

void GEW_Model::setScale( const GE_Vec3 &i_scale )
{
	m_model->setScale( ( GE_Vec3 )i_scale );
}

void GEW_Model::setTransform( GE_Mat4x4 *i_matrix )
{
	GE_Mat4x4 mat = ( *m_model->getWorld() ) * ( *i_matrix );
	GE_Vec3 pos;
	GE_Quat rot;
	GE_Vec3 Scale;
	mat.decompose( &pos, &rot, &Scale );
	m_model->setPosition( pos );
	m_model->setRotation( rot );
	m_model->setScale( Scale );
}

GE_Vec3 GEW_Model::getPosition()
{
	return m_model->getPosition();
}

GE_Vec3 GEW_Model::getRotation()
{
	GE_Vec3 rotation;
	m_model->getRotation().toYPR( &rotation );
	return rotation;
}

GE_Vec3 GEW_Model::getScale()
{
	return m_model->getScale();
}

void GEW_Model::getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList )
{
	if ( m_renderUtilityBB->isVisible() )
		i_outList.push_back( m_renderUtilityBB );

	if ( m_pxRenderUtilityCenterOfMass->isVisible() )
		i_outList.push_back( m_renderUtilityBB );

	foreach( auto *ruo, m_pxRenderUtilityObjects )
	if ( ruo->isVisible() )
		i_outList.push_back( ruo );
}

void GEW_Model::setTransparent( bool i_transparent )
{
	m_transparent = i_transparent;
}

bool GEW_Model::isTransparent()
{
	return m_transparent;
}

void GEW_Model::setTwoSideDepth( bool i_twoSide )
{
	m_twoSideDepth = i_twoSide;
}

bool GEW_Model::isTwoSideDepth()
{
	return m_twoSideDepth;
}

void GEW_Model::setTwoSideShade( bool i_twoSide )
{
	m_twoSideShade = i_twoSide;
}

bool GEW_Model::isTwoSideShade()
{
	return m_twoSideShade;
}


