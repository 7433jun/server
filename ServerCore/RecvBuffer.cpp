#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int size) : bufferSize(size)
{
	capacity = bufferSize * BUFFER_COUNT;
	buffer.resize(capacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clear()
{
	int dataSize = DataSize();
	if (dataSize == 0)
	{
		readPos = 0;
		writePos = 0;
	}
	else
	{
		if (FreeSize() < bufferSize)
		{
			memcpy(&buffer[0], &buffer[readPos], dataSize);
			readPos = 0;
			writePos = dataSize;
		}
	}
}

bool RecvBuffer::OnRead(int numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	readPos += numOfBytes;

	return true;
}

bool RecvBuffer::OnWrite(int numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	writePos += numOfBytes;

	return true;
}
