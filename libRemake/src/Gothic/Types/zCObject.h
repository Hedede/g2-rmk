#ifndef ZenGin_Object_H
#define ZenGin_Object_H
#include <Gothic/Types/zSTRING.h>

struct zCObject;
struct zCClassDef;
struct zCArchiver;

struct zCObject_vt
{
	zCClassDef *(__thiscall *GetClassDef)(zCObject *);
	void (__thiscall *Archive)(zCObject *, zCArchiver *);
	void (__thiscall *Unarchive)(zCObject *, zCArchiver *);
	void (__thiscall *dtor)(zCObject*, int);
};

struct zCObject {
	zCClassDef* GetClassDef()
	{
		return reinterpret_cast<zCObject_vt*>(_vtab)->GetClassDef(this);
	}

	~zCObject()
	{
		thiscall(0x5A8C50, this);
	}

	void Release()
	{
		if (1 >= refCtr--)
			reinterpret_cast<zCObject_vt*>(_vtab)->dtor(this,1);
			//delete this;
	}

	zSTRING const& GetObjectName() const
	{
		return objectName;
	}

	int SetObjectName(std::string const& name)
	{
		Thiscall<int(zCObject*, zSTRING const&)> call{0x5A9CE0};
		return call( this, zSTRING{name} );
	}

	void* _vtab;

	int refCtr    = 1;
	int hashIndex = -1;
	int hashNext  = 0;
	zSTRING objectName;
};

#include <Gothic/Game/zCClassDef.h>
struct zCObjectAllocator {
	static void* Allocate(size_t size, zCClassDef* classDef)
	{
		void* mem = ::operator new(size);

		auto object = reinterpret_cast<zCObject*>(mem);
		zCClassDef::ObjectCreated(object, classDef);

		return mem;
	}
};

#define zCLASS_DECLARATION(class_name) \
	static zCClassDef* classDef; \
	public: \
	void* operator new(size_t size) { return zCObjectAllocator::Allocate(size, classDef); }
#endif//ZenGin_Object_H
