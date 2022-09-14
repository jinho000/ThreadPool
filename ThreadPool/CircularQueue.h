#pragma once


class CircularQueue
{
private: // member var
	void**	m_ringBuffer;
	int		m_front;
	int		m_rear;
	size_t	m_bufferSize;

public: // default
	CircularQueue(size_t _bufferSize);
	~CircularQueue();

public: 
	size_t Size();

	bool IsEmpty();
	bool IsFull();

	void Enqueue(void* _memoryAddress);
	void Dequeue();

	void* Front();
};

