float zLIGHTMAP_GRID = 25.0;
float zLIGHTMAP_GRID_HALF = zLIGHTMAP_GRID * 0.5;

class zCLightMap : public zCObject {
	Z_OBJECT(zCLightMap);
public:
	zCLightMap() = default;
	~zCLightMap() override
	{
		Release(texture);
	}

	void SetTexture(zCTexture* texture)
	{
		Set(this->texture, texture);
	}

	void CalcLightmapOriginUpRight(zVEC3 const& origin, zVEC3 const& up, zVEC3 const& right)
	{
		float fac = 1.0 / (right.Length2() * up.Length2() * up.Length2();
		lmRight   = right * fac;
		lmUp      = up    * (1.0 / up.Length2());
		lmOrigin  = origin;
	}

private:
	zVEC3 lmOrigin;
	zVEC3 lmUp;
	zVEC3 lmRight;
	zCTexture* texture = nullptr;
};

#pragma pack(push, 2)
struct zCTransfer {
	zCPatch *parent;
	short val;
};
#pragma pack(pop)

struct zCTransferConstr {
	zCPatch *parent;
	float val;
};


/*
 * As far as I see,
 * PatchMap used to calculate lightning.
 */
struct zCPatch {
	zCPatch() = default;

	zVEC3 vec0; // pos
	zVEC3 vec1;
	zVEC3 vec2; // normal
	float __grid2;
	zVEC3 vec3; // color
	zVEC3 vec4;

	__int16 tilex;
	__int16 tiley;

	zCArray<zCTransfer> transfers;

	zVEC3 colorVec;
};

#pragma pack(push, 1)
struct zCPatchMap {
	zCPatchMap() = default;
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

	bool lit = false;
	short dim[2] {0,0};

	zCArray<zCPatch*>   patches;
	zCArray<zCPolygon*> surface;

	zTPlane plane0;
	zTPlane plane1;

	zTBBox3D bbox;

	zVEC3 __normal;
	zVEC3 __up;
	zVEC3 __right;

	zCPolygon* poly0;
};
#pragma pack(pop)
