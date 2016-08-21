class oCAIVobMove : public oCAISound {
	Z_OBJECT(oCAIVobMove);
public:
	virtual void Archive(zCArchiver& arc)
	{
		if ( arc->vtbl->InSaveGame(arc) ) {
			zCVob* vob = this->vob;
			if ( oCNpc::dontArchiveThisNpc == vob )
				vob = nullptr;
			arc.WriteObject("vob", vob);

			zCVob* owner = this->owner;
			if ( oCNpc::dontArchiveThisNpc == owner )
				owner = nullptr;
			arc.WriteObject("owner", owner);
		}
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		if (!arc.InSaveGame())
			return;

		for ( i = voblist->next; i; i = i->next )
			i->data->Release();

		voblist.DeleteList();

		if ( vob ) {
			vob->Release();
			vob = 0;
		}

		if ( owner ) {
			owner->Release();
			owner = 0;
		}

		vob = arc.ReadObject("vob", vob);
		owner = arc.ReadObject("owner", owner);
		if ( owner ) {
			voblist.Insert(owner);
			owner->AddRef();
		}
	}
	virtual ~oCAIVobMove();
	virtual void DoAI(zCVob *,int &);
	virtual void CanThisCollideWith(zCVob *);
	virtual void ReportCollisionToAI(zCCollisionReport const &);
	virtual void Init(zCVob *,zCVob *,zVEC3 &,float,float,zMAT4 *);
};
