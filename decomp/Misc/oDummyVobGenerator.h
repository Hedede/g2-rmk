class oCDummyVobGenerator : zCVob {
	Z_OBJECT(oCDummyVobGenerator);
public:
	oCDummyVobGenerator()
		: zCVob()
	{
		flags1.showVisual = false;
		SetCollDetStat(0);
		SetCollDetDyn(0);
		SetSleeping(0);
	}

	void ~oCDummyVobGenerator() override = default;
	void OnTrigger(zCVob* other, zCVob* instigator) override;
};

void oCDummyVobGenerator::OnTrigger(zCVob *other, zCVob *instigator)
{
	auto arrow = new oCItem("Arrow", 1); // 0x348
	auto ai = new oCAIArrow(); // 0x68

	arrow->SetAI(ai);
	arrow->SetSleeping(0);
	arrow->SetVobName("Pfeil von Dummy-Generator");
	homeWorld->AddVob(arrow);

	zMAT4 rot = trafoObjToWorld;
	rot.PostRotateY(90.0):
}
