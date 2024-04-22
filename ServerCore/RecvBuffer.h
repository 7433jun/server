#pragma once
class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };
private:
	// bufferSize * BUFFER_COUNT
	int capacity = 0;
	int bufferSize = 0;
	int readPos = 0;
	int writePos = 0;
	vector<BYTE> buffer;
public:
	RecvBuffer(int size);
	~RecvBuffer();
public:
	BYTE* ReadPos() { return &buffer[readPos]; }
	BYTE* WritePos() { return &buffer[writePos]; }
	int DataSize() const { return writePos - readPos; }
	int FreeSize() const { return capacity - writePos; }
public:
	void Clear();
	bool OnRead(int numOfBytes);
	bool OnWrite(int numOfBytes);
};

