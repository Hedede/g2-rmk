int ChangeMusicEnabled(zCOptionEntry* optentry)
{
	if ( zoptions->ReadBool(zOPT_SEC_SOUND, "musicEnabled", 1) ) {
		zCMusicSystem::DisableMusicSystem(0);
		zCZoneMusic::SetAutochange(1);

		if ( zCMenu::GetActive() )
			zCMenu::GetActive()->RestartMusicTheme();

		zINFO(3, "B: MUSIC: Turned ON"); // 191
	} else {
		zCZoneMusic::SetAutochange(0);
		zCMusicSystem::DisableMusicSystem(1);

		zINFO(3, "B: MUSIC: Turned OFF"); // 197
	}

	float vol = zoptions->ReadReal(zOPT_SEC_SOUND, ZOPT_SND_SFX_VOL, 1.0);
	zClamp01(vol);

	if ( zsound )
		zsound->SetMasterVolume(zsound, vol);

	vol = zoptions->ReadReal(zOPT_SEC_SOUND, ZOPT_SND_MUSIC_VOL, 0.8);
	zClamp01(vol);

	if ( zmusic )
		zmusic->SetVolume(zmusic, vol);

	return 1;
}
