//--------------------------------------
// Baum / Knoten. Für Vobtree
//--------------------------------------
template <class T> 
class zCTree {
	zCTree* parent;
	zCTree* firstChild;
	zCTree* next;
	zCTree* prev;
	T*      data;

public:
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
};
