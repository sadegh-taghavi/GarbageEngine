#include "GEW_Prs.h"
#include "ui_GEW_Prs.h"
#include "GEW_Item.h"
#include "GEW_ItemList.h"
#include <GE_Utility.h>
#include "GEW_Physics.h"
#include "GE_Model.h"
#include "GEW_Model.h"
#include "GE_Terrain.h"
#include "GEW_Terrain.h"
#include "GEW_Scene.h"
#include "GEW_Camera.h"
#include <GE_Bound.h>
#include <list>

GEW_Prs * GEW_Prs::m_singleton = NULL;
QIcon GEW_Prs::m_iconQuestion;

GEW_Prs::GEW_Prs( QWidget *i_parent /*= 0 */ )
: QWidget( i_parent ),
m_ui( new Ui::GEW_Prs )
{
	m_pickEnabled = false;
	m_singleton = this;
	m_pickRigidStatic = NULL;
	m_ui->setupUi( this );
	m_ui->Pick->addAction( m_ui->OnlyPickTerrain );
	m_item = NULL;
	m_historyPointer = 0;
	m_disableHistory = false;
	m_historyPt = new GE_PerformanceTimer();
	m_historyPt->start();
}

GEW_Prs::~GEW_Prs()
{
	delete m_historyPt;
	delete m_ui;
}

void GEW_Prs::slotSetItem( GEW_Item *i_item )
{
	m_item = i_item;
	if( !i_item )
	{
		m_ui->PRSActiveObjectIcon->setPixmap( m_iconQuestion.pixmap( QSize( 16, 16 ) ) );
		return;
	}

	m_ui->PRSActiveObjectIcon->setPixmap( m_item->icon( 0 ).pixmap( QSize( 16, 16 ) ) );

	m_ui->PRSx->blockSignals( true );
	m_ui->PRSy->blockSignals( true );
	m_ui->PRSz->blockSignals( true );
	if( m_ui->PRSPosition->isChecked() )
	{
		GE_Vec3 pos = m_item->getPosition();
		m_ui->PRSx->setValue( GE_ConvertToMeter( pos.x ) );
		m_ui->PRSy->setValue( GE_ConvertToMeter( pos.y ) );
		m_ui->PRSz->setValue( GE_ConvertToMeter( pos.z ) );
	} else if( m_ui->PRSRotation->isChecked() )
	{
		GE_Vec3 rot = m_item->getRotation();
		m_ui->PRSx->setValue( GE_ConvertToDegrees( rot.x ) );
		m_ui->PRSy->setValue( GE_ConvertToDegrees( rot.y ) );
		m_ui->PRSz->setValue( GE_ConvertToDegrees( rot.z ) );
	} else if( m_ui->PRSScale->isChecked() )
	{
		GE_Vec3 scale = m_item->getScale();
		m_ui->PRSx->setValue( scale.x );
		m_ui->PRSy->setValue( scale.y );
		m_ui->PRSz->setValue( scale.z );
	}
	m_ui->PRSx->blockSignals( false );
	m_ui->PRSy->blockSignals( false );
	m_ui->PRSz->blockSignals( false );
}

void GEW_Prs::slotUpdateItem()
{
	if( m_item == NULL ||
		m_item->type() == GEW_Item::Group ||
		m_item->type() == GEW_Item::Entity )
		return;

	Action action;
	action.ItemId = m_item->getId();
	if( m_ui->PRSPosition->isChecked() )
	{
		GE_Vec3 vec;
		action.MyType = Action::Move;
		if( !hasHistory( action.ItemId ) )
		{
			vec = m_item->getPosition();
			action.X = vec.x;
			action.Y = vec.y;
			action.Z = vec.z;
			push( action );
		}
		vec.x = action.X = GE_ConvertToUnit( ( float ) m_ui->PRSx->value() );
		vec.y = action.Y = GE_ConvertToUnit( ( float ) m_ui->PRSy->value() );
		vec.z = action.Z = GE_ConvertToUnit( ( float ) m_ui->PRSz->value() );
		m_item->setPosition( vec );
	} else if( m_ui->PRSRotation->isChecked() )
	{
		GE_Vec3 vec;
		action.MyType = Action::Rotate;
		if( !hasHistory( action.ItemId ) )
		{
			vec = m_item->getRotation();
			action.X = vec.x;
			action.Y = vec.y;
			action.Z = vec.z;
			push( action );
		}
		vec.x = action.X = GE_ConvertToRadians( ( float ) m_ui->PRSx->value() );
		vec.y = action.Y = GE_ConvertToRadians( ( float ) m_ui->PRSy->value() );
		vec.z = action.Z = GE_ConvertToRadians( ( float ) m_ui->PRSz->value() );
		m_item->setRotation( vec );
	} else if( m_ui->PRSScale->isChecked() )
	{
		GE_Vec3 vec;
		action.MyType = Action::Scale;
		if( !hasHistory( action.ItemId ) )
		{
			vec = m_item->getScale();
			action.X = vec.x;
			action.Y = vec.y;
			action.Z = vec.z;
			push( action );
		}
		vec.x = action.X = ( float ) m_ui->PRSx->value();
		vec.y = action.Y = ( float ) m_ui->PRSy->value();
		vec.z = action.Z = ( float ) m_ui->PRSz->value();
		m_item->setScale( vec );
	}
	push( action );
}

void GEW_Prs::slotUpdateItemOnEditFinished()
{
	if( m_item == NULL ||
		m_item->type() != GEW_Item::Group &&
		m_item->type() != GEW_Item::Entity )
		return;

	Action action;
	action.ItemId = m_item->getId();
	if( m_ui->PRSPosition->isChecked() )
	{
		GE_Vec3 vec;
		action.MyType = Action::Move;
		if( !hasHistory( action.ItemId ) )
		{
			vec = m_item->getPosition();
			action.X = vec.x;
			action.Y = vec.y;
			action.Z = vec.z;
			push( action );
		}
		vec.x = action.X = GE_ConvertToUnit( ( float ) m_ui->PRSx->value() );
		vec.y = action.Y = GE_ConvertToUnit( ( float ) m_ui->PRSy->value() );
		vec.z = action.Z = GE_ConvertToUnit( ( float ) m_ui->PRSz->value() );
		m_item->setPosition2( vec );
	} else if( m_ui->PRSRotation->isChecked() )
	{
		GE_Vec3 vec;
		action.MyType = Action::Rotate;
		if( !hasHistory( action.ItemId ) )
		{
			vec = m_item->getRotation();
			action.X = vec.x;
			action.Y = vec.y;
			action.Z = vec.z;
			push( action );
		}
		vec.x = action.X = GE_ConvertToRadians( ( float ) m_ui->PRSx->value() );
		vec.y = action.Y = GE_ConvertToRadians( ( float ) m_ui->PRSy->value() );
		vec.z = action.Z = GE_ConvertToRadians( ( float ) m_ui->PRSz->value() );

		GE_Mat4x4 rotMat;
		GE_Quat rot;
		GE_Vec3 pvot = m_item->getPivot();

		float ang = vec.x + vec.y + vec.z;
		if( ang != 0 )
		{
			vec.normalize();
			rot.rotationAxis( &vec, ang );
			rotMat.RotationQuaternion( &rot );
			m_item->setRotation( rotMat, pvot );
		}

		m_ui->PRSx->blockSignals( true );
		m_ui->PRSy->blockSignals( true );
		m_ui->PRSz->blockSignals( true );
		m_ui->PRSx->setValue( 0 );
		m_ui->PRSy->setValue( 0 );
		m_ui->PRSz->setValue( 0 );
		m_ui->PRSx->blockSignals( false );
		m_ui->PRSy->blockSignals( false );
		m_ui->PRSz->blockSignals( false );
	} else if( m_ui->PRSScale->isChecked() )
	{
		GE_Vec3 vec;
		action.MyType = Action::Scale;
		if( !hasHistory( action.ItemId ) )
		{
			vec = m_item->getScale();
			action.X = vec.x;
			action.Y = vec.y;
			action.Z = vec.z;
			push( action );
		}
		vec.x = action.X = ( float ) m_ui->PRSx->value();
		vec.y = action.Y = ( float ) m_ui->PRSy->value();
		vec.z = action.Z = ( float ) m_ui->PRSz->value();

		GE_Mat4x4 mat;
		GE_Vec3 pos( 0, 0, 0 );
		GE_Quat rot;
		rot.identity();
		mat.srp( pos, rot, vec );

		m_item->setTransform( &mat );

		m_ui->PRSx->blockSignals( true );
		m_ui->PRSy->blockSignals( true );
		m_ui->PRSz->blockSignals( true );
		m_ui->PRSx->setValue( 1.0 );
		m_ui->PRSy->setValue( 1.0 );
		m_ui->PRSz->setValue( 1.0 );
		m_ui->PRSx->blockSignals( false );
		m_ui->PRSy->blockSignals( false );
		m_ui->PRSz->blockSignals( false );
	}
	push( action );
}

void GEW_Prs::showBar()
{
	m_ui->PRSVisible->blockSignals( true );
	m_ui->PRSVisible->setChecked( true );
	m_ui->PRSVisible->blockSignals( false );
	setVisible( true );
}

void GEW_Prs::on_PRSx_valueChanged( double i_value )
{
	Q_UNUSED( i_value );
	slotUpdateItem();
}

void GEW_Prs::on_PRSy_valueChanged( double i_value )
{
	Q_UNUSED( i_value );
	slotUpdateItem();
}

void GEW_Prs::on_PRSz_valueChanged( double i_value )
{
	Q_UNUSED( i_value );
	slotUpdateItem();
}

void GEW_Prs::on_PRSx_editingFinished()
{
	slotUpdateItemOnEditFinished();
}

void GEW_Prs::on_PRSy_editingFinished()
{
	slotUpdateItemOnEditFinished();
}

void GEW_Prs::on_PRSz_editingFinished()
{
	slotUpdateItemOnEditFinished();
}

void GEW_Prs::on_PRSPosition_clicked( bool i_checked )
{
	if( !i_checked )
	{
		m_ui->PRSPosition->blockSignals( true );
		m_ui->PRSPosition->setChecked( true );
		m_ui->PRSPosition->blockSignals( false );
	} else
	{
		m_ui->PRSIcon->setPixmap( m_ui->PRSPosition->icon().pixmap( 16, 16 ) );
		m_ui->PRSIcon->setToolTip( "Move is active" );
		m_ui->PRSRotation->blockSignals( true );
		m_ui->PRSScale->blockSignals( true );
		m_ui->PRSx->blockSignals( true );
		m_ui->PRSy->blockSignals( true );
		m_ui->PRSz->blockSignals( true );

		m_ui->PRSRotation->setChecked( false );
		m_ui->PRSScale->setChecked( false );
		m_ui->PRSx->setMinimum( -100000.0 );
		m_ui->PRSy->setMinimum( -100000.0 );
		m_ui->PRSz->setMinimum( -100000.0 );
		m_ui->PRSx->setMaximum( 100000.0 );
		m_ui->PRSy->setMaximum( 100000.0 );
		m_ui->PRSz->setMaximum( 100000.0 );
		m_ui->PRSx->setSingleStep( 0.01 );
		m_ui->PRSy->setSingleStep( 0.01 );
		m_ui->PRSz->setSingleStep( 0.01 );
		m_ui->PRSx->setDecimals( 3 );
		m_ui->PRSy->setDecimals( 3 );
		m_ui->PRSz->setDecimals( 3 );

		m_ui->PRSRotation->blockSignals( false );
		m_ui->PRSScale->blockSignals( false );
		m_ui->PRSx->blockSignals( false );
		m_ui->PRSy->blockSignals( false );
		m_ui->PRSz->blockSignals( false );
	}
	slotSetItem( m_item );
}

void GEW_Prs::on_PRSRotation_clicked( bool i_checked )
{
	if( !i_checked )
	{
		m_ui->PRSRotation->blockSignals( true );
		m_ui->PRSRotation->setChecked( true );
		m_ui->PRSRotation->blockSignals( false );
	} else
	{
		m_ui->PRSIcon->setPixmap( m_ui->PRSRotation->icon().pixmap( 16, 16 ) );
		m_ui->PRSIcon->setToolTip( "Rotate is active" );
		m_ui->PRSPosition->blockSignals( true );
		m_ui->PRSScale->blockSignals( true );
		m_ui->PRSx->blockSignals( true );
		m_ui->PRSy->blockSignals( true );
		m_ui->PRSz->blockSignals( true );

		m_ui->PRSPosition->setChecked( false );
		m_ui->PRSScale->setChecked( false );
		m_ui->PRSx->setMinimum( -360.0 );
		m_ui->PRSy->setMinimum( -360.0 );
		m_ui->PRSz->setMinimum( -360.0 );
		m_ui->PRSx->setMaximum( 360.0 );
		m_ui->PRSy->setMaximum( 360.0 );
		m_ui->PRSz->setMaximum( 360.0 );
		m_ui->PRSx->setSingleStep( 0.01 );
		m_ui->PRSy->setSingleStep( 0.01 );
		m_ui->PRSz->setSingleStep( 0.01 );
		m_ui->PRSx->setDecimals( 3 );
		m_ui->PRSy->setDecimals( 3 );
		m_ui->PRSz->setDecimals( 3 );

		m_ui->PRSPosition->blockSignals( false );
		m_ui->PRSScale->blockSignals( false );
		m_ui->PRSx->blockSignals( false );
		m_ui->PRSy->blockSignals( false );
		m_ui->PRSz->blockSignals( false );
	}
	slotSetItem( m_item );
}

void GEW_Prs::on_PRSScale_clicked( bool i_checked )
{
	if( !i_checked )
	{
		m_ui->PRSScale->blockSignals( true );
		m_ui->PRSScale->setChecked( true );
		m_ui->PRSScale->blockSignals( false );
	} else
	{
		m_ui->PRSIcon->setPixmap( m_ui->PRSScale->icon().pixmap( 16, 16 ) );
		m_ui->PRSIcon->setToolTip( "Scale is active" );
		m_ui->PRSRotation->blockSignals( true );
		m_ui->PRSPosition->blockSignals( true );
		m_ui->PRSx->blockSignals( true );
		m_ui->PRSy->blockSignals( true );
		m_ui->PRSz->blockSignals( true );

		m_ui->PRSRotation->setChecked( false );
		m_ui->PRSPosition->setChecked( false );
		m_ui->PRSx->setMinimum( -100000.0 );
		m_ui->PRSy->setMinimum( -100000.0 );
		m_ui->PRSz->setMinimum( -100000.0 );
		m_ui->PRSx->setMaximum( 100000.0 );
		m_ui->PRSy->setMaximum( 100000.0 );
		m_ui->PRSz->setMaximum( 100000.0 );
		m_ui->PRSx->setSingleStep( 0.01 );
		m_ui->PRSy->setSingleStep( 0.01 );
		m_ui->PRSz->setSingleStep( 0.01 );
		m_ui->PRSx->setDecimals( 3 );
		m_ui->PRSy->setDecimals( 3 );
		m_ui->PRSz->setDecimals( 3 );

		m_ui->PRSRotation->blockSignals( false );
		m_ui->PRSPosition->blockSignals( false );
		m_ui->PRSx->blockSignals( false );
		m_ui->PRSy->blockSignals( false );
		m_ui->PRSz->blockSignals( false );
	}
	slotSetItem( m_item );
}

void GEW_Prs::updateUi()
{
	if( m_item )
		slotSetItem( m_item );
}

void GEW_Prs::on_PRSUndo_clicked()
{
	if( m_historyPointer == 0 )//empty
		return;

	if( m_historyPointer == m_history.size() )
		--m_historyPointer;
	Action action;
	if( m_historyPointer )
		action = m_history[ --m_historyPointer ];
	else
		return;

	m_ui->PRSRedo->setEnabled( true );
	if( m_historyPointer == 0 )//empty
		m_ui->PRSUndo->setEnabled( false );

	GEW_Item *item = GEW_ItemList::getSingleton().getRoot()->findItem( action.ItemId );
	if( !item )
		return;

	if( item->type() == GEW_Item::Group ||
		item->type() == GEW_Item::Entity )
	{
		if( action.MyType == Action::Move )
		{
			GE_Vec3 pos( action.X, action.Y, action.Z );
			item->setPosition2( pos );
		} else if( action.MyType == Action::Rotate )
		{
			//GE_Vec3 rot( action.X, action.Y, action.Z );
			//item->setRotation( rot );
		} else if( action.MyType == Action::Scale )
		{
			//GE_Vec3 scale( action.X, action.Y, action.Z );
			//item->setScale( scale );
		}
	} else
	{
		if( action.MyType == Action::Move )
		{
			GE_Vec3 pos( action.X, action.Y, action.Z );
			item->setPosition( pos );
		} else if( action.MyType == Action::Rotate )
		{
			GE_Vec3 rot( action.X, action.Y, action.Z );
			item->setRotation( rot );
		} else if( action.MyType == Action::Scale )
		{
			GE_Vec3 scale( action.X, action.Y, action.Z );
			item->setScale( scale );
		}
	}

	if( item == m_item )
		updateUi();
}

void GEW_Prs::on_PRSRedo_clicked()
{
	if( m_historyPointer == m_history.size() )//on top
		return;

	Action action;
	++m_historyPointer;
	if( m_historyPointer == m_history.size() )
		action = m_history[ m_historyPointer - 1 ];
	else
		action = m_history[ m_historyPointer ];

	m_ui->PRSUndo->setEnabled( true );
	if( m_historyPointer == m_history.size() - 1 )//on top
		m_ui->PRSRedo->setEnabled( false );

	GEW_Item *item = GEW_ItemList::getSingleton().getRoot()->findItem( action.ItemId );
	if( !item )
		return;

	if( item->type() == GEW_Item::Group ||
		item->type() == GEW_Item::Entity )
	{
		if( action.MyType == Action::Move )
		{
			GE_Vec3 pos( action.X, action.Y, action.Z );
			item->setPosition2( pos );
		} else if( action.MyType == Action::Rotate )
		{
			//GE_Vec3 rot( action.X, action.Y, action.Z );
			//item->setRotation( rot );
		} else if( action.MyType == Action::Scale )
		{
			//GE_Vec3 scale( action.X, action.Y, action.Z );
			//item->setScale( scale );
		}
	} else
	{
		if( action.MyType == Action::Move )
		{
			GE_Vec3 pos( action.X, action.Y, action.Z );
			item->setPosition( pos );
		} else if( action.MyType == Action::Rotate )
		{
			GE_Vec3 rot( action.X, action.Y, action.Z );
			item->setRotation( rot );
		} else if( action.MyType == Action::Scale )
		{
			GE_Vec3 scale( action.X, action.Y, action.Z );
			item->setScale( scale );
		}
	}

	if( item == m_item )
		updateUi();
}

void GEW_Prs::push( GEW_Prs::Action &i_action )
{
	if( m_disableHistory || m_historyPt->getTime() < 1.0f / 3.0f )
		return;
	m_historyPt->reStart();

	if( m_history.size() == INT32_MAX )
	{
		m_history.clear();
		m_historyPointer = 0;
	}

	if( m_historyPointer != 0 && m_history[ m_historyPointer - 1 ] == i_action )//reduplicative
		return;

	clearExtraHistory();

	m_history.push_back( i_action );
	++m_historyPointer;

	m_ui->PRSUndo->setEnabled( true );
	m_ui->PRSRedo->setEnabled( false );
}

void GEW_Prs::clearHistory()
{
	m_historyPointer = 0;
	m_history.clear();
	m_ui->PRSUndo->setEnabled( false );
	m_ui->PRSRedo->setEnabled( false );
}

bool GEW_Prs::hasHistory( uint32_t i_itemId )
{
	foreach( Action action, m_history )
		if( action.ItemId == i_itemId )
			return true;
	return false;
}

void GEW_Prs::clearExtraHistory()
{
	if( m_historyPointer < m_history.size() )
	{
		for( int i = m_history.size() - 1; i > m_historyPointer; --i )
			m_history.removeAt( i );
		m_historyPointer = m_history.size();
	}
	m_ui->PRSUndo->setEnabled( m_historyPointer != 0 );
	m_ui->PRSRedo->setEnabled( false );
}

void GEW_Prs::on_Pick_toggled( bool i_checked )
{
	if( i_checked )
	{
		if( m_ui->PRSScale->isChecked() )
			m_ui->PRSPosition->setChecked( true );

		//load all data
		physx::PxMaterial *pxMaterial = GEW_Physics::getSingleton().getPxMaterial( 0 );
		uint32_t shapeFlags = physx::PxShapeFlag::eSCENE_QUERY_SHAPE;
		physx::PxFilterData queryFilterData;
		queryFilterData.word3 = GEW_Physics::DRIVABLE_SURFACE;
		physx::PxFilterData simulationFilterData;
		simulationFilterData.word0 = GEW_Physics::COLLISION_FLAG_DRIVABLE_OBSTACLE;
		simulationFilterData.word1 = GEW_Physics::COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST;

		m_pickRigidStatic = GEW_Physics::getSingleton().m_physics->createRigidStatic(
			physx::PxTransform::createIdentity() );
		m_pickRigidStatic->setActorFlag( physx::PxActorFlag::eVISUALIZATION, false );
		m_pickRigidStatic->setActorFlag( physx::PxActorFlag::eDISABLE_GRAVITY, false );
		m_pickRigidStatic->setActorFlag( physx::PxActorFlag::eSEND_SLEEP_NOTIFIES, false );
		m_pickRigidStatic->setActorFlag( physx::PxActorFlag::eDISABLE_SIMULATION, false );
		//m_rigidStatic->setName( model->getObjectName().c_str() );

		if( !m_ui->OnlyPickTerrain->isChecked() )
		{
			foreach( GEW_Model *gewModel, GEW_Model::m_list )
			{
				GE_Model *model = gewModel->m_model;
				if( !model->isVisible() )
					continue;

				for( uint32_t i = 0; i < model->getNumberOfSubsets(); ++i )
				{
					physx::PxVec3 position = TO_PxVec3( model->getPosition() );
					physx::PxQuat rotation = TO_PxQuat( model->getRotation() );
					physx::PxTransform localPos( position, rotation );
					std::vector< GE_Vec3 > vertices;
					std::vector< uint32_t > indices;
					model->getSubsetVerticesData(
						i,
						&vertices,
						NULL,
						NULL,
						NULL,
						NULL,
						NULL,
						NULL,
						&indices );
					physx::PxTriangleMesh *triangleMesh = GEW_Physics::getSingleton().createTriangleMesh32(
						vertices.data(),
						( uint32_t ) vertices.size(),
						indices.data(),
						( uint32_t ) indices.size() / 3 );
					m_pickTriangleMeshes.push_back( triangleMesh );

					physx::PxTriangleMeshGeometry triangleMeshGeometry;
					triangleMeshGeometry.triangleMesh = triangleMesh;
					physx::PxShape *shape = m_pickRigidStatic->createShape(
						triangleMeshGeometry,
						*pxMaterial,
						( physx::PxShapeFlag::Enum )shapeFlags );
					shape->setLocalPose( localPos );
					shape->setSimulationFilterData( simulationFilterData );
					shape->setQueryFilterData( queryFilterData );
				}
			}
		}
		if( GEW_Terrain::getSingleton()->m_baseData.Enable )
		{
			physx::PxTransform localPose;
			localPose.p = physx::PxVec3( -( GE_Terrain::getSingleton( ).getWidth( ) * 0.5f ), ( GE_Terrain::getSingleton( ).getWidth( ) * 0.5f ), 0.0f );
			localPose.q = *( physx::PxQuat * )&GE_Quat( ).rotationAxis( &GE_Vec3( 1.0f, 0.0f, 0.0f ), GE_ConvertToRadians( 90.0f ) );
			physx::PxMaterial *material = GEW_Physics::getSingleton().getPxMaterial( 0 );
			physx::PxShape* shape = m_pickRigidStatic->createShape( GEW_Terrain::getSingleton()->getHeightFieldGeometry(), *material, localPose );

			shape->setSimulationFilterData( simulationFilterData );
			shape->setQueryFilterData( queryFilterData );
		}
		GEW_Physics::getSingleton().m_scene2->addActor( *m_pickRigidStatic );
		m_pickEnabled = true;
	} else
	{
		//unload all data
		if( m_pickRigidStatic )
		{
			GEW_Physics::getSingleton().m_scene2->removeActor( *m_pickRigidStatic );
			m_pickRigidStatic->release();
		}
		m_pickRigidStatic = NULL;
		foreach( physx::PxTriangleMesh *triMesh, m_pickTriangleMeshes )
			triMesh->release();
		m_pickTriangleMeshes.clear();
		m_pickEnabled = false;
	}
}

void GEW_Prs::updatePick( GEW_Input::Data &i_inputData, const GE_View &i_view, const GE_Vec2 &i_cursorPos, const GE_Vec2 &i_widthHeight )
{
	if( !m_pickEnabled )
		return;
	GE_Vec3 org;
	GE_Vec3 dir;
	( ( GE_View * ) &i_view )->getRayFromScreen( i_cursorPos, i_widthHeight, &org, &dir );
	physx::PxRaycastBuffer hit;
	physx::PxOverlapBuffer overHit;
	physx::PxTransform shapePose;
	GEW_Physics::getSingleton().m_scene2->raycast( TO_PxVec3( org ), TO_PxVec3( dir ), GE_ConvertToUnit( 1000000 ), hit );

	if( i_inputData.isMouseDown( GUM_BUTTON_LEFT ) && !m_disableHistory )
		m_disableHistory = true;
	else if( !i_inputData.isMouseDown( GUM_BUTTON_LEFT ) && m_disableHistory )
	{
		m_disableHistory = false;
		slotUpdateItem();
	}

	if( hit.hasBlock && i_inputData.isMouseDown( GUM_BUTTON_LEFT ) )
	{
		if( m_ui->PRSPosition->isChecked() )
		{
			if( !m_ui->PickX->isChecked() )
				m_ui->PRSx->setValue( GE_ConvertToMeter( hit.block.position.x ) );
			if( !m_ui->PickY->isChecked() )
				m_ui->PRSy->setValue( GE_ConvertToMeter( hit.block.position.y ) );
			if( !m_ui->PickZ->isChecked() )
				m_ui->PRSz->setValue( GE_ConvertToMeter( hit.block.position.z ) );
		} else if( m_ui->PRSRotation->isChecked() )
		{
			GE_Vec3 dir = TO_GEVec3( hit.block.normal );
			float angle = ( float ) acosf( dir.dot( &GE_Vec3( 0.0f, 0.0f, 1.0f ) ) );
			GE_Vec3 ax( 0.0f, 0.0f, 1.0f );
			ax.cross( &dir );
			if( !ax.x && !ax.y && !ax.z )
				ax.z = 1.0f;
			GE_Quat qt;
			qt.rotationAxis( &ax, angle );
			qt.normalize();
			qt.toYPR( &dir );

			if( !m_ui->PickX->isChecked() )
				m_ui->PRSx->setValue( GE_ConvertToDegrees( dir.x ) );
			if( !m_ui->PickY->isChecked() )
				m_ui->PRSy->setValue( GE_ConvertToDegrees( dir.y ) );
			if( !m_ui->PickZ->isChecked() )
				m_ui->PRSz->setValue( GE_ConvertToDegrees( dir.z ) );
		}
	}
}

void GEW_Prs::on_FocusToTarget_clicked()
{
	if ( m_item == NULL )
		return;
	list<GE_RenderUtilityObject *> ruo;
	m_item->getRenderUtilityObjects( ruo );
	if ( !ruo.size() )
		return;
	GE_Vec3 midPos = GE_Vec3(0.0f, 0.0f, 0.0f);
	GE_Vec3 minPos = GE_Vec3( GE_FLOAT32_MAX, GE_FLOAT32_MAX, GE_FLOAT32_MAX );
	GE_Vec3 maxPos = GE_Vec3( GE_FLOAT32_MIN, GE_FLOAT32_MIN, GE_FLOAT32_MIN );
	auto it = ruo.begin();
	for ( uint32_t i = 0; i < (uint32_t)ruo.size(); ++i )
	{
		GE_Bound *bound = &( *it )->getBound();
		midPos += bound->BoundCenter;
		minPos.x = GE_MIN( minPos.x, bound->AABoundBoxMin.x );
		minPos.y = GE_MIN( minPos.y, bound->AABoundBoxMin.y );
		minPos.z = GE_MIN( minPos.z, bound->AABoundBoxMin.z );

		maxPos.x = GE_MAX( maxPos.x, bound->AABoundBoxMax.x );
		maxPos.y = GE_MAX( maxPos.y, bound->AABoundBoxMax.y );
		maxPos.z = GE_MAX( maxPos.z, bound->AABoundBoxMax.z );

		it++;
	}
	midPos /= (float)ruo.size();
	GEW_Scene::getSingleton( ).m_camera->lookAt( midPos, ( maxPos - minPos ).length( ) * 0.5f );
}

void GEW_Prs::on_TurnToTarget_clicked()
{
	if ( m_item == NULL )
		return;
	list<GE_RenderUtilityObject *> ruo;
	m_item->getRenderUtilityObjects( ruo );
	if ( !ruo.size() )
		return;
	GE_Vec3 midPos = GE_Vec3( 0.0f, 0.0f, 0.0f );
	auto it = ruo.begin();
	for ( uint32_t i = 0; i < ( uint32_t )ruo.size(); ++i )
	{
		GE_Bound *bound = &( *it )->getBound();
		midPos += bound->BoundCenter;
		it++;
	}
	midPos /= ( float )ruo.size();
	GEW_Scene::getSingleton().m_camera->lookAt( midPos );
}

void GEW_Prs::on_PRSVisible_toggled( bool i_checked )
{
	setVisible( i_checked );
}

bool GEW_Prs::Action::operator==( Action &i_other )
{
	return
		MyType == i_other.MyType &&
		X == i_other.X &&
		Y == i_other.Y &&
		Z == i_other.Z &&
		ItemId == i_other.ItemId;
}
