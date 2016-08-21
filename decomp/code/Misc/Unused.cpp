void D_Reset() { }
void D_Print(zSTRING &,TDebug) {}
void D_Print(char *, TDebug) { }
void D_Print(char *,int,int) {}
void D_PrintA(char *,int,int) {}
void D_Print(float const &,zSTRING const &,TDebug) {}
void D_ClrWin() { }
void D_Print(zVEC3 const &,zSTRING const &,TDebug) {}
void D_DrawPolyWire(zCPolygon const &,zCOLOR const &) {}
void D_Print3D(zVEC3 const &,zSTRING &,zCOLOR const &) {}

void winCreateMenu() { }

int RegisterBlts()
{
	return 0;
}

void zLocal_Startup() { }
void zLocal_Shutdown() { }
void zLocal_PerFrame() { }
void Gfx_ChangeBPP() { }
