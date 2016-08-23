struct zMAT3 {
	float m0[3];
	float m1[3];
	float m2[3];
};

zMAT3 Alg_Identity2d()
{
	return {{1,0,0}, {0,1,0}, {0,0,1}};
}
