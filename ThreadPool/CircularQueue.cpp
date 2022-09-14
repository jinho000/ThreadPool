#include "CircularQueue.h"
#include <cassert>

CircularQueue::CircularQueue(size_t _bufferSize)
    : m_ringBuffer()
    , m_front(-1)
    , m_rear(-1)
    , m_bufferSize(_bufferSize)
{
    m_ringBuffer = new void* [_bufferSize];
}

CircularQueue::~CircularQueue()
{
    delete[] m_ringBuffer;
    m_ringBuffer = nullptr;
}

size_t CircularQueue::Size()
{
    return m_front < m_rear ? m_rear - m_front : m_front - m_rear;
}

bool CircularQueue::IsEmpty()
{
    return m_front == -1;
}

bool CircularQueue::IsFull()
{
    if (m_front == 0 && m_rear == m_bufferSize - 1) 
    {
        return true;
    }

    if (m_front == m_rear + 1) 
    {
        return true;
    }

    return false;
}

void CircularQueue::Enqueue(void* _memoryAddress)
{
    assert(IsFull() == false);

    if (m_front == -1)
    {
        m_front = 0;
    }

    m_rear = (m_rear + 1) % m_bufferSize;
    m_ringBuffer[m_rear] = _memoryAddress;    
}


void CircularQueue::Dequeue()
{
    assert(IsEmpty() == false);

    if (m_front == m_rear) 
    {
        m_front = -1;
        m_rear = -1;
    }
    else
    {
        m_front = (m_front + 1) % m_bufferSize;
    }
}

void* CircularQueue::Front()
{
    return m_ringBuffer[m_front];
}

//int CircularQueue::Rear()
//{
//    return m_ringBuffer[((m_rear + m_bufferSize) - 1) % m_bufferSize];
//}

