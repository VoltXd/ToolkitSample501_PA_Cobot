#pragma once

#include "Eigen/Dense"

namespace PA_Positionning
{
	void tx2ToKreonTransform(float* pose);
	Eigen::Matrix3f getRotationMatrix(float rx, float ry, float rz);
}