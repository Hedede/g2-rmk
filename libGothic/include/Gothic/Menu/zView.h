#ifndef Gothic_zView_H
#define Gothic_zView_H
#include <Hook/func.h>
#include <Gothic/Input/zCInputCallback.h>

struct zCViewBase {
	void *vtab;
};


struct zCView : zCViewBase, zCInputCallback {

};

zCView*& screen = Value<zCView*>(0xAB6468);

#endif//Gothic_zView_H
