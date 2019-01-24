struct oCParticleControl {
	oCParticleControl();
	oCParticleControl(zCWorld *theWorld);
	~oCParticleControl();

	zCVob* GetPFXVob()
	{
		return pfxVob;
	}

	void SetStartPos(zVEC3& pos)
	{
		startPos = pos;
	}

	void SetPFXName(zSTRING& newPFXName);

	void EditActivePFX();
	void EndEditActive();

	zCParticleFX *pfx = nullptr;
	zCVob *pfxVob     = nullptr;
	int unk1;
	zSTRING pfxName;
	zVEC3 startPos{ 0, 0, 0 };
	int unk2;
	zCWorld* world = nullptr;
	zCConsole *pfxConsole = nullptr;
};

//------------------------------------------------------------------------------
//_Ulf/oParticleControl.cpp
oCParticleControl::oCParticleControl()
{
	unk1 = -1;
	pfxName = "a.pfx";
	unk1 = 0;
	unk2 = 0;
}

oCParticleControl::oCParticleControl(zCWorld *theWorld)
	: oCParticleControl()
{
	pfx = new zCParticleFX(); // 97
	pfxVob = new zCVob(); // 98

	pfxVob->SetVobName("Particle_Test_Vob");
	pfxVob->SetCollDetStat(0);
	pfxVob->SetCollDetDyn(0);

	world = theWorld;
	world->AddVob(pfxVob);
}

oCParticleControl::~oCParticleControl()
{
	DELETE(pfxConsole);
	pfx = 0;
	pfxVob = 0;
	world = 0;
}

void oCParticleControl::SelectActivePFX()
{
	zCOLOR black = {0,0,0,-1};
	zrenderer->Vid_Clear(black, 3);
	zrenderer->Vid_Blit(1, 0, 0);
	zrenderer->Vid_Clear(black, 3);
	zrenderer->Vid_Blit(1, 0, 0);

	int x = 0;
	int y = 0;
	for ( int i = 0, e=zCParticleFX::s_emitterPresetList.GetNum(); i<e; ++i ) {
		auto& emitter = zCParticleFX::s_emitterPresetList[i];
		screen->Print(x, y, i + ":");
		zCView::Print(x + 220, y, emitter->pfxName);
		y += 200;
		if ( i % 36 == 35 ) {
			x += 2048;
			y = 0;
		}
	}

	int i = 0;
	do {
		zSTRING s = "";
		i = screen->Input(0, 7900, 2000, s).ToLong();
	}
	while ( i < 0 || i >= zCParticleFX::s_emitterPresetList.GetNum() );
	auto& emitter = zCParticleFX::s_emitterPresetList[i];

	pfxName = emitter->pfxName;

	while ( zinput->KeyPressed(KEY_MULTIPLY)  ||
	        zinput->KeyPressed(KEY_BACKSLASH) ||
	        zinput->KeyPressed(KEY_RETURN) )
	{
		zCInputCallback::GetInput();
		sysEvent();
	}
}

void oCParticleControl::EditActivePFX()
{
	if ( pfxVob && zCParticleFX::s_pfxParser )
	{
		if ( pfxConsole )
			delete pfxConsole;
		pfxConsole = new zCConsole(); // 179
		pfxConsole->SetParser(zCParticleFX::s_pfxParser);
		pfxConsole->SetPos(0, 0);
		pfxConsole->EditInstance(pfxName, pfx->emitter);

		EndEditActive(); // was inlined
	}
}

void oCParticleControl::StartActivePFX(zCVob *homeVob, zSTRING& mnode)
{
	if ( !world )
		return;

	DeleteLastPFX();

	auto model = static_cast<zCModel*>(homeVob->visual);

	if ( auto node = model->SearchNode(mnode) ) {
		pfx = new zCParticleFX(); // 205
		pfxVob = new zCVob(); // 206

		pfxVob->SetVobName("Particle_Test_Vob");
		pfxVob->SetCollDetStat(0);
		pfxVob->SetCollDetDyn(0);

		world->AddVob(pfxVob);

		model->AttachChildVobToNode( pfxVob, node );
		model->UpdateAttachedVobs();

		pfxVob->SetVisual(pfx);
		pfx->SetAndStartEmitter( pfxName, 0 );
		pfx->flags |= 2;
	}
}

void oCParticleControl::StartActivePFX()
{
	if ( world ) {
		DeleteLastPFX();

		// looks like it leaks memory?
		pfx = new zCParticleFX(); // 228
		pfxVob = new zCVob(); // 229

		pfxVob->SetVobName("Particle_Test_Vob");
		pfxVob->SetCollDetStat(0);
		pfxVob->SetCollDetDyn(0);
		pfxVob->Move(startPos.x, startPos.y, startPos.z);

		world->AddVob(pfxVob);

		pfxVob->SetVisual(pfx);
		pfx->SetAndStartEmitter( pfxName, 0 );
		pfx->flags |=  2;
	}
}

void oCParticleControl::CreateNewPFX(zSTRING& newPFXName)
{
	newPFXName.Upper();
	zoptions->ChangeDir(DIR_SCRIPTS);

	auto file = zfactory->CreateZFile( "System\\Pfx\\PfxInst.d" );
	file->Open( 0 ); // weird, why 0?
	file->Append();
	zSTRING s = "\nINSTANCE " + newPFXName + "(C_PARTICLEFX)\n";
	file->Write(s);
	s = "{\n";
	file->Write(s);
	s = "};\n";
	file->Write(s);
	file->Close();
	delete file;

	ParseParticleFXScript();
}

void oCParticleControl::BeginEditActive(void (__cdecl *func)(const zSTRING&))
{
	if ( pfxVob && zCParticleFX::s_pfxParser )
	{
		if ( pfxConsole )
			delete pfxConsole;
		pfxConsole = new zCConsole(); // 290
		pfxConsole->SetParser(zCParticleFX::s_pfxParser);
		pfxConsole->SetPos(0, 0);
		pfxConsole->EditInstance(pfxName, pfx->emitter);
		pfxConsole->SetChangedFunc(func);
	}
}


void oCParticleControl::SetPFXName(zSTRING& newPFXName)
{
	pfxName = newPFXName;
	if (pfx) {
		auto emitter = zCParticleFX::SearchParticleEmitter(pfxName);
		pfx->SetEmitter(emitter, 0);
	}
}

void oCParticleControl::SetWorld(zCWorld *theWorld)
{
	if ( pfxVob ) {
		world->RemoveVob(pfxVob);
		RELEASE( pfxVob );
	}

	world = theWorld;
}

void oCParticleControl::EndEditActive()
{
	auto& emitter = zCParticleFX::SearchParticleEmitter(pfxName);
	emitter = pfx->emitter;
	emitter->UpdateInternals();
	pfx->emitter->UpdateInternals();
}

void oCParticleControl::DeleteLastPFX()
{
	if ( pfxVob ) {
		if ( auto world = pfxVob->homeWorld ) {
			if ( pfxVob->refCtr > 1 )
				world->RemoveVob(pfxVob);
			RELEASE(pfxVob);

			pfxVob = new zCVob(); // 303
			pfx = new zCParticleFX(); //304
		}
	}
}
