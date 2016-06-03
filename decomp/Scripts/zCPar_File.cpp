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
