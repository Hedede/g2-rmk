class zCConvexPrimitiveUnitSphere {
 virtual void ~zCConvexPrimitiveUnitSphere(uint);
 virtual void PointNearestToPlaneLocal(zVEC3 const &,zVEC3 &);
 virtual void PointIsInLocal(zVEC3 const &);
 virtual void SymetrySphere();
 virtual void SymetryRotation(zVEC3 &);
 virtual void UpdateBBox();
 virtual void DrawVirtual();
};
