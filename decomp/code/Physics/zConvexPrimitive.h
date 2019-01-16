class zCConvexPrimitive {
public:
	virtual void ~zCConvexPrimitive();
	virtual void PointNearestToPlaneLocal(zVEC3 const &,zVEC3 &) = 0;
	virtual void GetCenterLocal(zVEC3	&);
	virtual void PointIsInLocal(zVEC3 const &) = 0;
	virtual void SymetrySphere();
	virtual void SymetryRotation(zVEC3 &);
	virtual void UpdateBBox();
	virtual void DrawVirtual();
};

class zCConvexPrimitiveScaleTrans : public zCConvexPrimitive{
public:
	void ~zCConvexPrimitiveScaleTrans() override;
	void PointNearestToPlaneLocal(zVEC3 const &,zVEC3 &) override;
	void GetCenterLocal(zVEC3 &) override;
	void PointIsInLocal(zVEC3 const &) override;
	void UpdateBBox() override;
	void DrawVirtual() override;
};

class zCConvexPrimitiveEllipsoid : public zCConvexPrimitiveScaleTrans {
public:
	void ~zCConvexPrimitiveEllipsoid(uint) override;
	void SymetryRotation(zVEC3 &) override;
};

class zCConvexPrimitiveUnitSphere : public zCConvexPrimitive {
public:
	void ~zCConvexPrimitiveUnitSphere() override;
	void PointNearestToPlaneLocal(zVEC3 const &,zVEC3 &) override;
	void PointIsInLocal(zVEC3 const &) override;
	void SymetrySphere() override;
	void SymetryRotation(zVEC3 &) override;
	void UpdateBBox() override;
	void DrawVirtual() override;
};
