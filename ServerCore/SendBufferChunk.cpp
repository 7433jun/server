#include "pch.h"
#include "SendBufferChunk.h"
#include "SendBuffer.h"

void SendBufferChunk::Init()
{
	open = false;
	usedSize = 0;
}

shared_ptr<SendBuffer> SendBufferChunk::Open(int size)
{
	if (size > BUFFER_SIZE)
		return nullptr;

	if (open)
		return nullptr;

	if (size > FreeSize())
		return nullptr;

	open = true;

	return make_shared<SendBuffer>(shared_from_this(), &buffer[usedSize], size);
}

void SendBufferChunk::Close(int size)
{
	if (!open)
		return;

	open = false;

	usedSize += size;
}
