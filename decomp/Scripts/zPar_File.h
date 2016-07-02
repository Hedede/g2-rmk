struct zCPar_File {
	static int IsNewerDate(zDATE& a, zDATE& b)
	{
		return a < b;
	}

	static int IsExactDate(zDATE& a, zDATE& b)
	{
		return a == b;
	}

	zCPar_File() = default;

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

	void Dispose()
	{
		delete startAddress;
		startAddress = nullptr;
	}

	void EnableTreeLoad(int b)
	{
		enableTreeLoad = b;
	}

	zCPar_TreeNode* GetTree()
	{
		return tree;
	}

	void SetTree(zCPar_TreeNode* node)
	{
		if (tree)
			delete tree;
		tree = node;
	}

	void DeleteTree()
	{
		Delete(tree);
	}

	zCPar_TreeNode* LoadTreeNode(zFILE* file);

private:
	int     fileNum  = 0;
	zSTRING fileName;

	void* startAddress = 0;
	int   fileSize     = 0;

	int labelCount       = 0;
	int enableTreeLoad   = 0;
	zCPar_TreeNode *tree = nullptr;

	// sizeof(zCPar_File) == 0x2C
};
