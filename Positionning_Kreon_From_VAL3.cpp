#include "StdAfx.h"
#include "Positionning_Kreon_From_VAL3.hpp"
#include "Eigen/Dense"

#pragma warning(disable:4244)	// disable implicit long double -> float cast warning

namespace PA_Positionning
{
	void tx2ToKreonTransform(float* pose)
	{
		// Kreon homogeneous position in TX2 head referential.
		Eigen::Vector4f kreonHomogeneousPositionRefTx2(70.0, 0, 150.0, 1);

		// Pose must be : { X, Y, Z, RX, RY, RZ }
		float x = pose[0];
		float y = pose[1];
		float z = pose[2];
		float rx = pose[3] * EIGEN_PI / 180.0;
		float ry = pose[4] * EIGEN_PI / 180.0;
		float rz = pose[5] * EIGEN_PI / 180.0;

		// Calculation of rotations matrixes (R = Rx * Ry' * Rz")
		Eigen::Matrix3f rotationXMatrix;
		Eigen::Matrix3f rotationYprimeMatrix;
		Eigen::Matrix3f rotationZsecondMatrix;
		rotationXMatrix <<	1, 0,		0,
							0, cos(rx),	-sin(rx),
							0, sin(rx),	cos(rx);
		
		rotationYprimeMatrix << cos(ry),	0, sin(ry),
								0,			1, 0,
								-sin(ry),	0, cos(ry);

		rotationZsecondMatrix <<	cos(rz),	-sin(rz),	0,
									sin(rz),	cos(rz),	0,
									0,			0,			1;
		Eigen::Matrix3f rotationMatrix = rotationXMatrix * rotationYprimeMatrix * rotationZsecondMatrix;
		
		// Calculation of head to base homogeneous matrix
		Eigen::Matrix4f tx2RefToBaseRefMatrix;
		tx2RefToBaseRefMatrix <<	rotationMatrix(0, 0),	rotationMatrix(0, 1),	rotationMatrix(0, 2),	x,
									rotationMatrix(1, 0),	rotationMatrix(1, 1),	rotationMatrix(1, 2),	y,
									rotationMatrix(2, 0),	rotationMatrix(2, 1),	rotationMatrix(2, 2),	z,
									0,						0,						0,						1;

		// Calculation of kreon homogeneous position in base referential.
		Eigen::Vector4f kreonPositionRefBase = tx2RefToBaseRefMatrix * kreonHomogeneousPositionRefTx2;

		// Retrieve the position vector in the pose vector.
		for (int i = 0; i < 3; i++)
			pose[i] = kreonPositionRefBase[i];
	}

	Eigen::Matrix3f getRotationMatrix(float rx, float ry, float rz)
	{
		Eigen::Matrix3f rotationXMatrix;
		Eigen::Matrix3f rotationYprimeMatrix;
		Eigen::Matrix3f rotationZsecondMatrix;
		rotationXMatrix <<	1, 0,		0,
							0, cos(rx), -sin(rx),
							0, sin(rx), cos(rx);

		rotationYprimeMatrix << cos(ry),	0, sin(ry),
								0,			1, 0,
								-sin(ry),	0, cos(ry);

		rotationZsecondMatrix <<	cos(rz),	-sin(rz),	0,
									sin(rz),	cos(rz),	0,
									0,			0,			1;
		Eigen::Matrix3f rotationMatrix = rotationXMatrix * rotationYprimeMatrix * rotationZsecondMatrix;

		return rotationMatrix;
	}
}