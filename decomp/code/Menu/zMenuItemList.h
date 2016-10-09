struct zCMenuItemList {
	~zCMenuItemList() override;
	void HandleEvent(int) override;

	void InitMenuItem() override {}
	void Run() override;

	void Enter() override {}
	void Leave() override {}

	virtual void DrawFront();
	virtual void Show();
	virtual void Hide();
	virtual void InsertInWin(zCView *);
	virtual void RemoveFromWin();
	void UpdateContent() override {}

private:
	zCArray<zTMenuItemListElement> elements;
	int curEntry;
	int unk[4];
};
