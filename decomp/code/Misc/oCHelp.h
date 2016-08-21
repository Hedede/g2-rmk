struct oCHelp {
	void ~oCHelp()
	{
		if ( view->ondesk )
			screen->RemoveItem( view );
		if ( view )
			delete view;
	}

	void Update() {}

	zCView *view;
	int posX[4];
	int posY[4];
	int unk[4];
	int textIdx[4];
};
