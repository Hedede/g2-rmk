int Print()
{
	zCParser* parser = zCParser::GetParser();

	zSTRING text;
	parser->GetParameter(&text);

	ogame->GetTextView()->PrintWin(text + "");

	return 0;
}

int PrintDebug()
{
	zCParser* parser = zCParser::GetParser();

	zSTRING text;
	parser->GetParameter(&text);

	if (zCView::GetShowDebug()) {
		zERROR::Message("U:Skript: " + text);
	}
	return 0;
}

int PrintScreen()
{
	int unk;
	zSTRING text;
	int x, y;
	zSTRING font;
	int timeSec;

	zCParser* parser = zCParser::GetParser();
	parser->GetParameter(&timeSec);
	parser->GetParameter(&font);
	parser->GetParameter(&y);
	parser->GetParameter(&x);
	parser->GetParameter(&text);
	parser->GetParameter(&unk);

	if (text.IsEmpty()) {
		parser->SetReturn(1);
		// return 0;
	}

	zCView* view = ogame->array_view[0];
	if ( view ) {
		int ret = 0;
		if (!font.IsEmpty()) {
			zSTRING tmp;
			view->SetFont(font);
			view->GetFont()->GetFontName(tmp);

			ret = font == tmp;
		}

		int x1 = (int)((double)x * 81.919998);
		int y1 = (int)((double)y * 81.919998);
		double time = double(1000 * timeSec);

		if ( x == -1 && y == -1)
		{
			game->array_view[0]->PrintTimedCXY(&text, time, 0);
		}
		else if ( x == -1)
		{
			game->array_view[0]->PrintTimedCX(y1, &text, time, 0);
		}
		else if ( y == -1 )
		{
			game->array_view[0]->PrintTimedCY(x1, &text, time, 0);
		}
		else
		{
			game->array_view[0]->PrintTimed(x1, y1, &text, time, 0);
		}

		parser->SetReturn(ret);
	} else {
		parser->SetReturn(0);
	}
	return 0;
}

int PrintDialog()
{
	int dialogNr;
	zSTRING text;
	int x, y;
	zSTRING font;
	int timeSec;

	zCParser* parser = zCParser::GetParser();
	parser->GetParameter(&timeSec);
	parser->GetParameter(&font);
	parser->GetParameter(&y);
	parser->GetParameter(&x);
	parser->GetParameter(&text);
	parser->GetParameter(&dialogNr);
	if ( !ogame->array_view[0] )
	{
		zCParser::SetReturn(parser, 0);
		return 0;
	}

	if ( oCNpc::player )
	{
		auto* omsg = new oCMsgConversation(21, oCNpc::player);
		omsg->posX = x;
		omsg->posY = y;
		omsg->timeSec = (double)timeSec;
		omsg->dialogNr = dialogNr;
		omsg->font = font;
		omsg->text = text;

		evtmgr = oCNpc::player->GetEM(0);
		evtmgr->OnMessage(omsg, oCNpc::player);
	}
	zCParser::SetReturn(parser, 1);
	return 0;
}

int PrintDialog()
{
	int dialogNr;
	zSTRING text;
	int x, y;
	zSTRING font;
	int timeSec;

	zCParser* parser = zCParser::GetParser();
	parser->GetParameter(&timeSec);
	parser->GetParameter(&font);
	parser->GetParameter(&y);
	parser->GetParameter(&x);
	parser->GetParameter(&text);
	parser->GetParameter(&dialogNr);
	if ( !ogame->array_view[dialogNr] ) {
		zCParser::SetReturn(parser, 0);
		return 0;
	}
	int retval = 0;
	oCGame::OpenView(ogame, dialogNr);
	auto* view = ogame->array_view[dialogNr];
	if ( font.size() != 0 )
	{
		view->SetFont(&font);
		auto vfont = view->GetFont();

		retval = (vfont.GetFontName() == font);
	}
	xf = (double)x * 81.919998;
	yf = (double)x * 81.919998;
	zCOLOR color;
	color.color[2] = -1;
	color.color[1] = -1;
	color.color[0] = -1;
	color.color[3] = -1;
	double time = 1000 * timeSec;
	if ( x == -1 && y == -1)
	{
		view->DialogCXY(&text, time, color);
	}
	else if ( y == -1 )
	{
		view->DialogCY(xf, &text, time, color);
	}
	else
	{
		view->Dialog(xf, yf, &text, time, color);
	}
	zCParser::SetReturn(parser, fontfound);
	return 0;
}

int PrintDebugInst()
{
	zCParser *parser = zCParser::GetParser();
	auto* self = parser->GetSymbol("SELF");
	zVob* selfinst = nullptr;
	if (self)
		selfinst = (zCVob *)self.GetInstanceAdr();
	zSTRING text;
	parser->GetParameter(&text);
	if (ogame->IsDebuggingAllInstances() ||
	    ogame->IsDebuggingInstance(slfinst) )
	{
		if ( zCView::GetShowDebug() ) {
			zERROR::Message("U: Sktipt: " + msg);
		}
	}
	return 0;
}

int PrintDebugInstCh()
{
	zCParser *parser = zCParser::GetParser();
	auto* self = parser->GetSymbol("SELF");
	zVob* selfinst = nullptr;
	if (self)
		selfinst = (zCVob *)self.GetInstanceAdr();
	zSTRING text;
	int channel;
	parser->GetParameter(&text);
	parser->GetParameter(&channel);
	if ((ogame->IsDebuggingAllInstances() ||
	     ogame->IsDebuggingInstance(slfinst)) &&
	    (ogame->IsDebuggingChannel(channel)) {
		if ( zCView::GetShowDebug() ) {
			zERROR::Message("U: Sktipt: " + text);
		}
	}
	return 0;
}

int PrintDebugCh()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING text;
	int channel;
	parser->GetParameter(&text);
	parser->GetParameter(&channel);
	if (ogame->IsDebuggingChannel(channel)) {
		if ( zCView::GetShowDebug() ) {
			zERROR::Message("U: Sktipt: " + text);
		}
	}
	return 0;
}


int PrintMulti()
{
	zSTRING strings[5];
	zCParser* parser = zCParser::GetParser();
	v3 = 5;
	for (int i = 5; i; --i)
		zCParser::GetParameter(parser, strings[i]);
	int k = 0;
	while (k < 5) {
		if ( strings[k].Length() <= 0 )
			break;
		++k;
	}
	zCView* view = ogame->vtbl->GetTextView();
	view->Printwin(strings[rand() % k]);
	return 0;
}

int ExitGame()
{
	if (gameMan)
		gameMan->ExitGame();
	return 1;
}

int ExitGame()
{
	if (gameMan)
		gameMan->ExitSession();
	return 1;
}

int PlayVideo()
{
	zCParser* parser = zCParser::GetParser();
	zSTRING videoBIK;
	parser->GetParameter(videoBIK);
	int ret = gameMan->PlayVideo(videoBIK);
	parser->SetReturn(ret);
}

int PlayVideoEx()
{
	zCParser* parser = zCParser::GetParser();
	int exitSession;
	int blendTime;
	zSTRING videoBIK;
	parser->GetParameter(exitSession);
	parser->GetParameter(blendTime);
	parser->GetParameter(videoBIK);
	int ret = gameMan->PlayVideoEx(videoBIK, blendTime, exitSession);
	parser->SetReturn(ret);
}
