#pragma once
#include "GEW_Item.h"

class GE_RenderUtilityObject;

class GEW_Entity : public GEW_Item
{
	GE_Vec3	m_position;
public:
	static QIcon				m_icon;
	static QList< GEW_Entity* >	m_list;
	static GEW_Entity			*CreateFromFile( GEW_Item *i_parent, QFile &i_file, const QString &i_resourcePath );

	struct SaveData
	{
		char		Name[ 64 ];
		bool		EnableChecked;
		bool		DynamicChecked;
		bool		PhysicsChecked;
		SaveData();
	};
	struct BindData
	{
		uint32_t	ParentID;
		uint32_t	BindType;
		uint32_t	BindPos;
		uint32_t	ChildID;
		bool		SaveOffset;
	};

	GEW_Entity( const QString &i_name, GEW_Item *i_parent );
	~GEW_Entity();

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
	void		setScale( const GE_Vec3 &i_scale );
	void		setTransform( GE_Mat4x4 *i_matrix );
	GE_Vec3		getPosition();
	GE_Vec3		getRotation();
	GE_Vec3		getScale();
	GE_Vec3		getPivot();
	void		getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList );
};