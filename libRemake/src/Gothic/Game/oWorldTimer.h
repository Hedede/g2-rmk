#ifndef Gothic_oWorldTimer_H
#define gothic_oworldtimer_h

struct oCWorldTimer {
	static constexpr double TicksPerHour  = 250000.0;
	static constexpr double TicksPerDay   = TicksPerHour * 24;
	static constexpr double TicksPerMin   = TicksPerHour / 60;
	static constexpr double TicksPerSec   = TicksPerMin  / 60;

	void Timer()
	{
		Thiscall<void(oCWorldTimer *)> call{0x780D80};
		call(this);
	}

	double GetSkyTime()
	{
		double t = worldTime / TicksPerDay + 0.5;
		while (t > 1.0)
			--t;
		return t;
	}

	float worldTime;
	int day;
};

#endif//Gothic_oWorldTimer_H
