
struct vertex3d {
	float x, y, z;
};

struct tri {
	vertex3d a, b, c;
};

vertex3d v1 = { 1.0f, 1.0f, 1.0f };
vertex3d v2 = { -1.0f, 0.0f, -1.0f };
vertex3d v3 = { 1.0f, -1.0f, 1.0f };

tri triangle = { v1, v2, v3 };

struct matrix3 {
	
};

