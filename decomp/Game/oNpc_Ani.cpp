
void oCNpc::SetMovLock(int f)
{
	if (f != IsMovLock()) {
		flags.movlock = f;
/* removed in relese version
		if ( f )
			zINFO(4,"B: NPC: MovLock ON"); //4320 oNPC.cpp
		else
			zINFO(4,"B: NPC: MovLock OFF"); // 4321
*/
	}
}
