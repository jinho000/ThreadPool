#include "CircularQueue.h"
#include <cassert>

CircularQueue::CircularQueue()
    : m_ringBuffer()
    , m_front(0)
    , m_rear(0)
{
    //m_ringBuffer = new int[_bufferSize];
}

CircularQueue::~CircularQueue()
{
    //delete m_ringBuffer;
}

int CircularQueue::Size()
{
    return m_front < m_rear ? m_rear - m_front : m_front - m_rear;
}

bool CircularQueue::IsEmpty()
{
    return m_front == m_rear;
}

bool CircularQueue::IsFull()
{
    return (m_rear + 1) % SIZE == m_front;
}

void CircularQueue::Enqueue(int _value)
{
    assert(IsFull() == false);

    m_ringBuffer[m_rear] = _value;
    m_rear = (m_rear + 1) % SIZE;
}

void CircularQueue::Dequeue()
{
    assert(IsEmpty() == false);
    m_front = (m_front + 1) % SIZE;
}

int CircularQueue::Front()
{
    return m_ringBuffer[m_front];
}

int CircularQueue::Rear()
{
    return m_ringBuffer[((m_rear + SIZE) - 1) % SIZE];
}
