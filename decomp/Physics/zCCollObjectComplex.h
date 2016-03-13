class zCCollObjectComplex {
 virtual void GetCollObjClass();
 virtual void ~zCCollObjectComplex(uint);
 virtual void CollisionResponse(zCArray<zCCollisionReport *> const &,int &);
 virtual void GetAdaptiveStepSize();
};
