class oCWaypoint : public zCWaypoint {
	Z_OBJECT(oCWaypoint);
public:
	virtual ~oCWaypoint();

	virtual void CanBeUsed(zCVob const *);
};
