#ifndef G2_zCArray
#define G2_zCArray
/* zCArray ist eine sehr einfache und gleichzeitig sehr wichtige Datenstruktur. */
template <class T>
struct zCArray {
	zCArray() = default;

	//Zeiger auf Speicherbereich
	T* array = nullptr;
	//Für wieviele Elemente ist gerade Speicher reserviert?
	int numAlloc = 0;
	//Anzahl der Elemente in diesem Speicherbereich
	int numInArray = 0;
};
#endif//G2_zCArray
