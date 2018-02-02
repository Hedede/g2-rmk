#include "SourcePool.h"
#include "SoundOpenAL.h"

#include <aw/utility/ranges/zip.h>
#include <aw/algorithm/in.h>

namespace g2 {

SourcePool::SourcePool(int max_sources)
	: free_sources(max_sources)
{
	Log( "SFX", "Allocating " + std::to_string(max_sources) + " sources"  );
	alGenSources( free_sources.size(), free_sources.data() );
	if (alGetError() != AL_NO_ERROR)
		Error( "SFX", "Could not allocate source pool" );
}


SourcePool::~SourcePool()
{
	/*
	   if (!sources.empty())
	   alDeleteSources( sources.size(), sources.data() );
	   */
}

Source SourcePool::request_source(int priority)
{
	int reserved = (priority < 0) ? 0 : 8;
	if (free_sources.size() > 8) {
		auto src = free_sources.back();
		free_sources.pop_back();
		used_sources.push_back(src);
		priorities.push_back(priority);
		return src;
	}

	for (auto [s,p] : aw::zip(used_sources, priorities)) {
		if (p > priority) {
			p = priority;
			return s;
		}
                ALint state;
                alGetSourcei(s, AL_SOURCE_STATE, &state);
		if (!aw::in(state, AL_PLAYING, AL_PAUSED)) {
			p = priority;
			return s;
		}
	}

	Warning( "SFX", "No free sources" );

	return {};
}
} //namespace g2
