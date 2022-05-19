#include "GEW_Group.h"
#include <QInputDialog>
#include <QMessageBox>
#include "GE_RenderUtility.h"

QIcon				GEW_Group::m_icon;
QList< GEW_Group* > GEW_Group::m_list;

GEW_Group *GEW_Group::CreateFromFile( GEW_Item *i_parent, QFile &i_file, const QString &i_resourcePath )
{
	GEW_Group::SaveData saveData;
	i_file.read( ( char* ) &saveData, sizeof( GEW_Group::SaveData ) );
	GEW_Group *gewGroup = new GEW_Group( saveData.Name, i_parent );
	gewGroup->setItemCheckState( GEW_Item::Enable, saveData.EnableChecked );
	gewGroup->setItemCheckState( GEW_Item::Dynamic, saveData.DynamicChecked );
	gewGroup->setItemCheckState( GEW_Item::Physics, saveData.PhysicsChecked );
	return gewGroup;
}

GEW_Group::SaveData::SaveData()
{
	memset( this, 0, sizeof( GEW_Group::SaveData ) );
}

GEW_Group::GEW_Group( const QString &i_name, GEW_Item *i_parent )
: GEW_Item( i_parent, GEW_Item::Group )
{
	m_bound = new GE_RenderUtilityObject();
	m_bound->createBox( GE_Vec3( 1, 1, 1 ), GE_Vec3( 0, 0, 0 ) );
	m_bound->setVisible( true );
	m_bound->setBillboard( false );
	m_bound->setDepthDisable( false );

	setText( 0, i_name );
	setIcon( 0, m_icon );
	setCheckState( GEW_Item::Enable, Qt::Checked );
	setCheckState( GEW_Item::Dynamic, Qt::Unchecked );
	setCheckState( GEW_Item::Physics, Qt::Unchecked );
	setToolTip( GEW_Item::Enable, "Enable" );
	setToolTip( GEW_Item::Dynamic, "Dynamic" );
	setToolTip( GEW_Item::Physics, "Physics" );
	setExpanded( true );
	setFlags(
		Qt::ItemFlag::ItemIsEnabled |
		Qt::ItemFlag::ItemIsUserCheckable |
		Qt::ItemFlag::ItemIsDragEnabled |
		Qt::ItemFlag::ItemIsDropEnabled |
		Qt::ItemFlag::ItemIsSelectable 
		);

	m_list.push_back( this );
}

GEW_Group::~GEW_Group()
{
	m_list.removeOne( this );
	delete m_bound;
	//delete all childes
}

GEW_Item* GEW_Group::duplicate()
{
	return NULL;
}

void GEW_Group::update()
{
	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->update();
}

void GEW_Group::select()
{
	m_selected = true;
	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->select();
}

void GEW_Group::deSelect()
{
	m_selected = false;
	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->deSelect();
}

void GEW_Group::checkStateChanged( GEW_Item::CheckIndex i_column )
{
	Qt::CheckState myCheckState = checkState( i_column );
	for( int i = 0; i < childCount(); ++i )
	{
		GEW_Item *item = ( GEW_Item* ) child( i );
		item->parentCheckStateChanged( i_column, myCheckState );
	}
}

void GEW_Group::parentCheckStateChanged( GEW_Item::CheckIndex i_column, Qt::CheckState i_checkState )
{
	setCheckState( i_column, i_checkState );
	checkStateChanged( i_column );
}

void GEW_Group::saveToFile( QFile &i_file, const QString &i_resourcePath )
{
	GEW_Item::SaveHeader header;
	header.MyType = ( GEW_Item::Type )type();
	header.ParentId = ( ( GEW_Item* ) parent() )->getId();;
	header.MyId = getId();
	i_file.write( ( const char * ) &header, sizeof( GEW_Item::SaveHeader ) );
	
	GEW_Group::SaveData saveData;
	strcpy_s( saveData.Name, text( 0 ).toStdString().c_str() );
	saveData.EnableChecked = checkState( GEW_Item::Enable ) == Qt::Checked;
	saveData.DynamicChecked = checkState( GEW_Item::Dynamic ) == Qt::Checked;
	saveData.PhysicsChecked = checkState( GEW_Item::Physics ) == Qt::Checked;
	i_file.write( ( const char * ) &saveData, sizeof( GEW_Group::SaveData ) );

	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->saveToFile( i_file, i_resourcePath );
}

void GEW_Group::setPosition( const GE_Vec3 &i_position )
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

void GEW_Group::setPosition2( const GE_Vec3 &i_position )
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

void GEW_Group::setRotation( const GE_Vec3 &i_rotation )
{

}

void GEW_Group::setScale( const GE_Vec3 &i_scale )
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

void GEW_Group::setTransform( GE_Mat4x4 *i_matrix )
{
	for( int i = 0; i < childCount(); ++i )
	{
		GEW_Item *item = ( GEW_Item * ) child( i );
		item->setTransform( i_matrix );
	}
}

GE_Vec3 GEW_Group::getPosition()
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

GE_Vec3 GEW_Group::getRotation()
{
	return GE_Vec3( 0, 0, 0 );
}

GE_Vec3 GEW_Group::getScale()
{
	return GE_Vec3( 1.0f, 1.0f, 1.0f );
}

GE_Vec3 GEW_Group::getPvot()
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

void GEW_Group::getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList )
{
	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->getRenderUtilityObjects( i_outList );
}
