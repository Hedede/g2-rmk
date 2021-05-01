oCItem* oCNpc::PutInInv(zSTRING const& name, int anz)
{
	auto idx = zparser.GetItem(name);
	return PutInInv(idx, anz);
}

oCItem* oCNpc::RemoveFromInv(oCItem* item, int amount)
{
	if (item) {
		item = inventory.Remove(item, amount);
		if (item && item->HasFlag(ITEM_ACTIVE)) {
			Equip(item);
		}
	}
	return item;
}

oCItem* oCNpc::RemoveFromInv(int inst, int amount)
{
	oCItem* removed = inventory.Remove(inst, amount);
	if ( removed && removed->HasFlag(ITEM_ACTIVE) )
		Equip(removed);
	return removed;
}

oCItem* oCNpc::GetFromInv(int instance, int amount)
{
	auto item = inventory.IsIn(inventory, amount);
	if (item && item->amount != amount) {
		auto orig = item;
		item = item->CreateCopy();
		item->amount = amount;
		orig->amount -= amount;
	}
	return item;
}

int oCNpc::HasMissionItem()
{
	inventory.UnpackCategory();
	unsigned num = inventory.GetNumItemsInCategory();

	for (unsigned i = 0; i < num; ++i) {
		auto item = inventory.GetItem(i);
		if ( item && item->HasFlag(ITEM_MISSION) )
			return 1;
	}
	return 0;
}

oCItem* oCNpc::GetTradeItem()
{
	if ( !trader )
		return nullptr;

	zCListSort<oCItem> contents = trader->GetContents()->next;

	if (contents)
		return contents->data;
	return nullptr;
}

int oCNpc::CanCarry(oCItem *item)
{
	if ( inventory.CanCarry(item) )
		return 1;
	if ( item->MultiSlot() ) {
		auto inst = item->GetInstance();
		if ( inventory.IsIn(inst, 1) )
			return 1;
	}

	human_ai->model->StartAni("T_CANNOTTAKE");
	return 0;
}

void oCNpc::CloseSteal()
{
	if (oCNpc::stealcontainer) {
		oCNpc::stealcontainer->Close();
		oCNpc::game_mode = 0;
	}
}

void oCNpc::CloseInventory()
{
	inventory.Close();
	CloseTradeContainer();
}

void oCNpc::CloseDeadNpc()
{
	if ( npccontainer ) {
		CloseInventory();

		npccontainer->Close();

		Delete(npccontainer);

		oCNpc::game_mode = 0;
	}
}

int oCNpc::EquipBestArmor()
{
	oCItem* armor = nullptr;

	inventory.UnpackCategory();
	unsigned num = inventory.GetNumItemsInCategory();
	for (unsigned i = 0; i < num; ++i) {
		auto item = inventory.GetItem(index);
		if ( item && CanUse(item) && item->HasFlag(ITEM_KAT_ARMOR) ) {
			armor = item;
			break;
		}
	}

	if ( armor && !armor->HasFlag(ITEM_ACTIVE) )
		EquipArmor(armor);
}

void oCNpc::EquipBestWeapon(int mode)
{
	oCItem* item = nullptr;
	if ( mode == FMODE_MELEE ) {
		inventory.UnpackCategory();
		unsigned num = inventory.GetNumItemsInCategory();
		for (unsigned i = 0; i < num; ++i) {
			item = inventory.GetItem(index);
			if ( item && CanUse(item) && item->HasFlag(ITEM_KAT_NF) )
				break;
		}
	}

	if ( mode == FMODE_FAR ) { // 4 (== FMODE_2H) v .exe
		inventory.UnpackCategory();
		unsigned num = inventory.GetNumItemsInCategory();
		for (unsigned i = 0; i < num; ++i) {
			item = inventory.GetItem(index);
			if ( item && CanUse(item) && item->HasFlag(ITEM_KAT_FF) ) {
				if (IsMunitionAvailable(item))
					break;
			}
		}
	}

	if (item && !item->HasFlag(ITEM_ACTIVE))
		EquipWeapon(item);
}

void oCNpc::OpenInventory(int mode)
{
	if ( ztimer.frameTimeFloat != 0.0 ) {
		if ( !IsBodyStateInterruptable() )
			return;

		inventory.SetMode(mode);
		if ( !inventory.IsOpen(y) && !IsMonster() ) {
			auto aniCtrl = GetAnictrl()
			if ( !aniCtrl ||  !aniCtrl->IsInWeaponChoose() )
			{
				if ( GetWeaponMode() ) {
					auto msg = new oCMsgWeapon(EV_REMOVEWEAPON, 0, 0);
					GetEM()->OnMessage( this, msg );
				}

				if ( GetAnictrl() )
					GetAnictrl()->StopTurnAnis();

				inventory.DisableManipulateItems(0);
				inventory.DisableTransferMoreThanOneItem(0);

				inventory.SetName(this->GetName());

				inventory.Open(&this->, IsSelfPlayer() ? 0x1000 : 0, 0, v16);
			}
		}
	}
}

oCItem* oCNpc::GetWeapon()
{
	TNpcSlot* invSlot = nullptr;
	switch (fmode)
	{
	case FMODE_NONE:
	case FMODE_FIST:
		break;
	case FMODE_MELEE:
	case FMODE_1H:
	case FMODE_2H:
	case FMODE_CBOW:
		invSlot = GetInvSlot(NPC_NODE_RIGHTHAND); // was inlined
		break;
	case FMODE_MAGIC:
	case FMODE_BOW:
		invSlot = GetInvSlot(NPC_NODE_LEFTHAND); // was inlined
		break;
	};

	if (invSlot)
		return zDYNAMIC_CAST<oCItem*>(invSlot->object);

	return nullptr;
}


bool oCNpc::IsInvSlotAvailable(zSTRING const& slotName)
{
	return GetInvSlot(slotName) != nullptr;
}
