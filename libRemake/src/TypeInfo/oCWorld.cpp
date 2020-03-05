#include "oCWorld.h"

namespace {
struct oCWorldTypeDescriptor : TypeDescriptor {
	const char name[22];
};

oCWorldTypeDescriptor type_descriptor {
	type_info_vtab(),
	0,
	".?oCWorld2@@"
};

RTTIBaseClassDescriptor base_class_descriptor {
	&type_descriptor, 3, 0, -1, 0, 0
};

auto& oCWorld_bcd  = Value<RTTIBaseClassDescriptor>(0x84B340);
auto& zCWorld_bcd  = Value<RTTIBaseClassDescriptor>(0x848DD4);
auto& zCObject_bcd = Value<RTTIBaseClassDescriptor>(0x844D88);

RTTIBaseClassDescriptor* base_class_array[] = {
	&base_class_descriptor,
	&oCWorld_bcd,
	&zCWorld_bcd,
	&zCObject_bcd,
	0
};

RTTIClassHierarchyDescriptor hierarchy_descriptor {
	0, 0, 4, +base_class_array
};
} // namespace

RTTICompleteObjectLocator oCWorld_rtti {
	0, 0, 0, &type_descriptor, &hierarchy_descriptor
};
