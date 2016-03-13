class oCAIDrop : public oCAISound {
	Z_OBJECT();
public:
	virtual void Archive(zCArchiver& arc)
	{
		if (arc.InProperties()) {
			// commented out?
		}
	}
	virtual void Unarchive(zCArchiver &)
	{
		if (arc.InProperties()) {
			// commented out?
		}
	}
	virtual ~oCAIDrop();
	virtual void DoAI(zCVob *,int &);
	virtual void CanThisCollideWith(zCVob *);
	virtual void ReportCollisionToAI(zCCollisionReport const &);
	virtual void HasAIDetectedCollision();
};
