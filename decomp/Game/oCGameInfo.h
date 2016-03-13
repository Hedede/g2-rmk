class oCGameInfo : public zCGameInfo {
public:
	virtual ~oCGameInfo();
	virtual void Init();
	virtual void Pack(zCBuffer&, uchar);
	virtual void Unpack(zCBuffer&);
};
