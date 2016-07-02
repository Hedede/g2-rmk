struct zDATE {
	zDATE& operator=(time_t time)
	{
		tm* t = localtime(&time);

		sec = t->tm_sec;
		min = t->tm_min;
		hour = t->tm_hour;
		day = t->tm_mday;
		month = LOWORD(t->tm_mon) + 1;
		year = t->tm_year + 1900;

		return *this;
	}

	int year;
	short month;
	short day;
	short hour;
	short min;
	short sec;
	short dunno;
};

