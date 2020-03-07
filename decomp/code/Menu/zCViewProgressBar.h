struct zTProgressRange {
	int id;
	float min, max;
};

class zCViewProgressBar : public zCView {
public:
	virtual void ~zCViewProgressBar();
	virtual void Init();
	virtual void HandleChange();
	virtual void Draw();

	void SetRange(int _low, int _high);

private:
	zCList<zTProgressRange> ranges;

	static double CalcPercentOf(float perc, float min, float max)
	{
		return (max - min) * perc * 0.001 + min;
	}
};

void zCViewProgressBar::SetRange(int min, int max)
{
	float mina = min;
	float maxa = max;

	for (auto range : ranges) {
		mina = CalcPercentOf(mina, range.min, range.max);
		maxa = CalcPercentOf(maxa, range.min, range.max);
	}

	auto idx = ranges.GetNumInlist();
	auto range = new zTProgressRange{idx, mina, maxa};
	ranges.InsertFront(range);
}

void zCViewProgressBar::ResetRange()
{
	auto idx  = ranges.GetNumInlist();
	auto last = ranges.Get(idx - 1);
	ranges.Remove(last);
}
