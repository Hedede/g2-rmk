struct zCPositionKey {
	int pad;
	zVEC3 pos;
	zCQuat rot;
};


struct oCTrajectory {
	zCArray<zCPositionKey*> posKeys;
	zCKBSpline *spline;
	int unk1;
	int unk2;
};

void oCTrajectory::VobCross(zCVob *vob)
{
	for ( int i = 1; i < posKeys.numInArray; ++i ) {
		auto k1 = posKeys[i - 1];
		auto k2 = posKeys[i];

		auto dir = k2->pos - k1->pos;
		if (!vob->homeWorld->TraceRayFirstHit(k1->pos, dir, 0, 4))
			continue;
		if (vob->homeWorld->traceRayReport.foundVob == vob)
			break;
	}
}
