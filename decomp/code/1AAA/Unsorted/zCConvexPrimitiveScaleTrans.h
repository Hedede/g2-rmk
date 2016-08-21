class zCConvexPrimitiveScaleTrans {
 virtual void ~zCConvexPrimitiveScaleTrans(uint);
 virtual void PointNearestToPlaneLocal(zVEC3 const &,zVEC3 &);
 virtual void GetCenterLocal(zVEC3 &);
 virtual void PointIsInLocal(zVEC3 const &);
 virtual void UpdateBBox();
 virtual void DrawVirtual();
};
