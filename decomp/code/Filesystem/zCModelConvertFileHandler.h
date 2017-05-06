// _dieter/zModelProto.cp
class zCModelConvertFileHandler : zCScanDirFileHandler {
	zCModelConvertFileHandler()
	{
		RegisterFileType("mds");
		RegisterFileType("asc");
	}

	~zCModelConvertFileHandler() = default;
	int HandleFile(zSTRING const& filename, char const* dir, _finddata_t fileinfo) override;
};

int zCModelConvertFileHandler::HandleFile(zSTRING const& filename, const char* dir, _finddata_t fileinfo)
{
	int result = 0;

	auto path = dir + DIR_SEPARATOR + filename;
	path.Upper();

	if (filename.Search(0, ".MDS", 1u) > 0)
	{
		zCModelPrototype::s_autoConvertMeshes = 1;
		auto proto = zCModelPrototype::Load(filename, 0);
		if (proto) {
			proto->Release();
			result = 1;
		}
		zCModelPrototype::s_autoConvertMeshes = 0;
	}
	else if ( filename.Search(0, ".ASC", 1u) > 0 )
	{
		if ( path.Search(0, "ANIMS\\ASC_MOBSI\\MESHES", 1u) > 0 )
		{
			auto visual = zCVisual::LoadVisual(filename);
			if ( visual ) {
				visual->Release();
				result = 1;
			}
		}
	} else {
		zWARNING("D: MDL: Convert: Unknown filetype: " + filename); // 5608
	}

	return result;
}
