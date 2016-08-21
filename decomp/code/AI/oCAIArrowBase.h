class oCAIArrowBase : public oCAISound {
	Z_OBJECT(oCAIArrowBase);
public:
	virtual void Archive(zCArchiver& arc);
	{
		if ( !arc.InProperties() && arc.InSaveGame() ) {
			zCVob* vob = this->vob;
			if ( oCNpc::dontArchiveThisNpc == vob )
				vob = nullptr;
			arc.WriteObject("vob", vob);
			arc.WriteBool("collision", collision);
			arc.WriteFloat("timeLeft", timeLeft);
			arc.WriteBool("startDustFX", startDustFX);
		}
	}
	virtual void Unarchive(zCArchiver& arc);
	{
		if ( !arc.InProperties() && arc.InSaveGame() ) {
			if ( vob ) {
				vob->Release();
				vob = 0;
			}

			vob = arc.ReadObject("vob", 0);
			arc.ReadBool("collision", collision);
			arc.ReadFloat("timeLeft", timeLeft);
			arc.ReadBool("startDustFX", startDustFX);
			if ( unk1 ) {
				unk1->Release();
				unk1 = 0;
			}
		}
	}
	virtual ~oCAIArrowBase();
	virtual void DoAI(zCVob *,int &);
	virtual void ReportCollisionToAI(zCCollisionReport const&);
	virtual void HasAIDetectedCollision();
	virtual void AICollisionResponseSelfDetected(zVEC3 const &,int &);
	virtual void GetIsProjectile();
};
