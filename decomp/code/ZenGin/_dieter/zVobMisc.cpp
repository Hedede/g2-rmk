#include <zEventScreenFX.h>

// TODO: macro description
zOBJECT_CLASSDEF(zCEventScreenFX); // g1alpha: 134

zCEventScreenFX::zCEventScreenFX(zTEventScreenFXSubType sub, float duration, zCOLOR color, float fovDeg)
	: zCEventMessage(sub),
	  fovDeg(fovDeg),
	  color(color),
{
	  this->duration = duration;
}
