#pragma once
#include <stdint.h>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFile>
#include <GE_Vec3.h>
#include <GE_Quat.h>

class GE_RenderUtilityObject;

class GEW_Item : public QTreeWidgetItem
{
	static uint32_t m_count;
	static uint32_t m_index;
	uint32_t m_id;
protected:
	bool	m_selected;
public:
	enum Type
	{
		Root = QTreeWidgetItem::UserType + 0,
		Model,
		Light,
		Flare,
		Group,
		EnvironmentProbe,
		Decal,
		Entity,
		Vehicle,
	};
	enum CheckIndex
	{
		Enable = 1,
		Dynamic = 2,
		Physics = 3,
	};

	struct SaveHeader
	{
		GEW_Item::Type	MyType;
		uint32_t		ParentId;
		uint32_t		MyId;
	};

	GEW_Item( GEW_Item *i_parent, Type i_type );
	GEW_Item( QTreeWidget *i_parent, Type i_type );
	~GEW_Item();
	static	uint32_t	getCount() { return m_count; }
	uint32_t			getId() { return m_id; }
	void				setId( uint32_t i_id ) { m_id = i_id; }

	virtual GEW_Item	*findItem( QString &i_name );
	virtual GEW_Item	*findItem( uint32_t i_id );
	virtual void		setItemCheckState( GEW_Item::CheckIndex i_index, bool i_checked );
	virtual GEW_Item	*duplicate();
	virtual void		update();
	virtual void		select();
	virtual void		deSelect();
	virtual bool		isSecected();
	virtual void		checkStateChanged( GEW_Item::CheckIndex i_column );
	virtual void		parentCheckStateChanged( GEW_Item::CheckIndex i_column, Qt::CheckState i_checkState );
	virtual void		saveToFile( QFile &i_file, const QString &i_resourcePath );
	virtual void		setObjectVisible( bool i_checked );
	virtual void		setPosition( const GE_Vec3 &i_position );
	virtual void		setPosition2( const GE_Vec3 &i_position );
	virtual void		setRotation( const GE_Vec3 &i_rotation );
	virtual void		setRotation( GE_Mat4x4 &i_rot, GE_Vec3 & i_pivot );
	virtual void		setScale( const GE_Vec3 &i_scale );
	virtual void		setTransform( GE_Mat4x4 *i_matrix );
	virtual GE_Vec3		getPosition();
	virtual GE_Vec3		getRotation();
	virtual GE_Vec3		getScale();
	virtual GE_Vec3		getPivot();
	virtual void		getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList );
};