struct oCSVM {
	oCSVM() = default;
	~oCSVM()
	{
		delete[] strings;
	}

	void InitByScript(int id);
	int GetOU(zSTRING const& name);

private:
	int      count   = 0;
	int      symbol  = 0;
	zSTRING* strings = 0;
};

void oCSVM::InitByScript(int id)
{
	int instance = zparser.GetIndex("SVM_"_s + id);
	if ( instance < 0 ) {
		zWARNING("U: SVM: Voice Module not found :"); // 146 oSvm.cpp
		return;
	}

	symbol = zparser.GetIndex("C_SVM");
	if ( symbol < 0 ) {
		zWARNING("U: SVM: Voice Module Class (C_SVM) not found."); // 143;
		return;
	}

	auto sym = zparser.GetSymbol(symbol);
	svm_count = sym->bitfield & 0xFFF;

	strings = new zSTRING[svm_count];

	zparser.CreateInstance(instance, strings);
}

int oCSVM::GetOU(zSTRING const& name)
{
	if (svm_count <= 0)
		return -1;

	zCPar_Symbol* sym = nullptr;
	for (int i = 0; i < svm_count; ++i) {
		sym = zparser.GetSymbol(symbol + i + 1);
		if (!sym)
			continue;

		auto str = sym->name;
		str.Delete("C_SVM.", 0);

		if (str != name)
			continue;

		if (!ogame->GetCutsceneManager())
			return -1;

		if (strings[i]->IsEmpty())
			return -1;

		return ogame->GetCutsceneManager()->LibValidateOU(strings[i]);
	}

	return -1;
}
