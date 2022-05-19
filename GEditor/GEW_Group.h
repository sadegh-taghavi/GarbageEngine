#pragma once
#include "GEW_Item.h"

class GE_RenderUtilityObject;

class GEW_Group : public GEW_Item
{
	GE_Vec3 m_position;
public:
	static QIcon				m_icon;
	static QList< GEW_Group* >	m_list;
	static GEW_Group			*CreateFromFile( GEW_Item *i_parent, QFile &i_file, const QString &i_resourcePath );
	
	struct SaveData
	{
		char		Name[ 64 ];
		bool		EnableChecked;
		bool		DynamicChecked;
		bool		PhysicsChecked;
		SaveData();
	};

	GE_RenderUtilityObject *m_bound;
	GEW_Group( const QString &i_name, GEW_Item *i_parent );
	~GEW_Group();

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
	GE_Vec3		getPvot();
	void		getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList );
};