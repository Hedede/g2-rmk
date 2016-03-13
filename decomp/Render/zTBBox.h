class zTBBox2D {
	zVEC2 mins; //zPOINT3	
	zVEC2 maxs; //zPOINT3
};

class zTBBox3D {
	zVEC3 mins; //zPOINT3	
	zVEC3 maxs; //zPOINT3
};

struct zCOBBox3D {
	zVEC3 center;
	zVEC3 axis[3];
	zVEC3 extent;
	zCList<zCOBBox3D>    childs;
};

void zCOBBox3D::SaveBIN(zCFileBIN *bin)
{
	bin->file->Write(this, sizeof(this->center));
	bin->file->Write(this->axis, sizeof(this->axis));
	bin->file->Write(&this->extent, sizeof(this->extent));

	zCList<zCOBBox3D>* next = childs.next;
	for (uint16_t i = 0; next; ++i )
		next = next->next;

	bin->file->Write(&i, sizeof(i));

	next = childs.next;
	for (; i > 0; --i) {
		next->data->SaveBIN(bin);
		--i;
		next = next->next;

	}
}
