class zCVobStartpoint : public zCVob {
	Z_OBJECT(zCVobStartpoint);
public:
	zCVobStartpoint()
	{
		type = VOB_TYPE_STARTPOINT;
	}

	virtual ~zCVobStartpoint() = default;
};
