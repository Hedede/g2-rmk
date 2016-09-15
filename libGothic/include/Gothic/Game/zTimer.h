#pragma once
class zCTimer {
	static unsigned& S_ForcedMaxFrameTime()
	{
		static unsigned& s_forcedMaxFrameTime = Value<unsigned>(0x99B404);
		return s_forcedMaxFrameTime;
	}

public:
	static void S_SetMaxFPS(unsigned fps)
	{
		S_ForcedMaxFrameTime() = fps;
	}

	static unsigned S_GetMaxFPS()
	{
		return S_ForcedMaxFrameTime();
	}

	void SetMaxFPS(unsigned fps)
	{
		forcedMaxFrameTime = fps;
	}

	void LimitFPS(int fps)
	{
		auto smooth_timer = zoptions->ReadBool("ENGINE", "zSmoothTimer", 1);

		if ( fps <= 0 )
			minFrameTime = 0;
		else
			minFrameTime = 8000 / fps;
	}


private:
	float factorMotion = 1.0;
	float frameTimeFloat = 1.0;      // [msec]
	float totalTimeFloat = 0.0;      // [msec]
	float frameTimeFloatSecs;
	float totalTimeFloatSecs;
	int lastTimer = 0;
	int frameTime = 1; // [msec]
	int totalTime;     // [msec]
	int minFrameTime = 0;

	int forcedMaxFrameTime;
};

auto& ztimer = Value<zCTimer>(0x99B3D4);
