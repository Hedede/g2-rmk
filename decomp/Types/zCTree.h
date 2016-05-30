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
};
