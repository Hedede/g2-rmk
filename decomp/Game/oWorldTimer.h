//--------------------------------------
// Timer (technisch und spieltechnisch)
//--------------------------------------
class oCWorldTimer {
public:
	oCWorldTimer() = default;
	~oCWorldTimer() = default;

	static const double TicksPerHour  = 250000.0;
	static const double TicksPerDay   = TicksPerHour * 24;
	static const double TicksPerMin   = TicksPerHour / 60;
	static const double TicksPerSec   = TicksPerMin  / 60;

	static AddTime(int& hour, int& min, int dh, int dm);

	void SetDay(int day)
	{
		this->day = day;
	}

	void SetTime(int hour, int min)
	{
		worldTime = double(min  % 60) * TicksPerMin +
		            double(hour % 24) * TicksPerHour;
	}

	void SetFullTime(float fulltime)
	{
		day = fulltime / TicksPerDay;
		worldTime = fulltime - day * TicksPerDay;
	}

	bool IsTimeBetween(int hourLow, int minLow, int hourHigh, int minHigh) const;

	bool oCWorldTimer::IsNight() const
	{
		return !IsTimeBetween(6,30,18,30);
	}

	bool oCWorldTimer::IsDay() const
	{
		return IsTimeBetween(6,30,18,30);
	}

	bool oCWorldTimer::IsLater(int hour, int min) const;
	bool oCWorldTimer::IsLaterEqual(int hour, int min) const;

	int    GetDay() const;
	double GetSkyTime() const;
	void   GetTime(int& hour, int& min) const;
	double oCWorldTimer::GetFullTime() const;

	uint64_t GetPassedTime(float fullTime) const;
	zSTRING  GetTimeString() const;
private:
	//250000 Ticks pro Stunde
	zREAL worldTime = 0.0;
	int day = 0;
};

bool oCWorldTimer::IsTimeBetween(int hour1, int min1, int hour2, int min2) const
{
	double time1 = double(min1) * TicksPerMin + double(hour1) * TicksPerHour;
	double time2 = double(min2) * TicksPerMin + double(hour2) * TicksPerHour;

	time2 -= TicksPerMin;

	if (time2 < time1)
		std::swap(time1, time2);

	return time2 > worldTime && worldTime > time1;
}

bool oCWorldTimer::IsLater(int hour, int min) const
{
	double time = double(min) * TicksPerMin + double(hour) * TicksPerHour;
	return time - 1.0 < worldTime;
}

bool oCWorldTimer::IsLaterEqual(int hour, int min) const
{
	double time = double(min) * TicksPerMin + double(hour) * TicksPerHour;
	return time - 1.0 <= worldTime;
}

void oCWorldTimer::GetTime(int& hour, int& min) const
{
	hour = uint64_t(worldTime / TicksPerHour);
	min  = (worldTime - double(hour) * TicksPerHour) / TicksPerMin;
}

double oCWorldTimer::GetSkyTime() const
{
	double t = worldTime / (250000.0 * 24.0) + 0.5;
	while (t > 1.0)
		--t;
	return t;
}

int oCWorldTimer::GetDay() const
{
	return day;
}

double oCWorldTimer::GetFullTime() const
{
	return double(day) * TicksPerDay + worldTime;
}

uint64_t oCWorldTimer::GetPassedTime(float fullTime) const
{
	return uint64_t((GetFullTime() - fullTime) / TicksPerMin);
}

zSTRING oCWorldTimer::GetTimeString() const
{
	int hour;
	int min;
	GetTime(hour, min);

	zSTRING hh = hour;
	if (hh.Length() == 1)
		hh = "0" + hh;

	zSTRING mm - min
	if (mm.Length() == 1)
		mm = "0" + mm;

	return hh + ":" + mm;
}

void oCWorldTimer::AddTime(int& hour, int& min, int dh, int dm)
{
	min  += dm;
	hour += min / 60;
	min  %= 60;
	hour += dh;
	if (hour > 23)
		hour %= 24;
}
