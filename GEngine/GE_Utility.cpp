#include "GE_Utility.h"
#include "GE_Vec3.h"
#include "GE_Core.h"

void GE_PerformanceTimer::start()
{
	LARGE_INTEGER qf;
	QueryPerformanceCounter( &qf );
	m_lastQueryCounter = qf.QuadPart;
}
float GE_PerformanceTimer::stop()
{
	LARGE_INTEGER qf;
	QueryPerformanceCounter( &qf );
	m_lastQueryCounter = qf.QuadPart - m_lastQueryCounter;
	return ( float ) ( ( double ) m_lastQueryCounter / ( double ) m_queryFrec );
}
float GE_PerformanceTimer::reStart()
{
	float t = stop();
	start();
	return t;
}

float GE_PerformanceTimer::getTime()
{
	LARGE_INTEGER qf;
	QueryPerformanceCounter( &qf );
	m_lastQueryCounter = qf.QuadPart - m_lastQueryCounter;
	return ( float ) ( ( double ) m_lastQueryCounter / ( double ) m_queryFrec );
}

GE_PerformanceTimer::GE_PerformanceTimer()
{
	LARGE_INTEGER qf;
	QueryPerformanceFrequency( &qf );
	m_queryFrec = qf.QuadPart;
}
GE_PerformanceTimer::~GE_PerformanceTimer()
{

}

_GE_FileChunk::_GE_FileChunk()
{
	Data = NULL;
	Len = 0;
}

_GE_FileChunk::~_GE_FileChunk()
{
	SAFE_DELETE_ARRAY( Data );
	Len = 0;
}

void _GE_FileChunk::load( std::string i_fileName )
{
	FILE *fp;
	if( Len )
		SAFE_DELETE_ARRAY( Data );
	fopen_s( &fp, i_fileName.c_str(), "rb" );
	fseek( fp, 0, SEEK_END );
	Len = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	Data = new char[ Len ];
	fread_s( Data, Len, Len, 1, fp );
	fclose( fp );
}

#define TESSELLATE_POLYCOUNT(Level) (3 * (1 << (2 * Level)))
//Your pointer TESSELLATE_POLYCOUNT(i_level) times going forward!

void GE_SphereVertex::createVertices( uint32_t i_level, float i_radius, GE_Vec3 i_offset )
{
	vector<GE_Vec3> tempVert;
	Vertices.clear();
	Indices.clear();
	tempVert.resize( TESSELLATE_POLYCOUNT( i_level ) * 8 );

	GE_Vec3 px0( 1.0f, 0, 0 );
	GE_Vec3 px1( -1.0f, 0, 0 );
	GE_Vec3 py0( 0, 1.0f, 0 );
	GE_Vec3 py1( 0, -1.0f, 0 );
	GE_Vec3 pz0( 0, 0, 1.0f );
	GE_Vec3 pz1( 0, 0, -1.0f );
	px0 += i_offset;
	px1 += i_offset;
	py0 += i_offset;
	py1 += i_offset;
	pz0 += i_offset;
	pz1 += i_offset;

	GE_Vec3 *dest = &tempVert.at( 0 );
	GE_TessellatePolygonForward( dest, py0, px0, pz0, i_level, i_radius );
	GE_TessellatePolygonForward( dest, py0, pz0, px1, i_level, i_radius );
	GE_TessellatePolygonForward( dest, py0, px1, pz1, i_level, i_radius );
	GE_TessellatePolygonForward( dest, py0, pz1, px0, i_level, i_radius );
	GE_TessellatePolygonForward( dest, py1, pz0, px0, i_level, i_radius );
	GE_TessellatePolygonForward( dest, py1, px0, pz1, i_level, i_radius );
	GE_TessellatePolygonForward( dest, py1, pz1, px1, i_level, i_radius );
	GE_TessellatePolygonForward( dest, py1, px1, pz0, i_level, i_radius );

	int32_t index;
	for( uint32_t i = 0; i < tempVert.size(); i++ )
	{
		index = -1;
		for( uint32_t j = 0; j < Vertices.size(); j++ )
		{
			if( Vertices.at( j ) == tempVert.at( i ) )
			{
				index = j;
				break;
			}
		}

		if( index == -1 )
		{
			Vertices.push_back( tempVert.at( i ) );
			Indices.push_back( ( uint32_t ) Vertices.size() - 1 );
		} else
		{
			Indices.push_back( index );
		}
	}
	tempVert.clear();
}

void GE_TessellatePolygonForward( GE_Vec3 *&i_dest, const GE_Vec3 &i_v0, const GE_Vec3 &i_v1, const GE_Vec3 &i_v2, uint32_t i_level, float i_radius )
{
	if( i_level )
	{
		i_level--;
		GE_Vec3 v3, v4, v5;
		v3.normalizeBy( &( ( GE_Vec3 ) i_v0 + i_v1 ) );
		v4.normalizeBy( &( ( GE_Vec3 ) i_v1 + i_v2 ) );
		v5.normalizeBy( &( ( GE_Vec3 ) i_v2 + i_v0 ) );

		GE_TessellatePolygonForward( i_dest, i_v0, v3, v5, i_level, i_radius );
		GE_TessellatePolygonForward( i_dest, v3, v4, v5, i_level, i_radius );
		GE_TessellatePolygonForward( i_dest, v3, i_v1, v4, i_level, i_radius );
		GE_TessellatePolygonForward( i_dest, v5, v4, i_v2, i_level, i_radius );
	} else
	{
		*i_dest++ = ( GE_Vec3 ) i_v0 * i_radius;
		*i_dest++ = ( GE_Vec3 ) i_v1 * i_radius;
		*i_dest++ = ( GE_Vec3 ) i_v2 * i_radius;
	}
}

void GE_TessellatePolygon( GE_Vec3 *&i_dest, const GE_Vec3 &i_v0, const GE_Vec3 &i_v1, const GE_Vec3 &i_v2, uint32_t i_level, float i_radius )
{
	GE_Vec3 *ptr = i_dest;
	GE_TessellatePolygonForward( ptr, i_v0, i_v1, i_v2, i_level, i_radius );
}

void GE_BoxVertex::createVertices( GE_Vec3 i_dimension, GE_Vec3 i_offset )
{
	GE_Vec3 maxP, minP;
	maxP = i_dimension / 2.0f;
	minP = -i_dimension / 2.0f;

	Vertices.resize( 8 );

	Vertices.data()[ 0 ] = minP + i_offset;
	Vertices.data()[ 1 ] = GE_Vec3( maxP.x, minP.y, minP.z ) + i_offset;
	Vertices.data()[ 2 ] = GE_Vec3( minP.x, minP.y, maxP.z ) + i_offset;
	Vertices.data()[ 3 ] = GE_Vec3( maxP.x, minP.y, maxP.z ) + i_offset;
	Vertices.data()[ 4 ] = GE_Vec3( minP.x, maxP.y, minP.z ) + i_offset;
	Vertices.data()[ 5 ] = GE_Vec3( maxP.x, maxP.y, minP.z ) + i_offset;
	Vertices.data()[ 6 ] = GE_Vec3( minP.x, maxP.y, maxP.z ) + i_offset;
	Vertices.data()[ 7 ] = maxP + i_offset;

	Indices.resize( 36 );

	Indices.data()[ 0 ] = 0; Indices.data()[ 1 ] = 2; Indices.data()[ 2 ] = 1;

	Indices.data()[ 3 ] = 1; Indices.data()[ 4 ] = 2; Indices.data()[ 5 ] = 3;

	Indices.data()[ 6 ] = 1; Indices.data()[ 7 ] = 3; Indices.data()[ 8 ] = 5;

	Indices.data()[ 9 ] = 5; Indices.data()[ 10 ] = 3; Indices.data()[ 11 ] = 7;

	Indices.data()[ 12 ] = 5; Indices.data()[ 13 ] = 7; Indices.data()[ 14 ] = 4;

	Indices.data()[ 15 ] = 4; Indices.data()[ 16 ] = 7; Indices.data()[ 17 ] = 6;

	Indices.data()[ 18 ] = 4; Indices.data()[ 19 ] = 6; Indices.data()[ 20 ] = 0;

	Indices.data()[ 21 ] = 0; Indices.data()[ 22 ] = 6; Indices.data()[ 23 ] = 2;

	Indices.data()[ 24 ] = 2; Indices.data()[ 25 ] = 6; Indices.data()[ 26 ] = 3;

	Indices.data()[ 27 ] = 3; Indices.data()[ 28 ] = 6; Indices.data()[ 29 ] = 7;

	Indices.data()[ 30 ] = 0; Indices.data()[ 31 ] = 1; Indices.data()[ 32 ] = 4;

	Indices.data()[ 33 ] = 4; Indices.data()[ 34 ] = 1; Indices.data()[ 35 ] = 5;

}
