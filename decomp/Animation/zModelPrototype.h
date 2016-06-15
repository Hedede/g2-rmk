class zCModelPrototype {
	zCModelAni* SearchAni(zSTRING const& aniName);
	
	zSTRING& GetModelProtoFileName()
	{
		return fileName;
	}

	void SetFileSourceType(int sourceType);

	void Release()
	{
		if (--refCtr <= 0) {
			++refCtr;
			delete this;
		}
	}

	void ReleaseMeshSoftSkinList()
	{
		for (auto skin : softSkins)
			Release(skin);
		softSkins.DeleteList();
	}

	void zCModelPrototype::ReleaseMeshes()
	{
		ReleaseMeshSoftSkinList();
		for (auto& node : nodes)
			node->SetNodeVisualS(0);
	}

private:
	static void ConvertVec3(zVEC3& vec);
	static void ConvertAngle(float& angle) {}
	void ReadComment() {}

	void AddAni(zCModelAni* ani);

private:
	zCModelPrototype* __next; // unsure
	zCModelPrototype* __prev; // unsure

	int refCtr;

	zSTRING modelProtoName; // stringized pointer to this
	zSTRING fileName;
	zCTree<zCModelNode*> nodeTree;
	zCArraySort<zCModelAni*> anis;
	zCArray<zCModelAniEvent*> aniEvents;
	zCArray<zCModelNode*> nodes;

	int unk1;

	zCArray<zCMeshSoftSkin*> softSkins;

	zSTRING unk2;

	zVEC3 unk3;
	zVEC3 unk4;
	zVEC3 unk5;
	zVEC3 unk6;
	int unk7;
	zVEC3 unk8;
	int unk9;
	zVEC3 unk10;
	int fileType;
	int unk12[3];
};

zCModelAni* zCModelPrototype::SearchAni(zSTRING const& aniName)
{
	auto idx = SearchAniIndex(aniName);
	if (idx >= 0)
		return anis[idx];
	return nullptr;
}

int zCModelPrototype::AddAni(zCModelAni *ani)
{
	if (SearchAni(ani->aniName)) {
		zFATAL("D: zModel(zCModelPrototype::AddAni): ani already defined: " + ani->aniName); // 1573, _dieter
	}

	anis.InsertSort(ani);
	return anis.GetNumInList();
}

void zCModelPrototype::ConvertVec3(zVEC3& vec)
{
	auto vecy = vec.y;

	vec.x = -vec.x;
	vec.y =  vec.z;
	vec.z = -vecy;
}

void zCModelPrototype::SetFileSourceType(int sourceType)
{
	this->fileType = sourceType;

	if (sourceType == 1)
		modelProtoName = ".ASC";
	else
		modelProtoName = ".MDS";
}
