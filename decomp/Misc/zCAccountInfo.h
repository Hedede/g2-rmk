struct zTIndexA {
	zSTRING s;
};

struct zTIndexB {
	zSTRING s;
	uint32_t i;
};

class zCAccountInfo {

};

bool zAccount     = true;
bool zAccountFull = true;

void zAccountInit()
{
	if ( zoptions ) {
		dword_AAC318 = 1;
		zAccount = zoptions->Parm("ZACCOUNT");
		zAccountFull = zoptions->Parm("ZACCOUNTFULL");
		zAccount |= zAccountFull;
	}
}
