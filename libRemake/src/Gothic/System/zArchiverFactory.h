#pragma once

class zCArchiver;
class zCArchiverFactory {
public:
	zCArchiver* CreateArchiverRead(std::string_view fileNameRead,int arcFlags)
	{
		zSTRING fileName(fileNameRead);

		Thiscall<zCArchiver*(zCArchiverFactory*,const zSTRING&,int)> call(0x51A120);
		return call(this, fileName, arcFlags);
	}


};



