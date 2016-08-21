class zCCollObjectPoint {
 virtual void GetCollObjClass();
 virtual void ~zCCollObjectPoint(uint);
 virtual void DetectCollisionsSelf(zCArray<zCCollisionObject	*> const &,zCArray<zCCollisionReport *>	&);
 virtual void CollisionResponse(zCArray<zCCollisionReport	*> const &,int &);
 virtual void GetLargestBBox3DLocal(zTBBox3D &);
};
