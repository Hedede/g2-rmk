// _Dieter/zProgMeshProto.cpp
class zCMeshSoftSkin : public zCProgMeshProto {
	Z_OBJECT(zCMeshSoftSkin);
public:
	zCMeshSoftSkin() = default;
	void ~zCMeshSoftSkin()
	{
		DeleteVertWeightStream();
		DeleteNodeOBBList();
	}

	void Load(zCFileBIN &) override;
	void Save(zCFileBIN &) override;

	static short GetFileVersion() { return 4; }
	static int GetLowestLODNumPolys() { return lowestLodNumPolys; }

#pragma pack(push, 1)
	struct zTWeightEntry {
		float Weight;
		zVEC3 VertexPosition;
		uint8_t NodeIndex;
	};
#pragma pack(pop)

	struct zTNodeWedgeNormal {
		float data[4];
	};

	void ResetVertWeightIterator()
	{
		weightIter = vertWeightStream;
	}

	void DeleteVertWeightStream()
	{
		delete[] vertWeightStream;
		vertWeightStream = 0;
		weightIter = 0;
	}

	void AllocVertWeightStream(int numSVerts, int numWeightEntrys)
	{
		DeleteVertWeightStream();
		auto size = sizeof(zTWeightEntry) * numWeightEntrys + sizeof(int) * numSVerts;
		vertWeightStream = new char[size];
		ResetVertWeightIterator();
	}

	void AddNumWeightEntry(const int& numSVert)
	{
		*(int*)weightIter = numSVert;
		++(int*)weightIter;
	}

	void AddWeightEntry(zTWeightEntry const& entry)
	{
		*(zTWeightEntry*)weightIter = entry;
		++(zTWeightEntry*)weightIter;
	}

	int GetVertWeightStreamSize()
	{
		ResetVertWeightIterator();
		int result = 0;
		for (int i = vertList.size; i > 0; --i) {
			int num = *(int*)weightIter;
			int size = sizeof(int) + num * sizeof(zTWeightEntry);

			result     += size;
			weightIter += size;
		}
		return result;
	}

	void DeleteNodeOBBList()
	{
		for (auto obb : nodeObbList)
			delete obb;
		nodeObbList.DeleteList();
	}

private:
	zCArray<int>               nodeIndexList;
	zCArray<zCOBBox3D*>        nodeObbList;
	zCArray<zTNodeWedgeNormal> nodeWedgeNormals;

	char *vertWeightStream = nullptr;
	char *weightIter       = nullptr;
};
