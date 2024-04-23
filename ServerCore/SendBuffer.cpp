#include "pch.h"
#include "SendBuffer.h"
#include "SendBufferChunk.h"

SendBuffer::SendBuffer(shared_ptr<SendBufferChunk> chunk, BYTE* start, int size) : sendBufferChunk(chunk), buffer(start), freeSize(size)
{
}

SendBuffer::~SendBuffer()
{
}

bool SendBuffer::Close(int usedSize)
{
	// 남은 공간이 사용한 것보다 작다면
	if (freeSize < usedSize)
		return false;

	// 사용한 사이즈에 넣어주고
	writeSize = usedSize;

	// 쓴만큼 닫아주고
	sendBufferChunk->Close(usedSize);

	return true;
}