void netInit()
{
}



int MainProg()
{
	CGameManager game;

	sysEvent();
	
	game.Init((HWND *)&hWndApp);
	game.Run();
	game.Done();

	return 0;
}
