// _dieter/zMorphMesh.cpp
class zCMorphMeshConvertFileHandler : zCScanDirFileHandler {
	zCMorphMeshConvertFileHandler()
	{
		RegisterFileType("mms");
	}
	~zCMorphMeshConvertFileHandler() = default;
	int HandleFile(zSTRING const& filename, char const* dir, _finddata_t fileinfo) override;
};


int zCMorphMeshConvertFileHandler::HandleFile(zSTRING const& filename, const char *dir, _finddata_t fileinfo)
{
	int result = 0;

	auto path = dir + DIR_SEPARATOR + filename;
	path.Upper();

	if (filename.Search(0, ".MMS", 1u) > 0) {
		if (path.Search(0, "\\LEVEL\\", 1) == -1 && path.Search(0, "\\_DIETER\\", 1u) == -1) {
			auto proto = zCMorphMeshProto::Load(filename);
			if ( proto ) {
				auto morph = new zCMorphMesh(proto);
				morph->SetAlphaTestingEnabled(proto->alphaTestingEnabled);
				proto->Release();
				morph->Release();
				result = 1;
			}
		}
	} else {
		zFAULT("D: MORPH: Convert: Unknown filetype: " + filename); //1231
	}
	return result;
}
