class oCViewDialogInventory : zCViewDialog {
	Z_OBJECT(oCViewDialogInventory);
public:
	void SetInventory(oCNpcInventory* inv)
	{
		inventory = inv;
		if ( inventory ) {
			inventory->DisableManipulateItems(1);
			inventory->DisableTransferMoreThanOneItem(0);
		}
	}

	int unk[2];
	oCNpcInventory *inventory;
	char kfjan[4];
};
