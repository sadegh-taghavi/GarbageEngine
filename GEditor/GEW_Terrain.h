#pragma once
#include <stdint.h>
#include <GE_Math.h>
#include <GE_Prerequisites.h>
#include <QFile>
#include <QVector>
#include <GE_View.h>
#include <GE_Texture.h>
#include "GEW_Input.h"
#include "GEW_Physics.h"

#define MAX_FILE_NAME 512

class GE_Texture;
class GEW_Light;

class GEW_Terrain
{
	GE_Vec2 m_lastHit;
	static GEW_Terrain m_singleton;
	GEW_Terrain();
	~GEW_Terrain();

	physx::PxHeightFieldGeometry m_heightFieldGeometry;
	physx::PxRigidStatic		 *m_heightFieldActor;

	ID3D11VertexShader *m_vS;
	ID3D11PixelShader *m_pS_HeightCopy;
	ID3D11PixelShader *m_pS_HeightSculptPositive;
	ID3D11PixelShader *m_pS_HeightSculptNegative;
	ID3D11PixelShader *m_pS_HeightSculptPositiveMask;
	ID3D11PixelShader *m_pS_HeightSculptNegativeMask;
	ID3D11PixelShader *m_pS_HeightSculptSmooth;
	ID3D11PixelShader *m_pS_HeightSculptSmoothMask;
	ID3D11PixelShader *m_pS_HeightSculptFlat;

	ID3D11PixelShader *m_pS_MaskCopy;
	ID3D11PixelShader *m_pS_MaskSculptPositive;
	ID3D11PixelShader *m_pS_MaskSculptNegative;
	ID3D11PixelShader *m_pS_MaskSculptPositiveMask;
	ID3D11PixelShader *m_pS_MaskSculptNegativeMask;
	ID3D11PixelShader *m_pS_MaskSculptSmooth;
	ID3D11PixelShader *m_pS_MaskSculptSmoothMask;
	ID3D11PixelShader *m_pS_MaskSculptFlat;

	ID3D11PixelShader *m_pS_BrushDecal;
	ID3D11Buffer	  *m_perFrameBrushDecal;
	ID3D11Buffer	  *m_perFrameBrushHeight;
	struct PerFrameBrushDecal
	{
		float	DecalFalloff;
		float	DecalHardness;
		float	DecalPadding0;
		float	DecalPadding1;
	}m_cPerFrameBrushDecal;

	struct PerFrameBrushHeight
	{
		float		__Padding;
		float		__Padding1;
		float		__Padding2;
		int			Channel;
		float		BrushSize;
		float		BrushFalloff;
		float		BrushHardness;
		float		BrushVolume;
		float		BrushFieldSize;
		float		BrushMaskRotation;
		GE_Vec2		BrushHitXY;
		GE_Vec2		BrushMaskPos;
		GE_Vec2		BrushMaskTile;
	}m_cPerFrameBrushHeight;
public:
	class GE_RenderTarget *m_heightMapRT[2];
	class GE_RenderTarget *m_maskMapRT[2];
	class GE_Texture	  *m_heightMaskMap;
	class GE_RenderTarget *m_brushDecalMapRT;
	class GE_RenderUtilityObject *m_horizonRUO;
	class GE_RenderUtilityObject *m_brushRUO;
	class GE_Decal *m_brushDecal;
	void reCreateHeightField();
	struct UndoParams
	{
		UndoParams();
		~UndoParams();
		int32_t						HeightMapMaxUndoSize;
		vector<GE_MemoryTexture>	HeightMapMemory;
		int32_t						HeightMapSaveIndex;
		int32_t						HeightMapUndoCounter;
		bool						HeightMapCanRedo;
		bool						HeightMapAllowSave;
		int32_t						MaskMapMaxUndoSize;
		vector<GE_MemoryTexture>	MaskMapMemory;
		int32_t						MaskMapSaveIndex;
		int32_t						MaskMapUndoCounter;
		bool						MaskMapCanRedo;
		bool						MaskMapAllowSave;
	}m_undoParams;
	physx::PxHeightFieldGeometry &getHeightFieldGeometry();
	void initHeightMapUndoMemory();
	void undoHeightMapSave();
	void undoHeightMap();
	void redoHeightMap();
	void initMaskMapUndoMemory();
	void undoMaskMapSave();
	void undoMaskMap();
	void redoMaskMap();
	void saveBrushPresets(QString i_filename);
	bool loadBrushPresets( QString i_filename );
	void updateBrushMap();
	void updateBrushParameters(const GE_Vec2 &i_position);
	void setHeightMap();
	void setMaskMap();
	void setSculptEnableChange();
	void createHeightMap(QString i_filename, uint32_t i_size, bool i_copyFromLast, bool i_justCopy = false);
	void createMaskMap(QString i_filename, uint32_t i_size, bool i_copyFromLast, bool i_justCopy = false);
	void resetAll();
	void init();
	struct PhysxData
	{
		bool VisualizeHeightfield;
		uint32_t QuadDensity;
		PhysxData( );
		void set();
	}m_physxData;
	struct BaseData
	{
		bool Enable;
		float Size;
		uint32_t NumberOfChunks;
		uint32_t QuadDensity;
		BaseData();
		void set();
	} m_baseData;
	struct BaseParam
	{
		float HeightBias;
		float MaxHeight;
		float NormalAspect;
		float LodMultiplier;
		GE_Vec2	NormalNoiseUVTile;
		char  HeightMapFileName[MAX_FILE_NAME];
		char  MaskMapFileName[MAX_FILE_NAME];
		void set();
		BaseParam();
	}m_baseParam;
	enum SculptType
	{
		POSITIVE,
		NEGATIVE,
		SMOOTH,
		FLAT,
	};
	struct SculptData
	{
		bool Enable;
		SculptData();
	}m_sculptData;
	struct PresetsSaveData
	{
		char	 GEB[3];
		uint32_t NumberOfPresets;
		PresetsSaveData();
		~PresetsSaveData();
	};
	struct SculptParam
	{
		char		Name[MAX_FILE_NAME];
		int			Type;
		int			Mode;
		uint32_t	ArrayIndex;
		float		Size;
		float		Volume;
		float		ReapeatDistance;
		float		Falloff;
		float		Hardness;
		bool		MaskEnable;
		GE_Vec2		MaskPositionUV;
		GE_Vec2		MaskTileUV;
		bool		Randomize;
		float		Angle;
		char		MaskFileName[MAX_FILE_NAME];
		SculptParam();
	}m_sculptCurrentParam;
	QVector<QString> m_splattingListNames;
	QVector<SculptParam> m_sculptPresets;
	void getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList);
	void getDecalObjects(std::list<class GE_Decal*> &i_outList);
	void update(GEW_Input::Data &i_inputData, const GE_View &i_view, const GE_Vec2 &i_cursorPos, const GE_Vec2 &i_widthHeight);
	static GEW_Terrain *getSingleton(){ return &m_singleton; }
	
};

