struct zCOBBox3D {
	zVEC3 center;
	zVEC3 axis[3];
	zVEC3 extent;
	zCList<zCOBBox3D>    childs;
};

void zCOBBox3D::SaveBIN(zCFileBIN& bin)
{
	bin.Write(&center, sizeof(center));
	bin.Write(&axis[0], sizeof(axis));
	bin.Write(&extent, sizeof(extent));

	zCList<zCOBBox3D>* next = childs.next;
	for (uint16_t i = 0; next; ++i )
		next = next->next;

	bin.Write(&i, sizeof(i));

	next = childs.next;
	for (; i > 0; --i) {
		next->Get()->SaveBIN(bin);
		--i;
		next = next->next;
	}
}
