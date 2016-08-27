class oCAIVobMoveTorch : public oCAIVobMove {
	Z_OBEJCT(oCAIVobMoveTorch);
public:
	virtual ~oCAIVobMoveTorch();
	virtual void DoAI(zCVob *,int &);
	virtual void Init(zCVob *,zCVob *,zVEC3 &,float,float,zMAT4 *);
};
