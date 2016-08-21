// _kurt\\zRndD3D_Init.cpp
// This one was brutal, I didn't think I'm gonna figure it out.
// It was horrible bowl of spaghetti, complete with:
//   - pointer to middle to the struct, offsets to other members relative to it
//   - overlappig structs, structs reused for completely different structs
//   - deep-nested ifs
//   - non-linear flow (the for in beginning was at the very end of the function with jump to the top)
//   - inlined code
int zCRnd_D3D::XD3D_InitPerDX(zTRnd_ScreenMode mode, unsigned x, unsigned y, int bpp, int id)
{
	zINFO("X: XD3D_InitPerDX: D3DXInitialize done");

	auto dev_count = D3DXGetDeviceCount();

	int res;
	int             dev_id[100];
	D3DX_DEVICEDESC dev_info[100];

	res = D3DXGetDeviceDescription(0, &dev_info[0])

	if (!DXTryWarning(res, "X: [RND3D]XD3D_InitPerDX: Can't get device description !"))
		return 0;

	auto  cur_guid = dev_info[0].ddDeviceID;
	auto* cur_idx  = &dev_id[0];

	for (unsigned i = 1; i < dev_count; ++i) {
		res = D3DXGetDeviceDescription(i, &dev_info[i]);
		if (!DXTryWarning(res, "X: [RND3D]XD3D_InitPerDX: Can't get device description !"))
			return 0;

		if (dev_info[i].ddDeviceID == dev_guid) {
			if (dev_info[i]. hwLevel == D3DX_HWLEVEL_TL || dev_info[i].hwLevel == D3DX_HWLEVEL_RASTER) 
				*cur_idx = i;
		} else {
			cur_guid = dev_info[i].ddDeviceID;
			*(++cur_idx) = i;
		}
	}

	numDevices = dev_count;
	for (unsigned i = 0; i < dev_count; ++i) {
		auto idx = dev_id[i];
		zINFO("X: XD3D_InitPerDX: Found "_s + dev_info[idx].driverDesc);
	}

	auto sel_id = dev_count - id;

	zINFO("X: XD3D_InitPerDX: Switching to " + dev_info[sel_id].driverDesc);

	auto dev_idx = dev_info[sel_id].deviceIndex;

	memset(&hwinfo, 0, sizeof(hwinfo));

	if ( dev_info[sel_id].hwLevel == D3DX_HWLEVEL_TL) {
		hwinfo.hw_tnl = 1;
		zINFO("X: XD3D_InitPerDX: Hardware TnL supported.");
	} else {
		hwinfo.hw_tnl = 0;
		zINFO("X: XD3D_InitPerDX: Hardware TnL not supported.");
	}

	auto refreshRate = zoptions->ReadDWord("RENDERER_D3D", "zVidRefreshRate", 0);
	if ( !refreshRate )
		refreshRate = D3DX_DEFAULT;

	res = D3DXCreateContextEx(dev_idx, mode, winHandle, 0, bpp, D3DX_DEFAULT, 32, 0, D3DX_DEFAULT, x, y, refreshRate, &zCRnd_D3D::xd3d_pd3dx)

	if (res) {
		zWARNING("X: XD3D_InitPerDX: Can't initialize with 32 bit depth buffer, trying 24 bit ... "); // 384

		res = D3DXCreateContextEx(dev_idx, mode, winHandle, 0, bpp, D3DX_DEFAULT, 24, 0, D3DX_DEFAULT, x, y, refreshRate, &zCRnd_D3D::xd3d_pd3dx)
	}

	if (res) {
		zWARNING("X: XD3D_InitPerDX: Can't initialize with 24 bit depth buffer, trying default ... "); // 389

		res = D3DXCreateContextEx(dev_idx, mode, winHandle, 0, bpp, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DX_DEFAULT, x, y, refreshRate, &zCRnd_D3D::xd3d_pd3dx);
	}

	if (res) {
		if ( bpp != 16 ) {
			zWARNING("X: XD3D_InitPerDX: Can't initialize with " + bpp + " bpp - try 16 bpp ... "); // 395

			res = D3DXCreateContextEx(dev_idx, mode, winHandle, 0, 16, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DX_DEFAULT, x, y, refreshRate, &zCRnd_D3D::xd3d_pd3dx);
		}

		if (!res)
			zINFO("X: XD3D_InitPerDX: Initialized with 16 bpp !");
	}

	if (DXTryFatal(res, "X: XD3D_InitPerDX: Can't create D3DX context !")) // I made this up // I just don't like working with c-style strings
		return 0;


	// I replaced all individual snippets with this lambda to save on
	// lines-of-code count
	auto Release = [&] {
		if ( xd3d_backBuffer ) {
			xd3d_backBuffer->Release();
			xd3d_backBuffer = 0;
		}
		if ( xd3d_primaryBuffer ) {
			xd3d_primaryBuffer->Release();
			xd3d_primaryBuffer = 0;
		}
		if ( zCRnd_D3D::xd3d_pd3dDevice7 ) {
			zCRnd_D3D::xd3d_pd3dDevice7->Release();
			zCRnd_D3D::xd3d_pd3dDevice7 = 0;
		}
		if ( zCRnd_D3D::xd3d_pd3d7 ) {
			zCRnd_D3D::xd3d_pd3d7->Release();
			zCRnd_D3D::xd3d_pd3d7 = 0;
		}
		if ( zCRnd_D3D::xd3d_pdd7 ) {
			zCRnd_D3D::xd3d_pdd7->Release();
			zCRnd_D3D::xd3d_pdd7 = 0;
		}
		if ( zCRnd_D3D::xd3d_pd3dx ) {
			zCRnd_D3D::xd3d_pd3dx->Release();
			zCRnd_D3D::xd3d_pd3dx = 0;
		}
	};

	zCRnd_D3D::xd3d_pdd7 = zCRnd_D3D::xd3d_pd3dx->GetDD();
	if ( !zCRnd_D3D::xd3d_pdd7 ) {
		zWARNING("X: XD3D_InitPerDX: Can't get DirectDraw !"); // 413,

		Release();
		return 0;
	}

	zCRnd_D3D::xd3d_pd3d7 = zCRnd_D3D::xd3d_pd3dx->GetD3D();
	if ( !zCRnd_D3D::xd3d_pd3d7 ) {
		zWARNING("X: XD3D_InitPerDX: Can't get Direct3D !"); // 420,

		Release();
		return 0;
	}

	zCRnd_D3D::xd3d_pd3dDevice7 = zCRnd_D3D::xd3d_pd3dxg->GetD3DDevice();
	if ( !zCRnd_D3D::xd3d_pd3dDevice7 ) {
		zWARNING("X: XD3D_InitPerDX: Can't get Direct3DDevice !"); // 428

		Release();
		return 0;
	}

	xd3d_primaryBuffer = zCRnd_D3D::xd3d_pd3dxg->GetPrimary();
	if ( !xd3d_primaryBuffer ) {
		zWARNING("X: XD3D_InitPerDX: Can't get pointer to frontbuffer !"); // 437,

		Release();
		return 0;
	}

	xd3d_backBuffer = zCRnd_D3D::xd3d_pd3dxg->GetBackBuffer(0);
	if ( !xd3d_backBuffer ) {
		zWARNING("X: XD3D_InitPerDX: Can't get pointer to backbuffer !"); //447,

		Release();
		return 0;
	}

	xd3d_zBuffer = zCRnd_D3D::xd3d_pd3dxg->GetZBuffer();
	if ( xd3d_zBuffer ) {
		zWARNING("X: XD3D_InitPerDX: Can't get pointer to z-buffer !"); // 458,

		Release();
		return 0;
	}

	auto enableAA = zoptions->ReadInt("ENGINE", "zVidEnableAntiAliasing", 0);
	if ( enableAA )
		XD3D_SetRenderState(D3DRENDERSTATE_ANTIALIAS, 2);
	return zCRnd_D3D::XD3D_TestCapabilities();
}

int XD3D_AppIDDEnumCallbackEx(GUID* guid, char* driverDescription, char* driverName, void* context, HMONITOR monitor)
{
	IDirectDraw7* pDD = 0;

	if (!DirectDrawCreateEx(guid, (LPVOID*)&pDD, &IID_IDirectDraw7, 0)) {
		if (guid) {
			dxDeviceGUIDflag = 1;
			dxDeviceGUID = *guid;
		} else {
			dxDeviceGUIDflag = 0;
		}

		pDD->GetDeviceIdentifier(&dddi, 0);

		int curDriver = 0;

		IDirect3D7* pD3D;
		if (!pDD->QueryInterface(IID_IDirect3D7, (void**)&pD3D)) {
			pD3D->EnumDevices(XD3D_EnumDeviceCallback, &curDriver);

			dxDeviceModeNum[dxDeviceNumber] = 0;

			pDD->EnumDisplayModes(0, 0, 0, XD3D_EnumModesCallback);

			if(dxDeviceModeNum[dxDeviceNumber] > 0)
				++dxDeviceNumber;
		}

		if (pD3D) {
			pD3D->Release();
			pD3D = 0;
		}
	}

	if(pDD)
		pDD->Release();
	return dxDeviceNum - 1 != dxSelectDevice;
}

// seriously it is "ggg" in debug info
int XD3D_AppIDDEnumCallback(GUID* guid, char* ggg, char* driverName, void* context)
{
	XD3D_AppIDDEnumCallbackEX(guid, ggg, driverName, context, NULL);
}

int zCRnd_D3D::XD3D_EnumerateModes()
{
	dxDeviceNum = 0;
	dxDeviceNumber = 0;
	dxSelectDevice = -1;
	DirectDrawEnumerateExA(XD3D_AppIDDEnumCallbackEx, 0, 5u);
	numDevices = dxDeviceNum;
	return 0;
}
