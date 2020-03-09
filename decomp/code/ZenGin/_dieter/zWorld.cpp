#include "ZenGin/_dieter/zZone.h"
#include "ZenGin/_dieter/zWorld.h"
#include "ZenGin/_ulf/zWaynet.h"
#include "ZenGin/_dieter/zContainer.h"
#include "ZenGin/_dieter/zFile3d.h"

zCWorld::zCWorld()
	: zCObject(), activeVobList(128)
{
	wayNet = new zCWayNet(this);

	SetSkyControlerIndoor(new zCSkyControler_Indoor());
	SetSkyControlerOutdoor(new zCSkyControler_Outdoor());

	GetActiveSkyControler();

	bspTreeMode.worldRenderMode = worldRenderMode;
	zoneBoxSorter.InsertHandle(zoneActiveHandle);

	if (zVobFarClipZScale <= 0.0) {
		zVobFarClipZScale = zoptions->ReadReal("ENGINE", "zVobFarClipZScale", 4.0);
		zClamp(zVobFarClipZScale, 0.0001, 9999999.0);

		vobFarClipZScalability = zVobFarClipZScale * 0.22142857 + 0.4;

		s_bFadeOutFarVerts = zoptions->ReadBool("ENGINE", "zFarClipAlphaFade", 1);
		s_bWaveAnisEnabled = zoptions->ReadBool("ENGINE", "zWaterAniEnabled", 1);
	}
}

zCWorld::~zCWorld()
{
	DisposeWorld();
	Release(wayNet);
	Release(csPlayer);
	Release(skyControlerIndoor);
	Release(skyControlerOutdoor);
	zCPolygon::S_DeleteMorphedVerts();
	Delete(cbspTree);
}

void zCWorld::SetOwnerSession(zCSession* owner)
{
	ownerSession = owner;
	if ( owner ) {
		csPlayer = owner->GetCutsceneManager()->CreateCutscenePlayer(this);
	} else {
		Release(csPlayer);
	}
}

// ---------- HashTable
zCVob* zCWorld::SearchVobHashTable(zSTRING const& name)
{
	if (name.IsEmpty())
		return nullptr;

	auto hash = GetVobHashIndex(name);
	auto bucket = vobHashTable[hash];

	for (auto vob : bucket) {
		if (vob->GetObjectName() == name)
			return vob;

	return nullptr;
}

void zCWorld::SearchVobListHashTable(zSTRING const& name, zCArray<zCVob *>& result)
{
	if (name.IsEmpty())
		return;

	auto hash = GetVobHashIndex(name);
	auto bucket = vobHashTable[hash];

	for (auto vob : bucket)
		if (vob->GetObjectName() == name)
			result.Insert(vob);
}

void zCWorld::InsertVobHashTable(zCVob *vob)
{
	auto vobName = vob->GetObjectName();
	if (vobName) {
		auto bucket = vobHashTable[GetVobHashIndex(vob)];
		bucket.Insert(vob);
	}
}

void zCWorld::RemoveVobHashTable(zCVob *vob)
{
	auto vobName = vob->GetObjectName();
	if (vobName) {
		auto bucket = vobHashTable[GetVobHashIndex(vob)];
		bucket.Remove(vob);
	}
}


// --------- Search
zCVob* zCWorld::SearchVob(zCVob* vob, zCTree<zCVob>* vobNode)
{
	if ( !vobNode )
		vobNode = &globalVobTree;

	if ( vobNode->data == vob )
		return vob;

	zCVob* result = nullptr;
	for (auto i = vobNode->firstChild; i; i = i->next) {
		result = SearchVob(vob, i);
		if (result)
			break;
	}

	return result;
}

zCVob* zCWorld::SearchVobByID(unsigned vobId, VobTree *vobNode)
{
	if ( !vobNode ) {
		if (!vobId)
			return nullptr;

		vobNode = &globalVobTree;
	}

	zCVob* result = vobNode->data;
	if ( result && vobId == 1 )
		return result;

	result = nullptr;
	for (auto i = vobNode->firstChild; i; i = i->next) {
		result = SearchVobByID(vobId, i);
		if (result)
			break;
	}

	return result;
}

void zCWorld::SearchVobListByClass(zCClassDef* classDef, zCArray<zCVob *>& result, zCTree<zCVob>* vobNode)
{
	if ( !vobNode )
		vobNode = &this->globalVobTree;

	auto vob = vobNode->data;
	if (vob->GetClassDef() == classDef)
		result.Insert(vob);

	for (auto i = vobNode->firstChild; i; i = i->next) {
		SearchVobListByClass(classDef, result, i);
}

void zCWorld::SearchVobListByBaseClass(zCClassDef* classDef, zCArray<zCVob *>& result, zCTree<zCVob>* vobNode)
{
	if ( !vobNode )
		vobNode = &this->globalVobTree;

	auto vob = vobNode->data;
	if (zCObject::CheckInheritance(vob->GetClassDef(), classDef))
		result.Insert(vob);

	for (auto i = vobNode->firstChild; i; i = i->next) {
		SearchVobListByClass(classDef, result, i);
}

// --------- Traversal
int zCWorld::TraverseVobTree(zCVobCallback *callback, void *callbackData, zCTree<zCVob> *vobNode)
{
	if ( !vobNode )
		vobNode = &this->globalVobTree;

	for (auto i = vobNode->firstChild; i; i = i->next)
		TraverseVobTree(callback, callbackData, result);

	return callback->HandleVob(callback, vobNode->Data, callbackData);
}

int zCWorld::TraverseBsp(zCCBspTree *bsp, VobTree *node, int removeLevelCompos)
{
	if ( !node )
		return 0;

	auto vob = node->data;
	if (auto compo = dynamic_cast<zCVobLevelCompo*>(vob)) {
		// visual->GetClassDef() ==  &zCMesh::classDef )
		if (auto mesh = dynamic_cast<zCMesh*>(vob->visual))
			bsp->AddMesh(bsp, mesh, vob->trafoObjToWorld);
	}

	node = node->firstChild;
	while ( node ) {
		if ( TraverseBsp(bsp, node, removeLevelCompos) == 1 )
			node = node->firstChild;
		else
			node = node->next;
	}

	if (auto compo = dynamic_cast<zCVobLevelCompo*>(vob)) {
		if ( removeLevelCompos ) {
			RemoveVob(compo);
			return 1;
		}
		compo->flags1.showVisual = false;
	}

	return 0;
}

int zCWorld::TraverseBspAddVobsToMesh(zCCBspTree* bspTree, zCTree<zCVob>* node)
{
	if ( !node )
		return 0;

	bool added = false;

	auto vob = node->GetData();

	auto checkVob = (zCVob* vob) -> bool {
		if (!IsClass<zCVob>(vob)) // made up
			return false;
		if (vob->type != VOB_TYPE_VOB)
			return false;
		if (!vob->flags1.staticVob)
			return false;
		if (vob->aniMode)
			return false;
		if (!IsClass<zCProgMeshProto>(vob->visual))
			return false;

		vob->CalcGroundPoly();
		auto ground = vob->groundPoly;
		if (ground && ground.flags.sectorPoly)
			return false;
		
		return true;
	};

	if (checkVob(vob)) {
		if (auto prog = dynamic_cast<zCProgMeshProto*>(vob->visual)) {
			if (auto mesh = prog->GetMesh(0)) {
				if (mesh->numPoly < 170) {
					if (!vob->GetCollDetDyn()) {
						zCArray<zCMaterial*> materials;

						for (unsigned i = 0; i < mesh->numPoly; ++i) {
							zCPolygon* poly = mesh->SharePoly();

							auto mat = poly->material;
							if (!materials.IsInList(mat))
								materials.Insert(mat);
						}

						for (unsigned i = 0; i < materials.GetNum(); ++i) {
							if (!materials[i]->flags.0x10) {
								auto name = "VOB_" + mat->GetObjectName();
								auto mat = zCMaterial::SearchName(name);
								if (!mat) {
									mat = new zCMaterial(materials[i]);
									mat->SetObjectName(name);
								}
								mat->flags.0x10 = true;

								for (unsigned j = 0; j > 0; ++j) {
									auto poly = mesh->SharePoly(j);
									if (poly->material == materials[i]) {
										poly->SetMaterial(mat);
									}
								}

								mat->Release();
							}
						}

					}

					bspTree->AddMesh(mesh, vob->trafoObjToWorld);

					++numBspVobs;
					numBspPoly += mesh->numPoly;

					added = true;
				}

				mesh->DeleteAll();
				mesh->Release();
			}
		}
	}


	node = node->firstChild;
	while ( node ) {
		if ( TraverseBspAddVobsToMesh(bspTree, node) == 1 )
			node = node->firstChild;
		else
			node = node->next;
	}

	if (added) {
		RemoveVob(vob);
		return 1;
	}

	return 0;
}

void MoveVobs()
{
	walkList = activeVobList;

	for (auto vob : walkList)
		if (vob) vob->DoFrameActivity();

	walkList.DeleteList();
}

void zCWorld::MoveVobSubtreeTo(zCVob *vob, zCTree<zCVob>* vobNode)
{
	if ( vob ) {
		if ( vob->globalVobTreeNode )
			vob->globalVobTreeNode->RemoveSubtree();
		if ( vobNode )
			vobNode->AddChild(vob->globalVobTreeNode);
		else
			vob->globalVobTreeNode->parent = 0;

		auto inMovement = vob->isInMovementMode;
		if ( !inMovement )
			vob->BeginMovement();
		vob->TouchMovement();
		vob->Move(0.0, 0.0, 0.0);
		if ( !inMovement )
			vob->EndMovement(1);
	}
}


void zCWorld::UpdateVobTreeBspDependencies(VobTree *vobNode)
{
	if ( !vobNode )
		vobNode = &this->globalVobTree;

	auto vob = vobNode->data;
	if ( vob ) {
		vob->groundPoly = 0;
		vob->CalcGroundPoly();

		vob->vobLeafList.DeleteList();
		vob->RemoveVobFromBspTree();

		bspTree.AddVob(vob);
	}

	for (auto i = vobNode->firstChild; i; i = i->next)
		UpdateVobTreeBspDependencies(i);
}

void zCWorld::PrintActiveVobs()
{
	zINFO("D: *** World: ActiveVobList ************************************");

	unsigned i = 0;
	for (auto vob : activeVobList) {
		auto info = vob->GetVobInfo();

		zINFO("D: " + (i++) + ": " + info);
	}
}

void zCWorld::PrintGlobalVobTree(zCTree<zCVob>* node, int indent)
{
	if ( !node ) {
		zINFO("D: ****** ZoneList ********");

		for (auto zone : zoneGlobalList)
			zINFO("D: " + zone->GetVobInfo());

		zINFO("D: ****** VobTree ********");
	}

	auto vob = node->GetData();
	if ( vob ) {
		auto info = vob->GetVobInfo();
		zINFO("D: " + Spaces(2 * indent) + "- " + info);
	}

	for (auto i = node->firstChild; i; i = i->next )
		PrintGlobalVobTree(i, indent + 1);
}

void zCWorld::ShowZonesDebugInfo()
{
	posy = 0;
	if (showZonesDebugInfo )
		zINFO(5,"P: "); // 665

		auto time = GetActiveSkyControler()->GetTime();

		time = time * 24.0 + 12.0;
		if ( time > 24.0 )
			time -= 24.0;

		zCCamera::activeCam->Activate();

		zSTRING msg = "** Active Zones **     (time: " + time + ")";
		screen->Print(0, 1000, msg);

		unsigned i = 0;
		auto posy = 1250;
		for (auto zone : zoneActiveList) {
			auto msg = zone->GetClassDef()->GetClassName() + ", ";
			if ( zone->GetObjectName()->Length() > 0 )
				msg += '"' + zone->GetObjectName() + '"';

			msg += zone->GetDebugDescString();

			screen->Print(0, posy, msg);
			posy += 250;

			zCOLOR color{-1,0,0,-1};
			zone->GetBBox3D->Draw(color);
		}
	}
}

void zCWorld::ShowTextureStats()
{
	if (!showTextureStats )
		return;

	zTRnd_Stats rndStats;
	zrenderer->GetStatistics();

	auto totalMem = zrenderer->GetTotalTextureMem() / 1000;
	auto num = rndStats.numUsed;
	auto memUsed = rndStats.memUsed / 1000;
	auto msg = "mem: " + memUsed + "k num: " + num + " (used), cardMem: " + totalMem + "k";
	screen->Print(0, 500, msg);

	auto numFetched = rndStats.numFetched;
	auto memFetched = rndStats.memFetched / 1000;
	msg = "mem: " + memFetched + "k num: " + numFetched + " (fetched)";
	screen->Print(0, 700, msg);

	zrenderer->ResetStatistics();
}

void zCWorld::PrintStatus()
{
	zINFO("D: *** World-Status ************************************");
	zINFO("D: Engine, Date: " + zSTRING() + ", " + build_date);
	zINFO("D: Objects in Main-Mem, shared between worlds:");
	zINFO("D: - Num Meshes      : "_s + zCMesh::s_numMeshes);
	zINFO("D: - Num Materials   : "_s + zCMaterial::classDef.objectList.GetNum());
	zINFO("D: - Num Textures    : "_s + zCTexture::classDef.objectList.GetNum())
	zINFO("D: - Num LightPresets: "_s + zCVobLight::lightPresetList.GetNum());
	zINFO("D: - Num ParticleFX  : " + zCParticleFX::s_emitterPresetList.GetNum());
	zINFO("D: World:");
	zINFO("D: - Num Vobs        : "_s + globalVobTree.CountNodes());
	zINFO("D: - Num activeVobs  : "_s + activeVobList.GetNum());
	zINFO("D: - vobFarClipZ     : "_s + vobFarClipZScalability * vobFarClipZ);
	zINFO("D: - perFrameCallback: "_s + perFrameCallbackList.GetNum());

	if ( compiled ) {
		zINFO("D: BSP: ");
		if ( bspTree.bspTreeMode ) {
			zINFO("D: - Mode            : outdoor");
		} else {
			zINFO("D: - Mode            : indoor");
		}

		zINFO("D: - Num meshPolys   : "_s + bspTree.mesh->numPoly);
		zINFO("D: - Num treePolys   : "_s + bspTree.numPolys);
		zINFO("D: - Num Leafs       : "_s + bspTree.numLeafs);
		zINFO("D: - Num Nodes       : "_s + bspTree.numNodes);
	} else {
		zINFO("D: uncompiled world");
	}

	zINFO("D: *****************************************************");
}

// --------- Save/Load
int zCWorld::MergeVobSubtree(zSTRING const& fileName, zCVob* paramVob, zTWorldLoadMergeMode mergeMode)
{
	zCWorld::s_worldMergeMode = mergeMode;
	if ( mergeMode ) {
		if ( !paramVob )
			return 0;
		s_firstVobMergeVobSubtree = paramVob;
		s_unarcTraverseVobsFirstParent = 0;
	} else {
		zCTree<zCVob>* tree = nullptr;
		if (paramVob)
			tree = paramVob->globalVobTreeNode;
		if (!tree)
			tree = &globalVobTree;

		s_firstVobMergeVobSubtree = 0;
		s_unarcTraverseVobsFirstParent = tree;
	}

	zCWorld::s_worldLoadMode = zWLD_LOAD_MERGE;
	zoptions->ChangeDir(DIR_WORLD);

	auto arc = zCArchiverFactory::CreateArchiverRead(fileName, 0);
	if ( arc ) {
		arc->ReadObject(this);
		arc->Close();
		Release(arc);
	}

	s_unarcTraverseVobsFirstParent = 0;
	return std::exchange(s_firstVobMergeVobSubtree, 0);
}

int zCWorld::SaveVobSubtree(zSTRING *fileName, zCVob *vob, int writeBinary, int saveLevelMesh)
{
	if ( vob && vob->homeWorld == this ) {
		zCWorld::s_worldSaveMode = 2;

		s_saveLevelMesh = saveLevelMesh;
		s_firstVobSaveWorld = vob->globalVobTreeNode;
		s_saveWayNet = 0;

		zoptions->ChangeDir(DIR_WORLD);

		auto arc = zarcFactory.CreateArchiverWrite(fileName, writeBinary ? 3 : 1, 0, 0);
		if ( arc ) {
			arc->WriteObject(this);
			arc->Close();
			arc->Release();
		}

		s_firstVobSaveWorld = 0;

		return arc != 0;
	}

	return 0;
}

int zCWorld::LoadBspTree(zFILE& file, int isDyn)
{
	zCFileBIN fileBin;
	fileBin.BinSetFile(file);

	auto load = bspTree.LoadBIN(fileBin, isDyn);

	if ( !isDyn ) {
		if ( bspTree.mesh ) {
			if ( bspTree.mesh->hasLightmaps ) {
				worldRenderMode = 1;
				bspTree.worldRenderMode = 1;
			} else {
				worldRenderMode = 0;
				bspTree.worldRenderMode = 0;
			}
		}
		compiled = load;
	}
	return load;
}

void zCWorld::SaveBspTree(zCFile& file)
{
	zSTRING fileName = file.GetFilename();

	zINFO(5, "D: WORLD: Saving BSP: " + fileName);

	zCFileBIN fileBin;
	fileBin.BinSetFile(arcFile);

	bspTree.SaveBIN(fileBin);
}

void zCWorld::SaveBspTreeMesh3DS(zSTRING *filename)
{
	zoptions->ChangeDir(DIR_MESHES);
	zCFile3DS::Save3DS(filename, bspTree.mesh);
}

int zCWorld::SaveWorld(zSTRING const& fileName, zTWorldSaveMode mode, int writeBinary, int saveLevelMesh)
{
	zCPolygon::S_ResetMorphedVerts();
	zINFO(5,"D: WORLD: Saving World: \"" + fileName + "\""); // 2937, zWorld.cpp

	zCVob* camVob = nullptr;
	if ( zCCamera::activeCam ) {
		camVob = zCCamera::activeCam->connectedVob;
		if ( camVob ) {
			++camVob->refCtr;
			RemoveVob(camVob);
		}
	}

	s_saveLevelMesh = saveLevelMesh;
	zCWorld::s_worldSaveMode = mode;
	s_firstVobSaveWorld = &globalVobTree;
	s_saveWayNet = mode != 3;

	auto arcMode = writeBinary ? 3 : 1;
	if ( writeBinary && mode == 0 ) {
		if (zoptions->ReadBool("INTERNAL", "zFastSaveGames", 1))
			arcMode = 0;
	}

	zoptions->ChangeDir(DIR_WORLD);

	auto arc = zarcFactory.CreateArchiverWrite(fileName, arcMode, mode == 0, 0);
	bool ret = arc != nullptr;

	if ( arc ) {
		arc->WriteObject(this);
		arc->Close();
		arc->Release();
	}
	if ( camVob ) {
		AddVobAsChild(camVob, &globalVobTree);
		camVob->Release();
		s_firstVobSaveWorld = 0;
	}

	return ret;
}

int zCWorld::LoadWorld(zSTRING const& filename, zTWorldLoadMode mode)
{
	zINFO(1, "D: WORLD: Loading WorldFile.. " + fileName); // 2604

	zCWorld::s_worldLoadMode = mode;

	DisposeVobs(0);
	zCVob::ResetIDCtr();

	if (in(mode,0,2,3,4))
		DisposeStaticWorld();

	zvertexBufferMan.StartChangeWorld();

	zoptions->ChangeDir(DIR_WORLD);

	auto arc = zCArchiverFactory::CreateArchiverRead(fileName, 0);
	bool ret = arc != nullptr;

	if ( arc ) {
		arc->ReadObject(this);
		arc->Close();
		arc->Release();
	}

	zvertexBufferMan.EndChangeWorld();

	zCWorld::s_bFadeOutFarVerts = zoptions->ReadBool("ENGINE", "zFarClipAlphaFade", 1);

	if (compiled && bspTree.bspTreeMode == 0)
		zCWorld::s_bFadeOutFarVerts = 0;

	zINFO(1,""); // 2662, zWorld.cpp

	if ( ret ) {
		zFILE_FILE file(fileName);

		levelName = file.GetFilename();

		bspTree.renderedFirstTime = 1;
	}

	return ret;
};

// ---------- Dispose
void zCWorld::ResetCutscenePlayer();
{
	if ( csPlayer )
		csPlayer->ResetCutscenePlayer();

	Release(csPlayer);

	if (ownerSession) {
		auto csman = ownerSession->GetCutsceneManager();
		csPlayer = csman->CreateCutscenePlayer(this);
	}
}

void zCWorld::RemoveAllZones()
{
	zoneBoxSorter.Clear();
	for (auto zone : zoneGlobalList)
		zone->ThisVobRemovedFromWorld(this);

	zoneActiveList.DeleteList();
	zoneLastClassList.DeleteList();
	zoneGlobalList.DeleteList();
	zoneBoxSorter.Clear();
}

int zCWorld::DisposeVobs(zCTree<zCVob> *vobNode)
{
	zCRenderLightContainer::S_ReleaseVobLightRefs();

	if ( !vobNode ) {
		Release(wayNet);
		wayNet = new zCWayNet(this);
		ResetCutscenePlayer();
		RemoveAllZones();

		vobNode = &this->globalVobTree;
		renderingFirstTime = 1;
	}

	for (auto i = vobNode->firstChild; i;) {
		if (DisposeVobs(i) == 1)
			i = vobNode->firstChild;
		else
			i = i->next;
	}

	if ( vobNode->data ) {
		RemoveVob(vobNode->data);
		return 1;
	}

	return 0;
}

int zCWorld::DisposeVobsDbg(zCTree<zCVob> *vobNode)
{
	zCRenderLightContainer::S_ReleaseVobLightRefs();

	if ( !vobNode ) {
		Release(wayNet);
		wayNet = new zCWayNet(this);
		ResetCutscenePlayer();
		RemoveAllZones();

		vobNode = &this->globalVobTree;
		renderingFirstTime = 1;
	}

	for (auto i = vobNode->firstChild; i;) {
		if (DisposeVobsDbg(i) == 1)
			i = vobNode->firstChild;
		else
			i = i->next;
	}

	if ( vobNode->data ) {
		RemoveVob(vobNode->data);
		return 1;
	}

	return 0;
}

void zCWorld::DisposeStaticWorld()
{
	if ( bspTree.numPolys != 0 ) {
		auto skyctrl = zDYNAMIC_CAST<zCSkyControler_Outdoor>(zCSkyControler::s_activeSkyControler);
		if (skyctrl)
			skyctrl->levelChanged = true;
	}

	zCPolygon::S_DeleteMorphedVerts();
	bspTree.DisposeTree();

	compiled = 0;
	compiledEditorMode = 0;
	renderingFirstTime = 1;

	UpdateVobTreeBspDependencies(&globalVobTree);
}

void zCWorld::DisposeWorld()
{
	zINFO(8, "D: WORLD: Disposing zCWorld.."); // 1674, _dieter\\zWorld.cpp

	DisposeVobs(0);
	DisposeStaticWorld();

	zINFO(8, "D: WORLD: ..zCWorld disposed."); // 1682,
}


// ---------- Zones
int zCWorld::ActiveZoneListCompare(const void* a1, const void *a2)
{
	auto zone1 = *static_cast<zCZone* const*>(a1);
	auto zone2 = *static_cast<zCZone* const*>(a2);

	if (zone1->GetZoneMotherClass() >= zone2->GetZoneMotherClass())
		return zone2->GetZoneMotherClass() < zone1->GetZoneMotherClass();
	return -1;
}

zCZone* SearchZoneDefaultByClass(zCClassDef* cd)
{
	for (auto zone : zoneGlobalList) {
		if (zone->GetClassDef() == cd)
			return zone;
		if (zone->GetClassDef() == zone->GetDefaultZoneClass())
			continue;
	}

	return nullptr;
}

void zCWorld::SearchZoneListByClass(zCClassDef *classDef, zCArray<zCZone*>& results)
{
	for (auto zone : zoneGlobalList) {
		if (zone->GetClassDef() == classDef)
			results.Insert(zone);
	}
}

void zCWorld::RemoveZone(zCZone *zone)
{
	if ( zone ) {
		auto defclass = zone->GetDefaultZoneClass();
		auto theclass = zone->GetClassDef();
		if ( theclass == defclass); {
			zoneGlobalList.RemoveOrder(zone);
		} else {
			zoneGlobalList.Remove(zone);
			zoneBoxSorter.Remove(zone);
		}
	
		zoneLastClassList.Remove(zone);
		zoneActiveList.Remove(zone);
	}
}

void zCWorld::AddZone(zCZone* zone)
{
	if ( zone ) {
		if (!zoneGlobalList.IsInList(zone)) {
			auto defclass = zone->GetDefaultZoneClass();
			auto theclass = zone->GetClassDef();

			if (defclass == theclass) {
				zoneGlobalList.InsertFront(zone);
			} else {
				zoneGlobalList.InsertEnd(zone);
				zoneBoxSorter.Insert(zone);
			}
		}
	}
}

void zCWorld::ProcessZones()
{
	GetActiveSkyControler()->SetOverrideColorFlag(0);
	GetActiveSkyControler()->SetOverrideColorFlag(1.0);

	auto& camBBox = zCCamera::activeCam->connectedVob->bbox3D;

	zVEC3 camMins = camBBox.mins;
	zVEC3 camMaxs = camBBox.maxs;
	zVEC3 camPos  = zCCamera::activeCam->connectedVob->GetPositionWorld();

	camMins = camPos;
	camMaxs = camPos;

	if (!zoneBoxSorter.sorted) {
		zoneBoxSorter->GetActiveList(camMins, zoneActiveHandle);
		zoneActiveList.DeleteList();
		for (auto zone : zoneActiveHandle.activeList)
			zoneActiveList.InsertEnd(zone);
	} else {
		zoneBoxSorter->UpdateActiveList(camMins, zoneActiveHandle);
		zoneActiveList.DeleteList();
		for (auto zone : zoneActiveHandle.activeList)
			zoneActiveList.InsertEnd(zone);
	}

	zoneActiveList.Compare = zCWorld::ActiveZoneListCompare;
	zoneActiveList.QSort();

	zCArray<zCZone*> zoneDefaultList;
	for (auto zone : zoneGlobalList) {
		if (zone->GetDefaultZoneClass() != zone->GetMotherZoneClass())
			break;
		zoneDefaultList.InsertEnd(zone);
	}

	int zoneNum[2] = {};
	zCArraySort<zCZone*> zoneList[2]; // zoneList, zoneDeactivateList

	zCArraySort<zCZone*>* zoneListp[2] = {&zoneActiveList, &zoneLastClassList};

	while ( 1 ) {
		for (unsigned i = 0; i < 2; ++i) {
			zoneList[i].Clear();
			zoneClass[i] = -1;
			if ( zoneNum[i] < zoneListp[i]->GetNum() )
				zoneClass[i] = zoneListp[i][zoneNum[i]]->GetZoneMotherClass();
		}

		if ( zoneClass[0] != zoneClass[1] ) {
			if ( zoneClass[0] >= zoneClass[1] ) {
				auto num = zoneListp[1]->GetNum();
				for (; zoneNum[1] < num; ++zoneNum[1]) {
					auto zc = zoneListp[1][zoneNum[1]]->GetZoneMotherClass();
					if ( zc != zoneClass[1] )
						break;

					zoneList[1].InsertEnd(zoneListp[1][zoneNum[1]]);
				}
			} else {
				auto num = zoneListp[0]->GetNum();
				for (; zoneNum[0] < num; ++zoneNum[0]) {
					auto zc = zoneListp[0][zoneNum[0]]->GetZoneMotherClass();
					if ( zc != zoneClass[0] )
						break;

					zoneList[0].InsertEnd(zoneListp[0][zoneNum[0]]);
				}
			}
		} else {
			if ( zoneClass[0] == -1 )
				break;

			for (unsigned i = 0; i < 2; ++i) {
				auto num = zoneListp[i]->GetNum();
				for (; zoneNum[i] < num; ++zoneNum[i]) {
					auto zc = zoneListp[i][zoneNum[i]]->GetZoneMotherClass();
					if (zc != zoneClass[i])
						break;
					zoneList[i].InsertEnd(zoneListp[i][zoneNum[i]]);
				}
				zoneList[i].QSort();
			}

			for (unsigned i = 0; i < zoneList[1].GetNum(); ++i) {
				if (zoneList[0].Search(zoneList[1][i]) != -1) {
					zoneList[1].RemoveIndex[i];
					--i;
				}
			}
		}

		zCZone* zone;
		if ( zoneList[0].GetNum() > 0 ) {
			zone = zoneList[0];
		} else if ( zoneList[1].GetNum() > 0 ) {
				zone = zoneList[1][0];
		}

		zone->ProcessZoneList(zoneList[0], zoneList[1], this);

		auto zc = zone->GetDefaultZoneClass();
		if (!zc) continue;
		for (auto i = 0u; i < zoneDefaultList.GetNum(); ++i) {
			if (zoneDefaultList[i]->GetDefaultZoneClass() == zc) {
				zoneDefaultList.RemoveIndex(i);
				break;
			}
		}
	}

	zoneList[0].Clear();
	zoneList[1].Clear();

	for (auto zone : zoneDefaultList)
		zone->ProcessZoneList(zoneList[0], zoneList[1], this);

	zoneLastClassList = zoneActiveList;
}


// ---------- Render
void zCWorld::RenderWaynet(zCCamera *cam)
{
	if ( showWaynet ) {
		if ( wayNet )
			wayNet->Draw(cam);
	}
}

void zCWorld::Render(zCCamera *cam)
{
	if (!cam)
		return;
	if (!cam->connectedVob)
		return;

	if ( cam->connectedVob->homeWorld != this)
		return;

	int hasLevelName = 0;
	if (!levelName.IsEmpty()) {
		hasLevelName = 1;
	} else if (auto oworld = zDYNAMIC_CAST<oCWorld>(this)) {
		hasLevelName = !oworld->worldFilename.IsEmpty();
	}

	zCCacheBase::S_PurgeCaches();

	if ( hasLevelName && zCWorld::s_bWaveAnisEnabled )
		zCFFT::S_SetFrameCtr(masterFrameCtr);

	cam->PreRenderProcessing();
	cam->UpdateViewport();
	cam->Activate();

	GetActiveSkyControler();

	if ( renderingFirstTime ) {
		zresMan->cacheInImmedMsec = 1500.0;
		renderingFirstTime = 0;
	}

	if ( hasLevelName ) {
		zsound->SetListener(zCCamera::activeCam->connectedVob);
		ProcessZones();
		zsound->DoSoundUpdate();
	}

	zCPolygon::PrepareRendering();

	bspTree.vobFarClipZ = vobFarClipZScalability * vobFarClipZ;
	bspTree.Render(/*cam*/);
	cam->PostRenderProcessing();
	zrenderer->FlushPolys();

	zCPolygon::S_ResetMorphedVerts();

	if ( hasLevelName ) {
		zlineCache.Flush(cam->targetView);
		RenderWaynet(cam);
		ShowZonesDebugInfo();
		ShowTextureStats();
	}

	cam->Activate();

	MoveVobs();

	if ( hasLevelName ) {
		zCEventManager::DoFrameActivity();
		zresMan->DoFrameActivity();

		if ( csPlayer )
			csPlayer->Process();

		for (auto callback : perFrameCallbackList)
			callback(this, cam);
	}

	++masterFrameCtr;
	zCVertex::ResetVertexTransforms();
	// it's private, so it should be in some func...
	++zCTexAniCtrl::masterFrameCtr;
}

int zCWorld::PickScene(zCCamera *cam, int xscr, int yscr, float dist)
{
	cam->Activate();

	auto invPos = cam->camMatrixInv.GetTranslation();

	float xpos = (xscr - cam->xcenter) * cam->viewDistanceXInv * 600000.0;
	float ypos = (cam->ycenter - yscr) * cam->viewDistanceYInv * 600000.0;
	float zpos = 600000.0;

	zVEC3 pos{xpos, ypos, zpos};
	zVEC3 ray = pos * cam->camMatrixInv - invPos;


	if ( dist > 0.0 ) {
		float len = 1.0 / ray.Length();
		ray *= len * dist;
	}

	memset(&traceRayReport, 0, sizeof(traceRayReport));

	traceRayIgnoreVobFlag = 1;

	unsigned flags = 0x220;
	if ( zCVob::s_showHelperVisuals )
		flags = 0x2220;

	auto res = TraceRayNearestHit(invPos, ray, 0, flags);

	traceRayIgnoreVobFlag = 0;

	if ( res ) {
		auto poly = traceRayReport.foundPoly;
		if ( poly ) {
			if ( !traceRayReport.foundVob ) {
				unsigned i = 0;
				float mindist = 3.4028235e38;
				while (i ++< poly->numVerts) {
					auto vert = poly->vertex[i];
					auto vpos = poly->vertex[i]->position;
					auto hpos = traceRayReport.foundIntersection;

					float dist = (vpos - hpos).Length2();
					if (dist < mindist) {
						mindist = dist;
						traceRayReport.foundVertex = vert;
					}
				}
			}
		}
	}

	return res;
}

// ----------- Ray tracing
int zCWorld::TraceRayNearestHit(zVEC3 const& start, zVEC3 const& ray, zCArray<zCVob *> const* ignoreList, int traceFlags)
{
	zVEC3 end = start + ray;
	if ( showTraceRayLines )
		zlineCache.Line3D(start, end, GFX_LBLUE, 0);

	traceRayVobList.DeleteList();

	traceRayReport = zTTraceRayReport{};

	zCArray<zCVob*>* traceRayList = nullptr;
	if (!traceFlags.ignoreVobs)
		traceRayList = &traceRayVobList;

	traceRayReport.foundHit = bspTree.TraceRay(
	                start, end, traceFlags,
	                &traceRayReport.foundIntersection,
	                &traceRayReport.foundPoly,
	                &traceRayList);

	if (traceFlags.ignoreVobs)
		return 0;

	if ( traceFlags.ignoreStatPoly )
		traceRayReport.foundHit = 0;

	float nearest = std::numeric_limits<float>::max();

	if (traceRayVobList.foundHit) {
		zVEC3 vec = traceRayReport.foundIntersection - start;
		nearest = vec.Length2();
	}

	zTTraceRayReport result;
	for (auto vob : traceRayVobList) {
		bool hasVisual =  vob->visual && vob->flags1.showVisual;
		bool ignVisual = !vob->visual && traceFlags.ignoreVisual;

		if (!(hasVisual || ignVisual))
			continue;
		if (!traceRayIgnoreVobFlag && vob->flags1.ignoredByTraceRay)
			continue;
		if (ignoreList && ignoreList->IsInList(vob))
			continue;
		if (traceFlags.ignoreCharacter && vob->GetCharacterClass())
			continue;
		if (traceFlags.ignoreProjectiles && vob->GetIsProjectile())
			continue;
		if (traceFlags.requireDynamic && !vob->GetCollDetDyn())
			continue;

		zTTraceRayReport report;
		if ( vob->TraceRay(start, ray, traceFlags, &report) ) {
			float dist = (report.foundIntersection - start).Length2();

			if (dist < nearest)
				result = report;
		}
	}

	if ( result.foundVob ) {
		traceRayReport = result;
		traceRayReport.foundHit  = 1;
		traceRayReport.foundPoly = 0;

	}

	if (traceFlags.polyNormal && traceRayReport.foundHit) {
		if ( traceRayReport.foundPoly && !traceRayReport.foundVob ) {
			auto n = traceRayReport.foundPoly->polyPlane.normal;
			traceRayReport.foundPolyNormal = n;
		}
	}

	return traceRayReport.foundHit;
}

int zCWorld::TraceRayFirstHit(zVEC3 const& start, zVEC3 const& ray, zCArray<zCVob *> const* ignoreList, int traceFlags)
{
	auto end = start + ray;
	if ( showTraceRayLines )
		zlineCache.Line3D(start, end, GFX_LBLUE, 0);

	traceRayVobList.DeleteList();

	traceRayReport = zTTraceRayReport{};

	zCArray<zCVob*>* traceRayList = nullptr;
	if (!traceFlags.ignoreVobs)
		traceRayList = &traceRayVobList;

	traceRayReport.foundHit = bspTree.TraceRay(
	                start, end, traceFlags | 0x1000,
	                &traceRayReport.foundIntersection,
	                &traceRayReport.foundPoly,
	                &traceRayList);

	if ( traceFlags.ignoreStatPoly )
		traceRayReport.foundHit = 0;

	if ( traceRayReport.foundHit ) {
		if ( traceFlags < 0 ) {
			auto foundPoly = traceRayReport.foundPoly;
			if ( foundPoly )
				traceRayReport.foundPolyNormal = foundPoly.polyPlane.normal;
		}
		return 1;
	}

	if (traceFlags.ignoreVobs)
		return 0;

	for (auto vob : traceRayVobList) {
		bool hasVisual =  vob->visual && vob->flags1.showVisual;
		bool ignVisual = !vob->visual && traceFlags.ignoreVisual;

		if (!(hasVisual || ignVisual))
			continue;
		if (!traceRayIgnoreVobFlag && vob->flags1.ignoredByTraceRay)
			continue;
		if (ignoreList && ignoreList->IsInList(vob))
			continue;
		if (traceFlags.ignoreCharacter && vob->GetCharacterClass())
			continue;
		if (traceFlags.ignoreProjectiles && vob->GetIsProjectile())
			continue;
		if (traceFlags.requireDynamic && !vob->GetCollDetDyn())
			continue;

		return vob->TraceRay(start, ray, traceFlags, &traceRayReport);
	}

	return 0;
}

int zCWorld::TraceRayNearestHit(zVEC3 const& start, zVEC3 const& ray, zCVob* ignoreVob, int traceFlags)
{
	if ( !ignoreVob )
		return TraceRayNearestHit(start, ray, (zCArray<zCVob>*)nullptr, traceFlags);

	traceRayTempIgnoreVobList.Insert(ignoreVob);

	auto result = TraceRayNearestHit(start, ray, &traceRayTempIgnoreVobList, traceFlags);
	traceRayTempIgnoreVobList.DeleteList();
	return result;
}

int zCWorld::TraceRayFirstHit(zVEC3 const& start, zVEC3 const& ray, zCVob* ignoreVob, int traceFlags)
{
	if ( !ignoreVob )
		return TraceRayFirstHit(start, ray, (zCArray<zCVob>*)nullptr, traceFlags);

	traceRayTempIgnoreVobList.Insert(ignoreVob);

	auto result = TraceRayFirstHit(start, ray, &traceRayTempIgnoreVobList, traceFlags);
	traceRayTempIgnoreVobList.DeleteList();
	return result;
}

void ShowTraceRayLine(zVEC3 const& start, zVEC3 const& ray)
{
	// I made this function to reduce duplicate code
	if ( showTraceRayLines ) {
		int effort = zCRayCache::LastTraceRayEffort() * 512.0;
		if ( effort > 255 )
			effort = 255;

		zCOLOR color = (GFX_RED * effort) / 256 + GFX_DGREEN;

		zVEC3 end = start + ray;
		zlineCache.Line3D(start, end, color, 0);
	}
}

int zCWorld::TraceRayNearestHitCache(zVEC3 const& start, zVEC3 const& ray, zCArray<zCVob *> const* ignoreList, unsigned traceFlags, zCRayCache* cache)
{
	zCBspTree::SetRayCache(cache);

	auto result = TraceRayNearestHit(start, ray, ignoreList, traceFlags);

	ShowTraceRayLine(start, ray);

	return result;
}

int zCWorld::TraceRayFirstHitCache(zVEC3 const& start, zVEC3 const& ray, zCArray<zCVob *> const* ignoreList, unsigned traceFlags, zCRayCache* cache)
{
	zCBspTree::SetRayCache(cache);

	auto result = TraceRayFirstHit(start, ray, ignoreList, traceFlags);

	ShowTraceRayLine(start, ray);

	return result;
}

int zCWorld::TraceRayNearestHitCache(zVEC3 const& start, zVEC3 const& ray, zCVob* ignoreVob, unsigned traceFlags, zCRayCache* cache)
{
	zCBspTree::SetRayCache(cache);

	auto result = TraceRayNearestHit(start, ray, ignoreVob, traceFlags);

	ShowTraceRayLine(start, ray);

	return result;
}

int zCWorld::TraceRayFirstHitCache(zVEC3 const& start, zVEC3 const& ray, zCVob* ignoreVob, unsigned traceFlags, zCRayCache* cache)
{
	zCBspTree::SetRayCache(cache);

	auto result = TraceRayFirstHit(start, ray, ignoreVob, traceFlags);

	ShowTraceRayLine(start, ray);

	return result;
}

// ----------- Lighting
int zCWorld::LightingTestRay(zVEC3 const& start, zVEC3 const& end, zVEC3& inters, zCPolygon*& hitPoly)
{
	float junk;

	if ( traceRayLastPoly ) {
		zVEC3 ray = end = start;
		if (traceRayLastPoly->CheckRayPolyIntersection(start, ray, inters, &junk)) {
			*hitPoly = traceRayLastPoly;
			return 1;
		}
		traceRayLastPoly = 0;
	}

	if ( bspTree.TraceRay(start, end, 0x120, inters, hitPoly, 0) ) {
		traceRayLastPoly = *hitPoly;
		return 1;
	}

	return 0;
}

zVEC3 zCWorld::GetPhongNormal(zCPolygon* poly, zVEC3 const& pos)
{
	unsigned numVerts = poly->numVerts;

	unsigned A = 0;
	unsigned B = 1;
	unsigned C = 2;
	if ( numVerts > 3u )
	{
		numVerts = numVerts;
		for (unsigned i = 0; i < numVerts; ++i) {
			A = i;
			B = i + 1;
			if ( A >= numVerts )
				A = 0;
			B = A + 1;
			if ( B >= numVerts )
				B = 0;

			auto vA = poly->vertex[A];
			auto vB = poly->vertex[B];
			auto vC = poly->vertex[C];

			auto diff1 = vB->position - vA->position;
			auto diff2 = vC->position - vB->position;

			auto d1len = diff1.Length();
			if (d1len == 0.0)
				continue;
			diff1 /= d1len;

			auto d2len = diff2.Length();
			if (d2len != 0.0)
				continue;
			diff2 /= d2len;

			if (!diff1.IsEqualEps(diff2)) {
				if (!diff1.IsEqualEps(-diff2))
					break;
			}
		}
	}

	auto& vert = poly->vertex;

	auto diff1 = vert[C]->position - vert[B]->position;
	auto diff2 = vert[A]->position - vert[B]->position;

	auto& feature = poly->feature;

	auto diff3 = feature[C]->vertNormal - feature[B]->vertNormal;
	auto diff4 = feature[A]->vertNormal - feature[B]->vertNormal;

	auto diff5 = pos - vert[B]->position;

	float dot1 = diff1 * diff2;

	float len1 = diff1.Length2();
	float len2 = diff2.Length2();

	float dot2 = diff1 * diff5;
	float dot3 = diff2 * diff5;

	float hz1 = (dot2 * len2 - dot3 * dot1) / (len1 * len2 - dot1 * dot1);
	float hz2 = (dot3 - hz1 * dot1) / len2;

	diff1 = diff3 * hz1;
	diff2 = diff4 * hz2;

	diff1 = diff1 + features[B]->vertNormal;

	return (diff1 + diff2).Normalized();
}

void TraverseMakeVobLightingDirty(zCTree<zCVob>* tree)
{
	auto vob = tree->GetData();
	if (vob) {
		vob->flags1.lightColorStatDirty = true;
		vob->flags1.lightColorDynDirty = true;
	}

	for (auto i = tree->firstChild; i; i = i->next)
		TraverseMakeVobLightingDirty(i);
}

void zCWorld::MakeVobLightingDirty()
{
	zINFO(7,"D: WLD: Make Vob-Lighting dirty"); // 1922,

	TraverseMakeVobLightingDirty(&globalVobTree);

	zINFO("D: WLD: ... finished"); // 1924, _dieter\\zWorldLight.cpp
}

int zCWorld::GetSurfaceLightmapBBox2D(zCArray<zCPolygon>& surface, zTBBox2D& lmBox, int* realDim)
{
	int ret = 1;

	zVEC3 origin, up, right;
	auto& polyPlane = surface[0]->polyPlane;
	polyPlane.GetOriginUpRight(origin, up, right);

	lmBox.Init();

	right *= 1.0 / right.Length2();
	up    *= 1.0 / up.Length2();

	for (auto poly : surface) {
		for (auto vert : poly->vertex) {
			auto pos = vert->position;
			float d = pos * polyPlane.normal - polyPlane.distance;
			zVEC3 nrm = d * polyPlane.normal;
			zVEC3 vec = pos - nrm - origin;

			float len1 = vec * right;
			float len2 = vec * up;

			if (lmBox.mins[0] >= len1)
				lmBox.mins[0] = len1;
			if (lmBox.maxs[0] <= len1)
				lmBox.maxs[0] = len1;
			if (lmBox.mins[1] >= len2)
				lmBox.mins[1] = len2;
			if (lmBox.maxs[1] <= len2)
				lmBox.maxs[1] = len2;
		}
	}

	for (unsigned i = 0; i < 2; ++i) {
		lmBox.mins[i] = floor(lmBox.mins[i] / zLIGHTMAP_GRID) * zLIGHTMAP_GRID;
		lmBox.maxs[i] =  ceil(lmBox.maxs[i] / zLIGHTMAP_GRID) * zLIGHTMAP_GRID;
	}

	// this part was HOLY CRAP, compiler added and subtracted
	// pointers to local variables to pointer to lmBox,
	// to get offsets to members of lmBox
	int dim[2];
	for (unsigned i = 0; i < 2; ++i) {
		dim[i] = (lmBox.maxs[i] - lmBox.mins[i]) / zLIGHTMAP_GRID;
		if (dim[i] <= 0)
			dim = 1;

		realDim[i] = dim[i];

		int size = 1 << zGetHighestBit(dim[i]);
		if (dim[i] > val)
			size *= 2;

		lmBox.maxs[i] = lmBox.mins[i] + size * zLIGHTMAP_GRID;
	}

	if (dim[0] > 256 || dim[1] > 256)
		ret = 0;

	zClamp(dim[0], 0, 256);
	zClamp(dim[1], 0, 256);
	zClamp(realDim[0], 0, 256);
	zClamp(realDim[1], 0, 256);

	if ( dim[0] <= dim[1] ) {
		auto ratio = dim[1] / float(dim[0]);
		if ( ratio > 8.0 )
			dim[0] = (ratio * 0.125 * dim[0]);
	} else {
		auto ratio = dim[0] / float(dim[1]);
		if ( ratio > 8.0 )
			dim[0] = (ratio * 0.125 * dim[1]);
	}

	for (unsigned i = 0; i < 2; --i)
		lmBox.maxs[i] = dim[i] * zLIGHTMAP_GRID + lmBox.mins[i];

	return ret;
}

void zCWorld::GenerateStaticWorldLighting(zTStaticWorldLightMode const& lightMode, zTBBox3D *updateBBox3D)
{
	if ( bspTree.mesh ) {
		bspTree.mesh->ResetStaticLight();
		zINFO(5,"C: WORLD: smoothing vertex normals for lightmap generation"); // 1874

		bspTree.mesh->CalcVertexNormals(1, &bspTree);
	}

	g_bIsInCompileLightMode = 1;
	if ( lightMode )
		GenerateLightmaps(lightMode, updateBBox3D);
	GenerateStaticVertexLighting();
	bspTree.CalcStaticLeafLightData();

	MakeVobLightingDirty();
	g_bIsInCompileLightMode = 0;
}

void zCWorld::LightPatchMap(zCPatchMap* patchMap)
{
	auto poly = patchMap->surface[0];

	for (auto light : lightVobList) {
		zVEC3 lpos = light->GetPositionWorld();

		float outside = 0.0;
		for (unsigned i = 0; i < 3; ++i) {
			if (lpos[i] < bbox.mins[i]) {
				float f = (lpos[i] - bbox.mins[0]);
				outside += f * f;
			} else if (lpos[i] > bbox.maxs[i]) {
				float f = (lpos[i] - bbox.maxs[0]);
				outside += f * f;
			}
		}

		float range = light->lightData.range;
		float rangeSq = range * range;

		if ( outside > rangeSq )
			continue;

		auto color = light->lightData.lightColor;
		zVEC3 colorVec{color[2], color[1], color[0]};

		auto rangeInv = light->lightData.rangeInv;
		auto spotCone = cos(light->lightData.spotConeAngleDeg / 180.0 * Pi);

		for (auto patch : patchMap->patches) {
			auto diff = patch - lpos;

			if (diff.Length2() > rangeSq)
				continue;

			diff.Normalize();

			float dot = -(diff * patch->vec2);
			if ( dot < 0.0 )
				continue;

			// (BYTE1(light->lightData.flags) & 0xF0) == 16
			if (light->lightData.flags.lightType == 4) {
				auto atvec = light->trafoObjToWorld.GetAtVector();
				if (atvec * diff < spotCone)
					continue;
			}

			float rng = diff.Length() * rangeInv;
			float val = 1.0 - rng * rng;

			zClamp(val, 0.0, 1.0);

			float val2 = dot * val;

			zVEC3 ray = patch->vec1 - lpos;

			if (TraceRayNearestHit(lpos, ray, light, 0x10))
				continue;

			auto vob = traceRayReport.foundVob;
			if (!vob && !vob->flags1.staticVob)
				continue;

			auto normal = poly->polyPlane.normal * 11.0;
			auto end = normal + patch->vec0;

			zVEC3 inters;
			zCPolygon* dummy;
			if (bspTree.TraceRay(lpos, end, 0x120, inters, &dummy, 0))
				continue;

			patch->vec3 = colorVec * val2;
			patch->vec4 += patch->vec3;
			patchMap->lit = 1;
		}
	}
}

void zCWorld::LightWorldStaticCompiled()
{
	auto temp_light = new zCVobLight();

	if ( bspTree.bspTreeMode == 1 ) {
		temp_light->lightData.flags.lightType = 8;
		temp_light->SetRange(100.0, 1);
		AddVob(temp_light);

		lightData.lightData.SetRGB(-1,-1,-1);

		zVEC3 vec{0.4495, -0.866, -0.2181};

		vec.Normalize();

		temp_light->SetHeadingAtWorld(&vec);

		lightVobList.DeleteList();
		lightVobList.Insert(temp_light);
	}

	auto mesh = bspTree.mesh;
	mesh->ResetStaticLight();
	mesh->CalcVertexNormals(0, &bspTree);

	zMAT4 mat;
	Alg_Identity3D(mat);

	for (auto light : lightVobList) {
		auto color = light->lightData.lightColor;
		auto range = light->lightData.range;

		auto name  = light->GetObjectName();
		auto colorDesc = color.GetDescription();

		zINFO(9, "D: WORLD: Light, id:"_s + 1 + ", name: " + name + ", Range: " + range + ", Col: " + colorDesc); // 573

		auto inv = mat.InverseLinTrafo();
		auto lpos = light->GetPositionWorld() * inv;

		for (auto i : range(0, mesh->numVert))
			mesh->vertList[i]->MyIndex = 0;

		for (auto i : range(0, mesh->numPoly))
			mesh->polyList[i]->LightStatic(light, lpos, mat, this);
	}

	zINFO(5,"C: now smoothing sector border colors..."); // 578, zWorldLight.cpp

	for (auto i : range(0, mesh->numPoly))
		mesh->SharePoly(i)->SmoothSectorBorder(mesh, this);

	zINFO(5,"C: ...done"); // 582

	RemoveVob(temp_light);
	Release(temp_light);
}

void zCWorld::LightWorldStaticUncompiled(zCTree<zCVob>* node)
{
	auto vob = node->data;
	if ( vob && vob->flags1.showVisual && vob->type == VOB_TYPE_LEVELCOMPO ) {
		if (auto visual = zSTRICT_CAST<zCMesh>(vob->visual)) { // made-up cast
			visual->ResetStaticLight();
			zINFO("D: MESH: Calculating Vertex-Normals for Mesh (smoothing)...");
			visual->CalcVertexNormals(0, 0);
			for (auto light : lightVobList) {
				auto& trafo = vob->trafoObjToWorld;

				auto color = light->lightData.lightColor;
				auto range = light->lightData.range;

				auto name  = light->GetObjectName();
				auto colorDesc = color.GetDescription();

				zINFO("D: WORLD: Light, id:"_s + 1 + ", name: " + name + ", Range: " + range + ", Col: " + colorDesc);

				visual->LightMesh(light, trafo, this);
			}
		}
	}

	for (auto i = node->firstChild; i; i = i->next)
		LightWorldStaticUncompiled(i);
}

void TraverseCollectLights(VobTree *node)
{
	auto vob = node->GetData();

	if (vob && vob->type == VOB_TYPE_LIGHT)
		if (static_cast<zCVobLight*>(vob)->lightData.flags.isStatic)
			lightVobList.Insert(vob);
	for (auto i = node->firstChild; i; i = i->next )
		TraverseCollectLights(i);
}

void zCWorld::GenerateStaticVertexLighting()
{
	zINFO("D: WORLD: Calculating static Vertex Light for world ...");

	bspTree.mesh->CreateListsFromArrays();

	TraverseCollectLights(&globalVobTree);

	int numLights = lightVobList.GetNumInList();

	zINFO("D: WORLD: NumLights: "_s + numLights);

	if ( compiled )
		LightWorldStaticCompiled();
	else
		LightWorldStaticUncompiled(&globalVobTree);

	lightVobList.DeleteList();

	zINFO("D: WORLD: ... Finished.");
}

void zCWorld::MakeTransfers()
{
	zINFO("D: WORLD: LM: Making transfers...");

	unsigned numTransfers;

	zCArray<zCTransferConstr> constrs(1024);

	unsigned i = 0;
	for (auto patchMap1 : patchMapList) {
		for (auto patch1 : patchMap1->patches) {
			constrs.Clear();
			float sum = 0.0;
			for (auto patchMap2 : patchMapList) {
				auto poly = patchMap2->surface[0];
				auto& plane = poly->polyPlane;

				// distance to plane
				float distp = plane->normal * patch1->vec0 - plane.distance;
				if (distp <= 0.0 || distp > 800.0*800.0)
					continue;

				for (auto patch2 : patchMap2->patches) {
					if (patch2 == patch1)
						continue;
					auto diff = patch2->vec0 - patch1->vec0;
					float dist_sq = diff.Length2();
					if (dist_sq > 800.0*800.0)
						continue;
					float dist = sqrt(dist_sq);
					diff *= 1.0 / dist;

					float dot = -(patch2->vec2*diff)*(patch1->vec2*diff);

					zVEC3 inters;
					zCPolygon* hitPoly;
					if (LightingTestRay(patch2->vec0, patch2->vec0, inters, hitPoly)) {
						if ((inters - patch2->vec0).Length2() > 50.0)
							continue;
					}

					// in original was dist*dist
					float val = dot * patch2->grid2 / (dist_sq * Pi);
					if (val <= 0.001)
						continue;

					zCTransferConstr constr;
					constr.parent = patch2;
					constr.val = val;
					sum += val;
					constrs.Insert(constr);
					++numTransfers;
				}
			}

			patch1->transfers.AllocAbs(constrs.GetNum());
			for (auto constr : constrs) {
				zCTransfer trans;
				trans.parent = constr.parent;
				trans.val = constr.val * 65536.0 / sum;
				patch1->transfers.Insert(trans);
			}
		}

		auto perc = (i / patchMapList.GetNum() * 100.0);
		if ( perc % 10 == 0 && perc != lastPerc ) {
			zINFO("D: WORLD: LM: .. "_s + perc + " % ..");
			lastPerc = perc;
		}
	}

	zINFO("D: WORLD: LM: NumTransfers: "_s + numTransfers + ", mem: " + numTransfers * 10 / (1024.0*1024.0) + " megs");
}


void zCWorld::GenerateLightmapFromPatchMap(zCPatchMap* patchMap)
{
	if (!patchMap->lit)
		return;

	int dimx = patchMap->dim[0];
	int dimy = patchMap->dim[1];

	auto srcMap = new zVEC3[dimx*dimy];
	auto dstMap = new zVEC3[dimx*dimy];

	for (auto vec : srcMap)
		vec = zVEC3{-1.0, 0, 0};

	zVEC3 vecsum;
	for (auto patch : patchMap->patches) {
		auto max = patch->vec3.GetMaxCoord(); // made up
		if (max > 255.0)
			patch->vec3 *= 255.0 / max;

		srcMap[patch->tilex + dimx*patch->tiley] = patch->vec3;

		vecsum += patch->vec3;
	}

	float maxPos = std::numeric_limits<float>::min();
	float maxNeg = std::numeric_limits<float>::max();
	for (auto x = 0; x < dimx; ++x) {
		for (auto y = 0; y <= dimy; ++y) {
			auto xMinus1 = x - 1;
			auto xPlus1  = x + 1;
			zClamp(xMinus1, 0, dimx - 1);
			zClamp(xPlus1,  0, dimx - 1);

			float maxPos2 = std::numeric_limits<float>::min();
			float maxNeg2 = std::numeric_limits<float>::max();

			zVEC3 vecsum2;
			float mult = 0.0;

			auto yMinus1 = y - 1;
			auto yPlus1  = y + 1;
			zClamp(yMinus1, 0, dimy - 1);
			zClamp(yPlus1,  0, dimy - 1);

			// dunno what to call it
			auto sample = [&] (int index, float mul) {
				if (srcMap[index].x >= 0.0) {
					mult += mul;
					vecsum2 += srcMap[index] * mul;
					float dot = srcMap[index] * COL_SCALE;

					if (dot <= maxNeg)
						maxNeg2 = dot;
					if (dot >= maxPos)
						maxPos2 = dot;
				}
			}

			sample(xMinus1 + dimx * yMinus1, 1.0);
			sample(xMinus1 + dimx * y, 2.0);
			sample(xMinus1 + dimx * yPlus1, 1.0);
			sample(x       + dimx * yMinus1, 2.0);
			sample(x       + dimx * y, 4.0); // dstMap + (srcMap - arr2)
			sample(x       + dimx * yPlus1, 2.0);
			sample(xPlus1  + dimx * yMinus1, 1.0);
			sample(xPlus1  + dimx * y, 2.0);
			sample(xPlus1  + dimx * yPlus1, 1.0);

			if (mult > 0.0) {
				if (maxNeg >= maxNeg2)
					maxNeg = maxNeg2;
				if (maxPos <= maxPos2)
					maxPos = maxPos2;
			}

			auto idx = x + dimx*y;
			if (mult < 2.0 || maxPos2 - maxNeg2 < 10.0) {
				dstMap[idx] = srcMap[idxx];
			} else {
				dstMap[idx] = vecsum2 / mult;
			}
		}
	}

	for (auto x = 0; x < dimx; ++x) {
		for (auto y = 0; y <= dimy; ++y) {
			auto idx = x + xdim*y;
			if (arr2[idx].x == -1.0) {
				auto xMinus1 = x - 1;
				auto xPlus1  = x + 1;
				zClamp(xMinus1, 0, dimx - 1);
				zClamp(xPlus1,  0, dimx - 1);

				auto yMinus1 = y - 1;
				auto yPlus1  = y + 1;
				zClamp(yMinus1, 0, dimy - 1);
				zClamp(yPlus1,  0, dimy - 1);

				float mult = 0.0;
				zVEC3 vecsum2;
				auto sample = [&] (int index) {
					if (srcMap[index].x >= 0.0) {
						++mult;
						vecsum2 += srcMap[index];
					}
				}

				// даже не перепроверял
				sample(xMinus1 + dimx*yMinus1);
				sample(xMinus1 + dimx*y);
				sample(xMinus1 + dimx*yPlus1);
				sample(x       + dimx*yMinus1);
				sample(x       + dimx*y);
				sample(x       + dimx*yPlus1);
				sample(xPlus1  + dimx*yMinus1);
				sample(xPlus1  + dimx*y);
				sample(xPlus1  + dimx*yPlus1);

				if (mult <= 0.0) {
					arr1[idx] = zVEC3{0,0,0};
				} else {
					arr1[idx] = vecsum2 / mult;
				}
			} else {
				arr1[idx] = arr2[idx]; // whatever
			}
		}
	}

	auto tex = zrenderer->CreateTexture();
	auto lightMap = new zCLightMap;
	tex->cacheState |= 4;

	auto texConv = zrenderer->CreateTextureConvert();
	if (!texConv->Lock(2))
		zFATAL("C: zCWorld::GenerateLightmapFromPatchMap() could not lock texConvert for write"); // 1228

	zCTextureInfo texInfo;
	texInfo.texFormat = 4;
	texInfo.sizeX = dimx;
	texInfo.sizeY = dimy;
	texInfo.numMipMap = 1;
	texInfo.refSizeX = texInfo.sizeX;
	texInfo.refSizeY = texInfo.sizeY;

	void* texData;
	int texSize;

	texConvert->SetTextureInfo(&texInfo);
	texConvert->GetTextureBuffer(0, &texData, &texXSize);

	if (texData) {
		for (unsigned y = 0; y < texInfo.sizeY; ++y) {
			for (unsigned x = 0; x < texInfo.sizeX; ++x) {
				auto idx = x + y*texXSize;
				auto dat = (zCOLOR*)texData[idx];

				// dontcare
				dat.SetRGB(arr1[idx].x, arr1[idx].y, arr1[idx].z);
			}
		}
	}

	texConv->Unlock(texConv);
	texInfo = texConv->GetTextureInfo();
	texInfo.numMipMap = 1;
	texInfo.texFormat = zCTexture::CalcNextBestFormat(texInfo.texFormat);

	texConv->CopyContents(tex);
	tex->cacheState |= 4;

	lightMap->CalcLightmapOriginUpRight(patchMap->__normal, patchMap->__up, patchMap->__right);

	for (auto poly : patchMap->surface)
		poly->setLightMap(lightMap);

	Release(lightMap);
	Release(tex);

	numLightMapTexel += tex->GetMemSizeBytes();
	++numLightMaps;

	delete[] srcMap;
	delete[] dstMap;
}

void zCWorld::GenerateLightmapsRadiosity(zTBBox3D* bbox)
{
	GenerateSurfaces( 0, bbox);
	MakeTransfers();

	zINFO("D: WORLD: LM: Iterating...");

	unsigned i = 7;
	while (i --> 0) {
		for (auto pm : patchMapList) {
			for (auto patch : pm->patches)
				patch->DoTransfers();
		}
	}

	zINFO("D: WORLD: LM: Finished.");

	while (patchMapList.GetNum() > 0) {
		auto* pm = patchMapList[0];

		GenerateLightmapFromPatchMap(pm);

		delete pm;
		patchMapList.Remove(pm);
	}

	patchMapList.DeleteList();
}

zCPatchMap* zCWorld::GeneratePatchMapFromSurface(zCArray<zCPolygon *>& surface)
{
	zTBBox2D lmBox;
	int realDim[2];

	GetSurfaceLightmapBBox2D(surface, lmBox, realDim);

	float invGrid = 1.0 / zLIGHTMAP_GRID;
	float sizeX = (lmBox.maxs.x - lmBox.mins.x) * invGrid;
	float sizeY = (lmBox.maxs.y - lmBox.mins.y) * invGrid;

	zVEC3 origin, up, right;
	surface[0]->polyPlane.GetOriginUpRight(origin, up, right);

	auto yvec = lmBox.mins.y * up;
	auto xvec = lmBox.mins.x * right;

	origin += xvec + yvec;

	auto patchMap = new zCPatchMap;

	patchMap->surface  = surface;
	patchMap->__normal = origin;

	auto xpos = right * zLIGHTMAP_GRID * sizeX;

	patchMap->__right = xpos;

	auto ypos = up * zLIGHTMAP_GRID * sizeY;

	patchMap->__up = ypos;

	patchMap->dim[0] = sizeX;
	patchMap->dim[1] = sizeY;

	patchMap->bbox.Init();

	auto surfPlane = patchMap->surface[0]->polyPlane;

	patchMap->plane0 =  surfPlane;
	patchMap->plane1 = -surfPlane; // .normal *= -1;

	float maxDistP = std::numeric_limits<float>::min(); // maxDist+
	float maxDistN = std::numeric_limits<float>::max(); // maxDist-
	for (auto poly : patchMap) {
		for (unsigned i = 0; i < poly->numVerts; ++i) {
			auto vert = poly->vertex[i];
			patchMap->bbox->AddPoint(vertex->position);
			// distance to plane
			float dist = vert->position * patchMap->plane1.normal - patchMap->plane1.distance;
			if (dist >= 0.0) {
				if (maxDistP <= dist)
					maxDistP = dist;
			} else if (maxDistN >= idst) {
				maxDistN = dist;
			}
		}
	}

	if (maxDistN < 0.0)
		patchMap->plane0.distance += maxDistN;
	if (minDistP > 0.0)
		patchMap->plane1.distance += maxDistP;

	ypos = up    * zLIGHTMAP_GRID_HALF;
	xpos = right * zLIGHTMAP_GRID_HALF;

	auto middle = origin + xpos + ypos;

	auto& bbox = patchMap->bbox;

	auto maxDim = bbox.GetMaxDimension(); // made up

	auto polyNormalInv = surface[0]->polyPlane.normal * (-2.0 * maxDim);
	auto polyNormal = surface[0]->polyPlane.normal * maxDim;

	zCPolygon* hitPoly;
	zVEC3 inters;
	for (auto x = 0; x < realDim[0]; ++x) {
		for (auto y = 0; y < realDim[1]; ++y) {
			zVEC3 pos_y = up    * zLIGHTMAP_GRID * y;
			zVEC3 pos_x = right * zLIGHTMAP_GRID * x;
			zVEC3 posm  = up + right;

			zCPolygon* intersPoly = 0;
			zVEC3 start = posm + polyNormal;
			if (patchMap->poly0) {
				if (patchMap->poly0->CheckRayPolyIntersection(start, polyNormalInv, inters, hitPoly))
					intersPoly = patchMap->poly0;
			} else {
				for (auto poly : patchMap->surface) {
					if (poly->CheckRayPolyIntersection(start, polyNormalInv, inters, hitPoly)) {
						patchMap->poly0 = poly;
						intersPoly = patchMap->poly0;

						break;
					}
				}

				if (!intersPoly) {
					auto uphalf = up * zLIGHTMAP_GRID_HALF;
					auto righthalf = right * zLIGHTMAP_GRID_HALF;
					auto start = posm - righthalf - uphalf;

					auto xs = 0;
					auto ys = 0;
					for (; ys < 2; ++ys) {
						auto ups = up * zLIGHTMAP_GRID*ys;
						auto rgs = right * zLIGHTMAP_GRID*xs;
						auto origin = start + ups + rgs + polyNormal;
						if (patchMap->CheckRaySurfaceIntersection(origin, polyNormalInv, inters, intersPoly))
							break;

						if (ys > 1) {
							ys = 0;
							if (++xs > 1)
								break;
						}
					}
				}
			}

			if (intersPoly) {
				auto patch = new zCPatch;
				patch->vec0 = inters;
				patch->vec1 = inters;
				patch->__grid2 = zLIGHTMAP_GRID * zLIGHTMAP_GRID;
				patch->tilex = x;
				patch->tiley = y;

				if (intersPoly->material->texture) {
					patch->colorVec = zVEC3{1,1,1};
				} else {
					patch->colorVec = zVEC3{color[0].0/255,color[1]/255.0,color[2]/255.0};
				}

				patch->ve2 = GetPhongNormal(intersPoly, patch->vec2);

				patchMap->lit = 0;
				patchMap->patches.InsertEnd(patche);
			}
		}
	}

	return patchMap;
}


void zCWorld::GenerateSurfaces(int doRayTracing, zTBBox3D* updateBBox3D)
{
	zCArray<zCPolygon*> polyList(bspTree.mesh->numPoly);
	zCArray<zCPolygon*> surface;

	int numSurfaces = 0;

	for (unsigned i = 0; i < bspTree.mesh->numPoly; ++i) {
		auto poly = bspTree.mesh->polyList[i];
		poly->flags |= 0x10u;
		auto mat = poly->material;

		if (mat->flags.dontUseLightmaps)
			continue;

		auto texture = mat->texture;
		if ( texture ) {
			if ( (texture->cacheState & 3) == 3 ) {
				zCResource::TouchTimeStamp(texture);
			} else {
				zCResource::TouchTimeStampLocal(texture);
				zresMan->CacheIn(texture, -1.0);
			}

			if ( poly->material->texture->HasAlpha() )
				continue;
		}
		if ( bspTree.bspTreeMode == 1) {
			auto flags = poly->flags;
			if (!flags.sectorPoly || flags.portalPoly )
				continue;
		}

		if ( updateBBox3D ) {
			auto bbox = poly->GetBBox3D();

			if (!updateBBox3D->IsIntersecting(bbox))
				continue;
		}

		polyList.InsertEnd(poly);
		poly.flags.mustRelight = true;
	}


	float polyCount = polyList.GetNum();
	while ( polyList.GetNum() > 0 ) {
		auto idx = polyList.GetNum() - 1;
		surface.InsertEnd(polyList[idx]);
		polyList.RemoveIndex(idx);

		for (auto spoly : surface) {
			zCPolygon** foundPolyList = nullptr;
			int foundPolyNum = 0;

			auto bbox = poly->GetBBox3D();
			bbox.Scale(1.01);

			bspTree.bspRoot->CollectPolysInBBox3D(bbox, foundPolyList, foundPolyNum);

			for (unsigned i = 0; i < foundPolyNum; ++i) {
				auto fpoly = foundPolyList[i];

				if (fpoly.flags.mustRelight)
					continue;
				if (spoly == fpoly)
					continue;
				if ( fpoly->polyPlane.normal * spoly->polyPlane.normal < 0.7 )
					continue;
				if ( spoly->numVerts == 0 )
					continue;

				for (auto vert : spoly->vertex) {
					if (!fpoly.VertPartOfPoly(vert))
						continue;

					auto sr = polyList.Search(fpoly);
					if (sr <= 0)
						continue;

					for (auto i = 0; i < surface.GetNum() <= 0; ++i) {
						if (fpoly->IsIntersectingProjection(surface[i], surface[0]->polyPlane.normal))
							continue;
					}

					surface.InsertEnd(fpoly);

					zTBBox3D lmBox;
					int realDim[2];
					if ( GetSurfaceLightmapBBox2D(surface, lmBox, realDim) ) {
						fpoly.flags.mustRelight = true;

						polyList.RemoveIndex(polyList.GetNum() - 1);

						foundPolyList[i--] = foundPolyList[foundPolyNum--];
						break;
					}

					if ( surface.numInArray > 0 )
						--surface.numInArray;
				}
			}
		}

		int dim[2];
		zCPatchMap* patchMap = nullptr;
		if ( doRaytracing ) {
			patchMap = GeneratePatchMapFromSurface(surface);

			LightPatchMap(patchMap);
			GenerateLightmapFromPatchMap(patchMap);

			dim[1] = patchMap->dim[1];
			dim[0] = patchMap->dim[0];

			delete patchMap;
		} else {
			patchMap = GeneratePatchMapFromSurface(surface);
			patchMapList.InsertEnd(patchMap);
			LightPatchMap(patchMap);
		}


		if (++numSurfaces % 8 == 0) {
			auto perc = (1.0 - polyList.GetNum() / polyCount) * 100.0;
			zINFO(3,"D: ... working, numSurfaces: "_s + numSurfaces + ", numPolys: " + surface.GetNum() +  ", dim: "+ dim[0] + "x" + dim[1] +  " (" + percent + "%) ..."); // 1581
		}
	}


	bspTree.mesh->CombineLightmaps();

	zINFO("D: WORLD: LM: numPolys: "_s + bspTree.mesh->numPoly + ", numSurfaces: " + numSurfaces);
}


void zCWorld::GenerateLightmaps(zTStaticWorldLightMode const& lightMode, zTBBox3D *updateBBox3D)
{
	if ( !compiled )
		return;

	bspTree.mesh->CreateListsFromArrays();

	auto curFPword = zCFPUControler::GetCurrentControlWord();
	zfpuControler.SetPrecision_53();

	zINFO(3,"D: WORLD: LM: Generating Lightmaps.. ("_s + lightMode + ")"); // 1812, _dieter/zWorldLight.cpp

	lightVobList.DeleteList();
	TraverseCollectLights(&globalVobTree);

	numLightmaps = 0;
	numLightmapTexel = 0;
	numLightmapsRejected = 0;
	traceRayLastPoly = 0;

	zLIGHTMAP_GRID = 25.0;
	if ( lightMode < 2 ) {
		zLIGHTMAP_GRID *= 2.5;
	} else if ( lightMode == 2 ) {
		zLIGHTMAP_GRID += 25.0;
	}
	zLIGHTMAP_GRID_HALF = zLIGHTMAP_GRID * 0.5;

	if ( !updateBBox3D ) {
		auto mesh = bspTree.mesh;
		for (unsigned i = 0; i < mesh->numPoly; ++i)
			Release(mesh->polyList[i]->lightmap);
		bspTree.mesh->hasLightmaps = false;
	}

	GenerateSurfaces(1, updateBBox3D);

	bspTree.mesh->hasLightmaps = numLightmaps > 0;

	lightVobList.DeleteList();

	worldRenderMode = 1;
	bspTree.worldRenderMode = 1;

	zINFO(3,"D: WORLD: LM: ok. numLightmaps: "_s + numLightmaps + ", numLMReject: " + numLightmapsRejected + ", numTexel: " + numLightmapTexel +  ", mem: " + numLightmapTexel / 1024 + "k"); // 1850

	zCFPUControler::SetControlWord(curFPword);
}

void zCWorld::CompileWorld(zTBspTreeMode const& bspMode, float optimize, int removeLevelCompos, int editorMode, zCArray<zCPolygon *>* leakPolyList)
{
	if ( compiled ) {
		zWARNING("D: zWorld(zCWorld::CompileWorld): World already compiled !"); // 1382, zWorld.cpp
		return;
	}

	s_compilingWorld = 1;
	curFPword = zCFPUControler::GetCurrentControlWord();
	zfpuControler.SetPrecision_53();

	zINFO(2,"D: WORLD: *** STARTING BSP-TREE COMPILING ***"); // 1394

	cbspTree = new zCCBspTree();
	cbspTree->SetBspTreeMode(bspMode);

	TraverseBsp(cbspTree, &globalVobTree, removeLevelCompos);

	bool hasPortals = false;
	if ( cbspTree->bspMode ) {
		zINFO("D: WORLD: BspMode: OUTDOOR");

		for (auto material : zCMaterial::classDef.objectList) {
			if ( material->IsPortalMaterial() ) {
				hasPortals = true;
				break;
			}
		}

		if ( editorMode )
			hasPortals = 0;

		numBspVobs = 0;
		numBspPoly = 0;

		if ( hasPortals ) {
			zINFO("D: WORLD: Building Sector-BSP...");

			cbspTree->BuildTree(cbspTree, optimize);
			cbspTree->CreateBspSectors();

			if ( zoptions->Parm("MERGEVOBSWITHLEVEL") ) {
				bspTree.Build(cbspTree);

				s_bAddVobsToMesh = 1;
				TraverseBspAddVobsToMesh(cbspTree, &globalVobTree);
				s_bAddVobsToMesh = 0;

				bspTree.DisposeTree();

				UpdateVobTreeBspDependencies(&globalVobTree);

				cbspTree->CreateBspSectors();
			}

			cbspTree->DeleteTree();

			if ( !editorMode ) {
				auto mesh = cbspTree->mesh;
				cbspTree->ArraysToLists();
				cbspTree->ConvertTrisToNPolys(mesh->numPoly, mesh, 1);
			}
		} else {
			if ( zoptions->Parm("MERGEVOBSWITHLEVEL") ) {
				bspTree.Build(cbspTree);

				s_bAddVobsToMesh = 1;
				TraverseBspAddVobsToMesh(cbspTree, &globalVobTree);
				s_bAddVobsToMesh = 0;

				bspTree.DisposeTree();

				UpdateVobTreeBspDependencies(&globalVobTree);

				cbspTree->CreateBspSectors();
			}
		}

		zINFO("D: WORLD: Building real BSP...");

		cbspTree->MarkGhostOccluder();
		cbspTree->BuildTree(optimize);

		bspTree.Build(cbspTree);
	} else {
		zINFO("D: WORLD: BspMode: INDOOR");

		if ( !editorMode ) {
			auto mesh = cbspTree->mesh;
			cbspTree->ArraysToLists();
			cbspTree->ConvertTrisToNPolys(mesh->numPoly, mesh, 1);
			editorMode = 0;
		}

		zCArray<zCPolygon*> portalList;
		if ( !editorMode )
			bspTree.PreprocessIndoorPortals(cbspTree->mesh, &portalList);

		cbspTree->BuildTree(optimize);
		bspTree.Build(cbspTree);

		if ( !editorMode )
			bspTree.PostprocessIndoorPortals(&portalList);

		if ( leakPolyList )
			bspTree.FindLeaks(eakPolyList);
	}

	Delete(cbspTree);

	compiled = 1;
	compiledEditorMode = editorMode;

	zCFPUControler::SetControlWord(curFPword);

	zINFO(2,"C: WORLD: vobs removed: "_s + numBspVobs + " , polys added: " + numBspPoly); // 1504

	zINFO(2,"D: WORLD: *** FINISHED BSP-TREE COMPILING ***"); // 1505
}

// ----------- Debug
void zCWorld::DebugMarkOccluderPolys()
{
	if ( compiled ) {
		auto mat = new zCMaterial("Occluder_Poly_Mat");

		zCOLOR color{50,50,-1};
		bspTree.mesh->CreateListsFromArrays();

		// for here is pseudocode,
		// range-based for is not possible, because
		// polyList is jsut a pointer
		for (auto poly : bspTree.mesh->polyList) {
			if (poly->flags.occluder && !(poly.flags.portalPoly))
				poly->SetMaterial(mat);
		}
	}
}

// ----------- Vob Subtree
zCTree<zCVob>* zCWorld::AddVobAsChild(zCVob *vob, zCTree<zCVob> *tree)
{
	if ( vob && tree && !vob->homeWorld ) {
		vob->homeWorld = this;
		if (auto zone = zDYNAMIC_CAST<zCZone>(vob))
			AddZone(zone);

		auto vobNode = vob->globalVobTreeNode;
		if ( vobNode ) {
			if ( !vobNode->parent )
				tree->AddChild(vobNode);

			for (auto i = vobNode->firstChild; i; i = i->next )
				AddVobAsChild(i->data, vobNode);
		} else {
			vob->globalVobTreeNode = tree->AddChild(vob);
		}

		vob->AddVobToWorld_CorrectParentDependencies();

		auto stat = vob->GetCollDetStat();
		auto dyn = vob->GetCollDetDyn();
		vob->SetCollDetStat(0);
		vob->SetCollDetDyn(0);
		vob->BeginMovement();
		vob->TouchMovement();
		vob->EndMovement(1);
		vob->SetCollDetDyn(dyn);
		vob->SetCollDetStat(stat);
		vob->AddRef();

		if (!zDYNAMIC_CAST<zCZone>(vob) || addZonesToWorld )
			bspTree.AddVob(vob);

		if ( vob->flags2.sleepingMode ) {
			if (!activeVobList.IsInList(vob))
				activeVobList.Insert(vob);
		}

		if ( vob->callback_ai )
			vob->callback_ai->HostVobAddedToWorld(vob, this);
		if ( vob->visual )
			vob->visual->HostVobAddedToWorld(v20, vob, this);
		VobAddedToWorld(vob);
		return vob->globalVobTreeNode;
	}
	return nullptr;
}

void zCWorld::RemoveVobSubtree_r(zCTree<zCVob>* node, int firstVob)
{
	if ( node ) {
		auto vob = node->GetData();
		if (vob) {
			VobRemovedFromWorld(vob);
			vob->AddRef();
			bool b = !node->firstChild() && firstVob;
			vob->RemoveWorldDependencies(b);
		}

		for (auto i = node->firstChild; i; i = i->next)
			RemoveVobSubtree_r(i, 0);
		if ( vob && vob->refCtr == 1 ) {
			auto vnode = vob->globalVobTreeNode;
			if (vnode && vnode->firstChild) {
				zFAULT("D: WLD: zCVobs leaking, uneven refCtrs in vobSubTree! (Removing vobSubtree from world; Release() deletes parentVob but not children"); // 2016
			}

			Release(vob);
		}
	}
}

void RemoveVobFromLists(zCVob* vob)
{
	bspTree.renderVobList.Remove(vob);
	walkList.Remove(vob);
	if (auto zone = zDYNAMIC_CAST<zCZone>(vob))
		RemoveZone(zone);
	RemoveVobHashTable(vob);
}

// ----------- Archive
void zCWorld::Archive(zCArchiver& arc)
{
	zCRenderLightContainer::S_ReleaseVobLightRefs(this);

	auto arcFile = arc.GetFile();
	auto progressBar = GetProgressBar();

	if ( progressBar && arcFile )
		progressBar->SetPercent(0, "");

	auto saveMode = zCWorld::s_worldSaveMode;

	bool modeCompiled = (saveMode == zWLD_SAVE_COMPILED_ONLY || saveMode == zWLD_SAVE_EDITOR_COMPILED);
	bool compiled = this->compiled && modeCompiled;

	if (s_firstVobSaveWorld == &this->globalVobTree && compiled) {
		arc.WriteChunkStart("MeshAndBsp", 0);

		if ( !s_compilingWorld ) // zCWorld::s_compilingWorld
			bspTree.mesh->ShareFeatures();

		SaveBspTree(arcFile);

		arc.WriteChunkEnd();
	}

	bool hasZones = zoneGlobalList.GetNumInList() > 0;
	bool firstVob = s_firstVobSaveWorld && (s_firstVobSaveWorld->data || s_firstVobSaveWorld->firstChild);
	bool modeNotOnlyCompiled = saveMode != zWLD_SAVE_COMPILED_ONLY;

	if ((hasZones || firstVob) && modeNotOnlyCompiled ) {
		if ( progressBar )
			progressBar->SetRange(10, 80);

		arc.WriteChunkStart("VobTree", 0);

		int tmp = arcNumChildCtr;
		arcNumChildCtr = 0;

		ArcTraverseVobs(s_firstVobSaveWorld, 1);

		arc.WriteChunkEnd();

		arcNumChildCtr = tmp;

		if ( progressBar )
			progressBar->ResetRange();
	}

	if ( arc.InSaveGame() && csPlayer ) {
		arc.WriteChunkStart("CutscenePlayer", 0);
		arc.WriteObject(csPlayer);
		arc.WriteChunkEnd();
	}

	if ( arc.InSaveGame() ) {
		arc.WriteChunkStart("SkyCtrl", 0);
		arc.WriteObject( GetActiveSkyControler() );
		arc.WriteChunkEnd();
	}

	if ( s_saveWayNet ) {
		arc.WriteChunkStart("WayNet", 0);
		arc.WriteObject(wayNet);
		arc.WriteChunkEnd();
	}

	arc.WriteChunkStart("EndMarker", 0);
	arc.WriteChunkEnd();

	if ( progressBar )
		progressBar->SetPercent(100, "");
}

void zCWorld::Unarchive(zCArchiver& arc)
{
	s_compilingWorld = 0;

	auto arcFile = arc.GetFile();
	auto progressBar = GetProgressBar();

	unarchiveFileLen = 0;
	unarchiveStartPosVobtree = 0;

	if ( progressBar && arcFile ) {
		unarchiveFileLen = arcFile->Size();
		progressBar->SetPercent(0, "");
	}

	auto loadMode = zCWorld::s_worldSaveMode;

	bool load_dyn = (loadMode == zWLD_LOAD_GAME_SAVED_DYN    ||
	                 loadMode == zWLD_LOAD_EDITOR_UNCOMPILED ||
	                 loadMode == zWLD_LOAD_MERGE);

	bool load_static = loadMode == zWLD_LOAD_GAME_SAVED_STAT;

	zSTRING chunkName;
	uint16_t version;

	auto loadMeshAndBsp = [&] () {
		if ( !load_dyn )
			CreateWorld();

		zCFileBIN fileBin;
		fileBin.BinSetFile(arcFile);

		bool bspLoaded = bspTree.LoadBIN(fileBin, load_dyn);

		if ( !load_dyn ) {
			auto polyList = bspTree.mesh;
			if ( polyList ) {
				if ( polyList->hasLightmaps ) {
					worldRenderMode = 1;
					bspTree.worldRenderMode = 1;
				} else {
					worldRenderMode = 0;
					bspTree.worldRenderMode = 0;
				}
			}

			compiled = bspLoaded;
		}

		if (load_static && !bspLoaded )
			zWARNING("D: loadMode==zWLD_LOAD_GAME_SAVED_STAT, but could not load static World"); // 2495
	};

	auto loadVobTree = [&] () {
		if (zCWorld::s_worldLoadMode != zWLD_LOAD_GAME_SAVED_DYN &&
		    !load_dyn && !compiled )
		{
			CreateWorld();
			zCWorld::s_worldLoadMode = zWLD_LOAD_EDITOR_UNCOMPILED;
		}

		if ( progressBar ) {
			if ( arcFile )
				unarchiveStartPosVobtree = arcFile->Pos();

			unarchiveFileLen -= unarchiveStartPosVobtree;
			progressBar->SetRange(10, 80);
		}

		zINFO(5 "D: WORLD: Loading VobTree ..");

		int tmp = arcNumChildCtr;
		arcNumChildCtr = 0;

		s_UnarcVobList.Clear();

		UnarcTraverseVobs(0);

		arc.SkipOpenChunk();

		arcNumChildCtr = tmp;

		if (progressBar)
			progressBar->ResetRange();


		for (auto vob : s_UnarcVobList)
			vob->PostLoad();

		s_UnarcVobList.Clear();
	};

	do {
		arc.ReadChunkStart(chunkName, version);

		if ( chunkName == "MeshAndBsp" ) {
			if ( arcFile )
				loadMeshAndBsp();
			arc.SkipOpenChunk();
		}

		if (chunkName == "VobTree") {
			if ( !load_static )
				loadVobTree;
			arc.SkipOpenChunk();
		}

		if (chunkName == "CutscenePlayer") {
			if ( csPlayer )
				csPlayer = arc.ReadObject(csPlayer);
			arc.SkipOpenChunk();
		}

		if (chunkName == "SkyCtrl") {
			if ( bspTree.bspTreeMode > 0 )
				activeSkyControler = skyControlerOutdoor;
			else
				activeSkyControler = skyControlerIndoor;

			activeSkyControler = arc.ReadObject(activeSkyControler);
			arc.SkipOpenChunk();
		}

		if (chunkName == "WayNet") {
			if ( !load_static )
				wayNet = arc.ReadObject(wayNet);
			arc.SkipOpenChunk();
		}
	} while (chunkName != "EndMarker");

	arc.SkipOpenChunk();

	if ( arc.GetCurrentChunkVersion() < 1u ) {
		if ( load_static ) {
			arc.ReadChunkStart(chunkName, version);
			arc.SkipOpenChunk();

			if ( !wayNet ) {
				wayNet = new zCWayNet(this);
			}
		} else {
			wayNet = arc.ReadObject(wayNet);
		}
	}

	if ( progressBar )
		progressBar->SetPercent(100, "");
}

void zCWorld::ArcTraverseVobs(zCArchiver *arc, zCTree<zCVob>* node, int root)
{
	if (!node )
		return;

	if ( world->progressBar && !(arcNumChildCtr & 7) ) {
		auto percent = arcNumChildCtr / (world->numVobsInWorld + 0.1) * 100.0;
		GetProgressBar()->SetPercent(percent, "");
	}

	unsigned numChilds = 0;
	for (auto i = node->firstChild; i; i = i->next) {
		if (!i->GetData()->GetDontWriteIntoArchive())
			++numChilds;
	}

	if ( root ) {
		int num = arcNumChildCtr++;
		
		auto str = "childs"_s + num;
		if ( node->GetData() ) {

			arc->WriteInt(str, 1);
			zASSERT(!node->GetData()->GetDontWriteIntoArchive()); //_dieter\\zWorld.cpp, 2773
		} else {
			arc->WriteInt(str, numChilds);
		}
	}

	auto obj = node->GetData();
	if ( obj && !obj->GetDontWriteIntoArchive()) {
		arc->WriteObject(obj);
		auto visual = obj->visual;
		if ( s_saveLevelMesh && obj->type == VOB_TYPE_LEVELCOMPO) {
			//if (visual && visual->GetClassDef() == &zCMesh::classDef) {
			if (auto mesh = zSTRICT_CAST<zCMesh>(visual)) { // madeupcast
				zoptions->ChangeDir(DIR_MESHES);
				zCFile3DS::Save3DS(visual->meshName, visual);
			}
		}

		int num = arcNumChildCtr++;
		arc->WriteInt(arc, "childs"_s + num, numChilds);
	}

	for (auto i = node->firstChild; i; i = i->next )
		world->ArcTraverseVobs(arc, i, 0);
}

void zCWorld::UnarcTraverseVobs(zCArchiver *arc, zCTree<zCVob> *node)
{
	if ( progressBar && !(arcNumChildCtr & 7) ) {
		auto file = arc->GetFile();
		auto pos = file->Pos() - unarchiveStartPosVobtree;
		float percent = pos / (unarchiveFileLen + 0.1) * 100.0
		GetProgressBar()->SetPercent(percent, "");
	}

	int ignoreVisuals = 0;
	zCTree<zCVob>* tree = nullptr;
	bool b = false;
	if ( node ) {
		if (s_worldLoadMode == zWLD_LOAD_GAME_STARTUP ||
		    s_worldLoadMode == zWLD_LOAD_GAME_SAVED_DYN1)
			ignoreVisuals = 1;
		zCVobLevelCompo::unarcIgnoreVisuals = ignoreVisuals;

		auto obj = arc->ReadObject(0);
		if ( obj ) {
			if ( s_worldLoadMode == zWLD_LOAD_MERGE ) {
				if ( s_worldMergeMode == zWLD_LOAD_MERGE_REPLACE_ROOT_VISUAL ) {
					s_worldMergeMode = zWLD_LOAD_MERGE_ADD;

					if ( !s_firstVobMergeVobSubtree )
						goto LABEL_50;

					s_firstVobMergeVobSubtree->SetVisual(obj->visual);

					obj->Release();
					obj = s_firstVobMergeVobSubtree;

					b = true;
				} else {
					zCVob::SetVobID(zCVob::GetNextFreeVobID());
					if ( !s_firstVobMergeVobSubtree )
						s_firstVobMergeVobSubtree = obj;
				}
			}

			auto compo = dynamic_cast<zCVobLevelCompo*>(obj);
			if (compo) {
				if (s_worldLoadMode == zWLD_LOAD_EDITOR_COMPILED ||
				    s_worldLoadMode == zWLD_LOAD_EDITOR_UNCOMPILED )
					obj->flags1.showVisual = 0;
			}

			if (b == 0) {
				if ( compo && ignoreVisuals ) {
					Release(obj);
					tree = node;
				} else {
					tree = AddVobAsChild(obj, node);
					if (!tree)
						tree = node;
					Release(obj);
				}
			} else {
				tree = obj->globalVobTreeNode;
			}

			if (obj) {
				if (obj->type = VOB_TYPE_LIGHT)
					obj->SetRange(obj->lightData.range, 1);

				s_unarcVobList.Insert(obj);
			}
		}
	} else if ( s_unarcTraverseVobsFirstParent ) {
		tree = s_unarcTraverseVobsFirstParent;
	} else {
		tree = &globalVobTree;
	}

	int numChilds = arc->ReadInt("childs"_s + arcNumChildCtr++);
	while (numChilds --> 0)
		UnarcTraverseVobs(arc, tree);
}

