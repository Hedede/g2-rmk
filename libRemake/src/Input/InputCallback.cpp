#include <Gothic/Input/zInput.h>
#include <Gothic/Input/zCInputCallback.h>
zCList<zCInputCallback>& zCInputCallback::callbacksList = Value<zCList<zCInputCallback>>(0xAB6464);
zCInput*& zinput = Value<zCInput*>(0x8D1650);
