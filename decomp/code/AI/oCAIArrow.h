class oCAIArrow : public oCAIArrowBase {
	Z_OBJECT(oCAIArrow);
public:
	virtual void Archive(zCArchiver& arc)
	{
		oCAIArrowBase::Archive(arc);

		if ( !arc.InProperties() && arc.InSaveGame() ) {
			arc.WriteObject("arrow", arrow);

			auto owner = ownerNpc;
			if ( oCNpc::dontArchiveThisNpc == owner )
				owner = nullptr;
			arc.WriteObject("ownerNpc", owner);
		}
	}
	virtual void Unarchive(zCArchiver& arc)
	{
		oCAIArrowBase::Unarchive(arc);
		if ( !arc.InProperties() && arc.InSaveGame() ) { {
			for (auto i = this->voblist; i; i = i->next )
				i->data->Release();

			if ( voblist ) {
				delete voblist;
				voblist = 0;
			}

			if ( arrow ) {
				arrow->Release();
				arrow = 0;
			}

			if ( ownerNpc ) {
				ownerNpc->Release();
				ownerNpc = 0;
			}

			if ( unk2 ) {
				unk2->Release();
				unk2 = 0;
			}
			arrow = arc.ReadObject("arrow", 0);
			ownerNpc = arc.ReadObject("ownerNpc", 0);
		}
	}
	virtual ~oCAIArrow();
	virtual void DoAI(zCVob *,int &);
	virtual void CanThisCollideWith(zCVob *);
	virtual void ReportCollisionToAI(zCCollisionReport const &);
};
