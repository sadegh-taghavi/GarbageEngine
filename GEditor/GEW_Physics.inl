#include "GEW_Physics.h"

//FilterShader------------------------------------------------------------------------------------------------------------------
physx::PxFilterFlags FilterShader( physx::PxFilterObjectAttributes attributes0,
	physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1,
	physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags,
	const void* constantBlock,
	physx::PxU32 constantBlockSize )
{
	PX_UNUSED( attributes0 );
	PX_UNUSED( attributes1 );
	PX_UNUSED( constantBlock );
	PX_UNUSED( constantBlockSize );

	if( ( 0 == ( filterData0.word0 & filterData1.word1 ) ) && ( 0 == ( filterData1.word0 & filterData0.word1 ) ) )
		return physx::PxFilterFlag::eSUPPRESS;

	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

	return physx::PxFilterFlags();
}

inline GEW_Physics &GEW_Physics::getSingleton()
{
	return m_singleton;
}

inline physx::PxScene *GEW_Physics::getScene2()
{
	return m_scene2;
}

inline bool GEW_Physics::isEnable()
{
	return m_enable;
}

inline void GEW_Physics::setTimeStep( float i_timeStep )
{
	m_timeStep = i_timeStep;
}

inline float GEW_Physics::getTimeStep()
{
	return m_timeStep;
}

inline bool GEW_Physics::singleStep( float i_elapsedTime )
{
	m_elapsedTime += i_elapsedTime;
	if( m_elapsedTime >= m_timeStep )
	{
		m_elapsedTime -= m_timeStep;
		m_scene->simulate( m_timeStep );
		m_scene->fetchResults( true );
	}
	return true;
}

inline void GEW_Physics::getColumnMajor( physx::PxTransform &i_transform, float* i_mat ) 
{
	physx::PxMat33 m( i_transform.q );
	i_mat[ 0 ] = m.column0[ 0 ];
	i_mat[ 1 ] = m.column0[ 1 ];
	i_mat[ 2 ] = m.column0[ 2 ];
	i_mat[ 3 ] = 0;

	i_mat[ 4 ] = m.column1[ 0 ];
	i_mat[ 5 ] = m.column1[ 1 ];
	i_mat[ 6 ] = m.column1[ 2 ];
	i_mat[ 7 ] = 0;

	i_mat[ 8 ] = m.column2[ 0 ];
	i_mat[ 9 ] = m.column2[ 1 ];
	i_mat[ 10 ] = m.column2[ 2 ];
	i_mat[ 11 ] = 0;

	i_mat[ 12 ] = i_transform.p[ 0 ];
	i_mat[ 13 ] = i_transform.p[ 1 ];
	i_mat[ 14 ] = i_transform.p[ 2 ];
	i_mat[ 15 ] = 1;
}