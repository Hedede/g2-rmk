class oCMob : public zCVob {
	virtual zCClassDef* GetClassDef();
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual void ~oCMob();
	virtual void OnDamage(zCVob *,zCVob *,float,int,zVEC3 const &);
	virtual void OnMessage(zCEventMessage *,zCVob *);
	virtual void CanThisCollideWith(zCVob *);
	virtual void SetVisual(zCVisual *);
	virtual void IsOwnedByGuild(int);
	virtual void IsOwnedByNpc(int);
	virtual void GetSoundMaterial_AM(zCSoundManager::zTSndManMedium &,oTSndMaterial &,int);
	virtual void SetSoundMaterial(oTSndMaterial);
	virtual void GetSoundMaterial();
	virtual void SetName(zSTRING const &);
	virtual void GetName();
	virtual zCModel* void GetModel()
	{
		return dynamic_cast<zCModel*>(visual);
	}
	virtual void GetScemeName();
	virtual void Destroy();
	virtual void AllowDiscardingOfSubtree();
private:
	//Mobeigenschaften:
	// Symbolischer Name, z.B. "MOBNAME_DOOR"
	zSTRING name;

	uint32_t hitpoints : 12;
	uint32_t damage    : 12;
	uint32_t isDestroyed    : 1;
	uint32_t moveable       : 1;  //    25;
	uint32_t takeable       : 1;  //    26;
	uint32_t focusOverride  : 1;  //    27;
	uint32_t sndMat         : 3;  //    28; //oTSndMaterial 


	zSTRING visualDestroyed;
	zSTRING ownerStr;
	zSTRING ownerGuildStr;

	int owner;
	int ownerGuild;
	
	//Index des Parsersymbols, dass den Namen enthält,
	//zum Beispiel Index des Symbols MOBNAME_DOOR.
	int focusNameIndex;

	zCList<zCVob>     ignoreVobList;
};
