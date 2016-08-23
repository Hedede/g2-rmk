struct zCInputCallback {
	zCInputCallback() = default;
	virtual int HandleEvent(int);
	static void GetInput();
private:
	zCList<zCInputCallback> callbacksList;
};

void zCInputCallback::GetInput()
{
	int key = zinput->GetKey(1, 1);
	if ( !key )
		return;

	auto next = callbacksList;
	for (auto next = callbacksList; next; next = next->next) {
		auto callback = next->data;

		if (callback->HandleEvent(key))
			break;
	}

	zinput->ProcessInputEvents(zinput);
}
