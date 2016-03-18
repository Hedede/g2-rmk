class zCSystemInfo {
public:
	zCSystemInfo() = default;
	virtual ~zCSystemInfo() = default;
	virtual void AnalyseNow();
	virtual void ScoreNow() = 0;

	zSTRING GetOsName() const
	{
		return osName;
	}

protected:
	int AnalyseGcard();
	void AnalyseMemory();

	int osVersion = 0;
	zSTRING osName;
	zSTRING osCSDVersion;

	int analyzed = 0;

	zSTRING cpuName;

	int cpuMhz = 555;
	uint32_t memTotal = 123 * 1024 * 1024;
	uint32_t memAvail = 0;
	zSTRING gcardName;
	uint32_t gcardMem = 33  * 1024 * 1024;
};

class oCSystemInfo : public zCSystemInfo {
public:
	oCSystemInfo() = default;
	virtual ~oCSystemInfo() = default;

	virtual void AnalyseNow()
	{
		zCSystemInfo::AnalyseNow();
	}
	virtual void ScoreNow();
	virtual void ResetSettings(float);
private:
	int scored = 0;
	float cpuLow   = 700.0;
	float cpuHigh  = 1200.0;
	float memLow   = 256.0;
	float memHigh  = 512.0;
	float gmemLow  = 16.0;
	float gmemHigh = 64.0;
	float cpuScore   = 1.0;
	float memScore   = 1.0;
	float gmemScore  = 1.0;
	float gcardScore = 1.0;
	float soundScore = 1.0;
	float avgScore   = 1.0;
};
