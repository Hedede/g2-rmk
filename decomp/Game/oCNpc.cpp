oCNpc::oCNpc()
{
	invSlots.InsertEnd(new TNpcSlot(NPC_MODE_RIGHTHAND));
	invSlots.InsertEnd(new TNpcSlot(NPC_MODE_LEFTHAND));
	invSlots.InsertEnd(new TNpcSlot(NPC_MODE_SWORD));
	invSlots.InsertEnd(new TNpcSlot(NPC_MODE_LONGSWORD));
	invSlots.InsertEnd(new TNpcSlot(NPC_MODE_BOW));
	invSlots.InsertEnd(new TNpcSlot(NPC_MODE_CROSSBOW));
	invSlots.InsertEnd(new TNpcSlot(NPC_MODE_HELMET));
	invSlots.InsertEnd(new TNpcSlot(NPC_MODE_TORSO));

	flags.csAllowedAsRole = 1;
	flags.askroutine = 1;

	InitDamage();

	oCNpc::ai_disabled = 0;

	attribute[ATR_REGENERATEHP]   = 600000;
	CheckModelOverlays();
	attribute[ATR_REGENERATEMANA] = 600000;
	CheckModelOverlays();

	SetPerceptionTime(5000.0);

	inventory.SetOwner(this);
	statec.SetOwner(this);

	RbtReset();

	type = VOB_TYPE_NPC;

	SetCollisionClass(oCCollObjectCharacter::s_oCollObjClass);
	SetCollDetStat(1);
	SetCollDetDyn(1);

	magFrontier.SetNPC(this);

	CreateTalentList();
}

~oCNpc::oCNpc()
{
	zINFO(5,"U: NPC: Deleting NPC" + GetInstanceName()); // 1533

	CleanUp();

	for (auto slot : invSlots) {
		invSlots.Remove(slot);
		delete slot;
	}

	for (auto& talent : talents) {
		Release(talent);
	}

	s_activeInfoCache.Delete(this);
	s_shrinkCache.Delete(this);

	if (oCNpc::player == this)
		player = nullptr;

	zparser.ResetWithVarReferenceList(s_npcVarReferenceList); // my name

	if (classDef.numLiving <= 1) {
		// бредятина, в которой не хочу разбираться,
		// код поход на заинлайненый zCCache::Clear,
		// но при этом s_modelOverlayCache (я его так назвал,
		// в ехешнике не было имени) представляет собой только
		// zCSparseArray<TOverlayIndex, ModelWhatever*>, без приблуд
		// zCCache.
		s_modelOverlayCache.Clear();
	}

	for (auto& effect : effectList) {
		// забавно, что он вызов Remove
		// превращает очистку массива в O(n^2)
		// т.к. Remove ищет указатель в массиве
		effectList.Remove(effect);
		effect->Kill();
		effect->Release();
	}

	timedOverlays.DeleteListDatas();
}

void oCNpc::Archive(zCArchiver& arc)
{
	if ( oCNpc::dontArchiveThisNpc == this )
		zFAULT("U: NPC: Object tries to save reference of player during levelchange. This is not allowed.");

	auto visual = this->visual;
	if ( visual )
		visual->AddRef();

	SetVisual(0);
	oCVob::Archive(arc);
	SetVisual(visual);

	if (visual)
		visual->Release();

	auto sym = zparser.GetSymbol(GetInstance());

	zSTRING npcInstance;
	if ( sym ) {
		npcInstance = sym->name;
	} else {
		zWARNING("U: NPC:Save:Instance not found : " + GetInstanceName());
		npcInstance = GetObjectName();
	}

	arc.WriteString("npcInstance", npcInstance);
	if ( !arc.InSaveGame() )
		return;

	arc.WriteVec3("modelScale",    model_scale);
	arc.WriteFloat("modelFatness", model_fatness);

	int numOverlays = activeOverlays.GetNumInList();
	arc.WriteInt("numOverlays", numOverlays);

	for (int i = numOverlays; i > 0; --i)
		arc.WriteString("overlay", activeOverlays[i]);

	if ( arc.InProperties(arc) )
		arc.WriteGroupBegin("scripted");

	arc.WriteInt("flags", v2->variousFlags);
	arc.WriteInt("guild", v2->guild);
	arc.WriteInt("guildTrue", v2->guildTrue);
	arc.WriteInt("level", v2->level);
	arc.WriteInt("xp", v2->experience_points);
	arc.WriteInt("xpnl", v2->experience_points_next_level);
	arc.WriteInt("lp", v2->learn_points);
	arc.WriteInt("numTalents", v2->talents.numInArray);

	for ( i = 0; i < v2->talents.numInArray; ++i )
		(*arc)->WriteObject_p(arc, v2->talents.array[i]);

	arc.WriteInt("fightTactic", fighttactic);
	arc.WriteInt("fightMode",   fmode);
	arc.WriteBool("wounded",    wounded);
	arc.WriteBool("mad",    mad);
	arc.WriteInt("madTime", mad_heal);

	if ( arc.InProperties(arc) ) {
		arc.WriteGroupEnd("scripted");
		return;
	}

	arc.WriteBool("player", this == oCNpc::player);

	for (int i = 0; i < ATR_INDEX_MAX; ++i)
		arc.WriteInt("atr" + i, attribute[i])

	for (int i = 1; i < MAX_HITCHANCE; ++i)
		arc.WriteInt("hc" + i, hitChance[i]);

	for (int i = 0; i < MAX_MISSIONS; ++i)
		arc.WriteInt("mission" + i, mission[i]);

	zSTRING str;
	if ( startAIState > 0 ) {
		auto sym = zparser.GetSymbol(startAIState);
		if ( sym )
			tmp = sym->name;
	}

	arc.WriteString("startAIState", str);
	arc.WriteRaw("scriptVars",  aiscripts, 400);
	arc.WriteString("scriptWp", wpname);
	arc.WriteInt("attitude",    attitude);
	arc.WriteInt("tmpAttitude", tmp_attitude);
	arc.WriteInt("nameNr", namenr);

	arc.WriteRaw("spells", spells, 4);

	knownNews.Archive(arc);

	arc.WriteObject("carryVob", carry_vob);

	if ( IsSelfPlayer() )
		SetEnemy(0);

	if ( enemy == oCNpc::dontArchiveThisNpc )
		SetEnemy(0);

	arc.WriteObject("enemy", enemy);
	arc.WriteBool("moveLock", movlock);

	inventory.Archive(arc);

	int numInv = invSlots.GetNumInList();
	arc.WriteInt("numInvSlots", numInv);

	for (int i = 0; i < invSlots; ++i) {
		auto slot = invSlots[i];
		auto item = slot->object;

		bool used = item != 0;

		if (item) {
			auto name = item->GetObjectName();
			if ( name == "ITLSTORCHBURNING")
				used = false;

			if (interactItem == item || slot->unko > 0 )
				used = false;
			
			if (!used)
				zWARNING("U: NPC: InteractItem " + GetObjectName() + " not saved.");
		}

		arc.WriteBool("used", used);
		arc.WriteBool("name", slot->name);

		if (used) {
			arc.WriteObject("vob", slot->object);
			arc.WriteBool("inInv", slot->inInv);
		}
	}

	states.Archive(arc);

	auto sym = zparser.GetSymbol(daily_routine);
	zSTRING routineName = sym ? sym->name : "";

	arc.WriteString("curRoutine", routineName);
	arc.WriteBool("respawn", respawnOn);
	arc.WriteInt("respawnTime", >spawnDelay);
	arc.WriteRaw("protection", protection, 32);
	arc.WriteInt("bsInterruptableOverride", bodyStateInterruptableOverride);
	arc.WriteInt("npcType", npcType);
	arc.WriteInt("spellMana", spellMana);
}



void oCNpc::Unarchive(zCArchiver& arc)
{
	oCVob::Unarchive(arc);

	zSTRING instName;
	arc.ReadString("npcInstance", instName);
	int index = zparser.GetIndex(instName);
	if (index < 0) {
		SetAI(0);
		instanz = -1;

		zSTRING msg = "U: NPC:Load:Instance not found : Vobname :" +
		              GetObjectName() +  "/ Scriptinstance:" +
		              instName + ". Correct instancename or remove Npc !";

		zFAULT(msg);
	} else {
		InitByScript(index, arc.InSaveGame());
	}

	if ( !arc.InSaveGame() )
		return;

	arc.ReadVec3("modelScale", model_scale);
	arc.ReadFloat("modelFatness", model_fatness);

	int numOverlays = 0;
	arc.ReadInt("numOverlays", numOverlays);
	for (int i = numOverlays ; i > 0; --i) {
		zSTRING tmp = arc.ReadString("overlay");
		tmp.Upper();

		if (tmp.Search(0, "HUMANS_TORCH", 1) == -1)
			continue;

		activeOverlays.InsertSort(tmp);
	}

	if ( callback_ai ) {
		AvoidShrink(1000);
	}

	arc.ReadInt("flags", variousFlags);
	arc.ReadInt("guild", guild);
	arc.ReadInt("guildTrue", guildTrue);
	arc.ReadInt("level", level);
	arc.ReadInt("xp", experience_points);
	arc.ReadInt("xpnl", experience_points_next_level);
	arc.ReadInt("lp", learn_points);

	int numTalents;
	arc.ReadInt("numTalents", numTalents);

	if (numTalents > talents.GetNumInList())
		zWARNING("U: NPC: Found more talents in archive than used in game!");
	else if (numTalents < talents.GetNumInList())
		zWARNING("U: NPC: Found less talents in archive than used in game!");

	if (numTalents > talents.GetNumInList())
		numTalents = talents.GetNumInList();

	for (int i = 0; i < numTalents; ++i) {
		if (talents[i]) {
			talents[i]->Release();
			talents[i] = 0;
		}

		auto obj = ReadObject(arc, 0);
		auto talent = zDYNAMIC_CAST<oCNpcTalent>(obj);

		if (talent) {
			if ( talent->skill > 100 ) {
				zSTRING msg = "C: corrupt archive found: npc " + GetName() +
				              " has an illegal talent skill for index: " + i;
				zWARNING(msg);
				talent->skill = 1;
			}

			SetTalentSkill(i, talent->skill);
			SetTalentValue(i, talent->value);
			talent->Release();
		}
	}

	uint32_t tmp;
	arc.ReadInt("fightTactic", &this_->fighttactic);
	arc.ReadInt("fightMode", &this_->fmode);
	arc.ReadBool("wounded", tmp);
	wounded = tmp;
	arc.ReadBool("mad", tmp);
	mad = tmp
	arc.ReadInt("madTime", mad_heal);

	if (arc.InProperties())
		return;

	bool isPlayer;
	arc.ReadBool("player", isPlayer);

	for (int i = 0; i < ATR_INDEX_MAX; ++i)
		arc.ReadInt("atr" + i, attribute[i]);

	for (int i = 1; i < MAX_HITCHANCE; ++i)
		arc.ReadInt("hc" + i, hitchance[i]);

	for (int i = 0; i < MAX_MISSIONS; ++i)
		arc.ReadInt("mission" + i, mission[i]);

	zSTRING startState;
	arc.ReadString("startAIState", startState);
	if (startState != "") {
		int idx = zparser.GetIndex(startState);
		startAIState = idx;
		if (idx < 0) {
			zSTRING msg = "U: NPC: Cannot reinitialize AI-State :" +
			              startState + " / " + GetInstanceName();
			zFAULT(msg);
		}
	}
	arc.ReadRaw("scriptVars", aiscripts, 400);
	arc.ReadString("scriptWp", wpname);
	arc.ReadInt("attitude", attitude);
	arc.ReadInt("tmpAttitude", tmp_attitude);
	arc.ReadInt("nameNr", namenr);
	arc.ReadRaw("spells", spells, 4);

	knownNews.Unarchive(arc);

	carry_vob = arc.ReadObject("carryVob", 0);
	auto obj = arc.ReadObject("enemy", &this->enemy);
	SetEnemy(obj);
	if ( obj )
		obj->Release();

	arc.ReadBool("moveLock", tmp);
	movlok = tmp;

	inventory.ClearInventory();
	inventory.Unarchive(arc);

	invSlots.DeleteListDatas();

	int numInvSlots;
	arc.ReadInt("numInvSlots", numInvSlots);

	for (int = 0; i < numInvSlots; ++i) {
		bool used = arc.ReadBool("used");
		zSTRING name = arc.ReadString("name");

		auto slot = new TNpcSlot;

		slot->inInv = 0;
		slot->state = 0;
		slot->name = name;

		invSlots.InsertEnd(slot);

		if ( used ) {
			auto vob = arc.ReadString(arc, "vob");

			slot = invSlots[i];
			slot->SetVob(vob);

			arc.ReadBool("inInv", slot->inInv);
			if ( slot->inInv ) {
				auto item = zDYNAMIC_CAST<oCItem>(slot->object);
				if (item && !item->HasFlag(0x40000000)) {
					item->SetFlag(0x40000000);
					item->CreateVisual();

					auto node = GetModel()->SearchNode(name);
					if ( node )
						GetModel()->SetNodeVisual(node, item->visual, 0);
					if (slot->object)
						SetItemEffectControledByModel(item, node, 1, 0, 0);
				}
			}
		}
	}

	if ( carry_vob )
		carry_vob->Release();

	states.Unarchive(arc);

	if ( callback_ai ) {
		auto ai = zDYNAMIC_CAST<oCAIHuman>(callback_ai);
		if ( humanAI ) {
			AvoidShrink(1000);
			if ( !this->human_ai ) {
				humanAI.AddRef();
				this->human_ai = humanAI;
			}

			human_ai->SetActionMode(human_ai->actionMode);

			if ( fmode < 0 || fmode >= 8 )
				fmode = 0;

			human_ai->SetFightAnis(fmode);
			human_ai->SetWalkMode(human_ai->walkmode);
			human_ai->SearchStandAni(1);
		}
	}

	if ( isPlayer && !oCNpc::player )
		oCNpc::player = this;

	auto curRoutine = arc.ReadString("curRoutine");

	index = zparser.GetIndex(curRoutine);
	if ( index >= 0 && index != daily_routine )
		states.ChangeRoutine(index);

	arc.ReadBool2(arc, "respawn", tmp);
	flags ^= (flags ^ (16 * tmp)) & 0x10;

	arc.ReadInt("respawnTime", spawnDelay);
	arc.ReadRaw("protection", protection, 32);
	arc.ReadInt("bsInterruptableOverride", bodyStateInterruptableOverride);
	arc.ReadInt("npcType", npcType);
	arc.ReadInt("spellMana", spellMana);

	if ( visual )
		InitModel();

	if ( human_ai )
		human_ai->Init(this);

	AddEffect(this->effect, 0);
}



oCItem* oCNpc::DetectItem(int flags, int)
{
	if (IsSelfPlayer())
		return nullptr;

	oCItem* result = nullptr;
	double minDist = 3.4028235e38;
	for (int i = 0, num = vobList.Size(); i < num; ++i) {
		oCItem* item = zDYNAMIC_CAST<oCItem>(vobList[i]);
		if (!item)
			continue;

		if (item->HasFlag(ITEM_NFOCUS))
			continue;

		if ((item->mainflag | item->flags) & flags) {
			float dist = GetDistanceToVob(next);
			if (dist < minDist) {
				minDist = dist;
				result = next;
			}
		}
	}

	return result;
}


oCNpc* oCNpc::DetectPlayer()
{
	if (IsSelfPlayer())
		return nullptr;

	oCNpc* player = nullptr;
	for (int i = 0, num = vobList.Size(); i < num; ++i) {
		oCNpc* npc = zDYMANIC_CAST<oCNpc>(vobList[i]);
		if (!npc)
			continue;

		if (npc->IsAIPlayer()) {
			player = npc;
			break;
		}
	}

	return player;
}

void oCNpc::CreateSpell(int spell)
{
	if ( !mag_book )
	{
		mag_book = new oCMag_Book();
	}
}

void oCNpc::LearnSpell(int spell)
{
	// bitfield3
	if (this->spells & (1 << spell)  <= 0)
	{
		CreateSpell(spell);
		spells |= 1 << spell;
	}
}

oCItem * oCNpc::GetTradeItem()
{
	if ( !trader )
		return nullptr;

	zCListSort<oCItem> contents = trader->GetContents()->next;

	if (contents)
		return contents->data;
	return nullptr;
}

int oCNpc::HasVobDetected(zCVob *vob)
{
	if ( IsSelfPlayer() )
		return false;

	return vobList.IsInList(vob);
}

int oCNpc::GetGuildAttitude(int guild) const
{
	return ogame->GetGuilds()->GetAttitude(guildTrue, guild);
}

int oCNpc::GetAttitude(oCNpc *other)
{
	if (!other)
		return ATT_NEUTRAL;

	if (other->IsAIPlayer()) {
		if (tmp_attitude == attitude)
			return attitude;
		else
			return tmp_attitude;
	}

	return GetGuildAttitude(other->guild);
}

int oCNpc::GetPermAttitude(oCNpc *other)
{
	if (other->IsAIPlayer())
		return attitude;
	return GetGuildAttitude(other->guild);
}

void oCNpc::ChangeAttribute(int attr, int value)
{
	if (value == 0)
		return;

	if ( value < 0 )
		if ( oCNpc::player == this && oCNpc::godmode )
			return;

	if (variousFlags & NPC_FLAG_IMMORTAL > 0)
		if (value != -999)
			return;
		
	attribute[attr] += value;
	if ( attribute[attr] < 0 )
		attribute[attr] = 0;

	if ( attr == ATR_HITPOINTS )
	{
		if ( attribute[ATR_HITPOINTS] > attribute[ATR_HITPOINTS_MAX] )
			attribute[ATR_HITPOINTS] = attribute[ATR_HITPOINTS_MAX];
	}
	else if ( attr == ATR_MANA )
	{
		if ( attribute[ATR_MANA] > attribute[ATR_MANA_MAX] )
			attribute[ATR_MANA] = attribute[ATR_MANA_MAX];
	}

	CheckModelOverlays();
}

void oCNpc::OpenScreen_Status()
{
	auto status = new oCStatusScreen;
	auto menu = status->menu;

	auto infoAttr = new oSMenuInfoAttribute;
	infoAttr->Value    = attribute[ATR_STRENGTH];
	infoAttr->MaxValue = attribute[ATR_STRENGTH];
	infoAttr->Type = 3;
	sym = zparser->GetSymbol("TXT_ATTRIBUTE_DESC");
	sym->GetValue(infoAttr->Description, 4);
	menu->AddAttribute(menu, infoAttr);

	infoAttr = new oSMenuInfoAttribute;
	infoAttr->Value    = attribute[ATR_DEXTERITY];
	infoAttr->MaxValue = attribute[ATR_DEXTERITY];
	infoAttr->Type = 1;
	sym = zparser->GetSymbol("TXT_ATTRIBUTE_DESC");
	sym->GetValue(infoAttr->Description, 5);
	menu->AddAttribute(menu, infoAttr);


	infoAttr = new oSMenuInfoAttribute;
	infoAttr->Value    = attribute[ATR_MANA];
	infoAttr->MaxValue = attribute[ATR_MANA_MAX];
	infoAttr->Type = 2;
	sym = zparser->GetSymbol("TXT_ATTRIBUTE_DESC");
	sym->GetValue(infoAttr->Description, 2);
	menu->AddAttribute(menu, infoAttr);


	infoAttr = new oSMenuInfoAttribute;
	infoAttr->Value    = attribute[ATR_HITPOINTS];
	infoAttr->MaxValue = attribute[ATR_HITPOINTS_MAX];
	infoAttr->Type = 0;
	sym = zparser->GetSymbol("TXT_ATTRIBUTE_DESC");
	sym->GetValue(infoAttr->Description, 0);
	menu->AddAttribute(menu, infoAttr);

	oCMenu_Status::SetExperience(level,experience_points, experience_points_next_level);
	oCMenu_Status::SetLearnPoints(learn_points);

	oSMenuInfoArmor prot;
	prot.Value = etProtectionByType(DAM_BLUNT);
	menu->AddArmor(prot);
	prot.Value = oCNpc::GetProtectionByType(DAM_POINT);
	menu->AddArmor(prot);
	prot.Value = oCNpc::GetProtectionByType(DAM_FIRE);
	menu->AddArmor(prot);
	prot.Value = oCNpc::GetProtectionByType(DAM_MAGIC);
	menu->AddArmor(prot);

	for (int i = 0; i < NPC_TALENT_MAX; ++i) {
		if (talent == NPC_TALENT_UNKNOWN)
			continue;

		auto talentInfo = new oSMenuInfoTalent;
		oCNpc::GetTalentInfo(i,
		                talent->Name,
		                talent->Description,
		                talent->SkillEnum);
		auto talent = talents[i];

		if ( talent ) {
			talentInfo->Skill = talent->skill;
			switch ( talent->talent ) {
			case NPC_TALENT_1H:
				talentInfo->Value = hitChance[1];
				break;
			case NPC_TALENT_2H:
				talentInfo->Value = hitChance[2];
				break;
			case NPC_TALENT_BOW:
				talentInfo->Value = hitChance[3];
				break;
			case NPC_TALENT_CROSSBOW:
				talentInfo->Value = hitChance[4];
				break;
			default:
				talentInfo->Value = talent->value;
				break;
			}
		} else {
			talentInfo->Value = 0;
			talentInfo->Skill = 0;
		}

		menu->AddTalent(talentInfo);
	}

	zSTRING npcName = GetName(1);
	oCMenu_Status::SetPlayerName(npcName);
	zSTRING guildName = GetGuildName(name);
	oCMenu_Status::SetGuild(guildName);

	int showBars = oCGame::GetShowPlayerStatus(ogame);
	oCGame::SetShowPlayerStatus(ogame, 0);
	oCStatusScreen::Show(status);
	oCGame::SetShowPlayerStatus(ogame, showBars);
}

void oCNpc::GetTalentInfo(oENpcTalent id, zSTRING& Name, zSTRING& Description, zSTRING& SkillEnum)
{
	zCPar_Symbol *txt_talents = zparser->GetSymbol("TXT_TALENTS");
	zCPar_Symbol *txt_talents_desc = zparser->GetSymbol("TXT_TALENTS_DESC");
	zCPar_Symbol *txt_talents_skills = zparser->GetSymbol("TXT_TALENTS_SKILLS");

	Name.Clear();
	Description.Clear();
	SkillEnum.Clear();

	if ( txt_talents )
		txt_talents->GetValue(Name, id);
	if ( txt_talents_desc )
		txt_talents_desc->GetValue(Desc, id);
	if ( txt_talents_skills )
		txt_talents_skills->GetValue(SkillEnum, id);
}


// NOT IN ORIGINAL
void oCNpc::GetWeaponModeFromString(zSTRING const& str)
{
	if (str == "FIST")
		return FMODE_FIST;
	if (str == "1H" || str == "1HS")
		return FMODE_1H;
	if (str == "2H" || str == "2HS")
		return FMODE_2H;
	if (str == "BOW")
		return FMODE_BOW;
	if (str == "CBOW")
		return FMODE_CBOW;
	if (str == "MAGIC")
		return FMODE_MAGIC;
	return FMODE_NONE;
}

void oCNpc::SetWeaponMode2(zSTRING const& fmode_str)
{
	int fmode = GetWeaponModeFromString(fmode_str);
	SetWeaponMode2(fmode);
}

TNpcSlot* oCNpc::GetInvSlot(zSTRING const& slotName)
{
	for (int i = 0, num = invSlots.GetNumInList(); i < num; ++i) {
		if (invSlots[i] == slotName)
			return invSlots[i];
	}

	return nullptr;
}

oCItem* oCNpc::GetSlotVob(zSTRING const& slotName)
{
	oCItem* vob = nullptr;
	TNpcSlot* slot = GetInvSlot(NPC_MODE_SWORD);

	if (slot)
		vob = zDYMANIC_CAST<zCVob>(slot->object);

	return vob;
}

oCItem* oCNpc::GetSlotItem(zSTRING const& slotName)
{
	oCItem* item = nullptr;
	TNpcSlot* slot = GetInvSlot(NPC_MODE_SWORD);

	if (slot)
		item = zDYMANIC_CAST<oCItem>(slot->object);

	return item;
}

oCItem* oCNpc::GetEquippedArmor()
{
	return GetSlotItem(NPC_MODE_TORSO);
}

oCItem oCNpc::GetEquippedMeleeWeapon()
{
	oCItem* item = GetSlotItem(NPC_MODE_SWORD);

	if (!item)
		item = GetSlotItem(NPC_MODE_LONGSWORD);

	return item;
}

oCItem oCNpc::GetEquippedRangedWeapon()
{
	oCItem* item = GetSlotItem(NPC_MODE_BOW);

	if (!item)
		item = GetSlotItem(NPC_MODE_CROSSBOW);

	return item;
}

void oCNpc::DoSpellBook()
{
	mag_book->DoPerFrame();
}

void oCNpc::InsertActiveSpell(oCSpell* spell)
{
	if (spell) {
		spell->AddRef();
		activeSpell.Insert(spell);
	}
}

// Unused, 100% sure was called by Npc_SetToMad, but was cut out
void oCNpc::SetToMad(float a3)
{
	mad_heal = a3; // wtf?
	if ( human_ai->IsStanding() && !(bitfield0 & mad) ) {
		if ( GetOverlay("_mad") ) {
			ApplyOverlay(this, "_mad");
			human_ai->InitAnimations();
			bitfield0 |= mad;
		}
	}
}

void oCNpc::AvoidShrink(int timeout)
{
	if ( !recursion && mds_name != "" ) {
		recursion = 1;
		shrinkHelperCache[this];
		recursion = 0;
	}
}

int oCNpc::IsInGlobalCutscene()
{
	if ( GetEM()->GetCutscene() ) { // zCCutscene*
		auto props = GetEM()->GetCutscene()->properties;
		if ( props )
			return props->globalCutscene;
	}
	return 0;
}
