#ifndef Gothic_Npc_H
#define Gothic_Npc_H

#include <Gothic/Game/zVob.h>
#include <Gothic/Types/zSTRING.h>
#include <Gothic/Types/zArray.h>


class oCVisualFX;
class oCItemContainer;
class zCWaypoint;

struct TNpcSlot
{
	zSTRING name;
	int inInv;
	int __someNum;
	zSTRING nume;
	zCVob *object;
	int tree;
};

struct oTRobustTrace
{
	int bitfield;
	zVEC3 targetPos;
	zCVob *targetVob;
	zCVob *obstVob;
	int targetDist;
	int lastTargetDist;
	int maxTargetDist;
	int dirTurn;
	int timer;
	zVEC3 dirFirst;
	int dirLastAngle;
	int **lastDirections_array;
	int lastDirections_numAlloc;
	int lastDirections_numInArray;
	int frameCtr;
	zVEC3 targetPosArray[5];
	int targetPosCounter;
	int targetPosIndex;
	int checkVisibilityTime;
	int positionUpdateTime;
	float failurePossibility;
};

struct oCMagFrontier
{
	oCVisualFX *warningFX;
	oCVisualFX *shootFX;
	oCNpc *npc;
	int bitfield;
};

class oCNpc;
class oCItem;
class oCRtnEntry;
class zCRoute;
struct oCNpc_States
{
	struct TNpcAIState
	{
		int index;
		int loop;
		int end;
		int timeBehaviour;
		int restTime;
		int phase;
		int valid;
		zSTRING name;
		int stateTime;
		int prgIndex;
		int isRtnState;
	};

	void* _vtbl;
	zSTRING name;
	oCNpc *npc;
	TNpcAIState curState;
	TNpcAIState nextState;
	int lastAIState;
	int hasRoutine;
	int rtnChanged;
	oCRtnEntry *rtnBefore;
	oCRtnEntry *rtnNow;
	zCRoute *rtnRoute;
	int rtnOverlay;
	int rtnOverlayCount;
	int walkmode_routine;
	int weaponmode_routine;
	int startNewRoutine;
	int aiStateDriven;
	zVEC3 aiStatePosition;
	oCNpc *parOther;
	oCNpc *parVictim;
	oCItem *parItem;
	int rntChangeCount;
};


struct oCNpc : oCVob {
	static oCNpc*& player;

	/*TNpcSlot* GetInvSlot(std::string const& slotName)
	{
		Thiscall<TNpcSlot*(oCNpc*, const zSTRING&)>(0x749AE0);
	}*/
	
	int idx;
	zSTRING name[5];
	zSTRING slot;
	zSTRING effect;
	int npcType;
	int variousFlags;
	int attribute[8];
	int hitChance[5];
	int protection[8];
	int damage[8];
	int damagetype;
	int guild;
	int level;
	int mission[5];
	int fighttactic;
	int fmode;
	int voice;
	int voicePitch;
	int mass;
	int daily_routine;
	int startAIState;
	zSTRING spawnPoint;
	int spawnDelay;
	int senses;
	int senses_range;
	int aiscripts[100];
	zSTRING wpname;
	int experience_points;
	int experience_points_next_level;
	int learn_points;
	int bodyStateInterruptableOverride;
	int noFocus;
	int parserEnd;
	int bloodEnabled;
	int bloodDistance;
	int bloodAmount;
	int bloodFlow;
	zSTRING bloodEmitter;
	zSTRING bloodTexture;
	int didHit;
	int didParade;
	int didShoot;
	int hasLockedEnemy;
	int isDefending;
	int wasAiming;
	int lastAction;
	oCNpc *enemy;
	float speedTurn;
	int foundFleePoint;
	int reachedFleePoint;
	zVEC3 vecFlee;
	zVEC3 posFlee;
	zCWaypoint *waypointFlee;
	oTRobustTrace rbt;
	void *timedOverlays_data;
	void **timedOverlays_next;
	zCArray<void*> talents;
	int spellMana;

	oCMagFrontier magFrontier;

	oCNpc_States states;

	int inventory[0xCC/4];
	oCItemContainer *trader;
	oCNpc *tradeNpc;

	int rangeToPlayer;

	zCArray<int> listOfVoiceHandles;
	int voiceIndex;

	zCArray<oCVisualFX*> effectList;

	struct {
		uint16_t showaidebug      : 1;
		uint16_t showNews         : 1;
		uint16_t csAllowedAsRole  : 1;
		uint16_t isSummoned       : 1; // 8
		uint16_t respawnOn        : 1; // 0x10
		uint16_t movlock          : 1; // 0x20
		uint16_t drunk            : 1; // 0x40
		uint16_t mad              : 1; // 0x80
		uint16_t overlay_wounded  : 1; // 0x100
		uint16_t inOnDamage       : 1; // 0x200
		uint16_t autoremoveweapon : 1; // 0x400
		uint16_t openinventory    : 1; // 0x800
		uint16_t askroutine       : 1; // 0x1000
		uint16_t spawnInRange     : 1; // 0x2000
	} flags;

	uint16_t body_TexVarNr;
	uint16_t body_TexColorNr;
	uint16_t head_TexVarNr;
	uint16_t teeth_TexVarNr;
	uint8_t guildTrue;
	uint8_t drunk_heal;
	int junk[146];
	zCArray<TNpcSlot*> invSlots;
};

#endif//Gothic_Npc_H
