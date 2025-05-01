#pragma once
static float genVoxel(x,y,z)
{
	float v1[3] = { x,y,z };
	float v2[3] = { x,y,z+1}
	float v3[3] = { x,y+1,z};

	float vertices[sizeof(v1 + v2 + v3)] = v1 + v2 + v3;
	return vertices

}