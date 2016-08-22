class zCVobSpot : public zCVob {
	Z_OBJECT(zCVobSpot);
pblic:
	zCVobSpot() = default;
	virtual ~zCVobSpot() = default;
	void MarkAsUsed(float timeDelta, zCVob* vob)
	{
		npc = vob;
		inUse = ztimer.totalTimeFloat + timeDelta;
	}

	bool IsAvailable(zCVob* vob);

private:
	float inUse = 0.0;
	oCNpc* npc  = nullptr;
};

int zCVobSpot::IsAvailable(zCVob *vob)
{
	if ( npc && npc != vob && ztimer.totalTimeFloat < inUse )
		return false;
	if ( homeWorld ) {
		zCArray<zCVob> voblist;
		zTBBox3D bbox;
		auto pos = trafoObjToWorld.GetTranslation();
		bbox.maxs.x = pos.x + 50.0;
		bbox.maxs.y = pos.y + 100.0;
		bbox.maxs.z = pos.z + 50.0;
		bbox.mins.x = pos.x + 50.0;
		bbox.mins.y = pos.y + 100.0;
		bbox.mins.z = pos.z + 50.0;
		homeWorld->bspTree.bspRoot->CollectVobsInBBox3D(&voblist, &bbox);

		for (auto& v : voblist) {
			if (v == npc)
				return v == vob;
		}

		npc = nullptr;
		return true;
	}
	return false;
}
