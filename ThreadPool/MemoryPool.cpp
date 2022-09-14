#include "MemoryPool.h"

MemoryPool::MemoryPool(size_t _memorySize, size_t _fixedSize)
	: m_memoryInfoQueue(_memorySize / _fixedSize)
	, m_fixedSize(_fixedSize)
{
	m_pMemory = new char[_memorySize];

	size_t memoryCount = _memorySize / _fixedSize;
	for (int i = 0; i < memoryCount; ++i)
	{
		void* pAddress = reinterpret_cast<void*>(m_pMemory + (i * _fixedSize));
		m_memoryInfoQueue.Enqueue(pAddress);
	}
}

MemoryPool::~MemoryPool()
{
	delete[] m_pMemory;
}

void* MemoryPool::Allocate()
{
	void* pAddress = m_memoryInfoQueue.Front();
	m_memoryInfoQueue.Dequeue();
	return pAddress;
}

void MemoryPool::DeAllocate(void* _address)
{
	m_memoryInfoQueue.Enqueue(_address);
}

