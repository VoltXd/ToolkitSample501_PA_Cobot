#pragma once

namespace PA_ToolBox
{
	float convertBytesToFloat(unsigned char *bytes, unsigned int startIndex);
	void convertFloatToBytes(float f, unsigned char *bytes, unsigned int startIndex);
	float getMean(const float* datas, unsigned int size);
	float getStandartDeviation(const float* datas, unsigned int size);
	void tic();
	void toc();
	float getTicTocDuration();
}