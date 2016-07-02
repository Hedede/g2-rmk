template<class Key, class Element>
class zTRayTurboValMap {
	struct zSNode {
		Key             key;
		Element         element;
		unsigned long   u32Hash;
		zSNode*         pNext;
	};

	zCArray<zSNode*> nodes;
};

zCVobLight::zCVobLight()
	: zCVob()
{
	v2 = &this->lightMap;
	v8 = v2;
	*(v2 + 8) = 0;
	*(v2 + 4) = 0;
	*v2 = 0;
	LOBYTE(v9) = 2;
	delete(0);
	*v2 = 0;
	this->lightMap.arr.numAlloc = 0;
	this->lightMap.arr.numInArray = 0;
	zCArray<zTRayTurboValMap<zCPolygon *,int>::zSNode *>::AllocDelta(&this->lightMap, 97);
	v3 = 97;
	do
	{
		v6 = 0;
		zCArray<zTRayTurboValMap<zCPolygon *,int>::zSNode *>::InsertEnd(&this->lightMap, &v6);
		--v3;
	}
	while ( v3 );
	LOBYTE(v9) = 3;
	this->lightPreset.data.field_0 = v5;
	std::string::_Tidy(&this->lightPreset.data, 0);
	this->lightPreset.vtable = &zSTRING::`vftable';
	LOBYTE(v9) = 4;

	this->type = VOB_TYPE_LIGHT;

	SetCollDetStat(0);
	SetCollDetDyn(0);

	LOBYTE(this->flags1) = this->flags1 & 0xFE ^ 1;
}
