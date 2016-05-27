class zCZoneMusic : public zCZone {
	Z_OBJECT(zCZoneMusic);
public:
	virtual ~zCZoneMusic() = default;
private:
	zCWorld    *world; //einzige neue Eigenschaft
};
