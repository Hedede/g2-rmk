#ifndef G2Remake_OpenALHeaders_H
#define G2Remake_OpenALHeaders_H
#include <AL/al.h>
#include <AL/alc.h>
// correct way to do things would be query extensions and then load them
// but I don't care really
#define AL_ALEXT_PROTOTYPES
#include <AL/alext.h>
#endif//G2Remake_OpenALHeaders_H
