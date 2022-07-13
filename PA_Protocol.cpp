#include "StdAfx.h"
#include "PA_Protocol.hpp"
#include "PA_ToolBox.hpp"

namespace PA_Protocol
{
	enum ReceptionStates
	{
		SOF1,
		SOF2,
		Data
	};

	const unsigned char dataSize = 24;
	float rcvPose[6] = { 0 };
	unsigned char rcvData[dataSize] = { 0 };
	unsigned char rcvDataIndex = 0;
	ReceptionStates rcvState = SOF1;

	bool decodeMessage(unsigned char c)
	{
		bool areDataDecoded = false;
		switch (rcvState)
		{
		case PA_Protocol::SOF1:
			if (c == 0xFE)
				rcvState = SOF2;
			break;

		case PA_Protocol::SOF2:
			if (c == 0x7F)
			{
				rcvState = Data;
				rcvDataIndex = 0;
			}
			else
				rcvState = SOF1;
			break;

		case PA_Protocol::Data:
			rcvData[rcvDataIndex++] = c;
			if (rcvDataIndex == dataSize)
			{
				for (int i = 0; i < 6; i++)
					rcvPose[i] = PA_ToolBox::convertBytesToFloat(rcvData, i * 4);
				areDataDecoded = true;
				rcvState = SOF1;
			}
			break;

		default:
			rcvState = SOF1;
			break;
		}
		
		return areDataDecoded;
	}

	void retrievePose(float* pose)
	{
		for (int i = 0; i < 6; i++)
			pose[i] = rcvPose[i];
	}
}