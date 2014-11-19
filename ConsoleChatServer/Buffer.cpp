#include "Buffer.h"
#include <string.h>


Buffer::Buffer(int bufferSize)
{
	m_BufferSize = bufferSize;
	m_UsingBytes = 0;
}

Buffer::~Buffer()
{
}

bool Buffer::write(const char* data, size_t bytes)
{
	EnterCriticalSection(&globalCriticalSection);

	size_t remainBytes = m_BufferSize - m_UsingBytes;

	if (remainBytes >= bytes)
	{
		memcpy_s(m_Buffer + m_UsingBytes, remainBytes, data, bytes);
		m_UsingBytes += bytes;

		return true;
	}

	return false;

	LeaveCriticalSection(&globalCriticalSection);
}

bool Buffer::read(OUT char* destBuf, size_t bytes)
{
	EnterCriticalSection(&globalCriticalSection);

	if (m_UsingBytes >= bytes)
	{
		memcpy_s(destBuf, bytes, m_Buffer, bytes);
		remove(bytes);

		return true;
	}

	return false;

	LeaveCriticalSection(&globalCriticalSection);
}

bool Buffer::peek(OUT char* destBuf, size_t bytes) const
{
	if (m_UsingBytes >= bytes)
	{
		memcpy_s(destBuf, bytes, m_Buffer, bytes);
		return true;
	}

	return false;
}

bool Buffer::remove(size_t bytes)
{
	EnterCriticalSection(&globalCriticalSection);

	if (m_UsingBytes >= bytes)
	{
		memmove_s(m_Buffer, m_BufferSize, m_Buffer + bytes, m_UsingBytes - bytes);
		m_UsingBytes -= bytes;

		return true;
	}

	return false;

	LeaveCriticalSection(&globalCriticalSection);
}

size_t Buffer::getUsingBytes()
{
	return m_UsingBytes;
}
