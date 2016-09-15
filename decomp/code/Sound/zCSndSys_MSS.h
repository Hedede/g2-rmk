// _carsten\\zSndMss.cpp
struct zCSndSys_MSS : zCSoundSystem {
	void ~zCSndSys_MSS() override;
	zCSoundFX* LoadSoundFX(zSTRING const& name) override;
	zCSoundFX* LoadSoundFXScript(zSTRING const& name) override;
	void GetSFXParser() override;
	void GetPlayingTimeMSEC(zSTRING const &) override;

	void SetMasterVolume(float) override;
	void GetMasterVolume() override;
	void PlaySound(zCSoundFX *,int,int,float,float) override;
	void PlaySound(zCSoundFX *,int) override;
	void PlaySound3D(zSTRING const &,zCVob *,int,zCSoundSystem::zTSound3DParams *) override;
	void PlaySound3D(zCSoundFX *,zCVob *,int,zCSoundSystem::zTSound3DParams *) override;
	void StopSound(int const &) override;
	void StopAllSounds() override;
	void GetSound3DProps(int const &,zCSoundSystem::zTSound3DParams &) override;
	void UpdateSound3D(int const &,zCSoundSystem::zTSound3DParams *) override;
	void GetSoundProps(int const &,int &,float &,float &) override;
	void UpdateSoundProps(int const &,int,float,float) override;
	void IsSoundActive(int const &) override;
	void DoSoundUpdate() override;
	void SetListener(zCVob *) override;
	void SetGlobalReverbPreset(int,float) override;
	void SetReverbEnabled(int) override;
	void GetReverbEnabled() override;
	void GetNumProvider() override;
	void GetProviderName(int) override;
	void SetProvider(int) override;
	void SetSpeakerType(zCSoundSystem::zTSpeakerType) override;
	void GetSpeakerType() override;
	void SetGlobalOcclusion(float) override;
	void GetCPULoad() override;

	void SfxCon_ParamChanged(zSTRING const&) { }

private:
	int unk1;
	int unk2;
	int unk3;
};

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
