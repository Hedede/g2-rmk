class oCWaypoint : public zCWaypoint {
	Z_OBJECT(oCWaypoint);
public:
	oCWaypoint() = default;
	~oCWaypoint() override = default;

	void CanBeUsed(zCVob const* vob) override
	{
		if (auto npc = dynamic_cast<oCNpc const*>(vob)) {
			return waterDepth <= 0.0 ||
			       npc->CanSwim()    ||
			       npc->CanDive() && underWater;
		}
		return true;
	}
};
