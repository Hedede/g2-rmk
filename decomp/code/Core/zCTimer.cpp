class zCTimer {
	static unsigned s_forcedMaxFrameTime;
public:
	zCTimer()
		: forcedMaxFrameTime{s_forcedMaxFrameTime}
	{
	}

	static void S_SetMaxFPS(unsigned fps)
	{
		s_forcedMaxFrameTime = fps;
	}

	static unsigned S_GetMaxFPS()
	{
		return s_forcedMaxFrameTime;
	}

	void SetMaxFPS(unsigned fps)
	{
		forcedMaxFrameTime = fps;
	}

	void SetMotionFactor(float factor)
	{
		factorMotion = factor;
	}

	void SetFrameTime(float timeMsec)
	{
		unsigned sysTime = sysGetTime();

		lastTimer = sysTime;
		frameTimeFloatSecs = timeMsec * 0.001;
		frameTimeFloat = timeMsec;

		totalTimeFloat += timeMsec;
		totalTimeFloatSecs = totalTimeFloat * 0.001;

		totalTime += double(frameTime) * factorMotion;
	}

	void FrameUpdate()
	{
		if ( smooth_timer ) {
			unsigned sysTime = sysGetTime();

			avg_time = (sysTime + 2 * avg_time + avg_time) / 4;

			unsigned max_time = avg_time;
			if ( sysTime > avg_time ) {
				max_time = sysTime;
				avg_time = sysTime;
			}

			if ( sysTime < max_time - 400 )
				avg_time = sysTime - 400;

			if ( ++force_update_timeout < 10 ) {
				if ( force_update_timeout < 3 )
					force_update_timeout = 3;
			} else {
				force_update_timeout = 10;
			}
		} else {
			avg_time = sysGetTime();
		}
	}

	void LimitFPS(int fps)
	{
		smooth_timer = zoptions->ReadBool("ENGINE", "zSmoothTimer", 1);

		if ( fps <= 0 )
			minFrameTime = 0;
		else
			minFrameTime = 8000 / fps;
	}

	void ResetTimer()
	{
		unsigned approxMax = approxMaxFrameTime;

		if ( approxMaxFrameTime < minFrameTime ) {
			approxMax = minFrameTime;
			approxMaxFrameTime = minFrameTime;
		}

		if ( approxMax > 1 )
			approxMaxFrameTime = approxMax - 1;

		unsigned timeDiff = 8 * (sysGetTime() - lastTimer);

		if ( approxMaxFrameTime <= timeDiff )
			approxMaxFrameTime = timeDiff;
		else
			Sleep((approxMaxFrameTime - timeDiff) / 8);

		if ( approxMaxFrameTime > forcedMaxFrameTime )
			approxMaxFrameTime = forcedMaxFrameTime;

		if ( force_update_timeout > 10 )
			force_update_timeout = 10;

		--force_update_timeout;

		if (force_update_timeout < 0) {
			FrameUpdate();
			force_update_timeout = 0;
		}

		frameTime = (avg_time - lastTimer) * factorMotion;
		if ( frameTime > 100 )
			frameTime = 100;

		totalTime += frameTime;
		lastTimer = avg_time;
		frameTimeFloat = frameTime;
		frameTimeFloatSecs = 0.001;

		totalTimeFloat += frameTimeFloat;
		totalTimeFloatSecs = totalTimeFloatSecs * 0.001;
	}


private:
	//nicht zu klein machen. Sonst: Freeze bei hoher Framerate!
	float factorMotion = 1.0;
	//Zeit der zwischen diesem und dem letzten Frame verstrichen ist
	float frameTimeFloat = 1.0;      // [msec]
	//gesamte Zeit
	float totalTimeFloat = 0.0;      // [msec]
	float frameTimeFloatSecs;
	float totalTimeFloatSecs;
	int lastTimer = 0;
	//nochmal als Ganzahl
	int frameTime = 1; // [msec]
	int totalTime;     // [msec]
	//antifreeze. Sonst wird die Framezeit auf 0 gerundet und nichts bewegt sich
	int minFrameTime = 0;

	//länger als das darf ein Frame (in Spielzeit) nicht dauern.
	//Um zu große Zeitsprünge für die Objekte zu vermeiden?
	//Jedenfalls sort dies dafür, dass das Spiel langsamer läuft,
	//wenn das Spiel mit rendern nicht hinterherkommt.
	int forcedMaxFrameTime;
};
