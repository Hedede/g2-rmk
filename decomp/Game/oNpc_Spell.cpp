oCSpell* oCNpc::IsSpellActive(int spellId)
{
	for (auto& spell : activeSpells) {
		if (spell->GetSpellID() == spellId)
			return spell;
	}
	return nullptr;
}

void oCNpc::SetActiveSpellInfo(int value)
{
	if ( GetWeaponMode() == FMODE_MAGIC ) {
		if ( mag_book ) {
			oCSpell* spell = mag_book->GetSelectedSpell();
			if ( spell )
				spell->SetSpellInfo(value);
		}
	}
}
