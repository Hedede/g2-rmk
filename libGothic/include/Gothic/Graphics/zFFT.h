#pragma once

struct zComplex {
	float v[4];
};

struct zCFFT;
inline auto& fft1 = Value<zCFFT>(0x8D85A0);
inline auto& fft2 = Value<zCFFT>(0x8D8558);
inline auto& fft3 = Value<zCFFT>(0x8D8574);

struct zCFFT {
	static void S_Init()
	{
		fft1.speed = 0.5;
		fft2.speed = 0.2;
		fft3.speed = 2.0;
		zVEC3 vec{1.0, 0, 0};
		fft1.CreateWaveMap(1.0, vec);
		fft2.CreateWaveMap(1.0, vec);
		fft3.CreateWaveMap(1.0, vec);
	}

	static void S_SetFrameCtr(int ctr)
	{
		fft1.frameCtr = ctr;
		fft2.frameCtr = ctr;
		fft3.frameCtr = ctr;
	}

	static zComplex** CreateArray()
	{
		zComplex** array = new zComplex*[32];
		for (int i = 0; i < 32; ++i)
			array[i] = new zComplex[32] { };
		return array;
	}

	void CreateWaveMap(float h, zVEC3& vec)
	{
		for (auto& array : arrays)
			array = CreateArray();
		FillH0Array(h, vec);
	}


	void FillH0Array(float f, zVEC3& vec)
	{
		Thiscall<void(zCFFT*, float, zVEC3&)> call{0x55A2B0};
		call(this, f, vec);
	}

	void* vtab;
	zComplex **arrays[3];
	float speed;
	int frameCtr;
	float frameCtr2;
};
