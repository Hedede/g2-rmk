struct zTIndexA {
	zSTRING s;
};

struct zTIndexB {
	zSTRING s;
	uint32_t i;
};

class zCAccountInfo {

};

bool accountInitialized = 0;
bool zAccount     = true;
bool zAccountFull = true;

void zAccountInit()
{
	if ( zoptions ) {
		accountInitialized = 1;
		zAccount = zoptions->Parm("ZACCOUNT");
		zAccountFull = zoptions->Parm("ZACCOUNTFULL");
		zAccount |= zAccountFull;
	}
}
