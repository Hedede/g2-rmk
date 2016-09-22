// _carsten\\zSndMss.cpp
zCParser* sfxParser;
struct zCSndSys_MSS : zCSoundSystem {
	void ~zCSndSys_MSS() override;
	zCSoundFX* LoadSoundFX(zSTRING const& name) override;
	zCSoundFX* LoadSoundFXScript(zSTRING const& name) override;
	zCParser* GetSFXParser() override
	{
		return sfxParser;
	}
	void GetPlayingTimeMSEC(zSTRING const &) override;

	void SetMasterVolume(float vol) override
	{
		if ( vol == -1.0 )
			prefs.volume = 127.0 / AIL_get_preference(MDI_DEFAULT_VOLUME);
		else
			prefs.volume = vol;
		AIL_set_digital_master_volume_level(sndDrv, prefs.volume);
	}
	float GetMasterVolume() override
	{
		// not sure about this one
		if (AIL_get_preference(MDI_DEFAULT_VOLUME) == prefs.volume)
			return -1.0;
		return prefs.volume / 127;
	}
	void PlaySound(zCSoundFX *,int,int,float,float) override;
	void PlaySound(zCSoundFX *,int) override;
	void PlaySound3D(zSTRING const &,zCVob *,int,zCSoundSystem::zTSound3DParams *) override;
	void PlaySound3D(zCSoundFX *,zCVob *,int,zCSoundSystem::zTSound3DParams *) override;
	void StopSound(int const &) override;
	void StopAllSounds() override
	{

		for (auto snd : zCActiveSnd::activeSndList) {
			auto sfx = snd->sndFx;
			if (!sfx || sfx->GetIsFixed())
				zCActiveSnd::RemoveSound(snd);
		}
		handleManager.DisposeAllSamples();
	}

	void GetSound3DProps(int const &,zCSoundSystem::zTSound3DParams &) override;
	void UpdateSound3D(int const &,zCSoundSystem::zTSound3DParams *) override;
	void GetSoundProps(int const &,int &,float &,float &) override;
	void UpdateSoundProps(int const &,int,float,float) override;
	void IsSoundActive(int const &) override;
	void DoSoundUpdate() override;
	void SetListener(zCVob *) override;
	void SetGlobalReverbPreset(int,float) override;

	void SetReverbEnabled(int) override
	{
		if ( reverbEnabled ) {
			if ( !b )
				SetGlobalReverbPreset(0, 0.0);
		}
		reverbEnabled = b;
	}
	int GetReverbEnabled() override
	{
		return reverbEnabled;
	}

	int GetNumProvider() override
	{
		return zoptions->ReadInt("SOUND", "extendedProviders", 0) ? 13 : 3;
	}
	zSTRING const& GetProviderName(int idx) override
	{
		return soundProviders[idx].name;
	}
	int SetProvider(int) override { return 1; }

	void SetSpeakerType(zCSoundSystem::zTSpeakerType) override;
	zTSpeakerType GetSpeakerType() override
	{
		return speakerType;
	}
	void SetGlobalOcclusion(float val) override
	{
		s_globalOcclusion = val;
	}
	float GetCPULoad() override
	{
		return AIL_digital_CPU_percent(sndDrv) * 0.01;
	}

	static void SfxCon_ParamChanged(zSTRING const&) { }

	void SetNumAudioChannels(int num)
	{
		AIL_set_preference(1, num);
		zCActiveSnd::maxChannels = num;
		zCSndSys_MSS::prefs.numChannels = num;
	}

	static int GetNumSoundsPlaying()
	{
		int result = 0;
		for (auto* snd : zCActiveSnd::activeSndList) {
			if ( snd->bitField & 1 )
				++result;
		}
		return result;
	}

	static int zCSndSys_MSS::RemoveAllActiveSounds()
	{
		for (auto* snd : zCActiveSnd::activeSndList) {
			if ( snd->bitField & 1 )
				zCActiveSnd::RemoveSound(snd);
		}
		return zCActiveSnd::activeSndList.GetNum();
	}

	static int GetUsedSoundMem()
	{
		return sndMemUsed;
	}

	_DIG_DRIVER* GetDigitalDriverHandle() const
	{
		return sndDrv;
	}

	int MuteSFX()
	{
		swap(lastVol, prefs.volume);
		return AIL_set_digital_master_volume_level(sndDrv, prefs.volume);
	}

private:
	zTSpeakerType speakerType;
	int eaxEnvironment;
	zBOOL reverbEnabled;
};

//------------------------------------------------------------------------------
void zCSndSys_MSS::SetListener(zCVob* listenerVob)
{
	Release(listener);
	if ( listenerVob ) {
		listener = listenerVob;
		listener->AddRef();
		trafoWStoLS = listener->trafoObjToWorld.InverseLinTrafo();
	}
}

//------------------------------------------------------------------------------
zCSoundFX* zCSndSys_MSS::LoadSoundFX(zSTRING const& fileName)
{
	zSTRING name = fileName;
	name.Upper();

	if (name.IsEmpty())
		name = MSS_NO_WAVE;

	zSTRING ext;
	if (name.Search(0, ".WAV", 1u) != -1)
		ext = ".WAV";
	else if (name.Search(0, ".MP3", 1u) != -1)
		ext = ".MP3";

	if ( ext == ".MP3" ) {
		zFAULT("C: SND: MP3 Files are not supported directly. You have to use a Mpeg Layer-3 file which is encapsulated into a WAV file (RIFF Format!)"); // 1104
		return nullptr;
	}

	if ( ext.IsEmpty() )
		return LoadSoundFXScript(name);

	auto dload = DIRECTLOAD_PREFIX + name;
	auto obj = zCSoundFX::classDef.SearchHashTable(dload);
	if (auto snd = zDYNAMIC_CAST<zCSoundFX>(snd)) {
		snd->AddRef();
		return snd;
	}

	auto snd = new zCSndFX_MSS;
	auto chan = new zCSndChannel;
	auto frame = new zCSndFrame;

	frame->instanceName = DIRECTLOAD_INSTANCENAME;
	frame->file = name;
	zINFO(5, "C: SND: Creating Sound " + name + "   (single) "); // 1126,

	chan->frames.InsertEnd(frame);
	chan->unk[2] = 1;
	snd->channels.InsertEnd(chan);

	snd->SetObjectName(DIRECTLOAD_PREFIX + name);
	return snd;
}

zCSoundFX* zCSndSys_MSS::LoadSoundFXScript(zSTRING const& identifier)
{
	zSTRING name = identifier;
	name.Upper();

	if (name.IsEmpty() || !sfxParser->GetSymbol(name)) {
		zWARNING("C: zSndMSS(zCSndSys_MSS::LoadSoundFXByIdentifier): Sound Identifier \"" + name + "\" unknown !"); // 1155
		return nullptr;
	}

	int len = 0;
	zSTRING word1;
	zSTRING name1;
	int digit = 0;
	int chan = 0;
	int multi = 0;
	for (int i = 1; ; ++i) {
		auto word1 = name.PickWord(i, "_", "_");

		int digit = 0;
		if (word1[0] == ID_CHANNEL_CHAR) {
			if ( isdigit(word1[1]) )
				digit = 1;
		}

		if ( digit ) {
			chan = word[1] - '0';
			multi = 1;
			break;
		}

		name1 += word1;
		len += word1.Length();
		if ( len >= name.Length() ) {
			name1 = name1.PickWord(1, ".", ".");
			break;
		}

		name += UNDERBAR;
		len += 1;

		if (word.IsEmpty())
			break;
	}


	zoptions->ChangeDir(DIR_SOUND);

	if ( multi )
		return LoadMulti(name1.Copied(0, name1.Length() - 1), chan);
	return LoadSingle(name1);
}

zTSndHandle zCSndSys_MSS::PlaySound(zCSoundFX* sfx, int slot)
{
	if (!sfx)
		return 0;

	// was inlined, private
	// friend or part of bigger func?
	sfx->ChooseVariance();
	auto chan = sfx->channels[sfx->curChannel];
	if ( chan->frames.GetNum() <= 1) {
		sfx->curFrame = 0;
	} else {
		if (sfx->selFrame == -1) {
			sfx->curFrame = rand() * (frames.GetNum() - 1) / 32767.0 + 0.5;
		} else {
			sfx->curFrame = sfx->selFrame;
		}
	}

	sfx->CacheIn();

	sfx->GetCurFrame()->SetDefaultProperties();

	if ( !sfx->GetCurFrame()->waveData->CheckForCorrectFormat(0) )
		return -1;

	auto actSnd = zCActiveSnd::AllocNextFreeSnd();
	if ( !actSnd ) {
		return -1;
	}


	// looks very similar to part of other func,
	// need to find common inlined stuff
	actSnd->handle = ++zCActiveSnd::lastHandle;
	actSnd->frame = sfx->GetCurFrame();
	// maybe not frequency?
	actSnd->frequency = actSnd->frame->CalcPitchVariance();
	actSnd->pan       = actSnd->frame->pan;
	actSnd->volume    = actSnd->frame->volume;

	actSnd->bitfield.loop = actSnd->frame->loop;

	if ( GetReverbEnabled() )
		actSnd->reverbLevel = actSnd->frame->reverbLevel;
	else
		actSnd->reverbLevel = 0.0;
	actSnd->reverbLevel *= 0.5 * globalReverbWeight;

	if ( slot ) {
		for (auto& snd : zCActiveSnd::activeSndList) {
			if (snd->bitfield.slot == slot) {
				zCActiveSnd::RemoveSound( snd );
				break;
			}
		}
	}

	if ( !actSnd->RequestChannel() )
		return -1;

	auto sample = actSnd->sampleHandle;

	if ( !sample ) {
		zWARNING("C: could not allocate sample"); //1349,
		return -1;
	}

	AIL_init_sample(sample);

	if ( eaxEnvironment ) {
		AIL_set_sample_processor(sample, 1, reverb3Filter);
		AIL_set_filter_sample_preference(sample, "Reverb EAX Environment", &eaxEnvironment);
		AIL_set_filter_sample_preference(sample, "Reverb Mix", &actSnd->reverbLevel);
	}

	AIL_set_sample_file(sample, actSnd->frame->waveData->soundData, 0);
	AIL_set_sample_playback_rate(sample, actSnd->frequency);

	auto vol = actSnd->volume / 127.0;
	auto pan = actSnd->pan    / 127.0;
	AIL_set_sample_volume_pan(sample, vol, pan);
	auto start = actSnd->frame->loopStartOffset;
	auto end   = actSnd->frame->loopEndOffset;
	AIL_set_sample_loop_block(sample, start, end);
	AIL_set_sample_loop_count(sample, !actSnd.bitfield.loop);
	AIL_start_sample(sample);

	zCActiveSnd.activeSndList.InsertSort(actSnd);
	D_SpyFrameInfo(actSnd, 0);
	actSnd->bitfield.playing = true;
	actSnd->sndFx = sfx;
	sfx->AddRef();
	return actSnd->handle;
}
