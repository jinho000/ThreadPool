#pragma once

// 용도 :
// 분류 :
// 첨언 :
class CircularQueue
{
private: // member var
	enum { SIZE = 1024 };

	int		m_ringBuffer[SIZE];
	int		m_front;
	int		m_rear;

public: // default
	CircularQueue();
	~CircularQueue();

public: 
	int Size();

	bool IsEmpty();
	bool IsFull();

	void Enqueue(int _value);
	void Dequeue();

	int Front();
	int Rear();
};

