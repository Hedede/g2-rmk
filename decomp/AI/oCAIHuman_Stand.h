class oCAIHuman_Stand : public zCAIBase {
	Z_OBJECT(oCAIHuman_Stand);
public:
	oCAIHuman_Stand(zCVob*) : zCAIBase() { }

	virtual ~oCAIHuman_Stand() = default;
	void DoAI(zCVob* vob, int& vobDeleted) override;
};

void oCAIHuman_Stand::DoAI(zCVob* vob, int& vobDeleted)
{
	vobDeleted = 0;
	auto world = vob->homeWorld;

	// looks VERY much like oCVob::SetOnFloor();
	// (but with differences: different flags and no check for foundVob)
	float suspension = vob->GetPositionWorld().y - vob->bbox3D.mins.y;
	zVEC3 origin = vob->GetPositionWorld();
	zVEC3 ray{0, -1000.0, 0};

	if ( world->TraceRayNearestHit(start, ray, vob, 0x20) ) {
		if ( world->foundPoly ) {
			zVEC3 floorPos = homeWorld->foundIntersection;
			floorPos.y += suspension + 4.0;
			vob->SetPositionWorld(floorPos);
			vob->SetPhysicsEnabled(0);
		}
	}
}
