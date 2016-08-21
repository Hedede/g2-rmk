class zCViewStatusBar : public zCView {
public:
	virtual void ~zCViewStatusBar(uint);
	virtual void SetMaxRange(float,float);
	virtual void SetRange(float,float);
	virtual void SetPreview(float);
	virtual void SetValue(float);
};
