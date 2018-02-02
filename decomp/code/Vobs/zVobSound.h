enum zTSndMode {
	LOOPING,
	ONCE,
	RANDOM
};

enum zTSndVolType {
	SPHERE,
	ELLIPSOID
};

class zCVobSound : public zCZone {
	Z_OBJECT(zCVobSound);
public:
	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;
	virtual ~zCVobSound();
	void OnTrigger(zCVob* otherVob, zCVob* vobInstigator) override
	{
		sndIsAllowedToRun = true; // flags |= 0x20u
		StartSound(true);
	}

	virtual void OnUntrigger(zCVob* otherVob, zCVob* vobInstigator) override
	{
		sndIsAllowedToRun = false; // flags |= ~0x20u
		StopSound();
	}

	void OnMessage(zCEventMessage* msg, zCVob* sourceVob) override {}

	virtual void EndMovement(int);

	void ThisVobAddedToWorld(zCWorld* homeWorld) override
	{
		zCVob::ThisVobAddedToWorld(homeWorld);
		sndWorld = homeWorld;
	}
	void ThisVobRemovedFromWorld(zCWorld* homeWorld) override
	{
		zCVob::ThisVobRemovedFromWorld(homeWorld);
		StopSound();
	}

	virtual void ProcessZoneList(zCArraySort<zCZone *> const &,zCArraySort<zCZone	*> const &,zCWorld *);
	virtual void GetDebugDescString();
	virtual void DoSoundUpdate(float);

	zCSoundFX* LoadSound(zSTRING const& soundfileName);

	void SetSoundRadius(float radius)
	{
		sndRadius = radius;
		if ( sndVolType == SPHERE ) {
			zTBBox3d bbox;
			bbox.mins[0] = bbox.mins[1] = bbox.mins[2] = -radius;
			bbox.maxs[0] = bbox.maxs[1] = bbox.maxs[2] = radius;
			zCVob::SetBBox3DLocal(bbox);
		}
	}

	void zCVobSound::SetSoundVolume(float volume)
	{
		sndVolume = volume;
	}

	void StopSound();

private:
	zCWorld* sndWorld = nullptr;

	zSTRING sndName;
	float sndRadius   = 1500.0;
	zTSndMode sndMode = LOOPING;

	// TODO: recheck again
	uint32_t sndStartOn        : 1 = 1; // 1
	uint32_t sndIsPlaying      : 1 = 0; // 2
	uint32_t sndAmbient3D      : 1 = 0; // 4
	uint32_t sndObstruction    : 1 = 0; // 8
	uint32_t sndVolType        : 1 = 0; // 10 // SPHERE
	uint32_t sndIsAllowedToRun : 1 = 1; // 20
	uint32_t sndIsRunning      : 1 = 1; // 40

	float sndRandDelay    = 5.0;
	float sndRandDelayVar = 2.0;
	float sndVolume    = 100.0;
	float sndConeAngle = 0.0;

	zCSoundFX *sndFx = nullptr;
	int sndHandle    = 0;

	float sndFrameTime = 0.0;

	float obsPrev  = -1.0;
	float obsCur   = -1.0;
	float obsTimer = -1.0;
};

void zCVobSound::Archive(zCArchiver& arc)
{
	zCZone::Archive(arc);

	arc.WriteGroupBegin("Sound");
	arc.WriteFloat("sndVolume", sndVolume);
	arc.WriteEnum("sndMode", "LOOPING;ONCE;RANDOM", sndMode);
	arc.WriteFloat("sndRandDelay", sndRandDelay);
	arc.WriteFloat("sndRandDelayVar", sndRandDelayVar);
	arc.WriteBool("sndStartOn", sndStartOn);
	arc.WriteBool("sndAmbient3D", sndAmbient3D);
	arc.WriteBool("sndObstruction", sndObstruction);
	arc.WriteFloat("sndConeAngle", sndConeAngle);
	arc.WriteEnum("sndVolType", "SPHERE;ELLIPSOID", sndVolType);
	arc.WriteFloat("sndRadius", sndRadius);
	arc.WriteString("sndName", sndName);
	arc.WriteGroupEnd("Sound");

	if ( arc.InSaveGame() ) {
		arc.WriteBool("soundIsRunning", sndIsRunning);
		arc.WriteBool("soundAllowedToRun", sndIsAllowedToRun);
	}
}

void zCVobSound::Unarchive(zCArchiver& arc)
{
	zCZone::Unarchive(arc);

	StopSound();

	arc.ReadFloat("sndVolume", sndVolume);
	sndMode = arc.ReadEnum("sndMode" );
	arc.ReadFloat("sndRandDelay", sndRandDelay);
	arc.ReadFloat("sndRandDelayVar", sndRandDelayVar);
	sndStartOn     = arc.ReadBool("sndStartOn");
	sndAmbient3D   = arc.ReadBool("sndAmbient3D");
	sndObstruction = arc.ReadBool("sndObstruction",);

	arc.ReadFloat("sndConeAngle", sndConeAngle);

	sndVolType = arc.ReadEnum("sndVolType");

	float radius = arc.ReadFloat("sndRadius");
	SetSoundRadius(radius);

	sndName = arc.ReadString("sndName", sndName);

	zCSoundFX* fx = LoadSound(sndName); // was inlined

	Release(sndFx);
	sndFx = fx;

	if ( arc.InSaveGame() ) {
		sndIsRunning       = arc.ReadBool("soundIsRunning");
		sndIsAllowedToRun  = arc.ReadBool("soundAllowedToRun");
	}
}

//------------------------------------------------------------------------------
CVobSound::zCVobSound()
{
	type = VOB_TYPE_SOUND;
	flags1.showVisual = false;
	SetSoundRadius( sndRadius );
}
void zCVobSound::~zCVobSound()
{
	StopSound();
	Release(sndFx);
	if (sndWorld)
		sndWorld->RemoveVobFromLists(this);
	sndWorld = 0;
}


//------------------------------------------------------------------------------
void zCVobSound::StartSound(int forceCacheIn)
{
	sndIsPlaying = sndIsPlaying && zsound->IsSoundActive(sndHandle);
	if ( sndIsPlaying )
		return;
	if ( zsound && sndFx ) {
		if ( forceCacheIn || sndFx->CacheIn( 0.5 ) ) {
			if ( forceCacheIn )
				sndFx->CacheIn( -1.0 );
			zTSound3DParams params;
			params.volume      = 1.0;
			params.radius      = -1.0;
			params.__isLooped  = 0;
			params.coneAngle   = 0.0;
			params.reverbLevel = 1.0;
			params.__isAmbient = 0;
			params.pitchOff    = -999999.0;
			params.__obstruction = CalcObstruction();
			params.volume        = CalcVolumeScale();
			params.radius      = sndRadius;
			params.__isLooped  = sndMode == LOOPING;
			params.coneAngle   = sndConeAngle;
			params.__isAmbient = sndAmbient3D;

			sndHandle = zsound->PlaySound3D(sndFx, this, 0, &params);
			sndIsPlaying = true;
		}
	}
}

void zCVobSound::StopSound()
{
	if ( sndIsPlaying ) {
		if ( zsound )
			zsound->StopSound(sndHandle);
		sndIsPlaying = false;
		sndHandle = 0;
	}
}

//------------------------------------------------------------------------------
zCSoundFX* zCVobSound::LoadSound(zSTRING const& soundfilename)
{
	if ( zsound ) {
		if ( soundfileName.Search(0, ".", 1u) == -1 )
			return zsound->LoadSoundFXScript(soundfileName);
		else
			return zsound->LoadSoundFX(soundfileName);
	}
	return nullptr;
}

void zCVobSound::SetSound(zSTRING const& soundfileName)
{
	sndName = soundfileName;
	sndIsPlaying = false;
	auto snd = LoadSound( soundfileName );
	Release( sndFx );
	sndFx = snd;
}



void zCVobSound::EndMovement(int calcGround)
{
	zCVob::EndMovement(calcGround);
	if ( sndWorld )
		sndWorld->UpdateZone(this);
	if ( sndVolType == ELLIPSOID ) {
		auto bbox = GetBBox3DLocal();
		auto max = bbox.GetMaxExtent();
		sndRadius = max * 0.5;
	}
}

void zCVobSound::ProcessZoneList(zCArraySort<zCZone *> const& zoneList, zCArraySort<zCZone *> const& zoneDeactivateList, zCWorld* homeWorld)
{
	for (auto zone : zoneList) {
		auto snd = static_cast<zCVobSound*>(zone);
		if (snd->sndIsAllowedToRun) {
			if (snd->sndIsRunning) {
				snd->sndIsRunning = false;
				snd->StartSound( true );
			}
			snd->DoSoundUpdate( 1.0 );
		} else {
			snd->StopSound();
		}
	}
	for (auto zone : zoneDeactivateList) {
		auto snd = static_cast<zCVobSound*>(zone);
		snd->StopSound();
	}
}

void zCVobSound::DoSoundUpdate(float volScale)
{
	switch (sndMode) {
	case LOOPING:
		StartSound(false);
		break;
	case ONCE:
		break;
	case RANDOM:
		if (ztimer.totalTimeFloat >= sndFrameTime ) {
			StartSound(true);
			auto randDelay = (zRandF2() * sndRandDelayVar + sndRandDelay) * 1000.0;
			sndFrameTime = (randDelay) * 1000.0 + ztimer.totalTimeFloat;
		}
		break;
	}

	if ( sndIsPlaying ) {
		if ( sndObstruction || sndVolType == ELLIPSOID || volScale < 1.0 ) {
			zTSound3DParams params;
			if ( zsound->GetSound3DProps(sndHandle, &params) ) {
				params.__outer_volume = CalcObstruction();
				params.radius = sndRadius;
				params.volume = CalcVolumeScale() * volScale;
				zsound->UpdateSound3D(sndHandle, &params);
				return;
			}
		}
		zsound->UpdateSound3D(sndHandle, 0);
	}
}

void zCVobSound::CalcObstruction()
{
	if ( sndObstruction && zCCamera::activeCam && zCCamera::activeCam->connectedVob ) {
		if ( vec2.z > 0.0 ) {
			auto obsTrans = obsTimer / 1000.0;
			float obstruction = (obsPrev - obsCur) * obsTrans + obsCur;
			obsTimer -= ztimer.frameTimeFloat;
			return obstruction;
		}

		obsTimer = 1000.0;

		zVEC3 start = GetPositionWorld();

		auto connvob = zCCamera::activeCam->connectedVob;

		zVEC3 pos = connvob->GetPositionWorld();
		zVEC3 ray;

		ray.x = pos.x - start.x;
		ray.y = pos.y - start.y;
		ray.z = pos.z - start.z;

		// yes, it is duplicated in assembly
		start.x = trafoObjToWorld.m[0][3];
		start.y = trafoObjToWorld.m[1][3];
		start.z = trafoObjToWorld.m[2][3];

		auto hit = sndWorld->TraceRayNearestHit(start, ray, 0, 2);
		obsPrev = obsCur;
		obsCur = hit ? 1.0 : 0.0;
		if ( obsPrev == -1.0 )// -1.0
			obsPrev = obsCur;
		return obsPrev;
	}

	return 0.0;
}

zSTRING zCVobSound::GetDebugDescString()
{
	zSTRING vol;
	if (sndVolType == ELLIPSOID) {
		auto volScale = CalcVolumeScale();
		vol = ", ellRad: "_s + volScale;
	}

	zSTRING obs;
	if ( sndObstruction ) {
		obs = "obs: " + obsCur;
	}
	
	auto zdesc = zCZone::GetDebugDescString();
	return zdesc + ", " + obs + vol;
}

double zCVobSound::CalcVolumeScale()
{
	float volScale = sndVolume * 0.01;
	if ( sndVolType == ELLIPSOID ) {
		if ( zCCamera::activeCam && zCCamera::activeCam->connectedVob ) {
			if ( this->sndRadius > 0.0 ) {
				auto connVob = zCCamera::activeCam->connectedVob;
				auto camPos = connVob->GetPositionWorld();
				auto sndPos = bbox3D->GetCenter();
				auto dir = camPos - sndPos;

				auto halfExtents = bbox3D.GetExtents() * 0.5;

				// wtf?
				for (int i = 0; i < 3; ++i) {
					if (halfExtents[i] <= 0.0)
						return 1.0;
					// fabs was "and eax, 7FFFFFFFh"
					halfExtents[i] = fabs(1.0 / halfExtents[i]);
				}

				auto prod = Alg_Prod(min, halfExtents);
				// same "and" thing
				float boxDist = fabs(prod.x) + fabs(prod.y) + fabs(prod.z);
				if ( boxDist == 0.0 )
					return 1.0;
				float inv = 1.0 / boxDist;

				// another wtf
				float f = 0.0;
				for (int i = 0; i < 3; ++i) {
					auto s = bbox3D.mins[i] + prod[i];
					prod[i] = fabs(s) * inv;
					f += (bbox3D.mins[i] - bbox3D.maxs[i]) * 0.5 * prod[i];

				}

				volScale *= f / sndRadius;
			}
		}
	}
	return volScale;
}
