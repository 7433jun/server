#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(int size)
{
	// 버퍼 크기 할당
	buffer.resize(size);
}

SendBuffer::~SendBuffer()
{
}

bool SendBuffer::CopyData(void* data, int len)
{
	// 복사할 데이터의 길이가 용량보다 크면
	if (Capacity() < len)
		return false;
	
	// 데이터 복사
	memcpy(buffer.data(), data, len);
	writeSize = len;
	return true;
}
