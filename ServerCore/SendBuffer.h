#pragma once
class SendBuffer : public enable_shared_from_this<SendBuffer>
{
private:
	vector<BYTE> buffer;
	int writeSize = 0;
public:
	SendBuffer(int size);
	~SendBuffer();
public:
	BYTE* GetBuffer() { return buffer.data(); }
	int WriteSize() const { return writeSize; }
	int Capacity() const { return buffer.size(); }
public:
	bool CopyData(void* data, int len);
};

