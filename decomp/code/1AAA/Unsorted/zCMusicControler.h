class zCMusicControler {
 virtual void _GetClassDef();
 virtual void ~zCMusicControler(uint);
 virtual void OnMessage(zCEventMessage	*,zCVob	*);
 virtual void ThisVobRemovedFromWorld(zCWorld *);
};
