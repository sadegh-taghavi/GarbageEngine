#include "GEW_Entity.h"
#include <GE_RenderUtility.h>

QList< GEW_Entity* > GEW_Entity::m_list;
QIcon GEW_Entity::m_icon;

GEW_Entity *GEW_Entity::CreateFromFile( GEW_Item *i_parent, QFile &i_file, const QString &i_resourcePath )
{
	GEW_Entity::SaveData saveData;
	i_file.read( ( char* ) &saveData, sizeof( GEW_Entity::SaveData ) );
	GEW_Entity *gewEntity = new GEW_Entity( saveData.Name, i_parent );
	gewEntity->setItemCheckState( GEW_Item::Enable, saveData.EnableChecked );
	gewEntity->setItemCheckState( GEW_Item::Dynamic, saveData.DynamicChecked );
	gewEntity->setItemCheckState( GEW_Item::Physics, saveData.PhysicsChecked );
	return gewEntity;
}

GEW_Entity::SaveData::SaveData()
{
	memset( this, 0, sizeof( GEW_Entity::SaveData ) );
}

GEW_Entity::GEW_Entity( const QString &i_name, GEW_Item *i_parent )
: GEW_Item( i_parent, GEW_Item::Entity )
{
	setText( 0, i_name );
	setIcon( 0, m_icon );
	setCheckState( GEW_Item::Enable, Qt::Checked );
	setCheckState( GEW_Item::Dynamic, Qt::Unchecked );
	setCheckState( GEW_Item::Physics, Qt::Unchecked );
	setToolTip( GEW_Item::Enable, "Enable / Visible" );
	setToolTip( GEW_Item::Dynamic, "Dynamic object" );
	setToolTip( GEW_Item::Physics, "Physics enable" );
	setExpanded( true );
	setFlags(
		Qt::ItemFlag::ItemIsEnabled |
		Qt::ItemFlag::ItemIsUserCheckable |
		Qt::ItemFlag::ItemIsDragEnabled |
		Qt::ItemFlag::ItemIsSelectable
		);

	m_list.push_back( this );
}

GEW_Entity::~GEW_Entity()
{
	m_list.removeOne( this );
}

GEW_Item *GEW_Entity::duplicate()
{
	return NULL;
}

void GEW_Entity::update()
{
	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->update();
}

void GEW_Entity::select()
{
	m_selected = true;
	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->select();
}

void GEW_Entity::deSelect()
{
	m_selected = false;
	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->deSelect();
}

void GEW_Entity::checkStateChanged( GEW_Item::CheckIndex i_column )
{
	Qt::CheckState myCheckState = checkState( i_column );
	for( int i = 0; i < childCount(); ++i )
	{
		GEW_Item *item = ( GEW_Item* ) child( i );
		item->parentCheckStateChanged( i_column, myCheckState );
	}
}

void GEW_Entity::parentCheckStateChanged( GEW_Item::CheckIndex i_column, Qt::CheckState i_checkState )
{
	setCheckState( i_column, i_checkState );
	checkStateChanged( i_column );
}

void GEW_Entity::saveToFile( QFile &i_file, const QString &i_resourcePath )
{
	GEW_Item::SaveHeader header;
	header.MyType = ( GEW_Item::Type )type();
	header.ParentId = ( ( GEW_Item* ) parent() )->getId();
	header.MyId = getId();
	i_file.write( ( const char * ) &header, sizeof( GEW_Item::SaveHeader ) );

	GEW_Entity::SaveData saveData;
	strcpy_s( saveData.Name, text( 0 ).toStdString().c_str() );
	saveData.EnableChecked = checkState( GEW_Item::Enable ) == Qt::Checked;
	saveData.DynamicChecked = checkState( GEW_Item::Dynamic ) == Qt::Checked;
	saveData.PhysicsChecked = checkState( GEW_Item::Physics ) == Qt::Checked;
	i_file.write( ( const char * ) &saveData, sizeof( GEW_Entity::SaveData ) );

	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->saveToFile( i_file, i_resourcePath );
}

void GEW_Entity::setPosition( const GE_Vec3 &i_position )
{
	if( childCount() )
	{
		GE_Vec3 offset = ( GE_Vec3 ) i_position - m_position;
		for( int i = 0; i < childCount(); ++i )
		{
			GEW_Item *item = ( GEW_Item* ) child( i );
			item->setPosition(
				item->getPosition() + offset );
		}
	}
	m_position = i_position;
}

void GEW_Entity::setPosition2( const GE_Vec3 &i_position )
{
	if( childCount() )
	{
		GE_Vec3 offset = ( GE_Vec3 ) i_position - m_position;
		for( int i = 0; i < childCount(); ++i )
		{
			GEW_Item *item = ( GEW_Item* ) child( i );
			item->setPosition2(
				item->getPosition() + offset );
		}
	}
	m_position = i_position;
}

void GEW_Entity::setRotation( const GE_Vec3 &i_rotation )
{
}

void GEW_Entity::setScale( const GE_Vec3 &i_scale )
{
	if( childCount() )
	{
		for( int i = 0; i < childCount(); ++i )
		{
			GEW_Item *item = ( GEW_Item* ) child( i );
			item->setScale( item->getScale() + i_scale );
		}
	}
}

void GEW_Entity::setTransform( GE_Mat4x4 *i_matrix )
{
	for( int i = 0; i < childCount(); ++i )
	{
		GEW_Item *item = ( GEW_Item * ) child( i );
		item->setTransform( i_matrix );
	}
}

GE_Vec3 GEW_Entity::getPosition()
{
	if( childCount() )
	{
		m_position = ( ( GEW_Item* ) child( 0 ) )->getPosition();
		for( int i = 1; i < childCount(); ++i )
			m_position += ( ( GEW_Item* ) child( i ) )->getPosition();
		m_position /= ( float ) childCount();
	} else
	{
		m_position.x = 0;
		m_position.y = 0;
		m_position.z = 0;
	}
	return m_position;
}

GE_Vec3 GEW_Entity::getRotation()
{
	return GE_Vec3( 0, 0, 0 );
}

GE_Vec3 GEW_Entity::getScale()
{
	return GE_Vec3( 1.0f, 1.0f, 1.0f );
}

GE_Vec3 GEW_Entity::getPivot()
{
	if( childCount() )
	{
		GE_Vec3 minPos;
		GE_Vec3 maxPos;
		minPos = maxPos = ( ( GEW_Item* ) child( 0 ) )->getPivot();
		for( int i = 1; i < childCount(); ++i )
		{
			GE_Vec3 pos = ( ( GEW_Item* ) child( i ) )->getPivot();
			minPos.x = GE_MIN( minPos.x, pos.x );
			minPos.y = GE_MIN( minPos.y, pos.y );
			minPos.z = GE_MIN( minPos.z, pos.z );
			maxPos.x = GE_MAX( maxPos.x, pos.x );
			maxPos.y = GE_MAX( maxPos.y, pos.y );
			maxPos.z = GE_MAX( maxPos.z, pos.z );
		}
		return ( ( maxPos + minPos ) * 0.5f );
	} 
	return GE_Vec3( 0, 0, 0 );
}

void GEW_Entity::getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList )
{
	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->getRenderUtilityObjects( i_outList );
}
