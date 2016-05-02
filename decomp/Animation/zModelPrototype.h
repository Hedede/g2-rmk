class zCModelPrototype {

	zCModelAni* SearchAni(zSTRING const& aniName);
	
	zSTRING& GetModelProtoileName()
	{
		return fileName;
	}

private:
	static void ConvertVec3(zVEC3& vec);
	static void ConvertAngle(float& angle) {}
	void ReadComment() {}

private:
	zCModelPrototype *__next;
	int unk0[2];
	zSTRING modelProtoName; // stringized pointer to this
	zSTRING fileName;
	zCTree<zCModelNode*> nodeTree;
	zCArraySort<zCModelAni*> anis;
	zCArray<zCModelAniEvent*> aniEvents;
	zCArray<zCModelNode*> nodes;

	int unk1[4];

	zSTRING unk2;

	zVEC3 unk3;
	zVEC3 unk4;
	zVEC3 unk5;
	zVEC3 unk6;
	int unk7;
	zVEC3 unk8;
	int unk9;
	zVEC3 unk10;
	int unk11;
	int unk12[3];
};

zCModelAni* zCModelPrototype::SearchAni(zSTRING const& aniName)
{
	auto idx = SearchAniIndex(aniName);
	if (idx >= 0)
		return anis[idx];
	return nullptr;
}

void zCModelPrototype::ConvertVec3(zVEC3& vec)
{
	auto vecy = vec.y;

	vec.x = -vec.x;
	vec.y =  vec.z;
	vec.z = -vecy;
}
