#pragma once
class matrix4
{
public:
	int val[4][4];	
	//creates a blank 4x4 2Dmatrix as a 2d array

	matrix4(float a11, float a12, float a13, float a14,
			float a21, float a22, float a23, float a24,
			float a31, float a32, float a33, float a34,
			float a41, float a42, float a43, float a44)
		//values for the matrix are taken into the class locally
	{
			val[0][0] = a11, val[0][1] = a12, val[0][2] = a13, val[0][3] = a14,
			val[1][0] = a21, val[1][1] = a22, val[1][2] = a23, val[1][3] = a24,
			val[2][0] = a31, val[2][1] = a32, val[2][2] = a33, val[2][3] = a34,
			val[3][0] = a41, val[3][1] = a42, val[3][2] = a43, val[3][3] = a44;
		//assigns the value of each element in the blank 2D matrix with inputted values
	}
};
