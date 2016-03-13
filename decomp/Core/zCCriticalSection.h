struct zCCriticalSection
{
	zCCriticalSection() = default;
	virtual ~zCCriticalSection();
	LPCRITICAL_SECTION critSect;
};
