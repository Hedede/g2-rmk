#pragma once
struct _PMD {
	int mdisp;
	int pdisp;
	int vdisp;
};

struct TypeDescriptor {
	const void* vtab;
	void *spare = 0;
	// char name[];
};

inline const void* type_info_vtab()
{
	return reinterpret_cast<const void*>(0x843C68);
}

struct RTTIBaseClassDescriptor {
	TypeDescriptor *pTypeDescriptor;
	unsigned numContainedBases;
	_PMD where;
	unsigned attributes;
};

struct RTTIClassHierarchyDescriptor {
	unsigned signature;
	unsigned attributes;
	unsigned numBaseClasses;
	RTTIBaseClassDescriptor** pBaseClassArray;
};


struct RTTICompleteObjectLocator {
	unsigned signature;
	unsigned offset;
	unsigned cdOffset;
	TypeDescriptor *pTypeDescriptor;
	RTTIClassHierarchyDescriptor *pClassDescriptor;
};
