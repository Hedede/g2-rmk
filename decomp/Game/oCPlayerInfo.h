class zCPlayerInfo {
public:
	virtual ~zCPlayerInfo();
	virtual void Init(zSTRING const &,void *);
	virtual void Init(zSTRING const &,zSTRING const &);
	virtual void SetName(zSTRING const &);
	virtual void SetPlayerVob(zCVob *);
	virtual void Reset();
	virtual void Deactivate(int);
	virtual void IsDeactivated();
	virtual void SetReady(int);
	virtual void IsReady();
	virtual void Pack(zCBuffer	&);
	virtual void Unpack(zCBuffer &);
};

class oCPlayerInfo : public zCPlayerInfo {
public:
	virtual ~oCPlayerInfo();
	virtual void Init(zSTRING const &, zSTRING const &);
	virtual void Reset();
	virtual void Pack(zCBuffer &);
	virtual void Unpack(zCBuffer &);
	virtual void SetInstance(TPlayerInstance);
	virtual void SetInstanceByName(zSTRING const &);
};
