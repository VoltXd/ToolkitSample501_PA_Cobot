#include "StdAfx.h"
#include "PA_ToolBox.hpp"

#include <cmath>
#include <chrono>

#pragma warning(disable:4018)	// disable uint > int comparison warning
#pragma warning(disable:4244)	// disable implicit long double -> float cast warning

namespace PA_ToolBox
{
	float convertBytesToFloat(unsigned char *bytes, unsigned int startIndex)
	{
		char bytesReversed[4];
		for (int i = 0; i < 4; i++)
			bytesReversed[i] = bytes[startIndex + 3 - i];
		float *fPtr = (float*)(bytesReversed);
		float f = *fPtr;
		return f;
	}

	void convertFloatToBytes(float f, unsigned char* bytes, unsigned int startIndex)
	{
		float *fPtr = &f;
		unsigned char *cPtr = (unsigned char *)fPtr;
		for (int i = 0; i < 4; i++)
			bytes[startIndex + i] = cPtr[i];

	}

	float getMean(const float* datas, unsigned int size)
	{
		float mean = 0;

		for (int i = 0; i < size; i++)
			mean += datas[i];
		mean *= 1.0 / size;
		
		return mean;
	}

	float getStandartDeviation(const float* datas, unsigned int size)
	{
		float mean = getMean(datas, size);
		float stDev = 0;
		
		for (int i = 0; i < size; i++)
			stDev += datas[i] * datas[i];
		stDev = sqrt(stDev / size - mean * mean);

		return stDev;
	}

	std::chrono::high_resolution_clock::time_point start;
	void tic()
	{
		start = std::chrono::high_resolution_clock::now();
	}

	std::chrono::high_resolution_clock::time_point stop;
	void toc()
	{
		stop = std::chrono::high_resolution_clock::now();
	}

	float getTicTocDuration()
	{
		float duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() * 0.000000001;
		
		return duration;
	}
}
