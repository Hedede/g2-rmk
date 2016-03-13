class oCSystemInfo : public zCSystemInfo {
public:
	virtual ~oCSystemInfo();
	virtual void AnalyseNow();
	virtual void ScoreNow();
	virtual void ResetSettings(float);
};
