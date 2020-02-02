class zCModelPrototype {
	static NumInList()
	{
		auto node =  zCModelPrototype::s_modelRoot;
		for (int i = 0; node; ++i)
			node = node->next;
		return i;
	}

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

	zSTRING __str;

	zTBBox3D __bbox;
	zVEC3 __vec2;
	zVEC3 __vec3;
	int hor;
	zVEC3 __vec4;
	int fileType;
	zCArray har;
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
