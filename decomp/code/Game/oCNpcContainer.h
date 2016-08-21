class oCNpcContainer {
public:
	virtual void HandleEvent(int);
	virtual void ~oCNpcContainer(uint);
	virtual void Insert(oCItem	*);
	virtual void Remove(oCItem *);
	virtual void CreateList();
};
