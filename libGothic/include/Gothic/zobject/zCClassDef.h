#ifndef G2_zCClassDef
#define G2_zCClassDef
#include <functional>
#include <Gothic/types/zSTRING.h>
#include <Gothic/types/zCArray.h>
#include <Gothic/types/zCArraySort.h>
//--------------------------------------
// zCClassDef
//--------------------------------------

/* Für jede (von zCObject abgeleitete) Klasse gibt es
 * ein "Verwaltungsobjekt" vom Typ zCClassDef.
 * Dieses kapselt einige nützliche Informationen zu der
 * Gesamtheit der Objekte dieser Klasse.
 * Mit MEM_GetClassDef ( objPtr) kann das
 * zu einem Objekt gehörige zCClassDef Objekt bestimmt
 * werden. Das heißt für ein übergebenes Vob, bekäme
 * man zCClassDef für die Klasse zCVob.
 */

enum zTClassFlags {
	zCLASS_FLAG_SHARED_OBJECTS = 1<<0, //Mehrfach benutzt Objekte (wie Visuals zum Beispiel)
	zCLASS_FLAG_TRANSIENT      = 1<<1, //Flüchtig, soll nicht gespeichert werden.
	zCLASS_FLAG_RESOURCE       = 1<<2, //keine Ahnung / vermutlich irrelevant
};

class zCObject;

class zCClassDef {
	friend class zCObject;
	static zCArraySort<zCClassDef*> classDefList;
public:
	using CreateObject = std::function<zCObject*(void)>;

	zCClassDef(zSTRING const& className,
	           zSTRING const& baseName,
	           CreateObject create, size_t classSize,
	           int classFlags, short version,
	           zSTRING const& scriptName);

	static void ObjectCreated(zCObject *obj, zCClassDef* def);
	static void ObjectDeleted(zCObject *obj, zCClassDef* def);

	void Init();

	uint16_t GetClassID()
	{
		return classDefList.Search(this);
	}

	zCObject* CreateNewInstance()
	{
		if ( createNewInstance )
			return createNewInstance();

		zFAULT("D: zObject(zCClassDef::CreateNewInstance): tried to create instance of abstrace class: " + className);
		return nullptr;
	}

	void ResetCreateNewInstance()
	{
		if ( createNewInstance )
			createNewInstance = createNewInstanceBackup;
	}
private:
	zSTRING className;
	zSTRING baseClassName;
	zSTRING scriptClassName;
	zCClassDef*  baseClassDef;            // //davon abgeleitet

	zCObject* ( *createNewInstance) (void);       //Pointer auf klassenspezifische Funktion
	zCObject* ( *createNewInstanceBackup) (void); //Pointer auf klassenspezifische Funktion

	uint32_t   classFlags = 0; //siehe enum
	uint32_t   classSize  = 0; //Größe in Bytes

	int numLivingObjects = 0; //Anzahl Objekte von dieser Klasse
	int numCtorCalled    = 0; //Konstruktor wurde sooft aufgerufen

	//Hashtabelle der Größe 1024. Objekte sind mit zCObject.hashNext verknüpft, falls mehrere auf den selben Wert hashen.
	zCObject**    hashTable;

	/*
	 * alle benannten
	 * (!) Objekte von genau
	 * (!) dieser Klasse
	 * (!) Ausrufezeichenanmerkungen:
	 * 1.) unbenannte sind nicht drin
	 * 2.) Objekte von Unterklassen sind nicht drin
	 * 3.) diese Eigenschaft kann sehr nützlich sein.
	 */
	zCArray<zCObject*> objectList;

	uint16_t archiveVersion    = 0; //vermutlich nutzlos
	uint16_t archiveVersionSum = 0; //vermutlich nutzlos
};
#endif//G2_zCClassDef
