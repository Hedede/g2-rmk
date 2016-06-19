/*
 * As far as I see,
 * PatchMap used to calculate lightning.
 */
struct zCPatch {
	zCPatch() = default;

	zVEC3 vec0;
	zVEC3 vec1;
	zVEC3 vec2;
	float __grid2;
	zVEC3 vec3;
	zVEC3 vec4;
	__int16 unk3;
	__int16 unk4;
	zCArray arr0;
	zVEC3 vec6;
};

#pragma pack(push, 1)
struct zCPatchMap {
	~zCPatchMap() {
		for (auto patch : patches) {
			if (patch) {
				patch->arr0.DeleteList();
				delete patch;
			}
		}

		poly0 = 0;
		// also implicit deletings of arrays
	}

	char unk0;
	__int16 unk1;
	__int16 unk2;

	zCArray<zCPatch*>   patches;
	zCArray<zCPolygon*> surface;

	zTPlane plane0;
	zTPlane plane1;

	zTBBox3D bbox;

	zVEC3 __normal_maybe;
	zVEC3 vec0;
	zVEC3 vec1;

	zCPolygon* poly0;
};
#pragma pack(pop)
