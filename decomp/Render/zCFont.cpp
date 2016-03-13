class zCFont {
public:
	zSTRING GetFontName()
	{
		return fontName;
	}

	zCTexture* GetFontTexture()
	{
		return fontTexture;
	}

	int GetFontX(zSTRING const& str);
	int GetFontY()
	{
		return this->Height;
	}
	int GetLetterDistance()
	{
		return 1;
	}
	int GetWidth(char Char)
	{
		return width[Char];
	}
private:
	zSTRING fontName;
	int height;
	zCTexture* fontTexture;
	char width[256];
	zVEC2 [256];
	zVEC2[256];
};

int zCFont::GetFontX(zSTRING const& str)
{
  int result = 0;
  for (int i = 0; str[i]; ++i) {
	  result += width[str[i]];
  }
  return result;
}

// not sure about names
int zCFont::GetFontData(uint8_t chId, int& width, zVEC2& min, zVEC2& max)
{
	if (chId > 256)
		return 0;

	width = this->width[chId];
	min.x = this->min[chId].x;
	min.y = this->min[chId].y;
	max.x = this->max[chId].x;
	max.y = this->max[chId].y;
	return 1;
}
