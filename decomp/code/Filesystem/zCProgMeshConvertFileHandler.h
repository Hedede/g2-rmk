// _dieter/zProgMeshProto.cpp
class zCProgMeshConvertFileHandler : zCScanDirFileHandler {
	zCProgMeshConvertFileHandler()
	{
		RegisterFileType("3ds");
	}

	~zCProgMeshConvertFileHandler() = default;

	int HandleFile(zSTRING const& filename, char const* dir, _finddata_t fileinfo) override;
};

int zCProgMeshConvertFileHandler::HandleFile(zSTRING const& filename, const char *dir, _finddata_t fileinfo)
{
	int result = 0;

	auto path = dir + DIR_SEPARATOR + filename;
	path.Upper();

	if (filename.Search(0, ".3DS", 1u) > 0) {
		if (path.Search(0, "\\LEVEL\\", 1) == -1 && path.Search(0, "\\_DIETER\\", 1u) == -1) {
			auto visual = zCVisual::LoadVisual(filename);
			if ( visual ) {
				visual->Release();
				result = 1;
			}
		}

	} else {
		zFAULT("D: PMESH: Convert: Unknown filetype: " + filename); //3521
	}
	return result;
}
