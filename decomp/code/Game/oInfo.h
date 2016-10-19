//_Ulf/oInfo.cpp
class oCInfoChoice {
	~oCInfoChoice() = default;
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
	oCInfo() = default;
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
		if ( !zparser.CreateInstance(instance, &par) )
			zWARNING("Info instance not found.");
	}

	virtual void RestoreParserInstance()
	{
		zparser.SetInstance(instance, &parserdef);
	}

	zSTRING& GetText { return par.description; }

	void* GetDataAdr() { return &par; }

	int GetConditionFunc() const { return par.conditions; }
	int GetNpcID() const { return par.npc; }
	int GetInstance() const { return instance; }

	int GetDataSize() const { return sizeof(par); }

	void SetTold(zBOOL b) { told = b; }
	bool WasTold() const
	{
		if (par.permanent)
			return false;
		return told;
	}

	int InfoConditions()
	{
		return *zparser.CallFunc(par.conditions);
	}

	void Info()
	{
		zparser.CallFunc(par.information);
		SetTold(true);
	}

	void RemoveAllChoices()
	{
		for (int i = 0, e = listChoices.GetNumInList(); i < e; ++i)
			listChoices.RemoveIndex(i);
		listChoices.DeleteList();
	}

	void AddChoice(zSTRING strText, int nFunc)
	{
		listChoices.Insert({strText, nFunc});
	}

	void RemoveChoice(zSTRING strText)
	{
		// dontcare
		int idx = listChoices.Search(strText);
		listChoices.Remove(idx);
	}

	void SetInstance(int nr)
	{
		int typ;
		instance = nr;
		name = zparser.GetSymbolInfo(instance, &typ, &nr);
	}

	int DoCheck()
	{
		if ( par.npc <= 0 ) {
			zWARNING("U: INF: Skriptinfo:npc is not/wrong initialised:" + par.description + " " + par.information); // 465
		}
		return 1;
	}

private:
	//Achtung: Nur "name" und "told" wandert in ein Savegame!
	oCInfo* next;
	zSTRING name;

	//Wohlbekannt:
	struct TParserDef {
		int     npc = 0;
		int     nr = -1;
		zBOOL   important = false;
		int     conditions  = -1;
		int     information = -1;
		zSTRING description;
		zBOOL   trade     = false;
		zBOOL   permanent = false;
	} par;

	//schonmal abgespielt (das was Npc_KnowsInfo abfragt)
	zBOOL told = false;

	//symbolindex der oCInfo im Parser.
	int instance = -1;

	//Choices:
	zCList< oCInfoChoice > listChoices;
};
