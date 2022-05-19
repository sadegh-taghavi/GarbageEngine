#include "GEW_Item.h"
#include "GEW_ItemList.h"

uint32_t GEW_Item::m_count = 0;
uint32_t GEW_Item::m_index = 1;

GEW_Item::GEW_Item( QTreeWidget *i_parent, Type i_type )
: QTreeWidgetItem( i_parent, ( int ) i_type )
{
	m_count++;
	m_id = m_index++;
}

GEW_Item::GEW_Item( GEW_Item *i_parent, Type i_type )
: QTreeWidgetItem( ( QTreeWidgetItem * ) i_parent, ( int ) i_type )
{
	m_count++;
	m_id = m_index++;
	m_selected = false;
}

GEW_Item::~GEW_Item()
{
	m_count--;
	m_id = m_index++;
	m_selected = false;
	int cc = childCount();
	for ( int i = 0; i < cc; ++i )
	{
		GEW_Item *item = ( GEW_Item* )child( 0 );
		delete item;
	}
}

GEW_Item * GEW_Item::findItem( QString &i_name )
{
	if( text( 0 ).toLower() == i_name )
		return this;

	GEW_Item *it;
	for( int i = 0; i < childCount(); ++i )
		if( ( it = ( ( GEW_Item * ) child( i ) )->findItem( i_name ) ) )
			return it;

	return NULL;
}

GEW_Item* GEW_Item::findItem( uint32_t i_id )
{
	if ( i_id == m_id )
		return this;

	GEW_Item *it;
	for ( int i = 0; i < childCount(); ++i )
	if ( ( it = ( ( GEW_Item * )child( i ) )->findItem( i_id) ) )
		return it;

	return NULL;
}

void GEW_Item::setItemCheckState( GEW_Item::CheckIndex i_index, bool i_checked )
{
	bool block = GEW_ItemList::getSingleton().isItemSignalBlocked();
	GEW_ItemList::getSingleton().blockItemSignal( true );
	setCheckState( i_index, i_checked ? Qt::Checked : Qt::Unchecked );
	GEW_ItemList::getSingleton().blockItemSignal( block );
}

GEW_Item *GEW_Item::duplicate()
{
	return NULL;
}

void GEW_Item::update()
{
	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->update();
}

void GEW_Item::select()
{
	m_selected = true;
}

void GEW_Item::deSelect()
{
	m_selected = false;
}

bool GEW_Item::isSecected()
{
	return m_selected;
}

void GEW_Item::checkStateChanged( GEW_Item::CheckIndex i_column )
{
}

void GEW_Item::parentCheckStateChanged( GEW_Item::CheckIndex i_column, Qt::CheckState i_checkState )
{
}

void GEW_Item::saveToFile( QFile &i_file, const QString &i_resourcePath )
{
	GEW_Item::SaveHeader header;
	header.MyType = ( GEW_Item::Type )type();
	header.ParentId = 0;
	header.MyId = getId();
	i_file.write( ( const char * ) &header, sizeof( GEW_Item::SaveHeader ) );

	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->saveToFile( i_file, i_resourcePath );
}

void GEW_Item::setObjectVisible( bool i_checked )
{
	if( type() != GEW_Item::Group && type() != GEW_Item::Entity )
		setHidden( !i_checked );
	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->setObjectVisible( i_checked );
}

void GEW_Item::setPosition( const GE_Vec3 &i_position )
{
}

void GEW_Item::setPosition2( const GE_Vec3 &i_position )
{
	for ( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* )child( i ) )->setPosition2( i_position );
}

void GEW_Item::setRotation( const GE_Vec3 &i_rotation )
{
}

void GEW_Item::setRotation( GE_Mat4x4 &i_rot, GE_Vec3 & i_pivot )
{
	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->setRotation( i_rot, i_pivot );
}

void GEW_Item::setScale( const GE_Vec3 &i_scale )
{
}

void GEW_Item::setTransform( GE_Mat4x4 *i_matrix )
{
}

GE_Vec3 GEW_Item::getPosition()
{
	return GE_Vec3( 0, 0, 0 );
}

GE_Vec3 GEW_Item::getRotation()
{
	return GE_Vec3( 0, 0, 0 );
}

GE_Vec3 GEW_Item::getScale()
{
	return GE_Vec3( 0, 0, 0 );
}

GE_Vec3 GEW_Item::getPivot()
{
	return getPosition();
}

void GEW_Item::getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList )
{
	for( int i = 0; i < childCount(); ++i )
		( ( GEW_Item* ) child( i ) )->getRenderUtilityObjects( i_outList );
}

