class oCVisFX_MultiTarget : public oCVisualFX {
	Z_OBJECT(oCVisFX_MultiTarget);
public:
	virtual void Archive(zCArchiver&)
	{
	}
	virtual void Unarchive(zCArchiver&)
	{
	}
	virtual ~oCVisFX_MultiTarget();
	virtual void Open();
	virtual void InvestNext();
	virtual void Cast(int);
	virtual void Stop(int);
	virtual void Play(float,zMAT4	const *,zMAT4 const *);
	virtual void IsFinished();
	virtual void SetByScript(zSTRING const	&);
	virtual void Reset();
	virtual void SetDamage(float);
	virtual void SetDamageType(int);
	virtual void Init(zCArray<zCVob *>);
	virtual void SetLevel(int);
};
