zCVobLight *__thiscall zCVobLight::zCVobLight(zCVobLight *this)
{
  int v2; // edi@1
  signed int v3; // ebp@1
  char v5; // [sp+13h] [bp-19h]@0
  int v6; // [sp+14h] [bp-18h]@2
  int v8; // [sp+1Ch] [bp-10h]@1
  int v9; // [sp+28h] [bp-4h]@1

  zCVob::zCVob(&this->vtbl);
  v9 = 0;
  zCVobLightData::zCVobLightData(&this->lightData);
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
  this->vtbl = &zCVobLight::`vftable';
  this->type = VOB_TYPE_LIGHT;
  zCVob::SetCollDetStat(&this->vtbl, 0);
  zCVob::SetCollDetDyn(&this->vtbl, 0);
  LOBYTE(this->flags1) = this->flags1 & 0xFE ^ 1;
  return this;
}
