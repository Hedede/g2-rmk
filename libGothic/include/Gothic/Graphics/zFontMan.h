#ifndef Gothic_zFontMan_H
#define Gothic_zFontMan_H
#include <Hook/Externals.h>
namespace g2 {
struct FontMan;
}

static g2::FontMan*& zfontman = Value<g2::FontMan*>(0xAB39D4);

#endif//Gothic_zFontMan_H
