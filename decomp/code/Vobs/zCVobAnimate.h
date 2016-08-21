class zCVobAnimate : public zCEffect {
	Z_OBJECT(zCVobAnimate);
public:
	zCVobAnimate()
	{
		ignoredByTraceRay = 1;
	}

	virtual ~zCVobAnimate() = default;

	virtual void Archive(zCArchiver& arc)
	{
		zCEffect::Archive(arc);
		arc.WriteBool("startOn", startOn);
		if ( arc.InSaveGame() )
			arc.WriteBool("isRunning", isRunning);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCEffect::Unarchive(arc);

		isRunning = 0;

		startOn = arc.ReadBool("startOn");

		if ( arc.InSaveGame() )
			isRunning = arc.ReadBool("isRunning");

		if ((arc.InSaveGame() && isRunning) || startOn ) {
			isRunning = 1;

			auto visAni = dynamic_cast<zCVisualAnimate>(visual);
			if ( visAni ) {
				auto ani = visAni->GetAnyAnimation();
				if (ani)
					visAni->StartAnimation(ani);
			}
		}
	}

	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void SetVisual(zCVisual *);

private:
	bool startOn   = true;
	bool isRunning = false;
};
