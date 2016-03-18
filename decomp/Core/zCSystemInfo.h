class zCSystemInfo {
public:
	virtual ~zCSystemInfo();
	virtual void AnalyseNow();
	virtual void ScoreNow() = 0;

protected:
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

	int osVersion;
	zSTRING osName;
	zSTRING osCSDVersion;
	int analyzed;
	zSTRING cpuName;
	int cpuMhz;
	uint32_t memTotal;
	uint32_t memAvail;
	zSTRING gcardName;
	uint32_t gcardMem;
};

class oCSystemInfo : public zCSystemInfo {
public:
	virtual ~oCSystemInfo();
	virtual void AnalyseNow()
	{
		zCSystemInfo::AnalyseNow();
	}
	virtual void ScoreNow();
	virtual void ResetSettings(float);
private:
	int scored;
	float cpuLow;
	float cpuHigh;
	float memLow;
	float memHigh;
	float gmemLow;
	float gmemHigh;
	float cpuScore;
	float memScore;
	float gmemScore;
	float gcardScore;
	float soundScore;
	float avgScore;
};
