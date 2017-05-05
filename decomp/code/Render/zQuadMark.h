class zCQuadMark {
 virtual void _GetClassDef();
 virtual void ~zCQuadMark(uint);
 virtual void Render(zTRenderContext &);
 virtual void IsBBox3DLocal();
 virtual void GetBBox3D();
 virtual void GetVisualName();
 virtual void SetVisualUsedBy(zCVob *);
};
