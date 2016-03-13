int IntToString()
{
	static zSTRING result;
	zCParser* parser = zCParser::GetParser();

	int value;
	parser.GetParameter(&value);
	result = zSTRING(value);
	parser.SetReturn(&result);
	return 0;
}

int FloatToString()
{
	static zSTRING result;
	zCParser* parser = zCParser::GetParser();

	float value;
	parser.GetParameter(&value);
	result = zSTRING(value);
	parser.SetReturn(&result);
	return 0;
}

int FloatToInt()
{
	float value;
	parser.GetParameter(&value);
	parser.SetReturn(int(value));
	return 0;
}

int IntToFloat()
{
	int value;
	parser.GetParameter(&value);
	parser.SetReturn(float(value));
	return 0;
}

int ConcatStrings()
{
	static zSTRING text1;
	zCParser* parser = zCParser::GetParser();

	zSTRING text2;
	parser.GetParameter(&text2);
	parser.GetParameter(&text1);
	text1 += text2;
	parser.SetReturn(&text1);
	return 0;
}

