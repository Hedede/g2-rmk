struct zCPar_File {
	~zCPar_File()
	{
		delete[]startAddress;
		Delete(tree);
	}

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

	char* startAddress = 0;
	int   fileSize     = 0;

	int labelCount       = 0;
	int enableTreeLoad   = 0;
	zCPar_TreeNode *tree = nullptr;

	// sizeof(zCPar_File) == 0x2C
};

zCPar_TreeNode* zCPar_File::LoadTreeNode(zFILE *file)
{
	auto node = new zCPar_TreeNode;

	int tmp;
	file->Read(file, &tmp, 4);

	if (tmp)
		file->Read(node->name);

	file->Read(&node->tok_type, 1);
	file->Read(&node->label_index, 4);
	file->Read(&node->stack_index, 4);
	file->Read(&node->left, 4);
	file->Read(&node->right, 4);
	file->Read(&node->next, 4);

	if (node->left)
		node->left  = LoadTreeNode(file);
	if (node->right)
		node->right = LoadTreeNode(file);
	if (node->next)
		node->next  = LoadTreeNode(file);

	return node;
}

void zCPar_File::SaveTreeNode(zFILE *file, zCPar_TreeNode *node)
{
	if ( node ) {
		file->Write(&node, 4);
		if ( node ) {
			zSTRING tmp = node->name + "\n";
			file->Write(tmp);
		}

		file->Write(&node->tok_type, 1);
		file->Write(&node->label_index, 4);
		file->Write(&node->stack_index, 4);
		file->Write(&node->left, 4);
		file->Write(&node->right, 4);
		file->Write(&node->next, 4);

		if (node->left)
			SaveTreeNode(file, node->left);
		if (node->right)
			SaveTreeNode(file, node->right);
		if (node->next)
			SaveTreeNode(file, node->next);
	}
}
