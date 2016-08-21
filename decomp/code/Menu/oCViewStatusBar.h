class oCViewStatusBar : public zCViewStatusBar {
public:
	virtual ~oCViewStatusBar();
	virtual void SetMaxRange(float,float);
	virtual void SetRange(float,float);
	virtual void SetPreview(float);
	virtual void SetValue(float);
};
