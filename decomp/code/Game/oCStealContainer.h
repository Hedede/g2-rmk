class oCStealContainer : public oCItemContainer {
	oCStealContainer() = default;
	virtual ~oCStealContainer();

	void HandleEvent(int) override;

	virtual void SetOwner(oCNpc *);
	virtual void GetOwner();
	virtual void CreateList();

	void SetOwner(oCNpc* npc)
	{
		owner = npc;
		CreateList();
	}

	oCNpc* GetOwner()
	{
		return owner;
	}

private:
	oCNpc* owner = nullptr;
};
