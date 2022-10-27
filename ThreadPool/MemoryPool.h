#pragma once
#include "CircularQueue.h"

class MemoryPool
{
private: // member var
	char*			m_pMemory;
	CircularQueue	m_memoryInfoQueue;
	size_t			m_fixedSize;

public: // default
	MemoryPool(size_t _memorySize, size_t _fixedSize);
	~MemoryPool();

public: // member Func
	void* Allocate();
	void DeAllocate(void* _address);
};
