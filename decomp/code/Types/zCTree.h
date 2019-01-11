//--------------------------------------
// Baum / Knoten. Für Vobtree
//--------------------------------------
extern zCMemPoolBase s_TreePool; // TODO: actual type

template <class T> 
class zCTree {

	zCTree* parent = nullptr;
	zCTree* firstChild = nullptr;
	zCTree* next = nullptr;
	zCTree* prev = nullptr;
	T*      data = nullptr;

public:
	zCTree() = default;

	void DeleteDataSubtree()
	{
		if ( data )
			delete data;
		for ( auto* i = firstChild; i; i = i->next )
			i->DeleteDataSubtree();
	}

	void RemoveSubtree()
	{
		if ( parent && parent->firstChild == this )
			parent->firstChild = this->next;
		if ( prev )
			prev->next = this->next;
		if ( next )
			next->prev = this->prev;

		this->next = nullptr;
		this->prev = nullptr;
	}

	void AddChild(zCTree<T> *node)
	{
		node->parent = this;
		node->prev = nullptr;
		node->next = this->firstChild;
		this->firstChild = node;
		auto next = node->next;
		if ( next )
			next->prev = node;
	}

	zCTree<T>* AddChild(zCVob *d)
	{
		auto node = new zCTree<T>;
		node->data = d;
		AddChild( node );
		return node;
	}

	int CountNodes() const
	{
		int result = 1;
		if (firstChild)
			result += firstChild->CountNodes();
		if (next)
			result += next->CountNodes();
		return result;
	}

	void operator delete(void *ptr)
	{
		s_TreePool.Free(ptr);
	}

	void operator new(size_t count)
	{
		return s_TreePool.Alloc();
	}
};

