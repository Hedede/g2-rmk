struct zCPar_TreeNode {
	static auto MakeMonadicNode(char tok, zCPar_TreeNode *right)
	{
		auto node = new zCPar_TreeNode(tok, 0);
		node->right = right;

		return node;
	}

	static auto MakeDyadicNode(zCPar_TreeNode* left, char tok, zCPar_TreeNode *right)
	{
		auto node = new zCPar_TreeNode(tok, 0);
		node->left = left
		node->right = right;

		return node;
	}

	zCPar_TreeNode();
	zCPar_TreeNode(char tok, int index);
	zCPar_TreeNode(char tok, zSTRING& name, int index);

	void SetNext(zCPar_TreeNode *next)
	{
		this->next = next;
	}

	void SetLeft(zCPar_TreeNode *left)
	{
		this->left = left;
	}

	void SetRight(zCPar_TreeNode *right)
	{
		this->right = right;
	}

private:
	zSTRING name;
	zPAR_TOK tok_type;
	int label_index;
	int stack_index;
	zCPar_TreeNode *left;
	zCPar_TreeNode *right;
	zCPar_TreeNode *next;
	int id;
	static lastId;
};

zCPar_TreeNode::zCPar_TreeNode()
	: tok_type(0), label_index(0), stack_index(0)
	  left(nullptr), right(nullptr), next(nullptr),
	  id(lastId++)
{
}

zCPar_TreeNode::zCPar_TreeNode(char tok, int index)
	: tok_type(tok), label_index(unk), stack_index(0)
	  left(nullptr), right(nullptr), next(nullptr),
	  id(lastId++)
{
}

zCPar_TreeNode::zCPar_TreeNode(char tok, zSTRING& name, int index)
	: name(name), tok_type(tok), label_index(unk), stack_index(0)
	  left(nullptr), right(nullptr), next(nullptr),
	  id(lastId++)
{
}


zCPar_TreeNode::~zCPar_TreeNode()
{
	if (right)
		delete right;
	if (left)
		delete left;
	if (next)
		delete next;
}

zCPar_TreeNode* zCPar_TreeNode::SeekEndTree(zCPar_TreeNode& node)
{
	zCPar_TreeNode *result;
	zCPar_TreeNode *i;

	result = &node;
	for (i = node.next; i; i = i->next)
		result = i;

	return result;
}
