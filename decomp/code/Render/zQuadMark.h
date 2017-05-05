class zCQuadMark : public zCVisual {
	Z_OBJECT(zCQuadMark);
public:
	~zCQuadMark()
	{
		Release(__mesh);
		Release(__mtl1);
		Release(__mtl2);
	}

	struct zTQuadMarkVert {
		int data[5];
	};

	struct zTQuadMarkAniState {
		int data[12];
	};

	virtual void Render(zTRenderContext &);
	bool IsBBox3DLocal() override { return true; }
	zTBBox3D GetBBox3D() override { return __mesh->GetBBox3D(); }
	zSTRING GetVisualName() override { return ""; }
	void SetVisualUsedBy(zCVob* vob) override
	{
		usedBy = vob;
		vob->flags1.0x20 = 1;
		vob->SetCollDetDyn(0);
		vob->SetCollDetStat(0);
	}

private:
	zCMesh *__mesh;
	zCVob *usedBy;
	zCMaterial *__mtl1;
	zCMaterial *__mtl2;
	float __num;
	int unk0;
	zCArray<zTQuadMarkAniState> aniStates;
	char qu[8];
	zCArray<zCPolygon*> polys;
	zCArray<zTQuadMarkVert> verts;
	int unk1;
	zVEC3 __vec;
};

int zCQuadMark::GetNumActive()
{
	if ( __num > 0.0 )
		return aniStates.numInArray;
	return __mesh->numPoly;
}
