class zCConvexPrimitive {
	virtual void ~zCConvexPrimitive(uint);
	virtual void GetCenterLocal(zVEC3	&);
 virtual void SymetrySphere();
 virtual void SymetryRotation(zVEC3 &);
 virtual void UpdateBBox();
 virtual void DrawVirtual();
};
