void zCSystemInfo::AnalyseCpu()
{
	BYTE data[1024];
	DWORD dataSize;
	HKEY phkResult;

	if ( RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Hardware\\Description\\System\\CentralProcessor\\0", 0, 1u, &phkResult) ) {
		zFAULT("B: SYS: Failed to analyse CPU"); // 49
	} else {
		dataSize = 1024;
		RegQueryValueExA(phkResult, "~MHz", 0, 0, data, &dataSize);
		cpuMhz = *(DWORD *)data;

		dataSize = 1024;
		RegQueryValueExA(phkResult, "VendorIdentifier", 0, 0, data, &dataSize);
		cpuName = data;
	}

	RegCloseKey(phkResult);
}

void zCSystemInfo::AnalyseOs()
{
	_OSVERSIONINFOA osInfo;
	osInfo.dwOSVersionInfoSize = 148;

	if ( GetVersionExA(&osInfo) ) {
		osCSDVersion = osInfo.szCSDVersion;
		if ( !osInfo.dwPlatformId ) {
			osVersion = 1;
		} else if ( osInfo.dwPlatformId == 1 ) {
			if (osInfo.dwMinorVersion >= 90)
				osVersion = 4;
			else if ( osInfo.dwMinorVersion >= 10 )
				osVersion = 3;
			else
				this->osVersion = 2;
		} else if (osInfo.dwPlatformId == 2) {
			if ( osInfo.dwMajorVersion < 5 )
				osVersion = 5;
			else
				osVersion = 6;
		} else {
			zFAULT("B: SYS: Failed to get correct Operating System."); //83
			osVersion = 7;
		}
	} else {
		zFAULT("B: SYS: Failed to analyse operating-system") // 86
	}

	switch ( this->osVersion ) {
	case 1:
		osName = "Windows 3.1";
		break;
	case 2:
		osName = "Windows 95"
		break;
	case 3:
		osName = "Windows 98";
		break;
	case 4:
		osName = "Windows 98 ME";
		break;
	case 5:
		osName = "Windows NT";
		break;
	case 6:
		osName = "Windows 2000";
		break;
	default:
		osName = "unknown";
		break;
	}
}

int AnalyseGcard()
{
	gcardMem = zrenderer->GetTotalTextureMem();
}

void AnalyseMemory()
{
	_MEMORYSTATUS memInfo;
	GlobalMemoryStatus(&memInfo);

	memTotal = memInfo.dwTotalPhys;
	memAvail = memInfo.dwAvailPhys;
}

void zCSystemInfo::AnalyseNow()
{
	zINFO(3,"B: SYS: Analysing system ...", 109);
	AnalyseCpu();
	AnalyseOs();
	AnalyseGcard();
	AnalyseMemory();
	GlobalMemoryStatus(&Buffer);

	zINFO(4,"B: SYS: CPU-Name: " + cpuName); //117
	zINFO(4,"B: SYS: CPU-Speed: "_s + cpuMhz + " Mhz"); // 118

	zINFO(4,"B: SYS: RAM-Size: "_s + (memTotal >> 20) + " MB"); // 121
	zINFO(4,"B: SYS: RAM-Available: "_s + (memAvail >> 20) + " MB"); // 122

	zINFO(4,"B: SYS: Graphics-Card: " + gcardName); // 125
	zINFO(4,"B: SYS: Graphics-Memory: "_s + (gcardMem >> 20) + " MB"); // 126

	zINFO(4,"B: SYS: OS: " + GetOsName() + " [" + osCSDVersion + "]"); // 129

	zINFO(4,""); // 131
}

zSTRING oCSystemInfo::GetScoreDescr(float value)
{
	if (value < 0.0)
		return "NO WAY";
	if (value < 0.2)
		return "BAD";
	if (value < 0.4)
		return "OK";
	if (value < 0.6)
		return "GOOD";
	if (value < 0.8)
		return "GREAT";
	return "UNBELIEVABLE";
}

float oCSystemInfo::GetGraphicsBonus()
{
	float bonus = 0.0;
	if (zrenderer->HasCapability(6))
		bonus += 0.2;
	if (zrenderer->HasCapability(2) == 2)
		bonus += 0.1;
	else if (zrenderer->HasCapability(2) >= 3)
		bonus += 0.2;

	if (!zrenderer->HasCapability(6)) {
		zoptions->WriteBool("RENDERER_D3D", "zFogRadial", 0, 0);
		if ( zrenderer )
			zrenderer->SetRadialFog(0);
	}
	return bonus;
}

void oCSystemInfo::ResetSettings(float score)
{
	int gameSight = 4;
	bool musicEnabled = true;
	float gameTextureDetail = 1.0;
	float gameModelDetail   = 0.5;
	zSTRING settingsName = "???";

	if (score < 0.0) {
		settingsName = "calculated";

		if (cpuScore <= 0.2) {
			gameSight = 2;
			gameModelDetail = 0.3;
		} else if (cpuScore <= 0.4) {
			gameSight = 3;
			gameModelDetail = 0.4;
		} else if (cpuScore > 0.8) {
			gameSight = 5;
		}

		if (memScore <= 0.2) {
			gameSight -= 2;
			gameTextureDetail = 0.4;
			musicEnabled = false;
		} else if (memScore <= 0.4) {
			gameSight -= 1;
			gameTextureDetail = 0.6;
		} else if (memScore <= 0.8) {
			gameTextureDetail = 0.8;
		} else {
			gameSight += 1;
		}

		if (gmemScore <= 0.35) {
			gameTextureDetail -= 0.4;
			gameSight -= 2;
		} else if ( v2->gmemScore <= 0.60000002 ) {
			gameTextureDetail -= 0.2;
			gameSight -= 1;
		}

		zClamp01(gameTextureDetail);
		zClamp01(gameModelDetail);
	}

	if ( score < 0.33 ) {
		settingsName = "low Quality / high Speed";
		gameTextureDetail = 0.4;
		gameModelDetail = 0.2;
		gameSight = 2;
		musicEnabled = 0;
	} else if ( score < 0.66 ) {
		settingsName = "mid Quality / mid Speed";
		gameTextureDetail = 0.7;
		gameModelDetail = 0.5;
		gameSight = 3;
	} else {
		settingsName = "high Quality / high Speed";
		gameTextureDetail = 1.0;
		gameModelDetail = 0.8;
	}

	zoptions->WriteInt(zOPT_SEC_PERFORMANCE, "sightValue", gameSight, 0);
	zoptions->WriteReal(zOPT_SEC_INTERNAL, "texDetailIndex", gameTextureDetail, 0);
	zoptions->WriteReal(zOPT_SEC_PERFORMANCE, "modelDetail", gameModelDetail, 0);
	zoptions->WriteBool(zOPT_SEC_SOUND, "musicEnabled", musicEnabled, 0);

	zINFO(3, "B: SYS: Resulting settings: " + settings); // 98
	zINFO(3, "B: SYS: gameSight: "_s + gameSight); // 99
	zINFO(3, "B: SYS: gameModelDetail: "_s + gameModelDetail); // 100
	zINFO(3, "B: SYS: gameTextureDetail: "_s + gameTextureDetail); // 101
	if ( musicEnabled )
		zINFO(3, "B: SYS: Music enabled"); // 102
	else
		zINFO(3, "B: SYS: Music disabled"); // 103
	zINFO(3,""); // 104
}

void oCSystemInfo::ScoreNow()
{
	zINFO(3,"B: SYS: Scoring your system ..."); // 110,

	cpuScore = (double(cpuMhz) - cpuLow) / (cpuHigh - cpuLow);
	memScore = (double(memTotal) / (1 << 20) - memLow) / (memHigh - memLow);
	gmemScore = (double(gcardMem) / (1 << 20) - gmemLow) / (gmemHigh - gmemLow);
	graphicsScore = 0.5 + GetGraphicsBonus();
	soundScore = 0.75;

	double cpumem = cpuScore + memScore;
	avgScore = (cpumem + cpumem + graphScore) * 0.2;

	zINFO(3,"B: SYS: Score CPU: "_s + cpuScore + " [" + GetScoreDescr(cpuScore) + "]"); // 119
	zINFO(3,"B: SYS: Score Memory: "_s + memScore + " [" + GetScoreDescr(memScore) + "]"); // 120
	zINFO(3,"B: SYS: Score Graphics: "_s + graphicsScore + " [" + GetScoreDescr(graphicsScore) + "]"); // 121
	zINFO(3,"B: SYS: Score Sound: "_s + soundScore + " [" + GetScoreDescr(soundScore) + "]"); // 122
	zINFO(3,"B: SYS: Resulting Score: "_s + avgScore + " [" + GetScoreDescr(avgScore) + "]"); // 123

	scored = 1;
}
