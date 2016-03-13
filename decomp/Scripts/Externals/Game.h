int IntroduceChapter()
{
	zCParser *parser = zCParser::GetParser();
	zSTRING chapter;
	zSTRING text;
	zSTRING fileTGA;
	zSTRING soundWAV;
	zSTRING time;
	parser->GetParameter(time);
	parser->GetParameter(soundWAV);
	parser->GetParameter(fileTGA);
	parser->GetParameter(text);
	parser->GetParameter(chapter);
	zERROR::Message("B: Introducing chapter: " + chapter + " (" + time + "ms)");
	CGameManager::IntroduceChapter(chapter, text, fileTGA, fileWAV, time);
	zERROR::Message("B: Introducing chapter ended.");
	zCParser::SetReturn(parser, 0);
	return 1;
}

int Game_InitGerman()
{
	zCParser::GetParser()->SetReturn(1);
	return 1;
}

int Game_InitEnglish()
{
	zCParser::GetParser()->SetReturn(1);
	return 1;
}

int Game_InitEngIntl()
{
	zCParser::GetParser()->SetReturn(1);
	return 1;
}
