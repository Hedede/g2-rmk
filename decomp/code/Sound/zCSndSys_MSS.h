struct zCDrvProperties_MSS {
	zCDrvProperties_MSS() = default;
	~zCDrvProperties_MSS() = default;
	float volume;
	int bitResolution;
	int sampleRate;
	int useStereo;
	int numChannels;
	zSTRING providerName;
	char unk4[100];
	zSTRING str[20];
};

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

	zTSndHandle PlaySound(zCSoundFX* sfx, int slot, int freq, float vol, float pan) override;
	zTSndHandle PlaySound(zCSoundFX* sft, int slot) override;
	zTSndHandle PlaySound3D(zSTRING const &,zCVob *,int,zCSoundSystem::zTSound3DParams *) override;
	zTSndHandle PlaySound3D(zCSoundFX *,zCVob *,int,zCSoundSystem::zTSound3DParams *) override;

	void StopSound(zTSndHandle const& handle) override;

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
// _carsten/zSndMss.cpp
zSTRING DIRECTLOAD_PREFIX = "%";
zSTRING ID_CHANNEL_CHAR   = "V";
zSTRING ID_FRAME_CHAR     = "A";

void Exc_ReleaseSound()
{
	zDELETE(zsound);
}

void zCSndSys_MSS::~zCSndSys_MSS()
{
	zCExceptionHandler::RemoveUnhandledExceptionReleaseCallback(Exc_ReleaseSound);
	zINFO(5, "C: Shutting down MSS"); // 629

	DELETE(actFrameEdited);
	DisposeAllSampleHandles();

	for (auto* snd : zCActiveSnd::activeSndList)
		snd->RemoveSound();

	handleManager.DisposeAllSamples();

	for (auto* snd : zCSoundFX::classDef.objectList)
		snd->cacheState &= ~2;

	CloseProvider(); // was inlined

	AIL_waveOutClose(sndDrv);
	AIL_shutdown();

	zCActiveSnd::CleanupPreAllocedData(); // was inlined


	zINFO(10, "C: done"); // 652

	Delete(sfxParser);
	Delete(listener);
}

//------------------------------------------------------------------------------
void zCSndSys_MSS::SetListener(zCVob* listenerVob)
{
	listener->Release();
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
	chan->__loaded = 1;
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

		// Похоже нигде не используется
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

zCSndFX_MSS* zCSndSys_MSS::LoadSingle(const zSTRING& id)
{
	auto snd = zCSoundFX::classDef.SearchHashTable( id );
	if (snd && CHECK_INHERITANCE( snd, zCSndFX_MSS )) {
		++snd->refCtr;
	} else {
		snd = new zCSndFX_MSS; // 1074 g1d
		auto chan = new zCSndChannel; // 1075 g1d

		zSTRING name = id;
		int i = 0;

		do {
			auto frame = new zCSndFrame; // 1083 g1d
			sfxParser->CreateInstance(name, frame);
			frame->scripted.file.Upper();
			if (!frame->scripted.file)
				frame->scripted.file = MSS_NO_WAVE;
			frame->instanceName = name;

			zINFO(5, "C: SND: Creating Sound Instance " + id + "   (alternative: " + i + ")"); //1297

			chan->frames.InsertEnd(frame);

			zSTRING num{++i};
			name = id + UNDERBAR + ID_FRAME_CHAR + num;
		} while (sfxParser->GetSymbol(&name))

		chan->__loaded = 1;
		snd->channels->InsertEnd(chan);
		snd->SetObjectName( id );
	}
	return snd;
}

zTSndHandle zCSndSys_MSS::PlaySound(zCSoundFX* sfx, int slot)
{
	if (!sfx)
		return 0;

	sfx->ChooseVariance(); // was inlined
	sfx->CacheIn();        // was inlined

	sfx->GetCurFrame()->SetDefaultProperties();

	if ( !sfx->GetCurFrame()->waveData->CheckForCorrectFormat(0) )
		return -1;

	auto actSnd = zCActiveSnd::AllocNextFreeSnd();
	if ( !actSnd )
		return -1;

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

	zCActiveSnd::AddSound(actSnd);
	D_SpyFrameInfo(actSnd, 0);
	actSnd->bitfield.playing = true;
	actSnd->sndFx = sfx;
	sfx->AddRef();
	return actSnd->handle;
}

int zCSndSys_MSS::PlaySound(zCSndFX_MSS *sfx, int slot, int freq, float vol, float pan)
{
	if (!sfx)
		return 0;
	sfx->ChooseVariance(); // was inlined
	sfx->CacheIn();        // was inlined
	sfx->GetCurFrame()->SetDefaultProperties();

	if ( !sfx->GetCurFrame()->waveData->CheckForCorrectFormat(0) )
		return 0;

	auto actSnd = zCActiveSnd::AllocNextFreeSnd();
	if ( !actSnd )
		return 0;

	actSnd->handle = ++zCActiveSnd::lastHandle;
	actSnd->bitfield.__isLooped = 0;
	actSnd->frame = sfx->GetCurFrame();

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

	if ( !actSnd->sampleHandle ) {
		zWARNING("C: could not allocate sample"); //1417,
		return -1;
	}


	AIL_set_sample_file( actSnd->sampleHandle, sfx->GetCurFrame()->waveData->soundData, 0);

	// TODO: these 1.0 - -1.0 are related to
	// MSS_VOL_MAX - MSS_VOL_MIN
	// MSS_PAN_RIGHT - MSS_PAN_LEFT
	if ( pan == -2.0 )
		actSnd->pan = 64;
	else
		actSnd->pan = (pan - -1.0) / (1.0 - -1.0) * 127.0;

	if ( vol == -1.0 )
		actSnd->volume = sfx->GetCurFrame()->scripted.vol;
	else
		actSnd->volume = vol * 127.0;

	if ( freq == -1 )
		actSnd->frequency = sfx->GetCurFrame()->waveData->waveInfo->rate;
	else
		actSnd->frequency = freq;

	actSnd->bitfield.loop = sfx->GetCurFrame()->scripted.loop;

	if ( GetReverbEnabled() )
		actSnd->reverbLevel = sfx->GetCurFrame()->scripted.reverbLevel;
	else
		actSnd->reverbLevel = 0.0;

	actSnd->reverbLevel->reverbLevel *= 0.5 * globalReverbWeight;

	if ( !actSnd->sampleHandle ) {
		zWARNING("C: could not allocate sample"); //1437,
		return -1;
	}

	// I copied this from other PlaySound, didn't check thorougly for diffrences
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

	zCActiveSnd::AddSound(actSnd);
	D_SpyFrameInfo(actSnd, 0);
	actSnd->bitfield.playing = true;
	actSnd->sndFx = sfx;
	sfx->AddRef();
	return actSnd->handle;
}

void zCSndSys_MSS::DisposeAllSampleHandles()
{
	for ( auto* snd : zCActiveSnd::activeSndList ) {
		if ( snd->bitfield.is3d ) {
			auto handle = snd->sampleHandle3d;
			// SMP_FREE == 1, not sure about that
			if ( !hanlde || AIL_3D_sample_status(handle) == SMP_FREE )
				continue;
			AIL_end_3D_sample(handle);
			AIL_release_3D_sample_handle(handle);
			snd->sampleHandle3d = 0;
		} else {
			auto handle = snd->sampleHandle;
			if ( !handle || AIL_sample_status(handle) == SMP_FREE )
				continue;
			AIL_stop_sample(handle);
			AIL_release_sample_handle(handle);
			while ( AIL_sample_status(handle) != SMP_FREE );
			snd->sampleHandle = 0;
		}
		snd->bitfield.playing = false;
	}
}

int zCSndSys_MSS::PlaySound3D(zSTRING  const& fileName, zCVob *sourceVob, int slot, zCSoundSystem::zTSound3DParams& params)
{
	if ( !fileName )
		return 0;

	auto sndFx = LoadSoundFX(fileName);
	if (auto snd = zDYNAMIC_CAST<zCSoundFX*>(sndFx)) {
		snd->timeStart = ztimer.totalTimeFloat;
		auto handle = PlaySound3D(snd, sourceVob, slot, params);
		Release(snd);
		return handle;
	}
	return 0;
}

void zCSndSys_MSS::StopSound(const int& sfxHandle)
{
	if (auto asnd = zCActiveSnd::GetHandleSound(sfxHandle))
		asnd->RemoveSound();
}

float zCSndSys_MSS::GetActiveVolume(int const& sfxHandle)
{
	if (auto asnd = zCActiveSnd::GetHandleSound(sfxHandle))
		return asnd->GetVolume() * asnd;
	return -1.0;
}

bool zCSndSys_MSS::IsSoundActive(int const& sfxHandle)
{
	auto asnd = zCActiveSnd::GetHandleSound(sfxHandle);
	if (asnd) {
		if (asnd->bitfield.playing)
			return true;
		if (asnd->sampleHandle)
			return true;
		if (asnd->sampleHandle3d)
			return true;
	}
	return false;
}

void zCSndSys_MSS::GetPlayingTimeMSEC(const zSTRING& fileName)
{
	if ( fileName ) {
		zSTRING name = fileName;
		if (fileName.Search(0, ".WAV", 1) != -1)
			name = DIRECTLOAD_PREFIX + name;

		auto snd = zCSoundFX::classDef.SearchHashTable( name );
		// it is useless, beacuse tehy both use &zCSoundFX::classDef
		// i.e. sndfx_mss doesn't have it's own classdef
		if ( CHECK_INHERITANCE( snd, zCSndFX_MSS ) )
			return snd->GetPlayingTimeMSEC();
	}
	return 0.0;
}

// private
void zCSndSys_MSS::CloseProvider()
{
	if ( act3dProvider ) {
		AIL_close_3D_provider(act3dProvider);
		act3dProvider = 0;
		actProviderIdx = -1;
		_snd_provider_name.Clear();
	}
}

void MSS_auto_cleanup()
{
	atexit(AIL_shutdown);
}

void zCSndSys_MSS::SetGlobalReverbPreset(int type, float weight)
{
	if ( reverbEnabled ) {
		if (_snd_provider_name == PROV_EAX2)
			weight *= 0.5;

		if ( eaxEnvironment != type || globalReverbWeight != weight ) {
			AIL_set_3D_room_type(act3dProvider, type);
			eaxEnvironment = type;
			if ( type == 0 )
				globalReverbWeight = 0;
			else
				globalReverbWeight = weight;
		}
	}
}

int zCSndSys_MSS::GetSound3DProps(const int& sfxHandle, zCSoundSystem::zTSound3DParams& params)
{
	auto asnd = zCActiveSnd::GetHandleSound( sfxHandle )
	if ( asnd ) {
		float outer_angle;
		int   outer_volume;
		if (asnd->bitfield & 8)
			AIL_3D_sample_cone(asnd->sampleHandle3d, &params.coneAngle, &outer_angle, outer_volume);

		params.__obstruction = asnd->__obstruction / 0.7;
		params.radius = asnd->radius;
		params.reverbLevel = GetReverbEnabled() ? asnd->reverbLevel : 0.0;
		params.volume = asnd->__master_volume;
		params.unk2 = (asnd->bitfield >> 2) & 1;
		params.pitchOff = asnd->pitchOff;
		params.__isLooped = asnd->__isLooped;
		return 1;
	}

	params.SetDefaults();
	return 0;
}

void zCSndSys_MSS::GetSoundProps(const int& sfxHandle, int& freq, float& vol, float& pan)
{
	auto asnd = zCActiveSnd::GetHandleSound( sfxHandle )
	if (asnd) {
		if (asnd->volume == asnd->frame.vol) {
			vol = -1.0
		} else {
			vol = asnd->volume / 127.0;
		}

		if (asnd->pan == 64) {
			pan = -2.0;
		} else {
			pan = asnd->pan / 127.0 * (1.0 - -1.0) + -1.0; // [sic!]
			// probably some constants
		}

		if (asnd->frequency == asnd->frame->waveData->waveInfo->rate) {
			freq = -1;
		} else {
			freq = asnd->frequency;
		}
	} else {
		zWARNING("C: zSndMSS.cpp(zCSndSys_MSS::GetSoundProps): Handle not used!"); // 2076
		vol = 0;
		freq = -1;
		pan = -2.0;
	}
}

void zCSndSys_MSS::SetSpeakerType(zTSpeakerType type)
{
	switch ( type ) {
	case ST_2_SPEAKERS:
		AIL_set_3D_speaker_type(act3dProvider, AIL_3D_2_SPEAKER);
		break;
	case ST_4_SPEAKERS:
		AIL_set_3D_speaker_type(act3dProvider, AIL_3D_4_SPEAKER);
		break;
	case ST_HEADPHONES:
		AIL_set_3D_speaker_type(act3dProvider, AIL_3D_HEADPHONE);
		break;
	case ST_SURROUND:
		AIL_set_3D_speaker_type(act3dProvider, AIL_3D_SURROUND);
		break;
	case ST_5_1_SPEAKERS:
		AIL_set_3D_speaker_type(act3dProvider, AIL_3D_51_SPEAKER);
		break;
	case ST_7_1_SPEAKERS:
		AIL_set_3D_speaker_type(act3dProvider, AIL_3D_71_SPEAKER);
		break;
	default:
		zINFO(10, "C: speaker type not supported"); // 2498
	speakerType = type;
}

void zCSndSys_MSS::DoSoundUpdate()
{
	if ( d_noUpdate )
		return;

	if ( listener )
		trafoWStoLS = listener->trafoObjToWorld.InverseLinTrafo();

	zCWavePool::GetPool().NewFrame(); // was inlined
	zCActiveSnd::NewFrame();
	zCSndFX_MSS::NewFrame(); // was inlined
}

void zCSndSys_MSS::FrmCon_ParamChanged(const zSTRING& in)
{
	auto str = in; str.Upper();

	for (auto* snd : zCSoundFX::classDef.objectList) {
		for (int i = 0, e = snd->GetNumChannels(); i < e; ++i) {
			auto* channel = snd->channels[i];
			for (auto* frame : channel->frames) {
				if (frame->instanceName == actFrameEdited->instanceName) {
					bool fileUpdated = false;
					if (actFrameEdited->scripted.file != frame->scripted.file) {
						frame->scripted.file = actFrameEdited.scripted.file;
						zCActiveSnd::StopSoundsByFrame(frame); // was inlined
						frame->CacheIn(); // wac inlined
						fileUpdated = true;
					}
					zCActiveSnd::UpdateSoundsByFrame(frame);
					if ( fileUpdated )
						zCActiveSnd::ResumeSoundsByFrame(frame); // waas inlined
				}
			}
		}
	}
}

void zCSndSys_MSS::UpdateSoundPropsAmbient(zCActiveSnd *snd, zCSoundSystem::zTSound3DParams* params)
{
	_snd = snd;
	_params = params;
	if ( params ) {
		snd->radius = (params->radius == -1) ? defaultRadius_3d : params->radius;
		snd->__master_volume = (params->volume == -1) ? 1.0 : params->volume;
		snd->__isLooped = params->__isLooped;
		if (params->__obstruction <= 0.0)
			snd->AutoCalcObstruction(0);
	} else {
		snd->AutoCalcObstruction(0);
	}

	if ( snd->flags.volOverride )
		snd->volume = snd->GetVolume() * 127.0;
	else
		snd->volume = snd->frame->volume;
	snd->volume *= snd->__master_volume;

	if ( snd->volume ) {
		bool loaded = 1;
		if ( !snd->sampleHandle ) {
			if ( !snd->RequestChannel() )
				return;
			loaded = 0;
			if ( !snd->sampleHandle ) {
				zWARNING("C: could not allocate sample"); // 2003,
				return;
			}
			AIL_init_sample(snd->sampleHandle);
			AIL_set_sample_file(snd->sampleHandle, snd->frame->waveData->soundData, 0);
			AIL_set_sample_processor(snd->sampleHandle, 1, reverb3Filter);
			snd->__lastUpdate = 0;
		}
		if ( params ) {
			snd->__obstruction = 0.7 * _params->__obstruction;
			snd->reverbLevel = GetReverbEnabled() ? params->reverbLevel : 0.0;
		}

		AIL_set_sample_playback_rate(snd->sampleHandle, snd->frequency);
		AIL_set_sample_volume_pan(snd->sampleHandle, (1.0 - snd->__obstruction) * (snd->volume * snd->__master_volume) / 127.0, snd->pan/127.0);
		AIL_set_sample_loop_block( snd->sampleHandle, snd->frame->scripted.loopStartOffset, snd->frame->scripted.loopEndOffset);
		if (snd->__isLooped)
			snd->loop = snd->__isLooped == 1;
		else
			snd->loop = snd->frame->scripted.loop;
		AIL_set_sample_loop_count(snd->sampleHandle, !snd->bitfield.loop );
		if ( GetReverbEnabled() ) {
			AIL_set_filter_sample_preference(snd->sampleHandle, "Reverb EAX Environment", eaxEnvironment);
			if ( eaxEnvironment == 0 )
				AIL_set_filter_sample_preference(_snd->sampleHandle, "Reverb Mix", &CONST_ZERO);
			else
				AIL_set_filter_sample_preference(_snd->sampleHandle, "Reverb Mix", &snd->reverbLevel);
		}
		AIL_set_filter_sample_preference(snd->sampleHandle, "Lowpass Cutoff", 20000.0 - s_globalOcclusion * 19800.0);
		if ( !snd.isPlaying ) {
			snd.isPlaying = 1;
			snd->__lastUpdate = 0;
			if ( loaded )
				AIL_resume_sample(snd->sampleHandle);
			else
				AIL_start_sample(snd->sampleHandle);
		}
	} else if (snd->playing) {
		snd->playing = false;
		AIL_stop_sample(snd->sampleHandle);
	}
}

void zCSndSys_MSS::UpdateSoundProps(const int& sfxHandle, int freq, float vol, float pan)
{
	if ( auto asnd = zCActiveSnd::GetHandleSound( sfxHandle ) ) {
		if ( asnd->is3d ) {
			zWARNING("C: shit happens: a sound which should be an ambient sound is suddenly 3d, what the heck!!|!"); // 2162
			return;
		}

		if ( pan == -2.0 )
			asnd->pan = 64;
		else
			asnd->pan = (pan - -1.0) / (1.0 - -1.0) * 127.0;
		if ( vol == -1.0 )
			asnd->volume = asnd->frame->scripted.vol;
		else
			asnd->volume = vol * 127.0;

		if ( freq == -1 )
			asnd->frequency = asnd->frame->waveData->waveInfo->rate;
		else
			asnd->frequency = freq;

		if ( asnd->volume ) {
			bool loaded = 1;
			if ( !asnd->sampleHandle ) {
				if ( !asnd->RequestChannel() )
					return;
				if ( !asnd->sampleHandle ) {
					zWARNING("C: could not allocate sample for 2d sound"); // 2188
					return;
				}
				AIL_init_sample(asnd->sampleHandle);
				AIL_set_sample_file(asnd->sampleHandle, asnd->frame->waveData->soundData, 0);
				if ( eaxEnvironment )
					AIL_set_sample_processor(asnd->sampleHandle, 1, reverb3Filter);
				asnd->__lastUpdate = 0;
				loaded = 0;
			}

			AIL_set_sample_playback_rate(asnd->sampleHandle, asnd->frequency);
			AIL_set_sample_volume_pan(asnd->sampleHandle, asnd->volume / 127.0, asnd->pan / 127.0);
			if ( eaxEnvironment && GetReverbEnabled() ) {
				AIL_set_filter_sample_preference(asnd->sampleHandle, "Reverb EAX Environment", &eaxEnvironment);
				AIL_set_filter_sample_preference(asnd->sampleHandle, "Reverb Mix", &asnd->reverbLevel);
			}
			AIL_set_sample_loop_block( asnd->sampleHandle, asnd->frame->scripted.loopStartOffset, asnd->frame->scripted.loopEndOffset);
			AIL_set_sample_loop_count(snd->sampleHandle, !snd->bitfield.loop );
			if ( !snd.isPlaying ) {
				snd.isPlaying = 1;
				snd->__lastUpdate = 0;
				if ( loaded )
					AIL_resume_sample(snd->sampleHandle);
				else
					AIL_start_sample(snd->sampleHandle);
			}
		} else if (snd->playing) {
			snd->playing = false;
			AIL_stop_sample(snd->sampleHandle);
		}
	}
}

int zCSndSys_MSS::UpdateSound3D(int const& sfxHandle, zTSound3DParams* params)
{
	if ( sfxHandle == -1 )
		return 0;
	auto idx = zCActiveSnd::GetHandleIndex(*sfxHandle);
	if ( idx == -1 )
		return 0;
	auto actSnd = zCActiveSnd::activeSndList.GetSafe(idx);
	if ( !actSnd )
		return 0;
	if ( !listener )
		return 0;

	if ( !actSnd->HasBeenUpdatedThisFrame() ) // was inlined
		return 0;

	actSnd->sndFx->CacheIn(); // was inlined

	// can't find a function, but possibly was inlined too
	if ( listener && listener->homeWorld )
		actSnd->updateFrameCtr = listener->homeWorld->masterFrameCtr;

	if ( actSnd->sndFx->cacheState != CACHED_IN )
		return 1;
	if ( !actSnd->sourceVob )
		return 0;
	if ( actSnd->bitfield & 4 ) {
		UpdateSoundPropsAmbient(actSnd, params);
		return 1;
	}

	if ( params ) {
		if ( params->radius == -1.0 )
			actSnd->radius = _this->defaultRadius_3d;
		else
			actSnd->radius = params->radius;

		if ( params->volume == -1.0 )
			actSnd->__master_volume = 1.0;
		else
			actSnd->__master_volume = params->volume;

		if ( actSnd->__master_volume > 1.0 )
			actSnd->__master_volume = 1.0;

		actSnd->isLooped = params->__isLooped;
	}

	if (!params || params->__obstruction)
		zCActiveSnd::AutoCalcObstruction(actSnd, 0);

	if ( (actSnd.GetVolume() * actSnd->__master_volume * 127.0) <= 0 ) {
		if ( actSnd->bitfield.playing ) {
			actSnd->bitfield.playing = 0;
			AIL_stop_3D_sample(actSnd->sampleHandle3d);
		}
		return 1;
	}

	zBOOL isStarted = 1;
	if ( !actSnd->sampleHandle3d ) {
		if ( !actSnd.RequestChannel() )
			return 0;
		if ( !actSnd->sampleHandle3d ) {
			zWARNING("C: could not allocate sample") // 1861,
			return 0;
		}
		AIL_set_3D_sample_file(actSnd->sampleHandle3d, actSnd->frame->waveData->soundData);
		actSnd->__lastUpdate = 0;
		isStarted = 0;
	}
	if ( params ) {
		auto inner = ( params->coneAngle == 0.0 ) ? 360.0 : params->coneAngle;
		auto outer = 360.0 - inner;
		AIL_set_3D_sample_cone(actSnd->sampleHandle3d, inner, outer, 0);
		auto obstruction = 0.7 * params->__obstruction;
		AIL_set_3D_sample_obstruction(actSnd->sampleHandle3d, obstruction);
		auto reverb =  GetReverbEnabled() ? params->reverbLevel : 0.0;
		actSnd->reverbLevel = reverb;
	}

	actSnd.CalcListenerVolume(); // was inlined

	AIL_set_3D_sample_occlusion(actSnd->sampleHandle3d, s_globalOcclusion);
	AIL_set_3D_sample_playback_rate(actSnd->sampleHandle3d, actSnd->frequency);

	auto innerRadius = 0.3 * actSnd->radius;

	// code is same as in GetVolume(), but not exactly
	auto srcPos = sourceVob->GetPositionWorld();
	auto lisPos = listener->GetPositionWorld();

	auto distApprox = (srcPos - lisPos).LengthApprox();

	// Don't know what to call it,
	// think thickness of a spherical shell
	double thickness = actSnd->radius - innerRadius;

	double vol = innerRadius;
	if ( (distApprox > innerRadius) && ( thickness != 0.0 ) )
		vol *= 1.0 - (distApprox - innerRadius) / thickness;

	auto minDist = RANGE_SCALE * vol;
	auto maxDist = RANGE_SCALE * actSnd->radius;
	AIL_set_3D_sample_distances(actSnd->sampleHandle3d, maxDist, minDist);
	AIL_set_3D_sample_volume(actSnd->sampleHandle3d, actSnd->volume * 127.0);

	if ( globalReverbWeight == 0.0 ) {
		AIL_set_3D_sample_effects_level(actSnd->sampleHandle3d, 0);
	} else {
		AIL_set_3D_sample_effects_level(actSnd->sampleHandle3d, globalReverbWeight * actSnd->reverbLevel);
	}

	AIL_set_3D_sample_loop_block( actSnd->sampleHandle3d, actSnd->frame->loopStartOffset, actSnd->frame->loopEndOffset);

	if ( actSnd->isLooped )
		actSnd->bitfield.loop = actSnd->isLooped;
	else
		actSnd->bitfield.loop = actSnd->frame->loop;

	// loop == true => 0 => infinite, false => 1 => one loop
	AIL_set_3D_sample_loop_count(actSnd->sampleHandle3d, !actSnd->bitfield.loop);

	auto srcWS = actSnd->sourceVob->trafoObjToWorld;
	auto srcLS = trafoWStoLS * srcWS;

	v110 = zVEC4::zVEC4((int)&v196, v109, v108, v107, v106);
	zMAT4::zMAT4(&mul, v110, v105, v100, v95);

	auto curTime   = ztimer.totalTimeFloat;
	auto timeDelta = ztimer.totalTimeFloat - actSnd->__lastFrameTime;

	// local space
	auto pos = srcLS->GetTranslation();
	auto at = srcLS->GetAtVector();
	auto up = srcLS->GetUpVector();

	// possibly inlined
	auto srcPos = actSnd->sourceVob->GetPositionWorld();
	zVEC3 vel;
	if ( timeDelta <= 0.0 ) {
		vel = srcPos;
	} else {
		auto dt = timeDelta * 100.0;
		vel = (srcPos - actSnd->__lastFramePos) / dt;
	}

	actSnd->__lastFramePos = srcPos;
	actSnd->__lastFrameTime = curTime;

	if ( listener == actSnd->sourceVob )
		AIL_set_3D_position(actSnd->sampleHandle3d, 0.0, 10.0, 0.0);
	else
		AIL_set_3D_position(actSnd->sampleHandle3d, pos.x, pos.y, pos.z);

	AIL_set_3D_orientation(actSnd->sampleHandle3d, at.x, at.y, at.z, up.x, up.y, up.z);
	AIL_set_3D_velocity_vector(actSnd->sampleHandle3d, vel.x, vel.y, vel.z);

	if ( actSnd->bitfield.playing & 1 )
		return 1;
	actSnd->bitfield.playing = 1;
	actSnd->__lastUpdate = 0;
	if ( isStarted )
		AIL_resume_3D_sample(actSnd->sampleHandle3d);
	else
		AIL_start_3D_sample(actSnd->sampleHandle3d);
	return 1;
}
