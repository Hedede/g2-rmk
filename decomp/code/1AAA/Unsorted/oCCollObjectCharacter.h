class oCCollObjectCharacter {
 virtual void GetCollObjClass();
 virtual void ~oCCollObjectCharacter(uint);
 virtual void GetPrimitiveCC();
 virtual void IsDead();
 virtual void DontLetHimComeNearer(zCVob const *);
 virtual void TurnSpeed();
	virtual void PushAround(zVEC3	const &);
};
