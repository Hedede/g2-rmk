#include <Gothic/zobject/zCObject.h>

#define CLASSDEF(T,B,c,v,f,s) \
        zCClassDef T::classDef(#T, #B, c, sizeof(T), v, f, s)

#define CLASSDEF_S(T,B,v,f,s) \
	CLASSDEF_S(T,B,T::_CreateNewInstance,v,f,s)

#define CLASS_S(classname, base, scriptname, version, flags) \
	CLASSDEF_S(classname, base, version, flags, #scriptname)

#define CLASS(classname, base, version, flags) \
	CLASSDEF_S(classname, base, version, flags, "")

#define ABSTRACT(classname, base, version, flags) \
	CLASSDEF(classname, base, nullptr, version, flags, "")

ABSTRACT(zCObject, NULL, 0, 0);
CLASS(zCObjectFactory, zCObject, 0, 0);
CLASS(oCObjectFactory, zCObjectFactory, 0, 0);

CLASS(zCWorld, zCObject, 0, 1);
CLASS(oCWorld, zCWorld, 0, 0);

CLASS(zCVob, zCObject, 0, 5);

ABSTRACT(zCZone, zCVob, 0, 0);
CLASS(zCZoneZFog, zCZone, 0, 2);
CLASS(zCZoneZFogDefault, zCZoneZFog, 0, 0);
CLASS(zCZoneVobFarPlane, zCZone, 0, 0);
CLASS(zCZoneVobFarPlaneDefault, zCZoneVobFarPlane, 0, 0);
CLASS(zCZoneReverb, zCZone, 0, 0);
CLASS(zCZoneReverbDefault, zCZoneReverb, 0, 0);
CLASS(zCZoneMusic, zCZone, 0, 0);
CLASS(oCZoneMusic, zCZoneMusic, 0, 0);
CLASS(oCZoneMusicDefault, oCZoneMusic, 0, 0);
CLASS(zCVobSound, zCZone, 0, 4);
CLASS(zCVobSoundDaytime, zCVobSound, 0, 0);


CLASS(zCWayNet, zObject, 0, 0);
CLASS(zCWaypoint, zObject, 0, 0);

CLASS(zCVobWaypoint, zCVob, 0, 0);
CLASS(zCVobStartpoint, zCVob, 0, 0);
CLASS(zCVobSpot, zCVob, 0, 0);
CLASS(zCVobStair, zCVob, 0, 0);

CLASS(zCVobLight, zCVob, 0, 5);
CLASS(zCVobLightPreset, zCObject, 0, 0);
CLASS(zCVobLevelCompo, zCVob, 0, 0);

ABSTRACT(zCEffect, zCVob, 0, 0);
CLASS(zCEarthquake, zCEffect, 0, 0);
CLASS(zCPFXControler, zCEffect, 0, 0);
CLASS(zCTouchDamage, zCEffect, 0, 1);
CLASS(oCTouchDamage, zCTouchDamage, 0, 0);
CLASS(zCTouchAnimate, zCEffect, 0, 0);
CLASS(zCTouchAnimateSound, zCTouchAnimate, 0, 0);
CLASS(zCVobAnimate, zCEffect, 0, 0);
CLASS(zCVobLensFlare, zCEffect, 0, 1);
CLASS(zCLensFlareFX, zCObject, zCLASS_FLAG_SHARED_OBJECTS, 2);
CLASS(zCVobScreenFX, zCEffect, 0, 1);
CLASS(zCMusicControler, zCEffect, 0, 0);

CLASS(oCDummyVobGenerator, zCVob, 0, 0);

ABSTRACT(oCVob, zCVob, 0, 0);
CLASS_S(oCItem, oCVob, C_ITEM, 0, 0);

CLASS(oCNpcTalent, zCObject, 0, 1);
CLASS_S(oCNpc, oCVob, C_NPC, 0, 7);
CLASS(oCNpcMessage, zCEventMessage, 0, 0);
CLASS(oCMsgDamage, oCNpcMessage, 0, 0);
CLASS(oCMsgWeapon, oCNpcMessage, 0, 0);
CLASS(oCMsgAttack, oCNpcMessage, 0, 0);
CLASS(oCMsgUseItem, oCNpcMessage, 0, 0);
CLASS(oCMsgState, oCNpcMessage, 0, 0);
CLASS(oCMsgManipulate, oCNpcMessage, 0, 1);
CLASS(oCMsgMagic, oCNpcMessage, 0, 1);

CLASS(oCMob, oCVob, 0, 2);
CLASS(oCMobMsg, zCEventMessage, 0, 0);
CLASS(oCMobInter, oCMob, 0, 3);
CLASS(oCMobBed, oCMobInter, 0, 0);
CLASS(oCMobSwitch, oCMobInter, 0, 0);
CLASS(oCMobWheel, oCMobInter, 0, 0);
CLASS(oCMobLadder, oCMobInter, 0, 0);
CLASS(oCMobFire, oCMobInter, 0, 2);
CLASS(oCMobItemSlot, oCMobInter, 0, 0);
ABSTRACT(oCMobLockable, oCMobInter, 0, 0);
CLASS(oCMobContainer, oCMobLockable, 0, 1);
CLASS(oCMobDoor, oCMobLockable, 0, 1);

ABSTRACT(zCTriggerBase, zCVob, 0, 0);
CLASS(zCTriggerUntouch, zCTriggerBase, 0, 1);
CLASS(zCMessageFilter, zCTriggerBase, 0, 0);
CLASS(zCCodeMaster, zCTriggerBase, 0, 0);
CLASS(zCMoverControler, zCTriggerBase, 0, 0);
CLASS(zCTriggerWorldStart, zCTriggerBase, 0, 0);
CLASS(zCTrigger, zCTriggerBase, 0, 2);
CLASS(zCTriggerList, zCTrigger, 0, 0);
CLASS(zCTriggerTeleport, zCTriggerBase, 0, 0);
CLASS(zCMover, zCTrigger, 0, 1);
CLASS(oCCSTrigger, zCTrigger, 0, 0);
CLASS(oCTriggerChangeLevel, zCTrigger, 0, 0);
CLASS(oCTriggerScript, zCTrigger, 0, 0);

CLASS(zCEventManager, zCObject, 0, 0);
CLASS(zCNetEventManager, zCEventManager, 0, 0);

CLASS(zCNetManager, zCObject, 0, 0);
CLASS(zCNetVobControl, zCObject, 0, 0);

CLASS(zCEventMessage, zCObject, 0, 1);
CLASS(zCEventCore, zCEventMessage, 0, 0);
CLASS(zCEventCommon, zCEventMessage, 0, 0);
CLASS(zCEventMover, zCEventMessage, 0, 0);
CLASS(zCEventScreenFX, zCEventMessage, 0, 0);
CLASS(zCEventMusicControler, zCEventMessage, 0, 0);
CLASS(zCEvMsgCutscene, zCEventMessage, 0, 0);
CLASS(zCCSCamera_EventMsg, zCEventMessage, 0, 6);
CLASS(zCCSCamera_EventMsgActivate, zCEventMessage, 0, 6);

CLASS(zCCSCamera, zCVob, 0, 0);
CLASS(zCCamTrj_KeyFrame, zCVob, 0, 7);

ABSTRACT(zCCSBlockBase, zCObject, 0, 0);
CLASS(zCCSBlock, zCCSBlockBase, 0, 0);
CLASS(zCCSSyncBlock, zCCSBlockBase, 0, 0);
CLASS(zCCSAtomicBlock, zCCSBlockBase, 0, 0);
CLASS(zCCutscene, zCCSBlock, 0, 3);
CLASS(zCCutsceneContext, zCCutscene, 0, 1);
CLASS(zCCSRole, zCObject, 0, 3);

CLASS(zCCSLib, zCObject, 0, 0);

CLASS(zCCSManager, zCObject, 0, 1);
CLASS(oCCSManager, zCCSManager, 0, 0);

CLASS(zCCSPlayer, zCObject, 0, 1);
CLASS(oCCSPlayer, zCCSPlayer, 0, 0);

CLASS(zCCSPoolItem, zCObject, 0, 0);

CLASS(zCCSProps, zCObject. 0, 3);
CLASS(oCCSProps, zCCSProps, 0, 1);

CLASS(oCSavegameInfo, zCObject, 0, 0);
CLASS(oCSpell, zCObject, 0, 0);

CLASS(oCVisualFX, zCEffect, zCLASS_FLAG_SHARED_OBJECTS, 1);
CLASS(oCVisFX_MultiTarget, oCVisualFX, 0, 0);

ABSTRACT(zCAIBase, zCObject, 0, 0);
CLASS(zCAICamera, zCAIBase, 0, 0);
CLASS(oCAICamera, zCAIBase, 0, 0);

CLASS(zCAIPlayer, zCAIBase, 0, 1);
CLASS(oCAniCtrl_Human, zCAIPlayer, 0, 0);
CLASS(oCAIHuman, oCAniCtrl_Human, 0, 0);
CLASS(oCAIHuman_Stand, zCAIBase, 0, 0);

ABSTRACT(oCAISound, zCAIBase, 0, 0);
CLASS(oCAIArrowBase, oCAISound, 0, 0);
CLASS(oCAIArrow, oCAIArrowBase, 0, 0);
CLASS(oCAIDrop, oCAISound, 0, 0);
CLASS(oCAIVobMove, oCAISound, 0, 0);
CLASS(oCAIVobMoveTorch, oCAIVobMove, 0, 0);

ABSTRACT(zCArchiver, zCObject, 0, 0);
CLASS(zCArchiverBinSafe, zCArchiver, 0, 0);
CLASS(zCArchiverGeneric, zCArchiver, 0, 0);

ABSTRACT(zCResource, zCObject, 0, 0);
ABSTRACT(zCSoundFX, zCResource, zCLASS_FLAG_RESOURCE|zCLASS_FLAG_SHARED_OBJECTS, 0);

CLASS(zCMaterial, zCObject, zCLASS_FLAG_SHARED_OBJECTS, 9);
CLASS(zCLightMap, zCObject, 0, 0);
CLASS(zCVertexBuffer, zCObject, 0, 0);

ABSTRACT(zCTexture, zCResource, zCLASS_FLAG_RESOURCE|zCLASS_FLAG_SHARED_OBJECTS, 0);
ABSTRACT(zCTextureFileFormat. zCObject, 0, 0);
CLASS(zCTextureFileFormatTGA, zCTextureFileFormat, 0, 0);
CLASS(zCTextureFileFormatInternal, zCTextureFileFormat, 0, 1);

ABSTRACT(zCVisual, zCObject, 0, 0)
ABSTRACT(zCVisualAnimate, zCVisual, 0, 0)
CLASS(zCDecal, zCVisual, 0, 1);
CLASS(zCParticleFX, zCVisual, 0, 0);
CLASS(zCMesh, zCVisual, 0, 1);
CLASS(zCMorphMesh, zCVisualAnimate, 0, 0);
CLASS(zCModel, zCVisualAnimate, 0, 0);
CLASS(zCModelAni, zCObject, 0, 0);
CLASS(zCModelMeshLib, zCObject, zCLASS_FLAG_SHARED_OBJECTS, 0);

CLASS(zCProgMeshProto, zCVisual, zCLASS_FLAG_SHARED_OBJECTS, 1);
CLASS(zCMeshSoftSkin, zCProgMeshProto, 0, 0);
CLASS(zCPolyStrip, zCVisual, 0, 0);
CLASS(zCQuadMark, zCVisual, zCLASS_FLAG_TRANSIENT, 0);

ABSTRACT(zCSkyControler, zCObject, 0, 0);
ABSTRACT(zCSkyControler_Mid, zCSkyControler, 0, 0);
CLASS(zCSkyControler_Indoor, zCSkyControler_Mid, 0, 0);
CLASS(zCSkyControler_Outdoor, zCSkyControler_Mid, 0, 1);

CLASS(zCViewObject, zCObject, 0, 0);
CLASS(zCViewDraw, zCViewObject, 0, 0);
CLASS(zCViewFX, zCViewDraw, 0, 0);
CLASS(zCViewPrint, zCViewFX, 0, 0);
CLASS(zCViewDialog, zCViewPrint, 0, 0);
CLASS(zCViewDialogChoice, zCViewDialog, 0, 0);
CLASS(oCViewDialogInventory, zCViewDialog, 0, 0);
CLASS(oCViewDialogItem, zCViewDialog, 0, 0);
CLASS(oCViewDialogItemContainer, zCViewDialog, 0, 0);
CLASS(oCViewDialogStealContainer, zCViewDialog, 0, 0);
CLASS(oCViewDialogTrade, zCViewDialog, 0, 0);
