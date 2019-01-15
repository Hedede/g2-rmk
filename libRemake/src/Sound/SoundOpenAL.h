#ifndef G2Remake_SoundOpenAL_H
#define G2Remake_SoundOpenAL_H
#include <unordered_map>
#include <Logging/Log.h>

#include <Sound/OpenALSource.h>

#include "OpenALHeaders.h"
#include "SourcePool.h"

struct zCVob;

namespace g2 {
struct Attributes {
	int max_sources;
};

struct SoundOpenAL {
	SoundOpenAL();
	~SoundOpenAL();

	int max_sources() const;
	void play( Source& src );

	SourcePool  pool;

	Attributes attributes() const { return attrib; }

protected:
	bool MakeContextCurrent();

private:
	ALCdevice*  dev;
	ALCcontext* ctx;
	Attributes attrib;
};
} // namespace g2
#endif//G2Remake_SoundOpenAL_H
