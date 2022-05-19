#pragma once
#include "GEW_Item.h"
#include <QString>
#include <QList>
#include <GE_Math.h>
#include <GE_Light.h>

//class GE_Light;
class GE_RenderUtilityObject;
class GE_Texture;


class GEW_Light : public GEW_Item
{
public:
	static QIcon				m_iconDirectional;
	static QIcon				m_iconPoint;
	static QIcon				m_iconSpot;
	static QList< GEW_Light* >	m_list;
	static GEW_Light			*find( QString &i_name );
	static GEW_Light			*find( uint32_t i_id );
	static GEW_Light			*CreateFromFile( GEW_Item *i_parent, QFile &i_file, const QString &i_resourcePath );

	GE_Light					*m_light;
	float						m_offDistance;
	float						m_shadowDistance;
	struct Probe
	{
		GE_RenderUtilityObject *Cage;
		GE_RenderUtilityObject *Target;
		GE_RenderUtilityObject *Billboard;
		GE_RenderUtilityObject *Spot;
		GE_RenderUtilityObject *Line;
		GE_RenderUtilityObject *Cross;
		GE_Texture *TexturePoint;
		GE_Texture *TextureSpot;
		GE_Texture *TextureDir;
		float LastSpotSpread;
		float LastRange;
		GE_Vec3	BoundColor;
		bool Visible;
		GE_Vec3 LastDir;
		Probe();
		~Probe( );
		void update( class GE_Light *i_light );
	} m_probe;

	struct SaveData
	{
		char	Name[ 64 ];
		float	OffDistance;
		float	ShadowDistance;
		GE_Vec3	ProbeBoundColor;
		GE_Light::Parameters Parameters;
	};

	GEW_Light( const QString &i_name, GEW_Item *i_parent );
	~GEW_Light();

	void setProbeXrayMode( bool i_xRayMode );
	bool isProbeXRayMode();
	void setProbeVisible( bool i_visible );
	bool isProbeVisible();
	void setType( GE_Light::Type i_type );
	int	 getType();
	
	void		update();
	void		select();
	void		deSelect();
	void		checkStateChanged( GEW_Item::CheckIndex i_column );
	void		parentCheckStateChanged( GEW_Item::CheckIndex i_column, Qt::CheckState i_checkState );
	void		saveToFile( QFile &i_file, const QString &i_resourcePath );
	void		setPosition( const GE_Vec3 &i_position );
	void		setPosition2( const GE_Vec3 &i_position );
	void		setRotation( const GE_Quat &i_rotation );
	void		setRotation( GE_Mat4x4 &i_rot, GE_Vec3 & i_pivot );
	void		setScale( const GE_Vec3 &i_scale );
	void		setTransform( GE_Mat4x4 *i_matrix );
	GE_Vec3		getPosition();
	GE_Vec3		getRotation();
	GE_Vec3		getScale();
	void		getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList );


};