class oCInfoChoice
{
    zSTRING Text;
    int Function;  //symbolindex    
};

/* Große Vorsicht!

Im Gegensatz zu C_NPC und C_ITEM zeigt eine C_INFO nicht auf den vergrößerten
Speicherbereich, sondern tatsächlich auf sich selbst.
MEM_InstGetOffset liefert also, auf eine C_INFO angewendet NICHT die Adresse
des umrahmenden oCInfo Objekts sondern die Adresse der enthaltenen C_INFO zurück.
Um an die oCInfo zu kommen müssen 4 + 4 + 20 = 28 abgezogen werden! */

class oCInfo {
public:
	oCInfo();
	virtual ~oCInfo();
	virtual void Archive(zCArchiver& arc)
	{
		if (arc.InSaveGame()) {
			arc.WriteBool("Told", told);
			arc.WriteString("InstName", name);
		}
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		if (!arc.InSaveGame())
			return;

		arc.ReadBool("Told", told);
		arc.ReadString("InstName", name);

		int instance = zparser.GetIndex(name);
		if ( !zparser.CreateInstance(instance, &parserdef) )
			zWARNING("Info instance not found.");
	}

	virtual void RestoreParserInstance()
	{
		zparser.SetInstance(instance, &parserdef);
	}

private:
	//Achtung: Nur "name" und "told" wandert in ein Savegame!
	oCInfo* next;
	zSTRING name;

	//Wohlbekannt:
	struct TParserDef {
		int     npc;
		int     nr;
		zBOOL   important;
		int     conditions;
		int     information;
		zSTRING description;
		zBOOL   trade;
		zBOOL   permanent;
	} parserdef;

	//schonmal abgespielt (das was Npc_KnowsInfo abfragt)
	zBOOL told;

	//symbolindex der oCInfo im Parser.
	int instance;

	//Choices:
	zCList< oCInfoChoice > listChoices;
};
