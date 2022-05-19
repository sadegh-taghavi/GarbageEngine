#pragma once
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>

DWORD WINAPI _StreamMapThreadFunction( LPVOID lpParam );

class GE_StreamMap
{
public:
	struct Level
	{
		std::vector< int32_t >	Chunks;
		std::vector< char * >	Memory;
		std::vector< bool >		Ready;
	};
	std::vector< FILE * >	m_files;
	std::vector< Level >	m_levels;
	std::vector< uint32_t >	m_levelChunksDescriptor;
	bool m_run;
	DWORD m_threadId;
	HANDLE m_threadHandel;

	struct Parameters
	{
		uint32_t Size;
		uint32_t Level;
		uint32_t MapDepth;
		uint32_t NumberOfChunk;
		uint32_t FixBytes;
		uint32_t UserData;
		bool     Mips;
	} m_params;
	bool loadParams(const char * i_fileName );
	void create(const char * i_fileName, const Parameters &i_params,const std::vector< uint32_t > &i_levelChunksDescriptor);
	bool open(const char * i_fileName, const std::vector< uint32_t > &i_levelChunksDescriptor );
	void write( int32_t i_level, uint32_t i_chunk );
	void read( int32_t i_level, int32_t i_chunkIndex );
	void close();
	uint32_t getChunkSize( uint32_t i_level );
	int32_t getChunkIndex(uint32_t i_level, uint32_t i_chunk);
	GE_StreamMap();
	~GE_StreamMap();
};

