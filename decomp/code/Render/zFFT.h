// This one is clearly made by other developers, as it doesn't use
// zERROR (uses stdout instead) and other stuff made by original three
class zCFFT {
	virtual ~zCFFT();

	static void S_Init();

	void SetSpeed(float speed) { this->speed = speed; }
	void SetFrameCtr(int ctr) { frameCtr = ctr; }

	zComplex **arrays[3];
	float speed;
	float frameCtr;
	float frameCtr2; // not sure what exactly
};

void zCFFT::S_Init()
{
	s_fft.speed = 0.5;
	s_fft2.speed = 0.2;
	s_fft3.speed = 2.0;

	zVEC3 v{1.0, 0, 0};

	s_fft.arrays[0] = zCFFT::CreateArray();
	s_fft.arrays[2] = zCFFT::CreateArray();
	s_fft.arrays[1] = zCFFT::CreateArray();
	s_fft.FillH0Array(1.0, v);

	s_fft2.arrays[0] = zCFFT::CreateArray();
	s_fft2.arrays[2] = zCFFT::CreateArray();
	s_fft2.arrays[1] = zCFFT::CreateArray();
	s_fft2.FillH0Array(1.0, v);

	s_fft3.arrays[0] = zCFFT::CreateArray();
	s_fft3.arrays[2] = zCFFT::CreateArray();
	s_fft3.arrays[1] = zCFFT::CreateArray();
	s_fft3.FillH0Array(1.0, v);
}
