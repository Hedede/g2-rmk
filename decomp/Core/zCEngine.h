class zCEngine {
public:
	zCEngine() = default;
	virtual ~zCEngine() = default;
	int Init(HWND* wnd);
};

int zCEngine::Init(HWND* wnd)
{
	zerr.onexit = sysExit;
	zCClassDef::EndStartup();

	if ( !zfactory )
		zfactory = new zCObjectFactory;

	sysEvent();

	zInitOptions();

	bool memProfiler = zoptions->Parm("ZMEMPROFILER");
	zCMallocGeneric::Init(memProfiler);

	zFILE_VDFS::InitFileSystem();

	sysEvent();

	zresMan = new zCResourceManager;

	bool noResThread = zoptions->Parm("ZNORESTHREAD");

	zresMan->SetThreadingEnabled(zresMan, !noResThread);

	zerr->Separator("");

	zINFO(0, "D: " + "*** ZenGin v1.00 - developed 1997-2001 Mad Scientists ");
	zINFO(0, "D: *** Mad Scientists: Hildebrandt, Wohlers, Speckels");
	zINFO(0, "D: *** compiled" + "Oct 22 2003" + ", release-build");
	zINFO(0, "D: *** zEngine-Init ...");

	zBert_StartUp();
	zUlfi_StartUp();

	sysEvent();

	zDieter_StartUp(wnd);
	zCarsten_StartUp(wnd);

	zINFO("D: *** zEngine-Init ... Done !");

	zerr->Separator("");

	bool texConvert = zoptions->Parm("ZTEXCONVERT");
	if ( texConvert )
	{
		zSTRING val = zoptions->ParmValue("ZTEXCONVERT");
		zCTexture::ScanConvertTextures(val);
	}

	zfpuControler->RestoreDefaultControlWord()
	zfpuControler->SaveCurrentControlWord();
	return 1;
}
