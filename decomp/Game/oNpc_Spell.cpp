void oCNpc::OpenSpellBook(int showMagicCircle)
{
	if ( mag_book ) {
		bool movmode = isInMovementMode;
		if ( isInMovementMode )
			EndMovement(1);

		mag_book->SetOwner(this);
		mag_book->Open(showMagicCircle);

		if ( movmode )
			BeginMovement();
	}
}

void oCNpc::CloseSpellBook(int removeall)
{
	if (mag_book)
		mag_book->Close(removeall)
}

void oCNpc::DestroySpellBook();
{
	if (mag_book)
		delete mag_book;
	mag_book = 0;
}

void oCNpc::InsertActiveSpell(oCSpell* spell)
{
	if (spell) {
		spell->AddRef();
		activeSpell.Insert(spell);
	}
}

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

int oCNpc::GetActiveSpellLevel() const
{
	if ( GetWeaponMode() == FMODE_MAGIC ) {
		if (mag_book) {
			oCSpell* spell = mag_book->GetSelectedSpell();
			if ( spell )
				return spell->GetLevel();
		}
	}
	return -1;
}

int oCNpc::GetActiveSpellCategory() const
{
	if ( GetWeaponMode() == FMODE_MAGIC ) {
		if (mag_book) {
			oCSpell* spell = mag_book->GetSelectedSpell();
			if ( spell )
				return spell->GetCategory();
		}
	}
	return -1;
}

int oCNpc::GetActiveSpellNr() const
{
	if ( GetWeaponMode() == FMODE_MAGIC ) {
		if (mag_book) {
			oCSpell* spell = mag_book->GetSelectedSpell();
			if ( spell )
				return spell->GetSpellID();
		}
	}
	return -1;
}

int oCNpc::GetActiveSpellIsScroll()
{
	if ( GetWeaponMode() == FMODE_MAGIC ) {
		if (mag_book) {
			int spell = mag_book->GetSelectedSpellNr();
			oCItem* item = mag_book->GetSpellItem(spell);
			return item && item->MultiSlot();
		}
	}
	return -1;
}
