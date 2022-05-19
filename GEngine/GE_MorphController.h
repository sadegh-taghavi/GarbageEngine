#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"
#include "GE_Model.h"

class GE_MorphController
{
public:
	friend class GE_Model;
private:
	
	float					m_lastFrame;
	std::vector< float >	m_frames;
	std::vector< float >	m_values;
	uint32_t				m_numberOfFrames;
	GE_Model				*m_parent;

	struct MorphData
	{
		char										GMM[ 3 ];
		uint32_t									NumberOfMorphTargets;
		uint32_t									NumberOfFrames;
	};

	struct MorphTargetVertex
	{
		GE_Vec3		Position;
		GE_Vec3		Normal;
		GE_Vec4		Tangent;
	};
	
	struct MorphTarget
	{
		char		Name[ MAX_NAME_LENGTH ];
	};

	vector< std::string > m_morphNames;
		
	void loadFromMem( const uint8_t *i_memory );
public:
	uint32_t getNumberOfFrames();
	void unload();
	GE_MorphController( GE_Model *i_parent );
	void loadAnimation( const std::string i_fileName );
	std::vector< float > *getFrames();
	std::vector< std::string > *getNames();
	void setFrame( float i_frame );
	std::vector< float > *getValues();
	~GE_MorphController(void);
};

