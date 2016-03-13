class zCAICamera : public zCAIBase {
	Z_OBJECT(zCAICamera);
public:
	virtual ~zCAICamera();
	virtual void DoAI(zCVob *,int &);
};
