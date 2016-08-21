void oCMob::Archive(zCArchiver& arc)
{
	oCVob::Archive(arc);

	arc.WriteGroupBegin("MOB");

	arc.WriteString("focusName", name);
	arc.WriteInt("hitpoints", hitpoints);
	arc.WriteInt("damage", damage);
	arc.WriteBool("moveable", moveable);
	arc.WriteBool("takeable", takeable);
	arc.WriteBool("focusOverride", focusOverride);

	arc.WriteEnum("soundMaterial", "WOOD;STONE;METAL;LEATHER;CLAY;GLAS", GetSoundMaterial());
	arc.WriteString("visualDestroyed", visualDestroyed);
	arc.WriteString("owner", ownerStr);
	arc.WriteString("ownerGuild", ownerGuildStr);

	arc.WriteGroupEnd("MOB");

	if ( !arc.InProperties() )
		arc.WriteBool("isDestroyed", isDestroyed);
}

void oCMOB::Unarchive(zCArchiver& arc)
{
	oCVob::Unarchive(arc);

	arc.ReadString("focusName", name);

	uint32_t tmp;
	arc.ReadInt("hitpoints", tmp);
	hitpoints = tmp;
	arc.ReadInt("damage", tmp);
	damage = tmp;
	arc.ReadBool("moveable", tmp);
	moveable = tmp;
	arc.ReadBool("takeable", tmp);
	takeable = tmp;
	arc.ReadBool("focusOverride", tmp);
	focusOverride = tmp;
	arc.ReadEnum("soundMaterial", tmp);
	soundMaterial = tmp;

	arc.ReadString("visualDestroyed", visualDestroyed);
	arc.ReadString("owner", ownerStr);
	arc.ReadString("ownerGuild", ownerGuildStr);

	if ( !arc.InProperties() ) {
		arc.ReadBool("isDestroyed", tmp);
		isDestroyed = tmp;

		owner = zparser.GetIndex(ownerStr);

		auto ownerGuild = zparser.GetSymbol(ownerGuildStr);

		int guild = -1;
		if ( ownerGuild )
			ownerGuild.GetValue(guild, 0);

		ownerGuild = guild;
	}

	if (!mobNameConv)
		SetName(name);

	if (mobNameConv && !name.IsEmpty()) {
		zSTRING tmp = name;
		if (tmp.Search(0, "MOBNAME_", 1) < 0)
			tmp = "MOBNAME_" + name;

		tmp.Upper();

		for (int i = 0; i < tmp.Length(); ++i) {
			if (tmp[i] < ' ')
				tmp[i] = '_';
		}

		SetName(tmp);
		if ( mobSaveConv ) {
			if (mobList.Search(tmp) < 0) {
				mobList.InsertEnd(tmp);
				zoptions.ChangeDir(8);
				auto file = zfactory->CreateZFile("MOBSI.D");
				if ( file->Exists() ) {
					file->Open(1);
					file->Append();
				} else {
					file->Create();
				}

				file->Write("CONST STRING " + tmp + " = \"" + name + "\"\n");
				file->Close();
				delete file;
			}
		}
	}
}
