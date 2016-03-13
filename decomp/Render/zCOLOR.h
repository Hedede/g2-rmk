class zCOLOR {
	zCOLOR(uint32_t color)
	{
		blue  =  color        & 0xFF;
		green = (color >> 8)  & 0xFF;
		red   = (color >> 16) & 0xFF;
		alpha = (color >> 24) & 0xFF;
	}

	zCOLOR(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
		: red(r), green(g), blue(b), alpha(a)
	{
	}
	
	SetRGB(uint8_t r, uint8_t g, uint8_t b)
	{
		red = r;
		green = g;
		blue = b;
	}

	Reset()
	{
		blue = 0;
		green = 0;
		red = 0;
		alpha = -1;
	}

	GetDescription()
	{
		zSTRING str = zSTRING(red)   + " " +
		              zSTRING(green) + " " +
		              zSTRING(blue)  + " " +
		              zSTRING(alpha);
		return str;
	}

	GetDescriptionRGB()
	{
		zSTRING str = zSTRING(red)   + " " +
		              zSTRING(green) + " " +
		              zSTRING(blue);
		return str;
	}

	void SetByDescription(zSTRING const& desc)
	{
		red   = desc.PickWord_Old(1, "").ToInt();
		green = desc.PickWord_Old(2, "").ToInt();
		blue  = desc.PickWord_Old(3, "").ToInt();
		alpha = desc.PickWord_Old(4, "").ToInt();
	}

	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t alpha;
};

