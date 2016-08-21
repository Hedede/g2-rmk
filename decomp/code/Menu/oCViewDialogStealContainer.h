class oCViewDialogStealContainer : zCViewDialog {
	Z_OBJECT(oCViewDialogStealContainer);
public:
	oCItem* GetSelectedItem()
	{
		if (container)
			return container->GetSelectedItem();
		return nullptr;
	}

	int GetSelectedItemCount()
	{
		if (container)
			return container->GetSelectedItemCount();
		return 0;
	}

	int unk[2];
	oCItemContainer *container;
	char gmoom[12];
};
