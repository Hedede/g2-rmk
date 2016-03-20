//Ein ganz gewöhnliches Vob (wird als Hilfsvob verwendet)
class zCVobWaypoint : public zCVob {
	Z_OBJECT(zCVobWaypoint);
public:
	zCVobWaypoint()
	{
		type = VOB_TYPE_WAYPOINT;
	}

	virtual ~zCVobWaypoint() = default;
};
