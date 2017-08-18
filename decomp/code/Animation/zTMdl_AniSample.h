struct zTMdl_AniSample {
	void UnpackQuat(zCQuat& q);
	void PackQuat(zCQuat const& q);
	void UnpackTrans(zVEC3& v);
	void PackTrans(zVEC3 const& v);
	void Unpack(zCQuat& q, zVEC3& v);
	void Pack(zCQuat const& q, zVEC3 const& v);
	static void SetSamplePosConstants(float r, float s);
	static float CalcSamplePosConstants(float r, float s);

	int16_t pack[6];
};

namespace {
float samplePosScaler;
float samplePosRangeMin;
}

void zTMdl_AniSample::UnpackQuat(zCQuat& q)
{
	q[0] = (pack[0] - 0x7FFF) * zMDL_SAMPLE_QUAT_SCALER;
	q[1] = (pack[1] - 0x7FFF) * zMDL_SAMPLE_QUAT_SCALER;
	q[2] = (pack[2] - 0x7FFF) * zMDL_SAMPLE_QUAT_SCALER;
	float len = q[0]*q[0] + q[1]*q[1] + q[2]*q[2];
	if (len <= 1.0 ) {
		q[3] = sqrt(1.0 - len);
	} else {
		q[0] /= len;
		q[1] /= len;
		q[2] /= len;
		q[3] = 0;
	}
}

void zTMdl_AniSample::PackQuat(zCQuat const& q)
{
	pack[0] = (q[0] / zMDL_SAMPLE_QUAT_SCALER) + 0x7FFF;
	pack[1] = (q[1] / zMDL_SAMPLE_QUAT_SCALER) + 0x7FFF;
	pack[2] = (q[2] / zMDL_SAMPLE_QUAT_SCALER) + 0x7FFF;
	if (q[3] < 0.0) {
		for (int i = 0; i < 3; ++i)
			pack[i] = -2 - pack[i];
	}
}

void zTMdl_AniSample::UnpackTrans(zVEC3& v)
{
	v.x = pack[3] * samplePosScaler + samplePosRangeMin;
	v.y = pack[4] * samplePosScaler + samplePosRangeMin;
	v.z = pack[5] * samplePosScaler + samplePosRangeMin;
}

void zTMdl_AniSample::PackTrans(zVEC3 const& v)
{
	// No idea what is this for
	float scaler = 1.0 / (samplePosScaler * 65535.0)
	pack[3] = (v.x - samplePosRangeMin) * scaler * 65535.0;
	pack[4] = (v.y - samplePosRangeMin) * scaler * 65535.0;
	pack[5] = (v.z - samplePosRangeMin) * scaler * 65535.0;
}

void zTMdl_AniSample::Unpack(zCQuat& q, zVEC3& v)
{
	UnpackQuat(q);
	UnpackTrans(v);
}

void zTMdl_AniSample::Pack(zCQuat const& q, zVEC3 const& v)
{
	PackQuat(q);
	PackTrans(v);
}


void zTMdl_AniSample::SetSamplePosConstants(float r, float s)
{
	samplePosRangeMin = r;
	samplePosScaler = s;
}

float zTMdl_AniSample::CalcSamplePosConstants(float r, float s)
{
	samplePosRangeMin = r;
	samplePosScaler = zMDL_SAMPLE_POS_BITS_INV * s;
	return samplePosScaler;
}
