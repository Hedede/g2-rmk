zCPar_TreeNode* zCParser::PushOnStack(zCPar_TreeNode *node)
{
	lastNode = node;
	
	for (auto right = node->right; right; right = PushOnStack(right));
	for (auto left  = node->left;  left;  left  = PushOnStack(left));

	int type = node->tok_type;

	if (type < zPAR_OP_UN_NEG) {
		stack.PushByte(type);
		return node->next;
	}

	if (type == zPAR_ROK_FLOAT) {
		stak.PushByte(zPAR_TOK_PUSHINT);
		zCPar_Stack::PushInt(stack, node->label_index);
		return node->next;
	}
	
