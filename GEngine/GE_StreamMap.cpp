#include "GE_StreamMap.h"
#include <string>
using namespace std;

DWORD WINAPI _StreamMapThreadFunction(LPVOID lpParam)
{
	GE_StreamMap *map = (GE_StreamMap*)lpParam;
	while (map->m_run)
	{
		for (uint32_t i = 0; i < map->m_params.Level; ++i)
		{
			for (uint32_t chId = 0; chId < map->m_levels[i].Ready.size(); ++chId)
			{
				if (map->m_levels[i].Ready[chId] == false && map->m_levels[i].Chunks[chId] > -1)
				{
					uint32_t size = map->getChunkSize(i);
					uint32_t offset = size * map->m_levels[i].Chunks[chId];
					fseek(map->m_files[i], offset, 0);
					fread_s(map->m_levels[i].Memory[chId], size, size, 1, map->m_files[i]);
					map->m_levels[i].Ready[chId] = true;
				}
			}
		}
	}
	return 0;
}

GE_StreamMap::GE_StreamMap()
{
	m_threadHandel = NULL;
	m_run = false;
}

GE_StreamMap::~GE_StreamMap()
{
	close();
}

string getFileName(const char * i_fileName, int32_t i_index)
{
	char id[10];
	memset(id, 0, 10);
	_itoa_s(i_index, id, 10);
	string str = i_fileName;
	str += id;
	return str;
}

void GE_StreamMap::create(const char * i_fileName, const Parameters &i_params, const vector< uint32_t > &i_levelChunksDescriptor)
{
	close();
	m_params = i_params;
	m_levelChunksDescriptor = i_levelChunksDescriptor;
	FILE *fp;
	fopen_s(&fp, i_fileName, "wb");
	fwrite((const void *)&m_params, sizeof(Parameters), 1, fp);
	fclose(fp);

	m_files.resize(m_params.Level);
	m_levels.resize(m_params.Level);
	string fileName = i_fileName;
	fileName.substr(0, fileName.length() - 4);

	for (uint32_t i = 0; i < m_params.Level; ++i)
	{
		uint32_t size = m_levelChunksDescriptor[i];
		m_levels[i].Memory.resize(size);
		m_levels[i].Chunks.resize(size);
		m_levels[i].Ready.resize(size);

		fopen_s(&m_files[i], getFileName(fileName.c_str(), i).c_str(), "w+b");
		for (uint32_t chID = 0; chID < size; ++chID)
		{
			m_levels[i].Memory[chID] = new char[getChunkSize(i)];
			memset(m_levels[i].Memory[chID], 0, getChunkSize(i));
			m_levels[i].Chunks[chID] = -1;
			m_levels[i].Ready[chID] = true;
		}
	}
	
	m_run = true;
	m_threadHandel = CreateThread(
		NULL,					// default security attributes
		0,						// use default stack size  
		_StreamMapThreadFunction,// thread function name
		(LPVOID)this,			// argument to thread function 
		0,						// use default creation flags 
		&m_threadId);			// returns the thread identifier 
}

bool GE_StreamMap::open(const char * i_fileName, const vector< uint32_t > &i_levelChunksDescriptor)
{
	close();
	FILE *fp;
	fopen_s(&fp, i_fileName, "rb");
	if (!fp)
		return false;
	fread_s((void *)&m_params, sizeof(Parameters), sizeof(Parameters), 1, fp);
	m_levelChunksDescriptor = i_levelChunksDescriptor;
	fclose(fp);

	m_files.resize(m_params.Level);
	m_levels.resize(m_params.Level);

	string fileName = i_fileName;
	fileName.substr(0, fileName.length() - 4);

	for (uint32_t i = 0; i < m_params.Level; ++i)
	{
		uint32_t size = m_levelChunksDescriptor[i];
		m_levels[i].Memory.resize(size);
		m_levels[i].Chunks.resize(size);
		m_levels[i].Ready.resize(size);

		fopen_s(&m_files[i], getFileName(fileName.c_str(), i).c_str(), "r+b");
		for (uint32_t chID = 0; chID < size; ++chID)
		{
			m_levels[i].Memory[chID] = new char[getChunkSize(i)];
			memset(m_levels[i].Memory[chID], 0, getChunkSize(i));
			m_levels[i].Chunks[chID] = -1;
			m_levels[i].Ready[chID] = true;
		}
	}

	m_run = true;
	m_threadHandel = CreateThread(
		NULL,					// default security attributes
		0,						// use default stack size  
		_StreamMapThreadFunction,// thread function name
		(LPVOID)this,			// argument to thread function 
		0,						// use default creation flags 
		&m_threadId);			// returns the thread identifier 
	return true;
}

void GE_StreamMap::write(int32_t i_level, uint32_t i_chunk)
{
	if (i_level < 0)
	{
		for (uint32_t i = 0; i < m_params.Level; ++i)
		{
			uint32_t size = getChunkSize(i);
			uint32_t offset = i_chunk * size;
			while (m_levels[i].Ready[0] == false);
			fseek(m_files[i], offset, 0);
			fwrite(m_levels[i].Memory[0], size, 1, m_files[i]);
			fflush(m_files[i]);
			m_levels[i].Chunks[0] = i_chunk;
		}
	}
	else
	{
		uint32_t size = getChunkSize(i_level);
		uint32_t offset = i_chunk * size;
		while (m_levels[i_level].Ready[0] == false);
		fseek(m_files[i_level], offset, 0);
		fwrite(m_levels[i_level].Memory[0], size, 1, m_files[i_level]);
		fflush(m_files[i_level]);
		m_levels[i_level].Chunks[0] = i_chunk;
	}
}

void GE_StreamMap::read(int32_t i_level, int32_t i_chunkIndex)
{
	if (i_level == -1)
	{
		for (uint32_t i = 0; i < m_params.Level; ++i)
		{
			if (i_chunkIndex < 0)
			{
				for (uint32_t chId = 0; chId < m_levelChunksDescriptor[i]; ++chId)
					m_levels[i].Ready[chId] = false;
			}
			else
				m_levels[i].Ready[i_chunkIndex] = false;
		}		
	}
	else
	{
		if (i_chunkIndex < 0)
		{
			for (uint32_t chId = 0; chId < m_levelChunksDescriptor[i_level]; ++chId)
				m_levels[i_level].Ready[chId] = false;
		}
		else
			m_levels[i_level].Ready[i_chunkIndex] = false;
	}
}

void GE_StreamMap::close()
{
	if (!m_files.size())
		return;
	m_run = false;
	WaitForMultipleObjects(1, &m_threadHandel, TRUE, INFINITE);
	CloseHandle(m_threadHandel);
	for (int i = 0; i < m_files.size(); ++i)
	{
		fclose(m_files[i]);
		for (uint32_t chID = 0; chID < m_levels[i].Memory.size(); ++chID)
			delete[] m_levels[i].Memory[chID];
	}
	m_files.clear();
	m_levels.clear();
}

uint32_t GE_StreamMap::getChunkSize(uint32_t i_level)
{
	uint32_t size = (m_params.Size * m_params.Size * m_params.MapDepth) / (uint32_t)pow(2.0f, (int)i_level);
	if (m_params.Mips)
		size += (uint32_t)((float)size / 3.0f);
	return size + m_params.FixBytes;
}

int32_t GE_StreamMap::getChunkIndex(uint32_t i_level, uint32_t i_chunk)
{
	for (uint32_t i = 0; i < m_levels[i_level].Chunks.size(); ++i)
	{
		if (m_levels[i_level].Chunks[i] == (int32_t)i_chunk)
			return i;
	}
	return -1;
}

bool GE_StreamMap::loadParams(const char * i_fileName)
{
	FILE *fp;
	fopen_s(&fp, i_fileName, "rb");
	if (!fp)
		return false;
	fread_s((void *)&m_params, sizeof(Parameters), sizeof(Parameters), 1, fp);
	fclose(fp);
	return true;
}
