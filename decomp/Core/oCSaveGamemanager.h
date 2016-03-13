void oCSavegameManager::ClearCurrent()
{
	zCOption::ChangeDir(zoptions, 2);
	Dir_Clear(SAVEGAME_CURRENT_DIR + '/', "*.*");
}
