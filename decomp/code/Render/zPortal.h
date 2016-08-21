struct zCPortal : zCPolygon {
	virtual ~zCPortal() = default;

	zCPortal()
		: zCPolygon()
	{
		flags.portalPoly = 1;
	}

	zCPortal(zTPlane const& plane, zCMesh* containerMesh)
		: zCPortal()
	{
		if ( !portalDummyMaterial )
			portalDummyMaterial = new zCMaterial("Z_PORTALDUMMYMAT");

		SetMaterial(portalDummyMaterial);

		int axisNo = -1;
		float max = -30000.0;
		for (unsigned i = 0; i < 3; ++i) {
			if (plane.normal[i] > max) {
				max = plane.normal[i];
				axis = i;
			}
		}

		if (axisNo == -1)
			zWARNING("D: zBSP(zCPortal::CreateFromPlane): no axis found"); /// zBsp, 939

		zVEC3 axis;
		if (axisNo >= 0) {
			if (axis <= 1) {
				axis.z = 1.0;
			} else if (axis == 2) {
				axis.x = 1.0;
			}
		}

		float dot = -axis * plane.normal;

		zVEC3 vec = plane.normal * dot + axis;
		vec.Normalize();

		zVEC3 vec2 = plane.normal * plane->distance;

		auto cross = Alg_Cross(vec2, plane.normal); // madeup

		auto vert = new zCVertex;

		vert1->position = vec2 + (vec - cross) * 10000.0;
		vert2->position = vec2 + (vec + cross) * 10000.0;
		vert3->position = vec2 + (cross - vec) * 10000.0;
		vert4->position = vec2 + (-cross - vec) * 10000.0;

		AllocVerts(4);
		SetVertex(0, vert4);
		SetVertex(1, vert3);
		SetVertex(2, vert2);
		SetVertex(3, vert1);

		polyPlane = plane;
	}

	void RemoveFromBsp()
	{
		if (unk1)
			unk1.RemovePoly(this);
		if (unk2)
			unk2.RemovePoly(this);
	}

private:
	zCMaterial* portalDummyMaterial;
	zCMemPool<zCPortal> s_memPool;
public:
	// TODO: operator new (optimized out)
	void operator delete(void* mem)
	{
		s_memPool.Free(mem);
	}

private:
	zCCBspNode* unk1 = nullptr;
	zCCBspNode* unk2 = nullptr;
	int unk3 = -1;
};
