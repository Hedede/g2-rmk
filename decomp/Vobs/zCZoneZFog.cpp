void zCZoneZFog::Archive(zCArchiver& arc)
{
	zCZone::Archive(arc);
	arc.WriteGroupBegin("ZoneZFog");
	arc.WriteFloat("fogRangeCenter", fogRangeCenter);
	arc.WriteFloat("innerRangePerc", innerRangePerc);
	arc.WriteColor("fogColor", fogColor);
	arc.WriteBool("fadeOutSky", fadeOutSky);
	arc.WriteBool("overrideColor", overrideColor);
	arc.WriteGroupEnd("ZoneZFog");
}

void zCZoneZFog::Unarchive(zCArchiver& arc)
{
	zCZone::Unarchive(arc);
	SetCollDetStat(0);
	SetCollDetDyn(0);
	flags.ignoredByTraceRay = 1;
	arc.ReadFloat("fogRangeCenter", fogRangeCenter);
	arc.ReadFloat("innerRangePerc", innerRangePerc);
	if ( innerRangePerc < -0.0 )
		innerRangePerc = 0.0;
	else if (innerRangePerc > 1.0)
		innerRangePerc = 1.0;

	arc.ReadColor("fogColor", fogColor);
	arc.ReadBool("fadeOutSky", fadeOutSky);
	arc.ReadBool("overrideColor", overrideColor);
}
