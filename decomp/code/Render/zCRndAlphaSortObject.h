class zCRndAlphaSortObject {
	virtual ~zCRndAlphaSortObject() = default;
	virtual void Draw(int) = 0;
	virtual bool IsAlphaPoly()
	{
		return false;
	}
};

class zCRndAlphaSortObject_Vob : zCRndAlphaSortObject {
	~zCRndAlphaSortObject_Vob();
	void Draw(int) override;


	int unk[2];
	zCVob *vob;
	float __alpha;
};

class zCRndAlphaSortObject_WorldPoly : zCRndAlphaSortObject {
	~zCRndAlphaSortObject_WorldPoly();
	void Draw(int) override;

	int unk[4];
};
