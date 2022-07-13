#include "StdAfx.h"
#include "PA_CB.hpp"
#include <stdlib.h>

namespace PA_Communication
{
	CircularBuffer::CircularBuffer(unsigned int size)
	{
		mCBSize = size;
		mCBHead = 0;
		mCBTail = 0;
		mCB = new char[mCBSize];
	}

	CircularBuffer::~CircularBuffer()
	{
		delete mCB;
	}

	void CircularBuffer::Add(const char* msg, unsigned int size)
	{
		for (unsigned int i = 0; i < size; i++)
		{
			mCB[mCBHead++] = msg[i];
			if (mCBHead == mCBSize)
				mCBHead = 0;
		}
	}

	char CircularBuffer::Get()
	{
		char value = mCB[mCBTail++];
		if (mCBTail == mCBSize)
			mCBTail = 0;
		
		return value;
	}

	bool CircularBuffer::IsDataAvailable()
	{
		return (mCBHead != mCBTail);
	}

	unsigned int CircularBuffer::GetDataSize()
	{
		if (mCBHead >= mCBTail)
			return mCBHead - mCBTail;
		else
			return mCBSize - (mCBTail - mCBHead);
	}

	unsigned int CircularBuffer::GetRemainingSize()
	{
		return mCBSize - GetDataSize();
	}
}
