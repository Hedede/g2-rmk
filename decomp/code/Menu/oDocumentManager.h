struct oCDocumentManager : zCInputCallback {
	static auto GetDocumentManager() -> oCDocumentManager&
	{
		static oCDocumentManager docMan;
		return docMan;
	}

	oCDocumentManager()
		: zCInputCallback()
	{
		documentList = new zCListSort<oCViewDocument>();

		// TODO
	}

	virtual ~oCDocumentManager()
	{
		delete documentList;
		Release(dlg1);
	}

	void HandleEvent(int) override;

	int GetNextAvailableID() const
	{
		return documentList->GetNumInList();
	}

private:
	zCListSort<oCViewDocument>* documentList;

	zCViewDialog* dlg1;
};
