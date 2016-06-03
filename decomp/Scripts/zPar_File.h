struct zCPar_File {
	zSTRING GetName()
	{
		return fileName;
	}

	void SetFileNumber(int num)
	{
		fileNum = num;
	}

	int GetFileNumber()
	{
		return fileNum;
	}

	void SetLabelCount(int count)
	{
		labelCount = count;
	}

	int GetLabelCount()
	{
		return labelCount;
	}

	int GetStartAddress()
	{
		return startAddress;
	}

	int GetEndAddress()
	{
		return startAddress + fileSize - 1;
	}

	void EnableTreeLoad(int b)
	{
		enableTreeLoad = b;
	}

	zCPar_TreeNode* GetTree()
	{
		return tree;
	}

	zCPar_TreeNode* LoadTreeNode(zFILE* file);

private:
	int     fileNum;
	zSTRING fileName;

	void* startAddress;
	int   fileSize;

	int labelCount;
	int enableTreeLoad;
	zCPar_TreeNode *tree;
	int unk4[3];
};
