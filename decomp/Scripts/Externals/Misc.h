int SetPercentDone(void *this)
{
	zCParser* parser = zCParser::GetParser();
	int percent;
	parser->GetParameter(percent);
	parser->SetPercentDone(percent);
	return 1;
}
