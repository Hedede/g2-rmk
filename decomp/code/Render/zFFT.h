// This one is clearly made by other developers, as it doesn't use
// zERROR (uses stdout instead) and other stuff made by original three
class zCFFT {
	virtual ~zCFFT();

	void SetSpeed(float speed) { this->speed = speed; }
	void SetFrameCtr(int ctr) { frameCtr = ctr; }

	zComplex **arrays[3];
	float speed;
	float frameCtr;
	float frameCtr2; // not sure what exactly
};
