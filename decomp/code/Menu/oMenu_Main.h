struct oCMenu_Main : public zCMenu {
	void HandleEvent(int key) override;
	oCMenu_Main(zSTRING const& name) : zCMenu(name) {}
	~oCMenu_Main() override = default;
	void ScreenInit() override { zCMenu::ScreenInit(); }
	void ScreenDone() override { zCMenu::ScreenDone(); }
};

int oCMenu_Main::HandleEvent(int key)
{
	auto ret = zCMenu::HandleEvent(key);
	if ( !ret ) {
		auto boundkey = zinput->GetFirstBindedLogicalKey(key);
		if ( !boundkey ) {
			// _key = KEY_LEFTARROW;
			switch ( key ) {
			case 0x20A: case 0x20B:
			case KEY_DOWNARROW:
			case KEY_UPARROW:
				return zCMenu::HandleEvent(key);
			case KEY_RIGHTARROW:
			case KEY_LEFTARROW:
			case MOUSE_BUTTONRIGHT:
				ret = 1;
				break;
			case MOUSE_BUTTONLEFT:
			case KEY_RETURN:
				zsound->PlaySound(zCMenu::selectSnd, 0);
				eventOccured[1] = 1;
				exitState = 2;
				ret = 1;
				break;
				goto LABEL_17;
			case KEY_ESCAPE:
				goto LABEL_16;
			default:
				break;
			}
		} else {
			ret = 1;
		}
	}

	updateTimer = zCMenu::timer->frameTime + updateTimer;
	if ( updateTimer > eventTimerUpdateMSec ) {
		updateTimer = 0;
		eventOccured[4] = 1;
	}
	return ret;
}
