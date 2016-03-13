class zCParticleFX {
 virtual void _GetClassDef();
	virtual void ~zCParticleFX(uint);
 virtual void Render(zTRenderContext &);
	virtual void IsBBox3DLocal();
	virtual void GetBBox3D();
 virtual void GetVisualName();
	virtual void GetVisualDied();
 virtual void SetVisualUsedBy(zCVob	*);
 virtual void GetRenderSortKey();
	virtual void HostVobRemovedFromWorld(zCVob *,zCWorld *);
 virtual void HostVobAddedToWorld(zCVob *,zCWorld *);
 virtual void GetFileExtension(int);
 virtual void LoadVisualVirtual(zSTRING const &);
};
