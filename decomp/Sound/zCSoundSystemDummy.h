class zCSoundSystemDummy {
 virtual void ~zCSoundSystemDummy(uint);
 virtual void LoadSoundFX(zSTRING const &);
 virtual void PlaySound(zCSoundFX *,int,int,float,float);
 virtual void PlaySound(zCSoundFX *,int);
 virtual void PlaySound3D(zSTRING const &,zCVob *,int,zCSoundSystem::zTSound3DParams	*);
 virtual void PlaySound3D(zCSoundFX *,zCVob *,int,zCSoundSystem::zTSound3DParams *);
 virtual void GetSound3DProps(int const &,zCSoundSystem::zTSound3DParams &);
 virtual void UpdateSound3D(int const &,zCSoundSystem::zTSound3DParams	*);
 virtual void DoSoundUpdate();
 virtual void SetGlobalOcclusion(float);
};
