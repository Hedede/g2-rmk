class zCCamTrj_KeyFrame : public zCVob {
	Z_OBJECT(zCCamTrj_KeyFrame);
public:
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual ~zCCamTrj_KeyFrame();
	virtual void ThisVobAddedToWorld(zCWorld *);
	virtual void ThisVobRemovedFromWorld(zCWorld *);
private:
	float time;
	float angleRollDeg;
	float camFOVScale;
	int motionType;
	int motionTypeFOV;
	int motionTypeRoll;
	int motionTypeTimeScale;
	zBOOL timeIsFixed;
	float tension;
	float bias;
	float continuity;
	float timeScale;
	zMAT4 originalPose;
	zMAT4 pose;
	void* unk1;
	zCCSCamera* cam;
	void* unk3;
};

void zCCamTrj_KeyFrame::Archive(zCArchiver& arc)
{
	zCVob::Archive(arc);

	arc.WriteFloat("time", time);
	arc.WriteFloat("angleRollDeg", angleRollDeg);
	arc.WriteFloat("camFOVScale", camFOVScale);
	arc.WriteEnum("motionType", "UNDEF;SMOOTH;LINEAR;STEP;SLOW;FAST;CUSTOM", motionType);
	arc.WriteEnum("motionTypeFOV", "UNDEF;SMOOTH;LINEAR;STEP;SLOW;FAST;CUSTOM", motionTypeFOV);
	arc.WriteEnum("motionTypeRoll", "UNDEF;SMOOTH;LINEAR;STEP;SLOW;FAST;CUSTOM", motionTypeRoll);
	arc.WriteEnum("motionTypeTimeScale", "UNDEF;SMOOTH;LINEAR;STEP;SLOW;FAST;CUSTOM", motionTypeTimeScale);

	arc.WriteGroupBegin("Details");

	arc.WriteFloat("tension", tension);
	arc.WriteFloat("bias", bias);
	arc.WriteFloat("continuity", continuity);
	arc.WriteFloat("timeScale", timeScale);
	arc.WriteBool("timeIsFixed", timeIsFixed);
	arc.WriteGroupEnd(arc, "Details");

	if (!arc.InProperties())
		arc.WriteRaw("originalPose", originalPose, sizeof(zMAT4));

	if (arc.InSaveGame())
		;
}

void zCCamTrj_KeyFrame::Unarchive(zCArchiver& arc)
{
	zCVob::Unarchive(arc);

	arc.ReadFloat("time", time);
	arc.ReadFloat("angleRollDeg", angleRollDeg);
	arc.ReadFloat("camFOVScale", camFOVScale);
	arc.ReadEnum("motionType", motionType);
	arc.ReadEnum("motionTypeFOV", motionTypeFOV);
	arc.ReadEnum("motionTypeRoll", motionTypeRoll);
	arc.ReadEnum("motionTypeTimeScale", motionTypeTimeScale);
	arc.ReadFloat("tension", tension);
	arc.ReadFloat("bias", bias);
	arc.ReadFloat("continuity", continuity);
	arc.ReadFloat("timeScale", timeScale);
	arc.ReadBool("timeIsFixed", timeIsFixed);

	if ( !arc.InProperties() ) {
		arc.ReadRaw("originalPose", originalPose, sizeof(zMAT4));
		pose = originalPose;
	}
}
