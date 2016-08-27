struct zDATE {
	zDATE& operator=(time_t time)
	{
		tm* t = localtime(&time);

		sec = t->tm_sec;
		min = t->tm_min;
		hour = t->tm_hour;
		day = t->tm_mday;
		month = short(t->tm_mon) + 1;
		year = t->tm_year + 1900;

		return *this;
	}

	time_t ToTime_t() const
	{
		tm t;
		t.tm_sec = sec;
		t.tm_min = min;
		t.tm_hour = hour;
		t.tm_mday = day;
		t.tm_mon = month - 1;
		t.tm_year = year - 1900;
		return mktime(&t);
	}

	int year;
	short month;
	short day;
	short hour;
	short min;
	short sec;
	short dunno;
};

