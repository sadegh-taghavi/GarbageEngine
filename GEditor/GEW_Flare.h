#pragma once
#include "GEW_Item.h"
#include <QString>
#include <QList>
#include <GE_Math.h>
#include <GE_Lens.h>

class GE_RenderUtilityObject;

class GEW_Flare : public GEW_Item
{
public:
	static QIcon				m_iconFlare;
	static QList< GEW_Flare* > 	m_list;
	static GEW_Flare			*find( QString &i_name );
	static GEW_Flare			*CreateFromFile( GEW_Item *i_parent, QFile &i_file, const QString &i_resourcePath );

	struct SaveData
	{
		char		Name[ 64 ];
		bool		Enable;
		GE_Vec3		BoundColor;
		GE_Vec3		LightPosition;
		uint32_t	VisiblePixelOffset;
		float		VisibleAngleOffset;
		GE_Vec3		OccluderPositionOffset;
		GE_Quat		OccluderRotation;
		GE_Vec3		OccluderSize;
		char		Map[ 512 ];
		uint32_t	FlareCount;
		SaveData();
	};

	struct FlareSaveData
	{
		char					Name[ 64 ];
		GE_LensFlare::Parameter Parameter;
	};

	GE_LensFlare				*m_flare;
	GE_RenderUtilityObject		*m_occluderObject;
	QList< QString >			m_flareNames;
	bool m_enable;
	GE_Vec3 m_boundColor;
	GEW_Flare( const QString &i_name, GEW_Item *i_parent );

	void		update();
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
	~GEW_Flare();
};

