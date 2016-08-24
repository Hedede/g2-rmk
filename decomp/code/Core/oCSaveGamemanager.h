
void oCSavegameManager::Reinit()
{
	initialized = false;
	Init();
}

void oCSavegameManager::ClearCurrent()
{
	zCOption::ChangeDir(zoptions, 2);
	Dir_Clear(SAVEGAME_CURRENT_DIR + '/', "*.*");
}
