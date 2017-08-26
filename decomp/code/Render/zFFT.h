// zCFFT from Gothic2 uses telecomm-FFT
class zCFFT {
	zCFFT() = default;
	virtual ~zCFFT()
	{
		for (auto array : arrays)
			FreeArray(array);
	}

	static void S_Init();

	void SetSpeed(float speed) { this->speed = speed; }
	void SetFrameCtr(int ctr) { frameCtr = ctr; }

	zComplex **arrays[3];
	float speed   = 1.0;
	int frameCtr  = 0;
	int frameCtr2 = -1; // not sure what exactly
};

void zCFFT::S_SetFrameCtr(int ctr)
{
	s_fft.frameCtr  = ctr;
	s_fft2.frameCtr = ctr;
	s_fft3.frameCtr = ctr;
}

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
	for (int i = 0; i < 32; ++i)
		array[i] = new zComplex[32] { 0, 0, 0, 0 };
	return array;
}

void zCFFT::FreeArray(zComplex** array)
{
	for (int i = 0; i < 32; ++i)
		delete array[i];
	delete array;
}

#include "fourier.h"

// this one is based on fft_float, but it was modified to take zComplex* instead of float*,float*
void zCFFT::fft_float(unsigned NumSamples, int InverseTransform, zComplex* in, zComplex* out );

int      zCFFT::IsPowerOfTwo( unsigned x );
unsigned zCFFT::NumberOfBitsNeeded( unsigned PowerOfTwo );
unsigned zCFFT::ReverseBits( unsigned index, unsigned NumBits );
double   zCFFT::Index_to_frequency( unsigned NumSamples, unsigned Index );
