struct zCSndChannel {
	zCSndFrame() = default;
	~zCSndChannel()
	{
		for (auto frame : frames)
			Delete(frame); // was inined
	}

	int actFrame = -1;
	int refCtr = 1;
	int __loaded = 0;
	zCArray<zCSndFrame *> frames;
};
