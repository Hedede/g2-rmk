int oBert_Console_EvalFunc(const zSTRING& cmdString, zSTRING& out)
{
	zSTRING word1 = cmdString.PickWord(1, " ", zSTR_SKIP);
	zSTRING word2 = cmdString.PickWord(2, " ", zSTR_SKIP);
	zSTRING word3 = cmdString.PickWord(3, " ", zSTR_SKIP);
	zSTRING word4 = cmdString.PickWord(4, " ", zSTR_SKIP);


	out = "ok";

	if (word1 == "SET") {
		if (word2 == "CLIPPINGFACTOR") {
			float factor = word3.ToFloat();
			
			if (factor < 0.05)
				factor = 0.05;
			if (factor > 10.0)
				factor = 10.0;

			zCWorld* world = ogame->GetWorld();
			if (world) {
				zCSkyControler skyctrl = world->GetActiveSkyControler();
				if (skyctrl)
					skyctrl->SetFarZScalability(factor);
			}
		}
		return 0;
	}
	if ( word1 == "LIST") {
		if (word2 == "CS") {
			zCWorld* world = ogame->GetWorld();

			world->csPlayer->PrintListOfCutscenes(word3);

			out = "toggled cutscene-debuginfos";
			return 1;
		}
		return 0;
	}
	if ( word1.data == "HERO" )
	{
		if ( word2.data == "EXPORT" && oCNpc::player ) {
			if ( word3.IsEmpty())
				word3 = "hero_export.zen";

			if ( word3.Search(0, ".zen", 1) < 0 )
				word3 += ".zen";

			zSTRING dir = zoptions->GetDirString(2);
			zFILE_FILE file(dir + word3);

			if ( file.Exists() )
				file.FileDelete();

			if ( file.Create() != 0 ) {
				out = "\"" + file.GetFullPath() + "\" could not be created.";
				return 0;
			}

			auto archiver = zarcFactory->CreateArchiverWrite(file, 1, 1, 0);

			archiver->WriteObject(oCNpc::player);
			archiver->Close()

			out = "Exported your hero to filee \"" + file.GetFullPath() + "\".";
			return 1;
		}
		if ( word2.data == "IMPORT" && oCNpc::player ) {
			if ( word3.IsEmpty())
				word3 = "hero_export.zen";

			if ( word3.Search(0, ".zen", 1) < 0 )
				word3 += ".zen";

			zSTRING dir = zoptions->GetDirString(2);
			zFILE_FILE file(dir + word3);

			if ( !file.Exists() || file->Open(0) != 0)
			{
				out = "\"" + file.GetFullPath() + "\" does not exist.";
				return 1;
			}

			auto* archiver = zCArchiverFactory::CreateArchiverRead(file, 0);
			zCObject* obj;
			archiver->ReadObject(obj);
			oCNpc* hero = dynamic_cast<oCNpc*>(obj);

			archiver->Close();

			if ( !hero || !ogame )
				return 1;

			oCNpc* player = oCNpc::player;

			zVEC3 translation;
			zVEC3 atvec;
			player->trafoObjToWorld.GetTranslation(translation);
			player->trafoObjToWorld.GetAtVector(atvec);

			player->RemoveVobFromWorld();
			hero->SetPositionWorld(trafoObjToWorld);
			hero->SetHeadingAtWorld(atvec);

			zCWorld* world = ogame->GetWorld();
			world->AddVob(hero);
			hero->SetAsPlayer();

			out = "Imported your hero from file \"" + file.GetFullPath() + "\".";
			return 1;
		}
		return 0;
	}
	if ( word1.data == "TOGGLE" )
	{
		if ( word2.data == "CUTSCENES" && word3.data == "ENABLED" )
		{
			zCCSCutsceneContext::DisablePlayCutscenes(dword_8C218C == 0);
			if ( dword_8C218C )
			{
				out = "Cutscene-System disabled ";
				zCWorld* world = ogame->GetWorld();
				world->csPlayer->StopAllCutscenes(0);
				return 1;
			}
			output = "Cutscene-System enabled";
			return 1;
		}
		if ( word2.data == "CS" && word3.data == "LIST" )
		{
			zCWorld* world = ogame->GetWorld();
			world->csPlayer->ToggleDebugInfo();
			output = "toggled cutscene-debuginfos";
		}
		return 0;
	}
	if ( word1.data == "SHOW" ) {
		if ( word2.data != "CS" )
			return 0;

		zINFO(1, "B: CS: Informations about cutscene \"" + word3 +"\"");
		zCCSManager* csman = ogame->GetCutsceneManager();

		int numplayed = csman->PoolNumPlayed(word3);
		if ( numplayed >= 0 ) {
			zINFO(1, "B: CS: CSPool: Has been started.");
			zINFO(1, "B: CS: CSPool: Has been played " + zSTRING(numplayed) + " times");
		}
		else
		{
			zINFO(1, "B: CS: CSPool: It wasn't played yet.");
		}

		zCCSCutsceneContext* cscontext = ogame->GetWorld()->csPlayer->GetStartedCutscene(word3);
		if ( cscontext ) {
			if ( cscontext->IsPlaying() )
				zINFO(1, "B: CS: CSPlayer: Cutscenes is playing right now.");
			else
				zINFO(1, "B: CS: CSPlayer: Cutscenes is sleeping.");
		}
		zINFO(1, "");
		return 1;
	}

	if ( word1 != "PLAY")
		return 0;

	if (word2 == "CS") {
		oCNpc* npc = oCNpc::player;
		if ( oCNpc::player->GetFocusNpc() )
			npc = oCNpc::player->GetFocusNpc();

		if ( npc )
		{
			auto msg = oCMsgConversation::Create(6, word3, npc->GetObjectName())
			npc->GetEM(0)->OnMessage(msg, npc);
			out = "Started playing cutscene for npc \"" + npc->GetObjectName() + "\"";
			return 1;
		}
	} else if ( word2 == "VIDEO") {
		gameMan->PlayVideo(gameMan, word3);
		return 1;
	}
	return 0;
}

// _dieter/zzDieter.cpp
int zDieter_EvalFunc(zSTRING const& cmd, zSTRING& msg)
{
	if ( !zcon.GetWorld() ) {
		msg = "ERROR: no zCWorld registered to zcon !";
		return 0;
	}

	msg = "ok";
	if ( cmd.Search(0, "ZFOGZONE", 1u) != -1 ) {
		if ( !done ) {
			done = 1;

			auto zone1 = new zCZoneZFog; //v3, v4
			callback.HandleVob = v3;
			auto zonedef = new zCZoneZFogDefault; // v5
			callback.HandleVob = v5;

			zVEC3 pos{50327.0, -2527.0, -11829.0};
			zVEC3 min{-30000.0, -30000.0, -30000.0};
			zVEC3 max{30000.0, 30000.0, 30000.0};

			zTBBox3d bbox;
			bbox.mins = min + pos;
			bbox.maxs = max + pos;
			zone1->SetBBox3DWorld(bbox);
			zone1->move(pos);

			zone1->innerRangePerc = 0.5f;
			zone1->fogRangeCenter = 5000.0f;

			zcon.GetWorld()->AddVob(zone1);

			auto zone2 = new zCZoneZFog();
			zTBBox3d bbox2;
			bbox2.mins = {-61574.0, 0, -130000.0}
			bbox2.maxs = {-42000.0, 16000.0, 30000.0}
			zVEC3 pos2{-51787.0, 8000.0, -50000.0};
			zone2->SetPositionWorld(pos2);
			zone2->SetBBox3DWorld(bbox2);

			zcon.GetWorld()->AddVob(zone2);

			zonedef->SetPositionWorld(zVEC3{0,0,0});
			zonedef->fogRangeCenter(50000.0);

			zcon.GetWorld()->AddVob(zondef);
			return 1;
		}
		return 0;
	}

	if ( cmd.Search(0, "ZMARK", 1u) != -1 ) {
		zcon.GetWorld()->DebugMarkOccluderPolys();
		msg = "marking occluder polys (outdoor)":
		return 1;
	}

	if ( cmd.Search(0, "ZWORLD STATUS", 1u) != -1 ) {
		zcon.GetWorld()->PrintStatus();
		return 1;
	}

	if ( cmd.Search(0, "ZWORLD VOBTREE", 1u) != -1 ) {
		zcon.GetWorld()->PrintGlobalVobTree(0, 0);
		return 1;
	}

	if ( cmd.Search(0, "ZWORLD ACTIVEVOBS", 1u) != -1 ) {
		zcon.GetWorld()->PrintActiveVobs();
		return 1;
	}

	if ( cmd.Search(0, "ZTOGGLE PFXINFOS", 1u) != -1 ) {
		zCParticleFX::s_showDebugInfo = !zCParticleFX::s_showDebugInfo;
		return 1;
	}

	if ( cmd.Search(0, "ZSET RELIGHTTIME", 1u) != -1 ) {
		auto word = cmd.PickWord(3, " ", " ");
		auto val = word.ToFloat();
		if ( zCSkyControler::s_activeSkyControler )
			zCSkyControler::s_activeSkyControler->SetRelightTime(val);
		return 1;
	}

	if ( cmd.Search(0, "ZTOGGLE LIGHTSTAT", 1u) != -1 ) {
		auto world = zcon.GetWorld();
		if ( world->worldRenderMode == 1 ) {
			world->worldRenderMode = 0;
			world->bspTree.worldRenderMode = 0;
		} else {
			world->worldRenderMode = 1;
			world->bspTree.worldRenderMode = 1;
		}
		return 1;
	}

	if ( cmd.Search(0, "ZTOGGLE SHOWZONES", 1u) != -1 ) {
		auto world = zcon.GetWorld();
		world->showZonesDebugInfo = !world.showZonesDebugInfo;
		return 1;
	}

	if ( cmd.Search(0, "ZTOGGLE SHOWTRACERAY", 1u) != -1 ) {
		auto world = zcon.GetWorld();
		world->showTraceRayLines = !world->showTraceRayLines;
		return 1;
	}

	if ( cmd.Search(0, "ZRMODE MAT", 1u) != -1 ) {
		zrenderer->SetPolyDrawMode(0);
		return 1;
	}

	if ( cmd.Search(0, "ZRMODE WMAT", 1u) != -1 ) {
		zrenderer->SetPolyDrawMode(1);
		return 1;
	}

	if ( cmd.Search(0, "ZRMODE FLAT", 1u) != -1 ) {
		zrenderer->SetPolyDrawMode(2);
		return 1;
	}

	if ( cmd.Search(0, "ZRMODE WIRE", 1u) != -1 )
	{
		zrenderer->SetPolyDrawMode(3);
		return 1;
	}

	if ( cmd.Search(0, "ZTOGGLE VOBBOX", 1u) != -1 ) {
		Toggle(zcon.GetWorld()->bspTree.drawVobBBox3D);
		return 1;
	}

	if ( cmd.Search(0, "ZTOGGLE RENDERVOB", 1u) != -1 ) {
		Toggle(zCVob::s_renderVobs);
		return 1;
	}

	if ( cmd.Search(0, "ZRNDSTAT", 1u) != -1 ) {
		zTRnd_Stats stats;
		zrenderer->GetStatistics(stats);
		zINFO("D: *** zCRenderer Statistics ***");
		zINFO("D: texture memory fetched   : "_s + stats.memFetched);
		zINFO("D: texture memory used      : "_s + stats.memUsed);
		return 1;
	}

	if ( cmd.Search(0, "ZVIDEORES", 1u) != -1 ) {
		auto x = cmd.PickWord(2, " ", " ").ToLong();
		auto y = cmd.PickWord(3, " ", " ").ToLong();
		auto b = cmd.PickWord(4, " ", " ").ToLong();

		zCView::SetMode(x, y, b, 0);
		return 1;
	}

	if ( cmd.Search(0, "ZTOGGLE MARKPMESHMATERIALS", 1u) != -1 ) {
		Toggle(zCProgMeshProto::s_markMeshMaterials);
		return 1;
	}

	if ( cmd.Search(0, "ZTOGGLE PMESHSUBDIV", 1u) != -1 ) {
		Toggle(zCProgMeshProto::s_autoSubdivEnabled);
		return 1;
	}

	if ( cmd.Search(0, "ZTOGGLE TNL", 1u) != -1 ) {
		if ( Toggle(zCRenderManager::s_useRendererTNL) ) {
			msg = "tnl on";
		} else {
			msg = "tnl off";
		}
		return 1;
	}

	if ( cmd.Search(0, "ZTOGGLE FARFADE", 1u) != -1 ) {
		Toggle(zCWorld::s_bFadeOutFarVerts);
		return 1;
	}

	if ( cmd.Search(0, "ZLIST MESHESWITHLOTSOFMATERIALS", 1u) != -1 ) {
		auto min = cmd.PickWord(3u, " ", " ").ToLong();
		if (min < 4)
			min = 4;

		unsigned i = 0;
		for (auto obj :  zCProgMeshProto::classDef.objectList) {
			auto prog = dynamic_cast<zCProgMeshProto*>(obj);
			if ( prog->numSubMeshes >= min ) {
				zINFO("D "_s + i + ", " + prog->GetObjectName() + ": " + prog->numSubMeshes);
			}
		}

		unsigned nummesh[8];
		unsigned numsub = 0;
		unsigned numtri = 0;
		unsigned numverts = 0;

		for (auto obj :  zCProgMeshProto::classDef.objectList) {
			auto prog = dynamic_cast<zCProgMeshProto*>(obj);
			numsub += prog->numSubMeshes;
			numtri += prog->GetNumTri();
			numverts += prog->numVerts;
			++nummesh[numsub];
		}

		unsigned numprog = zCProgMeshProto::classDef.objectList.GetNum();
		zINFO(0, "D: avgMatsPerMesh: "_s  + float(numsub) / numprog); // 467,
		zINFO(0, "D: avgTrisPerMesh: "_s  + float(numtri) / numprog); // 468
		zINFO(0, "D: avgVertsPerMesh: "_s + float(numverts) / numprog); // 468

		for (unsigned i = 0; i < 7; ++i)
			zINFO("D: numMat: "_s + i + ", numMeshes: " + nummesh[i]); // 472

		return 1;
	}

	if ( cmd.Search(0, "ZLIST MESH", 1u) != -1 ) {
		zINFO("D: *** enumerating zCMesh ***");
		unsigned i = 0;
		for (auto node = zCMesh::s_meshRoot; node; node = node->next)
			zINFO("D: "_s + (i++) + ": " + node->GetObjectName() + " (ref: " + node->refCtr + ")");

		zINFO("D: ****************************");
		return 1;
	}


	if ( cmd.Search(0, "ZLIST TEX", 1u) != -1 ) {
		zINFO("D: *** enumerating zCTexture ***");

		unsigned cachedIn  = 0;
		unsigned numLocked = 0;
		for (auto tex : zCTexture::classDef.objectList) {
			if ( (tex->cacheState & 3) == 3 )
				++cachedIn;
			if ( tex->cacheState & 4 )
				++numLocked;
		}

		zINFO(0, "D: numLiving objects: "_s + zCTexture::classDef.numLiving); // 501
		zINFO(0, "D: numObjectList    : "_s + zCTexture::classDef.objectList.GetNum()); // 502
		zINFO(0, "D: numCachedIn      : "_s + cachedIn); // 503
		zINFO(0, "D: numLocked        : "_s + numLocked);

		if ( cmd.Search(0, "ZLIST TEX FULL", 1u) != -1 ) {
			size_t mem = 0;
			for (auto tex : zCTexture::classDef.objectList) {
				int x, y;
				tex->GetObjectName(x, y);
				zINFO("D: "_s + (i++) + ": " + tex->GetObjectName() + ", " + x + "x" + y + " (ref: " + tex->refCtr + ")");
				mem += tex->GetMemSizeBytes();
			}

			zINFO( "D: mem total: "_s + mem );
		}

		zINFO("D: ****************************");
		return 1;
	}

	if ( cmd.Search(0, "ZLIST MAT", 1u) != -1 ) {
		zINFO("D: *** enumerating zCMaterial ***");

		for (auto obj : zCMaterial::classDef.objectList)
			zINFO("D: "_s + (i++) + ": " + obj->GetObjectName() + " (ref: " + obj->refCtr + ")");

		zINFO("D: ****************************");
		return 1;
	}

	if ( cmd.Search(0, "ZLIST CLASSTREE", 1u) != -1 ) {
		zCObject::PrintClassHierarchy(0, 0, 0, 0);
		return 1;
	}

	if ( cmd.Search(0, "ZLIST VOBSWITHBIGBBOX", 1u) != -1 ) {
		zCVobCallback_PrintBigBBox callback;

		zINFO_B(1, "D: ***********"); // 187
		zcon.GetWorld()->TraverseVobTree(callback, 0, 0);
		zINFO_C(1, "D: ***********"); // 188
	
		return 1;
	}

	if ( cmd.Search(0, "ZLIST", 1u) != -1 ) {
		auto word = cmd.PickWord(2, " ", " ");

		auto classdef = GetClassDefTypeInsensitive(word);
		if ( word ) {
			zINFO( "D: *** Enumerating " + word + " ***");

			int i = 0;
			for (auto obj : classdef.objectList)
				zINFO("D: "_s + (i++) + ": " + obj->GetObjectName() + " (ref: " + obj->refCtr + ")");

			zINFO("D: ****************************");
		}

		return 1;
	}

	if ( cmd.Search(0, "ZMOVECAMTOVOB", 1u) != -1 ) {
		auto vob = GetVob(cmd, 2u);
		if ( !vob ) {
			msg = "no vob found";
			return 1;
		}

		if ( zCCamera::activeCam && zCCamera::activeCam->connectedVob ) {
			auto pos = vob->GetPositionWorld();
			zCCamera::activeCam->connectedVob->SetPositionWorld(pos);
		}
		return 1;
	}

	if ( cmd.Search(0, "ZWORLD VOBPROPS", 1u) != -1 ) {
		auto vob = GetVob(cmd, 3u);
		if ( vob ) {
			zSTRING str;
			auto arc = zarcFactory.CreateArchiverWrite(2, 0, 0);

			zASSERT( arc ); //  567

			arc->WriteObject(vob);
			arc->GetBufferString(str);
			arc->Close();
			arc->Release();

			for (int i = 0; i < 100; ++i) {
				auto line = str.PickWord(i, "\r\n", "\r\n");

				if (!line)
					break;
				zINFO("D: " + line);
			}
			return 1;
		}

		msg = "no vob found";
		return 1;
	}

	if ( cmd.Search(0, "ZMODEL PRINTTREE", 1u) != -1 ) {
		auto word = cmd.PickWord(3, " ", " ");

		if ( auto proto = zCModelPrototype::SearchName(word) )
			proto->DescribeTree(0, 0);

		return 1;
	}

	if ( cmd.Search(0, "ZSOUNDMAN DEBUG", 1u) != -1 ) {
		if ( zsndMan )
			Toggle(zsndMan->debugEnabled);
		return 1;
	}

	if ( cmd.Search(0, "ZTRIGGER", 1u) != -1 ) {
		auto word = cmd.PickWord(2, " ", " ");

		if ( word.Search(0, "TIMEDEMO", 1u) == -1 ) {
			zCArray<zCVob*> vobs;
			zcon.GetWorld()->SearchVobListByName(word, vobs);
			for (auto vob : vobs) {
				auto cv = zCCamera::activeCam->connectedVob;
				vob->GetEM()->OnTrigger(cv, cv);
			}
		}
		return 1;
	}

	if ( cmd.Search(0, "ZUNTRIGGER", 1u) != -1 ) {
		zCArray<zCVob*> vobs;
		auto word = cmd.PickWord(2, " ", " ");
		zcon.GetWorld()->SearchVobListByName(word, vobs);
		for (auto vob : vobs) {
			auto cv = zCCamera::activeCam->connectedVob;
			vob->GetEM()->OnUntrigger(cv, cv);
		}
		return 1;
	}

	if ( cmd.Search(0, "ZTOGGLE MODELSKELETON", 1u) != -1 ) {
		Toggle(zCModel::s_drawSkeleton);
		return 1;
	}

	if ( cmd.Search(0, "ZTOGGLE SMOOTHROOTNODE", 1u) != -1 ) {
		if ( Toggle(zCModel::s_bSmoothRootNode) ) {
			msg = "smoothing on";
		} else {
			msg = "smoothing off";
		}

		return 1;
	}

	if ( cmd.Search(0, "ZSTARTANI", 1u) != -1 ) {
		auto vob = GetVob(cmd, 2u);
		if ( vob ) {
			auto vis = dynamic_cast<zCVisualAnimate*>(vob->GetVisual());

			auto ani = cmd.PickWord(3, " ", " ");
			vob->SetSleeping(0);
			vis->StartAnimation(ani);
		}

		return 1;
	}

	if ( cmd.Search("ZARCTEST", 1u) != -1 ) {
		zCArchiverGeneric::TestClassIntegrity(0);
		zCEventMessage::TestClassIntegrity();
		return 1;
	}

	if ( cmd.Search("ZRNDMODELIST", 1u) != -1 ) {
		if ( !zrenderer )
			return 1;
		zINFO("D: Enumerating renderer modes/devices...");

		for (auto i = 0; i < zrenderer->Vid_GetNumDevices(); ++i ) {
			zTRnd_DeviceInfo info;
			zrenderer->Vid_GetDeviceInfo(info);

			auto active = zrenderer->Vid_GetActiveDeviceNr();
			zINFO("D: device "_s + i + ": " + info.name + ((active == i) ?  " (active)" : ""));
		}

		for (auto i = 0; i < zrenderer->Vid_GetNumModes(); ++i) {
			zTRnd_VidModeInfo info;
			zrenderer->Vid_GetModeInfo(info);
			zINFO("D: mode "_s + i + ": " + info.xdim + "x" + info.ydim + "x" + info.bpp + ((info.canWindowed) ? " f/w" : " f"));
		}

		return 1;
	}

	if ( cmd.Search("ZOVERLAYMDS APPLY", 1u) == -1 ) {
		auto vob = GetVob(cmd, 3u);
		if ( vob ) {
			auto visual = vob->GetVisual();
			if (auto model = dynamic_cast<zCModel*>(visual)) {
				// yes, not a typo
				model = dynamic_cast<zCModel*>(visual);

				auto word = cmd.PickWord(4, " ", " ");
				model->ApplyModelProtoOverlay(word);
				return 1;
			}
		}

		msg = "not found";
		return 1;
	}

	if ( cmd.Search("ZOVERLAYMDS REMOVE", 1u) == -1 ) {
		auto vob = GetVob(cmd, 3u);
		if ( vob ) {
			auto visual = vob->GetVisual();
			if (auto model = dynamic_cast<zCModel*>(visual)) {
				// yes, not a typo
				model = dynamic_cast<zCModel*>(visual);

				auto word = cmd.PickWord(4, " ", " ");
				model->RemoveModelProtoOverlay(word);
				return 1;
			}
		}

		msg = "not found";
		return 1;
	}

	if ( cmd.Search("ZTOGGLE TEXSTATS", 1u) != -1 ) {
		auto world = zcon.GetWorld(); // didn't fit on the line
		world->SetShowTextureStats(!world->GetShowTextureStats());
		return 1;
	}

	if ( cmd.Search("ZTOGGLE RESMANSTATS", 1u) != -1 ) {
		zresMan->SetShowDebugInfo(zresMan->GetShowDebugInfo());
		return 1;
	}

	if ( cmd.Search("ZPROGMESHLOD", 1u) != -1 ) {
		auto factor = cmd.PickWord(2, " ", " ").ToFloat();
		zCProgMeshProto::SetLODParamStrength(factor);
		return 1;
	}

	if ( cmd.Search("ZTOGGLE SHOWMEM", 1u) != -1 ) {
		InstallShowMem();
		return 1;
	}

	if ( cmd.Search("ZTOGGLE VOBMORPH", 1u) != -1 ) {
		zCVob::SetAnimationsEnabled(!zCVob::GetAnimationsEnabled());
		return 1;
	}

	if ( cmd.Search("ZTOGGLE MATMORPH", 1u) != -1 ) {
		zCWorld::SetWaveAnisEnabled(!zCWorld::GetWaveAnisEnabled());
		return 1;
	}

	if ( cmd.Search("ZMEM DUMPHEAP BYTESPERLINE", 1u) != -1 )
	{
		zCMalloc::DumpHeap(0);
		return 1;
	}

	if ( cmd.Search("ZMEM DUMPHEAP BLOCKSPERLINE", 1u) != -1 ) {
		zCMalloc::DumpHeap(1);
		return 1;
	}

	if ( cmd.Search("ZMEM DUMPHEAP SORTBYTIME", 1u) != -1 ) {
		zCMalloc::DumpHeap(5);
		return 1;
	}

	if ( cmd.Search("ZMEM CHECKHEAP", 1u) != -1 ) {
		if ( !zCMalloc::CheckHeap() ) {
			msg = "CheckHeap() failed";
		}
		return 1;
	}

	if ( cmd.Search("ZTOGGLE SHOWPORTALS", 1u) != -1 ) {
		zCBspTree::SetShowPortals(!zCBspTree::GetShowPortals());
		return 1;
	}

	if ( cmd.Search("ZTOGGLE SHOWHELPVERVISUALS", 1u) != -1 ) {
		zCVob::SetShowHelperVisuals(!zCVob::GetShowHelperVisuals());
		return 1;
	}

	if ( cmd.Search("ZSTARTRAIN", 1u) != -1 ) {
		auto sktrl = zcon.GetWorld()->GetActiveSkyControler();
		if ( auto outdoor = dynamic_cast<zCSkyControler_Outdoor*>(sktrl) ) {
			auto factor = cmd.PickWord(2, " ", " ").ToFloat();

			outdoor->SetWeatherType(1);
			outdoor->SetRainFXWeight(factor, 0.1);

			msg = "started rain";
		}
		return 1;
	}

	if ( cmd.Search("ZSTARTSNOW", 1u) != -1 ) {
		auto sktrl = zcon.GetWorld()->GetActiveSkyControler();
		if ( auto outdoor = dynamic_cast<zCSkyControler_Outdoor*>(sktrl) ) {
			auto factor = cmd.PickWord(2, " ", " ").ToFloat();

			outdoor->SetWeatherType(0);
			outdoor->SetRainFXWeight(factor, 0.1);

			msg = "started rain";
		}
		return 1;
	}

	if ( cmd.Search("ZSET VOBFARCLIPZSCALER", 1u) != -1 ) {
		auto factor = cmd.PickWord(3, " ", " ").ToFloat();
		zcon.GetWorld()->SetVobFarClipZScalability(factor);
		return 1;
	}

	if ( cmd.Search("ZSET LEVELFARCLIPZSCALER", 1u) != -1 ) {
		auto factor = cmd.PickWord(3, " ", " ").ToFloat();
		zcon.GetWorld()->GetActiveSkyControler()->SetFarZScalability(factor);
		return 1;
	}

	if ( cmd.Search("ZHIGHQUALITYRENDER", 1u) != -1 ) {
		auto world = zcon.GetWorld();
		world->SetVobFarClipZScalability(30.0);
		world->GetActiveSkyControler()->SetFarZScalability(30.0);
		zCProgMeshProto::SetLODParamStrengthModifier(1.0);
		zCTexture::RefreshTexMaxSize(0x4000);
		return 1;
	}

	if ( cmd.Search("ZTIMER REALTIME", 1u) == -1 ) {
		ztimer.SetMotionFactor(1.0);
		return 1;
	}

	if ( cmd.Search("ZTIMER MULTIPLIER", 1u) != -1 ) {
		auto factor = cmd.PickWord(3, " ", " ").ToFloat();
		ztimer.SetMotionFactor( factor );
		return 1;
	}

	msg.Clear();
	return 0;
}
