struct oCMenu_ChgKeys : zCMenu {
	oCMenu_ChgKeys() = default;
	virtual ~oCMenu_ChgKeys() = default;

	void HandleEvent(int key) override;
	void ScreenInit() override
	{
		if ( !screenInitDone ) {
			InitText();
			zCMenu::ScreenInit();
		}
	}

	void Leave() override;
	void ExecCommand(zSTRING const& cmd) override;

	void InitText();

private:
	// no additional data members
};

void oCMenu_ChgKeys::Leave()
{
	zCMenu::Leave();
	for (auto item : listItems) {
		auto& option = item->parOnChgSetOption;
		if (option.Length() > 0) {
			void* buf;
			if (!zoptions->ReadRaw(zOPT_SEC_KEYS, option, buf, 0, 0))
				zoptions->RemoveEntry(zOPT_SEC_KEYS, option);
		}
	}

	zinput->BindKeys(0);
}

int oCMenu_ChgKeys::HandleEvent(int key)
{
	auto ret = zCMenu::HandleEvent(key);
	if ( key == KEY_DELETE &&
	     listItems[mainSel]->parOnSelAction[0] == SEL_ACTION_EXECCOMMANDS )
	{
		auto word = listItems[mainSel]->parOnSelAction_S.PickWord(2, " ", zSTR_SKIP);
		if ( !word.IsEmpty() ) {
			auto it = zCMenuItem::GetByName(word);
			it->SetText("", 0, 0);
			it->Draw();
			it->parUserString[0].Clear();
			it->parUserString[1].Clear();

			key = 0;
			zoptions->WriteRaw(zOPT_SEC_KEYS, it->parOnChgSetOption, &key, 2, 0);
			zinput->BindKeys(0);

			// 			delete 0;
		}
		result = 1;
	}
	return result;
}

void oCMenu_ChgKeys::InitText()
{
	for (auto item : listItems) {
		if (item->parOnChgSetOption.Length()) {
			auto opt = item->parOnChgSetOption;
			short* raw;
			auto sz = zoptions->ReadRaw(zOPT_SEC_KEYS, opt, &raw, 0, 0);
			item->parUserString[0] = "";
			item->parUserString[1] = "";

			for (auto i = 0; i < sz / 2; ++i) {
				auto str = zCInput::GetNameByControlValue(raw[i]);
				item->parUserString[i] = str;
			}

			auto str = item->parUserString[0];
			if (item->parUserString[1].Length())
				str += ", " + item->parUserString[1];

			item->SetText(str, 0, 0);
		}
	}
}

int oCMenu_ChgKeys::ExecCommand(zSTRING const& cmd)
{

	auto word = cmd.PickWord(1, " ", zSTR_SKIP);

	if (word == "RUN") {
		auto word = cmd.PickWord(2, " ", zSTR_SKIP);

		auto it = zCMenuItem::GetByName(word);
		it->SetIsActive(1);

		short key = 0;
		do {
			sysEvent();
			zinput->ProcessInputEvents();
			zCMenu::GetActive()->Render();
			key = zinput->GetKey(1, 1);
			auto name = zCInput::GetNameByControlValue(key);
			if (!name) key = 0
		}
		while (!key || key == KEY_RETURN || key < 0 );

		zinput->ClearKeyBuffer();
		if ( key != KEY_ESCAPE ) {
			auto name = zCInput::GetNameByControlValue(key);
			if (it->parUserString[0] != name && it->parUserString[1] != name) {
				zSTRING* str;
				if ( it->parUserString[1].IsEmpty() ) {
					str = &it->parUserString[1];
				} else if ( it->parUserString[0].IsEmpty() ) {
					str = &it->parUserString[0];
				} else {
					str = &it->parUserString[1];
					it->parUserString[0] = it->parUserString[1];
				}

				*str = name;

				zSTRING tmp = parUserString[0]; // name?
				if ( parUserString[1] )
					tmp += ", " + parUserString[1];
				it->SetText(tmp, 0, 0);
				it->Draw();

				// find conflicts
				for (auto m = 0; m < listItems.numInArray; ++m ) {
					auto it2 = listItems[m];
					if ( it2 == it ) continue;
					if (it2->parUserString[0] == name || it2->parUserString[1] == name) {
						zCArray<unsigned short> keys;
						zSTRING key_name;
						if ( it->parUserString[0] == name ) {
							if ( it2->parUserString[1]) {
								it2->parUserString[0] = item->parUserString[1];
								it2->parUserString[1].Clear();
								key_name = it2->parUserString[0];
								keys.InsertEnd(zCInput::GetControlValueByName(key_name));
							} else {
								key_name = "";
								it2->parUserString[0].Clear();
								it2->parUserString[1].Clear();
							}
						} else {
							it2->parUserString[1].Clear();
							key_name = it2->parUserString[0];
							keys.InsertEnd(zCInput::GetControlValueByName(key_name));
						}

						if (keys.Size()) {
							zoptions->WriteRaw(zOPT_SEC_KEYS, it2->parOnChgSetOption, keys.Data(), keys.Size(), 0);
						} else {
							short zero = 0;
							zoptions->WriteRaw(zOPT_SEC_KEYS, it2->parOnChgSetOption, &zero, 0, 0);
						}

						it2->SetText(key_name, 0, 0);
						it2->Draw();
					}
				}
				zCArray<unsigned short> keys;
				if ( it->parUserString[0] )
					keys.InsertEnd(zCInput::GetControlValueByName(it->parUserString[0]));
				if ( it->parUserString[1] )
					keys.InsertEnd(zCInput::GetControlValueByName(it->parUserString[1]));
				short zero = 0;
				if (keys.Size())
					zoptions->WriteRaw(zOPT_SEC_KEYS, it2->parOnChgSetOption, keys.Data(), keys.Size(), 0);
				else
					zoptions->WriteRaw(zOPT_SEC_KEYS, it2->parOnChgSetOption, &zero, 0, 0);
				zinput->BindKeys(0);
			}
		}

		it->SetIsActive(0);
	} else if (word == "SETDEFAULT") {
		for ( auto it : listItems ) {
			if ( it->parOnChgSetOption )
				zoptions->RemoveEntry(&zOPT_SEC_KEYS, parOnChgSetOption);
		}
		zinput->BindKeys(0);
		InitText();
		for ( auto it : listItems ) {
			if ( it->parOnChgSetOption )
				it->Draw();
		}
	} else if (word == "SETALTERNATIVE") {
		for ( auto it : listItems ) {
			if ( it->parOnChgSetOption )
				zoptions->RemoveEntry(&zOPT_SEC_KEYS, parOnChgSetOption);
		}
		zinput->BindKeys(1);
		InitText();
		for ( auto it : listItems ) {
			if ( it->parOnChgSetOption )
				it->Draw();
		}
	}

	return 1;
}
