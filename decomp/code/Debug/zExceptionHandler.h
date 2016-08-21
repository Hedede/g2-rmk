struct zCExceptionHandler {
	zBOOL isActive;

	void zCExceptionHandler::SetIsActive(int b)
	{
		if ( b ) {
			SetCrashHandlerFilter(UnhandledExceptionFilter);
			isActive = b;
		} else {
			SetCrashHandlerFilter(0);
			isActive = 0;
		}
	}
};

zBOOL& zExHandler = *reinterpret_cast<zBOOL*>(0x89D9F4);
