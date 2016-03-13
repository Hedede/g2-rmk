class zCPolyStrip {
 virtual void _GetClassDef();
 virtual void ~zCPolyStrip(uint);
 virtual void Render(zTRenderContext	&);
 virtual void IsBBox3DLocal();
 virtual void GetBBox3D();
 virtual void GetVisualName();
 virtual void SetVisualUsedBy(zCVob *);
 virtual void FreeResources();
};
