#pragma once

namespace PA_Communication
{
	class CircularBuffer
	{
	public:
		CircularBuffer(unsigned int size);
		~CircularBuffer();

		void Add(const char* msg, unsigned int size);
		char Get();
		bool IsDataAvailable();
		unsigned int GetRemainingSize();
		unsigned int GetDataSize();

	private:
		char* mCB;
		unsigned int mCBTail;
		unsigned int mCBHead;
		unsigned int mCBSize;
	};
}