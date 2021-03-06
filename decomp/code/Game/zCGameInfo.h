class zCGameInfo {
public:
	virtual void ~zCGameInfo();
	virtual void Init();
	virtual void Reset();

	virtual void SetName(zSTRING const& _name)
	{
		name = _name;
	}

	virtual zSTRING GetName() const
	{
		return name;
	}

	virtual void AddPlayer(zCPlayerInfo *);
	virtual void RemPlayer(zCPlayerInfo *);

	virtual void GetPlayerByID(int);
	virtual void GetPlayerByVobID(ulong);
	virtual void GetPlayerByNetAddress(zTNetAddress &);

	virtual void Pack(zCBuffer &,uchar);
	virtual void Unpack(zCBuffer&);

private:
	zSTRING name;
};
