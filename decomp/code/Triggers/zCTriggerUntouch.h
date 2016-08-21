class zCTriggerUntouch {
 virtual void _GetClassDef();
 virtual void ~zCTriggerUntouch(uint);
 virtual void OnTrigger(zCVob *,zCVob *);
 virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void OnTouch(zCVob	*);
 virtual void OnUntouch(zCVob *);
};
