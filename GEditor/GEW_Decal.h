#pragma once
#include "GEW_Item.h"
#include <QString>
#include <QList>
#include <GE_Math.h>

class GE_Decal;
class GE_RenderUtilityObject;

class GEW_Decal : public GEW_Item
{
public:
	static uint32_t				 m_newID;
	static int32_t				 m_selectedItem;
	static QIcon				 m_iconDecal;
	static QList< GEW_Decal* > 	 m_list;
	static GEW_Decal			 *find( QString &i_name );
	static GEW_Decal			 *createFromFile( GEW_Item *i_parent, QFile &i_file, const QString &i_resourcePath );

	GE_Decal			     *m_decal;
	GE_Vec3					m_groupPos;
	struct Property
	{
		GE_RenderUtilityObject*	Bound;
		GE_Vec3					BoundColor;
		QString					Name;
		float					VisibleDistance;
		bool					Enable;
		GE_Vec3					Position;
		GE_Vec3					Scale;
		GE_Quat					Rotation;
		Property();
		~Property( );
	};
	QList< Property *> m_properties;
	bool m_enable;

	struct SaveData
	{
		bool		Enable;
		char		Name[ 64 ];
		char		DiffuseMap[ 512 ];
		char		NormalMap[ 512 ];
		char		GlowSpecularMap[ 512 ];
		char		ReflectMap[ 512 ];
		uint32_t	DecalCount;
		SaveData();
	};

	struct DecalSaveData
	{
		bool		Enable;
		char		Name[ 64 ];
		GE_Vec3		BoundColor;
		float		VisibleDistance;
		GE_Vec3		Position;
		GE_Vec3		Scale;
		GE_Quat		Rotation;
		float		MaxAngle;
		float		Bumpiness;
		float		Glowness;
		float		Roughness;
		float		Reflectmask;
		float		Layer;
		float		RotationF;
		GE_Vec2		UVTile;
		GE_Vec2		UVMove;
		GE_Vec4    DiffuseColor;
		DecalSaveData( );
	};

	GEW_Decal( const QString &i_name, GEW_Item *i_parent );
	void		update();
	void updateTransform( bool i_updateAll = false );
	void		select();
	void		deSelect();
	void		checkStateChanged( GEW_Item::CheckIndex i_column );
	void		parentCheckStateChanged( GEW_Item::CheckIndex i_column, Qt::CheckState i_checkState );
	void		saveToFile( QFile &i_file, const QString &i_resourcePath );
	void		setPosition( const GE_Vec3 &i_position );
	void		setPosition2( const GE_Vec3 &i_position );
	void		setRotation( const GE_Vec3 &i_rotation );
	void		setRotation( GE_Mat4x4 &i_rot, GE_Vec3 & i_pivot );
	void		setScale( const GE_Vec3 &i_scale );
	void		setTransform( GE_Mat4x4 *i_matrix );
	GE_Vec3		getPosition();
	GE_Vec3		getRotation();
	GE_Vec3		getScale();
	void		getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList );
	~GEW_Decal( );
};
