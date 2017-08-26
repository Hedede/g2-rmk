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

	s_fft.CreateWaveMap(1.0, v);
	s_fft1.CreateWaveMap(1.0, v);
	s_fft2.CreateWaveMap(1.0, v);
}

void zCFFT::CreateWaveMap(float h, zVEC3& vec)
{
	arrays[0] = zCFFT::CreateArray();
	arrays[2] = zCFFT::CreateArray();
	arrays[1] = zCFFT::CreateArray();
	FillH0Array(h, vec);
}

zComplex **zCFFT::CreateArray()
{
	zComplex** array = new zComplex*[32];
	for (int i = 0; i < 32; ++i) {
		array[i] = new zComplex[32] { 0, 0, 0, 0 };
	}
	return array;
}
