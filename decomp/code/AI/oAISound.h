class oCAISound : public zCAIBase {
	Z_OBJECT(oCAISound);
public:
	void ~oCAISound() override
	{
		RemoveSlideSound();
	}

	void RemoveSlideSound()
	{
		if ( soundPlaying ) {
			soundPlaying = false;
			zsound->StopSound(soundHandle);
		}
	}

	bool CheckSlideSound(zCVob* vob)
	{
		if ( vob->flags1.physicsEnabled && (vob->GetRigidBody()->flags & 4) == 4 )
		{
			if ( soundPlaying )
				return zsound->UpdateSound3D(soundHandle, 0);
			int mg = 0;
			if ( vob->groundPoly )
				mg = vob->groundPoly->material->matGroup;
			soundPlaying = 1;
			soundHandle = zsndMan->StartSlideSound(vob, mg);

			return soundHandle != nullptr;
		}

		if ( soundPlaying ) {
			soundPlaying = 0;
			return zsound->StopSound(soundHandle);
		}

		return false;
	}

	void StartCollisionSound(zCVob* source, zCCollisionReport& rep)
	{
		int med1;
		int mat1 = GetSoundMaterial(source, med1);
		if ( rep->col2->GetCollObjClass() == &zCCollObjectLevelPolys::s_oCollObjClass )
		{
			switch ( rep->col2->polys[0]->material->matGroup ) {
			case 1:
				zsndMan->StartHitSound(source, med1, 3, mat1, 2, rep);
				break;
			case 3:
				zsndMan->StartHitSound(source, med1, 3, mat1, 0, rep);
				break;
			case 6:
				zsndMan->StartHitSound(source, med1, 3, mat1, 3, rep);
				break;
			case 4:
				zsndMan->StartHitSound(source, med1, 3, mat1, 4, rep);
				break;
			case 5:
				zsndMan->StartHitSound(source, med1, 3, mat1, 5, rep);
				break;
			default:
				zsndMan->StartHitSound(source, med1, 3, mat1, 1, rep);
				break;
			}
		} else {
			int med2;
			int mat2 = GetSoundMaterial(rep->col2->parent, med2);
			zsndMan->StartHitSound(source, med1, med2, mat1, mat2, rep);
		}
	}


private:
	int soundHandle;
	bool soundPlaying;
};

int oCAISound::GetSoundMaterial(zCVob *vob, zCSoundManager::zTSndManMedium& med)
{
	if ( !vob )
		return 99;

	if ( vob->type == VOB_TYPE_ITEM ) {
		med = 1;
		return vob->GetSoundMaterial();
	}

	if ( vob->type == VOB_TYPE_NPC ) {
		if (auto npc = zDYNAMIC_CAST<oCNpc>(vob)) {
			med = 1;
			if (auto armor = npc->GetEquippedArmor())
				return armor->GetSoundMaterial();
			return 0;
		}
	}

	if ( auto mob = zDYNAMIC_CAST<oCMob>(vob) ) {
		med = 1;
		return vob->GetSoundMaterial();
	}

	med = 1;
	if ( auto pm = zSTATIC_CAST<zCProgMeshProto>(vob->visual) ) {
		for (auto i = 0, i < pm->numSubMeshes; ++i) {
			while ( pm->subMeshes[i]->material->flags.noCollDet )
				continue;

			switch ( pm->subMeshes[i]->material->matGroup ) {
			case MAT_WOOD:
				return 0;
			case MAT_METAL:
				return 2;
			case MAT_SNOW:
				return 3;
			case MAT_EARTH:
				return 4;
			case MAT_WATER:
				return 5;
			default:
				return 6;
			}
		}
	}

	return 99;
}
