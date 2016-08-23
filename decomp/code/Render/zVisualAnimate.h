class zCVisualAnimate : public zCVisual {
	Z_OBJECT(zCVisualAnimate);
public:
	zCVisualAnimate() = default;
	~zCVisualAnimate() override = default;

	virtual void StartAnimation(zSTRING const &) = 0;
	virtual void StopAnimation(zSTRING const &) = 0;
	virtual bool IsAnimationActive(zSTRING const &) = 0;
	virtual zCModelAni* GetAnyAnimation() = 0;
};
