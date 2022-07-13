#pragma once

namespace PA_Enums
{
	enum PositioningDevice
	{
		CobotTx2Touch,
		Ctrack
	};

	enum CobotMessage
	{
		Acquire = 0xFE,
		EndAcquire = 0xEF,
		Position = 0x01
	};
}
