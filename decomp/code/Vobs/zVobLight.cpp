void zCVobLightData::Archive(zCArchiver& arc)
{
	arc.WriteEnum("lightType", "POINT;SPOT;_RES_;_RES_", flags.lightType);

	arc.WriteFloat("range", rangeBackup);
	arc.WriteColor("color", lightColor);
	arc.WriteFloat("spotConeAngle", spotConeAngleDeg);
	arc.WriteBool("lightStatic", flags.lightStatic);
	arc.WriteEnum("lightQuality", "HIGH;MEDIUM;LOW_FASTEST", flags.lightQuality);

	zSTRING lensflare = lensFlareFX ? lensFlareFX->GetObjectName() : "";
	arc.WriteString("lensflareFX", lensflareFX);

	if ( arc.InProperties() || !flags.lightStatic ) {
		arc.WriteGroupBegin("Dynamic Light");
		arc.WriteBool("turnedOn", flags.turnedOn);

		zSTRING rangeAniScale;
		int num = rangeAniScaleList.GetNumInList();
		for (int i = 0; i < num; ++i) {
			rangeAniScale += rangeAniScaleList[i] + " "_s;
		}

		arc.WriteString("rangeAniScale", rangeAniScale);
		arc.WriteFloat("rangeAniFPS", rangeAniFPS * 1000.0);
		arc.WriteBool("rangeAniSmooth", flags.rangeAniSmooth);

		zSTRING colorList;
		num = colorAniList.GetNumInList();
		for (int i = 0; i < num; ++i) {
			auto color = colorAniList[i];
			if (color.Red() != color.Green() ||
			    color.Red() != color.Blue())
			{
				colorList += "(" + color.GetDescriptionRGB() + ") ";
			} else {
				colorList += color.Red() + " "_s;
			}
		}

		arc.WriteString("colorAniList", colorList);
		arc.WriteFloat("colorAniFPS", colorAniFPS * 1000.0);
		arc.WriteBool("colorAniSmooth", flags.colorAniSmooth);
		arc.WriteBool("canMove", flags.canMove);
		arc.WriteGroupEnd("Dynamic Light");
	}
}

void zCVobLightData::Unarchive(zCArchiver& arc)
{
	flags.lightType = arc.ReadEnum("lightType");
	range = arc.ReadFloat("range");
	if (range < 0.0)
		range = 0.0;

	if (range == 0.0)
		rangeInv = std::numeric_limits<float>::max
	else
		rangeInv = 1.0 / range;

	rangeBackup = range;

	lightColor = arc.ReadColor("color");
	
	spotConeAngleDeg = arc.ReadFloat("spotConeAngle");
	flags.lightStatic = arc.ReadBool("lightStatic");
	flags.lightQuality = arc.ReadEnum("lightQuality");

	zSTRING flareFxName = arc.ReadString("lensflareFX");
	if (flareFxName.Length() > 0) {
		flareFxName.Upper();
		lensflareFX = zCLensFlareFX::classDef.SearchHashTable(flareFxName);
		lensFlateFXNo = zCLensFlareFX::classDef.objectList.Search(flareFxName);
	}

	if (flags.lightStatic && !arc.InProperties())
		break;

	flags.turnedOn = arc.ReadBool("turnedOn");
	zSTRING rangeAni = arc.ReadString("rangeAniScale");
	rangeAniScaleList.Clear();

	if (!rangeAni.IsEmpty()) {
		int i = -1;
		zSTRING word;
		do {
			word = rangeAni.PickWord(i + 2, " ", " ");
			++i;
		} while ( word.Length() > 0);

		if (i > 0) {
			rangeAniScaleList.AllocAbs(i);
			for (int j = 0; j < i; ++j) {
				word = rangeAni.PickWord(j, " ", " ");
				// In original: BUG! only first entry processed
				// if !IsEmppty -> break было
				if (word.IsEmpty())
					break;
				rangeAniScaleList.InsertEnd(word.ToFloat());
			}
		}
	}

	rangeAniFPS = arc.ReadFloat("rangeAniFPS") * 0.001;
	rangeAniActFrame = 0;

	flags.rangeAniSmooth = arc.ReadBool("rangeAniSmooth");

	zSTRING colorList = arc.ReadString("colorAniList");
	colorAniList.Clear();
	if (!colorList.IsEmpty()) {
		zCOLOR color;
		zSTRING word;
		for (int i = 1; ; ++i) {
			word = colorList.PickWord(i, " ", " ");
			if (word.IsEmpty)
				break;
	
			if ( word.Search(0, "(", 1u) == -1 ) {
				color[0] = word.ToLong();
				color[1] = color[0];
				color[2] = color[0];
			} else {
				word.Delete("(", 3);
				color[2] = word.ToLong();

				word = colorList.PickWord(++i, " ", " ");
				color[1] = word.ToLong();

				word = colorList.PickWord(++i; ") ", ") ");
				color[0] = word.ToLong();
			}

			colorAniList.InsertEnd(color);
		}
	}

	colorAniActFrame = 0;
	colorAniFPS = arc.ReadFloat("colorAniFPS"); * 0.001;

	flags.colorAniSmooth = arc.ReadBool("colorAniSmooth");
	flags.canMove = arc.ReadBool("canMove");
}

