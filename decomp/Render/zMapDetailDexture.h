

struct zCMapDetailTexture {
	struct zTMapDetailTextureEntry {
		zTMapDetailTextureEntry() = default;
		zSTRING name;
		float scale;
	};

	static void S_Cleanup() {}
	static zTMapDetailTextureEntry* S_GetDetailTextureInfo(zSTRING const& name)
	{

		auto res = detailTextureMap.find(name);
		if (res != std::end(detailTextureMap))
			return detailTextureMap[name];
		return nullptr;
	}
	static void S_Init()
	{
		auto dir = zoptions->GetDirString(DIR_PRESETS);
		auto filename = dir + "detailTexturePresets.txt";

		auto file = zfactory->CreateZFile(filename);
		if (!file->Exists()) {
			delete file;
			return;
		}
		file->Open(0);

		while (!file->Eof()) {
			zSTRING tmp;
			file->Read(tmp);
			auto word1 = tmp.PickWord(1, zSTR_SKIP, zSTR_SKIP).Upper();
			auto word2 = tmp.PickWord(2, zSTR_SKIP, zSTR_SKIP).Upper();
			auto word3 = tmp.PickWord(2, zSTR_SKIP, zSTR_SKIP).ToFloat();

			// insert in original, but this is shorter
			detailTextureMap.try_emplace(word1, word2, word3);
		}

		delete file;
	}

private:
	static std::map<zSTRING,zTMapDetailTextureEntry> detailTextureMap;
};
