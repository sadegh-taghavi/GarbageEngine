#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"
class GE_PerformanceTimer
{
	long long	m_lastQueryCounter, m_queryFrec;
public:
	void start();
	float stop();
	float reStart();
	float getTime();
	GE_PerformanceTimer();
	~GE_PerformanceTimer();
};

struct _GE_FileChunk
{
	char *Data;
	uint32_t Len;

	_GE_FileChunk();
	~_GE_FileChunk();
	void load( std::string i_fileName );
};

struct GE_SphereVertex
{
	std::vector<GE_Vec3>	Vertices;
	std::vector<uint32_t>	Indices;
	void createVertices( uint32_t i_level, float i_radius = 1.0f, GE_Vec3 i_offset = GE_Vec3( 0.0f, 0.0f, 0.0f ) );
};

struct GE_BoxVertex
{
	std::vector<GE_Vec3>	Vertices;
	std::vector<uint32_t>	Indices;
	void createVertices( GE_Vec3 i_dimension, GE_Vec3 i_offset = GE_Vec3( 0.0f, 0.0f, 0.0f ) );
};

void GE_TessellatePolygonForward( GE_Vec3 *&i_dest, const GE_Vec3 &i_v0, const GE_Vec3 &i_v1, const GE_Vec3 &i_v2, uint32_t i_level, float i_radius );
void GE_TessellatePolygon( GE_Vec3 *&i_dest, const GE_Vec3 &i_v0, const GE_Vec3 &i_v1, const GE_Vec3 &i_v2, uint32_t i_level, float i_radius );