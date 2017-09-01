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
