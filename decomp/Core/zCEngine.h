struct zCEngine {
	zCEngine() = default;
	virtual ~zCEngine() = default;

	static int Init(HWND* wnd);
	static int ShutDown();
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

int zCEngine::Shutdown()
{
	zerr.Separator("");
	zINFO(0,"D: *** zEngine-ShutDown ...")// 189, _dieter\\zEngine.cpp

	zCarsten_ShutDown();
	zDieter_ShutDown();
	zUlfi_ShutDown();
	zBert_ShutDown();

	zerr.Separator("");

	zresMan->EndThread();
	Delete(zresMan);

	zDeinitOptions();

	Release(zfactory);

	zFILE_VDFS::DeinitFileSystem();
	zCMallocGeneric::Shutdown();

	zINFO(0,"D: *** zEngine-ShutDown ... Done!"); //218, _dieter

	return 1;
}
