
//패킷 데이터 저장을 담당하는 버퍼 클래스.

#ifndef __BUFFER_H_
#define __BUFFER_H_

#include "TypeDefine.h"

class Buffer
{
public:
	Buffer(int bufferSize);
	~Buffer();
	bool write(const char* data, size_t bytes);
	bool read(OUT char* destBuf, size_t bytes);
	bool peek(OUT char* destBuf, size_t bytes) const;
	bool remove(size_t bytes);
	size_t getUsingBytes();

private:
	size_t m_BufferSize;
	size_t m_UsingBytes;
	char* m_Buffer;
};

#endif